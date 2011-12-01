/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifdef WEBOS

#include "common/scummsys.h"
#include "common/system.h"
#include "common/str.h"
#include "common/translation.h"

#include "backends/events/webossdl/webossdl-events.h"
#include "gui/message.h"
#include "engines/engine.h"

// PDL.h provided by the official Palm WebOS PDK.
#include <PDL.h>

/**
 * WebOS devices only have a Shift key and a CTRL key. There is also an Alt
 * key (the orange key) but this is already processed by WebOS to change the
 * mode of the keys so ScummVM must not use this key as a modifier. Instead
 * pressing down the gesture area is used as Alt key.
 *
 * @param mod   The pressed key modifier as detected by SDL.
 * @param event The ScummVM event to setup.
 */
void WebOSSdlEventSource::SDLModToOSystemKeyFlags(SDLMod mod,
		Common::Event &event) {
	event.kbd.flags = 0;

	if (mod & KMOD_SHIFT)
		event.kbd.flags |= Common::KBD_SHIFT;
	if (mod & KMOD_CTRL)
		event.kbd.flags |= Common::KBD_CTRL;

	// Holding down the gesture area emulates the ALT key
	if (_gestureDown)
		event.kbd.flags |= Common::KBD_ALT;
}

/**
 * Before calling the original SDL implementation this method checks if the
 * gesture area is pressed down.
 *
 * @param ev    The SDL event
 * @param event The ScummVM event.
 * @return True if event was processed, false if not.
 */
bool WebOSSdlEventSource::handleKeyDown(SDL_Event &ev, Common::Event &event) {
	// Handle gesture area tap.
	if (ev.key.keysym.sym == SDLK_WORLD_71) {
		_gestureDown = true;
		return true;
	}

	// Ensure that ALT key (Gesture down) is ignored when back or forward
	// gesture is detected. This is needed for WebOS 1 which releases the
	// gesture tap AFTER the backward gesture event and not BEFORE (Like
	// WebOS 2).
	if (ev.key.keysym.sym == 27 || ev.key.keysym.sym == 229) {
		 _gestureDown = false;
	}

	// handle virtual keyboard dismiss key
	if (ev.key.keysym.sym == 24) {
		int gblPDKVersion = PDL_GetPDKVersion();
		// check for correct PDK Version, as this determines whether an
		// OS-supplied virtual keyboard is available on this device.
		if (gblPDKVersion >= 300) {
			PDL_SetKeyboardState(PDL_FALSE);
			return true;
		}
	}

	// Call original SDL key handler.
	return SdlEventSource::handleKeyDown(ev, event);
}

/**
 * Before calling the original SDL implementation this method checks if the
 * gesture area has been released.
 *
 * @param ev    The SDL event
 * @param event The ScummVM event.
 * @return True if event was processed, false if not.
 */
bool WebOSSdlEventSource::handleKeyUp(SDL_Event &ev, Common::Event &event) {
	// Handle gesture area tap.
	if (ev.key.keysym.sym == SDLK_WORLD_71) {
		_gestureDown = false;
		return true;
	}

	// handle virtual keyboard dismiss key
	if (ev.key.keysym.sym == 24) {
		int gblPDKVersion = PDL_GetPDKVersion();
		// check for correct PDK Version, as this determines whether an
		// OS-supplied virtual keyboard is available on this device.
		if (gblPDKVersion >= 300) {
			PDL_SetKeyboardState(PDL_FALSE);
			return true;
		}
	}

	// Call original SDL key handler.
	return SdlEventSource::handleKeyUp(ev, event);
}

/**
 * Handles mouse button press.
 *
 * @param ev    The SDL event
 * @param event The ScummVM event.
 * @return True if event was processed, false if not.
 */
bool WebOSSdlEventSource::handleMouseButtonDown(SDL_Event &ev,
		Common::Event &event) {
	_dragDiffX[ev.button.which] = 0;
	_dragDiffY[ev.button.which] = 0;
	_fingerDown[ev.button.which] = true;
	_screenDownTime[ev.button.which] = g_system->getMillis();

	if (ev.button.which == 0) {
		// Do a click when the finger lifts unless we leave the range
		_doClick = true;
		// Queue up dragging if auto-drag mode is on
		if (_autoDragMode)
			_queuedDragTime = g_system->getMillis() + QUEUED_DRAG_DELAY;
		// Turn drag mode on instantly for a double-tap
		else if (g_system->getMillis() - _dragStartTime < DOUBLETAP_LIMIT) {
			_dragging = true;
			event.type = Common::EVENT_LBUTTONDOWN;
			processMouseEvent(event, _curX, _curY);
		}
		// If we're not in touchpad mode, move the cursor to the tap
		if (!_touchpadMode) {
			_curX = MIN(_screenX, MAX(0, 0 + ev.motion.x));
			_curY = MIN(_screenY, MAX(0, 0 + ev.motion.y));
			// If we're already clicking, hold it until after the move.
			if (event.type == Common::EVENT_LBUTTONDOWN) {
				processMouseEvent(event, _curX, _curY);
				g_system->getEventManager()->pushEvent(event);
			}
			// Move the mouse
			event.type = Common::EVENT_MOUSEMOVE;
			processMouseEvent(event, _curX, _curY);
		}
		// Watch for a double-tap-triggered drag
		_dragStartTime = g_system->getMillis();
	} else if (ev.button.which == 1) {
		// Kill any queued drag event if a second finger goes down
		if (_queuedDragTime > 0)
			_queuedDragTime = 0;
		_doClick = false;
	}
	return true;
}

/**
 * Handles mouse button release.
 *
 * @param ev    The SDL event
 * @param event The ScummVM event.
 * @return True if event was processed, false if not.
 */
bool WebOSSdlEventSource::handleMouseButtonUp(SDL_Event &ev,
		Common::Event &event) {
	// Only react if the finger hasn't been virtually lifted already
	if (_fingerDown[ev.button.which]) {
		// No matter what, if it's the first finger that's lifted when
		// we're dragging, just lift the mouse button.
		if (ev.button.which == 0 && _dragging) {
			event.type = Common::EVENT_LBUTTONUP;
			processMouseEvent(event, _curX, _curY);
			_dragging = false;
		} else {
			// If it was the first finger and the click hasn't been
			// canceled, it's a click.
			if (ev.button.which == 0 && _doClick &&
					!_fingerDown[1] && !_fingerDown[2]) {
				event.type = Common::EVENT_LBUTTONUP;
				processMouseEvent(event, _curX, _curY);
				g_system->getEventManager()->pushEvent(event);
				event.type = Common::EVENT_LBUTTONDOWN;
				if (_queuedDragTime > 0)
					_queuedDragTime = 0;
			} else if (ev.button.which == 1 &&
					_fingerDown[0] && _fingerDown[1] && !_fingerDown[2]) {
				// If the first finger's down and the second taps, it's a
				// right mouse click.
				event.type = Common::EVENT_RBUTTONDOWN;
				processMouseEvent(event, _curX, _curY);
				_queuedRUpTime = g_system->getMillis() + QUEUED_RUP_DELAY;
			} else if (ev.button.which == 2 &&
					_fingerDown[0] && _fingerDown[1]) {
				// If two fingers are down and a third taps, it's a middle
				// click -- but lift the second finger so it doesn't register
				// as a right click.
				event.type = Common::EVENT_MBUTTONUP;
				processMouseEvent(event, _curX, _curY);
				g_system->getEventManager()->pushEvent(event);
				event.type = Common::EVENT_MBUTTONDOWN;
				_fingerDown[1] = false;
			}
		}
		// Officially lift the finger that was raised.
		_fingerDown[ev.button.which] = false;
	}
	return true;
}

/**
 * Handles mouse motion.
 *
 * @param ev    The SDL event
 * @param event The ScummVM event.
 * @return True if event was processed, false if not.
 */
bool WebOSSdlEventSource::handleMouseMotion(SDL_Event &ev,
		Common::Event &event) {
	if (_fingerDown[ev.motion.which]) {
		_dragDiffX[ev.motion.which] += ev.motion.xrel;
		_dragDiffY[ev.motion.which] += ev.motion.yrel;

		switch (ev.motion.which) {
		case 0:
			// If our dragDiff goes too many pixels in either direction,
			// kill the future click and any queued drag event.
			if (_doClick && (ABS(_dragDiffX[0]) > MOUSE_DEADZONE_PIXELS ||
					ABS(_dragDiffY[0]) > MOUSE_DEADZONE_PIXELS)) {
				_doClick = false;
				if (_queuedDragTime > 0)
					_queuedDragTime = 0;
			}
			// If only one finger is on the screen and moving, that's
			// the mouse pointer.
			if (!_fingerDown[1] && !_fingerDown[2]) {
				if (_touchpadMode) {
					_curX = MIN(_screenX, MAX(0, _curX + ev.motion.xrel));
					_curY = MIN(_screenY, MAX(0, _curY + ev.motion.yrel));
				} else {
					_curX = MIN(_screenX, MAX(0, 0 + ev.motion.x));
					_curY = MIN(_screenY, MAX(0, 0 + ev.motion.y));
				}
				event.type = Common::EVENT_MOUSEMOVE;
				processMouseEvent(event, _curX, _curY);
			}
			break;
		case 1:
			// Check for a two-finger swipe
			if (_fingerDown[0] && !_fingerDown[2]) {
				// Check for a vertical swipe
				if (ABS(_dragDiffY[0]) > _swipeDistY &&
						ABS(_dragDiffY[1]) > _swipeDistY) {
					// Virtually lift fingers to prevent repeat triggers
					_fingerDown[0] = _fingerDown[1] = false;
					if (_dragDiffY[0] < 0 && _dragDiffY[1] < 0) {
						// A swipe up triggers the keyboard, if it exists. We
						// test for existance of a virtual OS keyboard by
						// checking for the version of the linked PDK libs.
						int gblPDKVersion = PDL_GetPDKVersion();
						if (gblPDKVersion >= 300)
							PDL_SetKeyboardState(PDL_TRUE);
					} else if (_dragDiffY[0] > 0 && _dragDiffY[1] > 0) {
						// A swipe down triggers the menu
						if (g_engine && !g_engine->isPaused())
							g_engine->openMainMenuDialog();
					}
					return true;
				}
				// Check for a horizontal swipe
				if (ABS(_dragDiffX[0]) > _swipeDistX &&
						ABS(_dragDiffX[1]) > _swipeDistX) {
					// Virtually lift fingers to prevent repeat triggers
					_fingerDown[0] = _fingerDown[1] = false;
					if (_dragDiffX[0] < 0 && _dragDiffX[1] < 0) {
						// A swipe left presses escape
						event.type = Common::EVENT_KEYDOWN;
						event.kbd.flags = 0;
						event.kbd.keycode = Common::KEYCODE_ESCAPE;
						event.kbd.ascii = Common::ASCII_ESCAPE;
						_queuedEscapeUpTime = g_system->getMillis() +
							QUEUED_KEY_DELAY;
					} else if (_dragDiffX[0] > 0 && _dragDiffX[1] > 0) {
						// A swipe right toggles touchpad mode
						_touchpadMode = !_touchpadMode;
						g_system->showMouse(_touchpadMode);
						// I18N: Touchpad mode toggle status.
						Common::String dialogMsg(_("Touchpad mode is now"));
						dialogMsg += " ";
						// I18N: Touchpad mode on or off.
						dialogMsg += (_touchpadMode ? _("ON") : _("OFF"));
						dialogMsg += ".\n";
						// I18N: Instructions to toggle Touchpad mode.
						dialogMsg +=
							_("Swipe two fingers to the right to toggle.");
						GUI::TimedMessageDialog dialog(dialogMsg, 1500);
						dialog.runModal();
					}
					return true;
				}
			}
			break;
		case 2:
			// Check for a three-finger swipe
			if (_fingerDown[0] && _fingerDown[1]) {
				if (_dragDiffX[0] > _swipeDistX &&
						_dragDiffX[1] > _swipeDistX &&
						_dragDiffX[2] > _swipeDistX) {
					// Swipe right to emulate right arrow
					// Virtually lift fingers to prevent repeat triggers
					_fingerDown[0] = _fingerDown[1] = _fingerDown[2] = false;
					// Press right
					event.type = Common::EVENT_KEYDOWN;
					event.kbd.flags = 0;
					event.kbd.keycode = Common::KEYCODE_RIGHT;
					event.kbd.ascii = Common::ASCII_RIGHT;
					_queuedRightUpTime = g_system->getMillis() +
						QUEUED_KEY_DELAY;
				} else if (_dragDiffX[0] * -1 > _swipeDistX &&
						_dragDiffX[1] * -1 > _swipeDistX &&
						_dragDiffX[2] * -1 > _swipeDistX ) {
					// Swipe left to emulate left arrow
					// Virtually lift fingers to prevent repeat triggers
					_fingerDown[0] = _fingerDown[1] = _fingerDown[2] = false;
					// Press right
					event.type = Common::EVENT_KEYDOWN;
					event.kbd.flags = 0;
					event.kbd.keycode = Common::KEYCODE_LEFT;
					event.kbd.ascii = Common::ASCII_LEFT;
					_queuedLeftUpTime = g_system->getMillis() +
						QUEUED_KEY_DELAY;
				} else if (_dragDiffY[0] > _swipeDistY &&
						_dragDiffY[1] > _swipeDistY &&
						_dragDiffY[2] > _swipeDistY ) {
					// Swipe down to emulate down arrow
					// Virtually lift fingers to prevent repeat triggers
					_fingerDown[0] = _fingerDown[1] = _fingerDown[2] = false;
					// Press right
					event.type = Common::EVENT_KEYDOWN;
					event.kbd.flags = 0;
					event.kbd.keycode = Common::KEYCODE_DOWN;
					event.kbd.ascii = Common::ASCII_DOWN;
					_queuedDownUpTime = g_system->getMillis() +
						QUEUED_KEY_DELAY;
				} else if (_dragDiffY[0] * -1 > _swipeDistY &&
						_dragDiffY[1] * -1 > _swipeDistY &&
						_dragDiffY[2] * -1 > _swipeDistY ) {
					// Swipe up to emulate up arrow
					// Virtually lift fingers to prevent repeat triggers
					_fingerDown[0] = _fingerDown[1] = _fingerDown[2] = false;
					// Press right
					event.type = Common::EVENT_KEYDOWN;
					event.kbd.flags = 0;
					event.kbd.keycode = Common::KEYCODE_UP;
					event.kbd.ascii = Common::ASCII_UP;
					_queuedUpUpTime = g_system->getMillis() +
						QUEUED_KEY_DELAY;
			  }
			}
		}
	}
	return true;
}

/**
 * Before calling the original SDL implementation, this method loads in
 * queued events.
 *
 * @param event The ScummVM event
 */
bool WebOSSdlEventSource::pollEvent(Common::Event &event) {
	uint32 curTime = g_system->getMillis();

	// Event-dependent nitializations for when SDL runs its first poll.
	if (_firstPoll) {
		// Set the initial dimensions
		calculateDimensions();
		
		// Having a mouse pointer on screen when not in Touchpad mode is poor
		// interface design, because the user won't know whether to tap buttons
		// or drag the pointer to them.  On the first poll, set the appropriate
		// pointer visibility.
		g_system->showMouse(_touchpadMode);
		_firstPoll = false;
	}

	// Run down the priority list for queued events. The built-in
	// event queue runs events on the next poll, which causes many
	// WebOS devices (and a few game engines) to ignore certain inputs.
	// Allowing keys and clicks to stay "down" longer is enough to register
	// the press.
	if (_queuedEscapeUpTime != 0 && curTime >= _queuedEscapeUpTime) {
		event.type = Common::EVENT_KEYUP;
		event.kbd.flags = 0;
		event.kbd.keycode = Common::KEYCODE_ESCAPE;
		event.kbd.ascii = Common::ASCII_ESCAPE;
		_queuedEscapeUpTime = 0;
		return true;
	} else if (_queuedRightUpTime != 0 && curTime >= _queuedRightUpTime) {
		event.type = Common::EVENT_KEYUP;
		event.kbd.flags = 0;
		event.kbd.keycode = Common::KEYCODE_RIGHT;
		event.kbd.ascii = Common::ASCII_RIGHT;
		_queuedRightUpTime = 0;
		return true;
	} else if (_queuedLeftUpTime != 0 && curTime >= _queuedLeftUpTime) {
		event.type = Common::EVENT_KEYUP;
		event.kbd.flags = 0;
		event.kbd.keycode = Common::KEYCODE_LEFT;
		event.kbd.ascii = Common::ASCII_LEFT;
		_queuedLeftUpTime = 0;
		return true;
	} else if (_queuedDownUpTime != 0 && curTime >= _queuedDownUpTime) {
		event.type = Common::EVENT_KEYUP;
		event.kbd.flags = 0;
		event.kbd.keycode = Common::KEYCODE_DOWN;
		event.kbd.ascii = Common::ASCII_DOWN;
		_queuedDownUpTime = 0;
		return true;
	} else if (_queuedUpUpTime != 0 && curTime >= _queuedUpUpTime) {
		event.type = Common::EVENT_KEYUP;
		event.kbd.flags = 0;
		event.kbd.keycode = Common::KEYCODE_UP;
		event.kbd.ascii = Common::ASCII_UP;
		_queuedUpUpTime = 0;
		return true;
	} else if (_queuedRUpTime != 0 && curTime >= _queuedRUpTime) {
		event.type = Common::EVENT_RBUTTONUP;
		processMouseEvent(event, _curX, _curY);
		_queuedRUpTime = 0;
		return true;
	} else if (_queuedDragTime != 0 && curTime >= _queuedDragTime) {
		event.type = Common::EVENT_LBUTTONDOWN;
		_dragging = true;
		processMouseEvent(event, _curX, _curY);
		_queuedDragTime = 0;
		return true;
	}

	return SdlEventSource::pollEvent(event);
}

/**
 * Sets the _screenX and _screenY variables to the effective screen dimensions,
 * and alters _swipeDistX and _swipeDistY to the correct relative values.
 */
void WebOSSdlEventSource::calculateDimensions() {
	_screenX = g_system->getOverlayWidth();
	_screenY = g_system->getOverlayHeight();
	_swipeDistX = _screenX * SWIPE_PERCENT_HORIZ / 100;
	_swipeDistY = _screenY * SWIPE_PERCENT_VERT / 100;
}

#endif
