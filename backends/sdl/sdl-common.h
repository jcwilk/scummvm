/* ScummVM - Scumm Interpreter
 * Copyright (C) 2001/2002 The ScummVM project
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header$
 *
 */

#ifndef SDL_COMMON_H
#define SDL_COMMON_H

#include "stdafx.h"
#include "scummsys.h"
#include "system.h"

#include <SDL.h>
#include <SDL_thread.h>

class OSystem_SDL_Common : public OSystem {
public:
	// Set colors of the palette
	void set_palette(const byte *colors, uint start, uint num) = 0;

	// Set the size of the video bitmap.
	// Typically, 320x200
	void init_size(uint w, uint h);

	// Draw a bitmap to screen.
	// The screen will not be updated to reflect the new bitmap
	void copy_rect(const byte *buf, int pitch, int x, int y, int w, int h);

	void move_screen(int dx, int dy, int height);

	// Update the dirty areas of the screen
	void update_screen() = 0;

	// Either show or hide the mouse cursor
	bool show_mouse(bool visible);
	
	// Set the position of the mouse cursor
	void set_mouse_pos(int x, int y);
	
	// Set the bitmap that's used when drawing the cursor.
	void set_mouse_cursor(const byte *buf, uint w, uint h, int hotspot_x, int hotspot_y);
	
	// Shaking is used in SCUMM. Set current shake position.
	void set_shake_pos(int shake_pos);
		
	// Get the number of milliseconds since the program was started.
	uint32 get_msecs();
	
	// Delay for a specified amount of milliseconds
	void delay_msecs(uint msecs);
	
	// Create a thread
	void *create_thread(ThreadProc *proc, void *param);
	
	// Get the next event.
	// Returns true if an event was retrieved.	
	bool poll_event(Event *event);
	
	// Set function that generates samples 
	bool set_sound_proc(void *param, SoundProc *proc, byte sound);
		
	// Poll CD status
	// Returns true if cd audio is playing
	bool poll_cdrom();

	// Play CD audio track
	void play_cdrom(int track, int num_loops, int start_frame, int end_frame);

	// Stop CD audio track
	void stop_cdrom();

	// Update CD audio status
	void update_cdrom();

	// Quit
	void quit();

	// Set a parameter
	uint32 property(int param, Property *value);

	// Add a callback timer
	void set_timer(int timer, int (*callback)(int));

	// Mutex handling
	void *create_mutex(void);
	void lock_mutex(void *mutex);
	void unlock_mutex(void *mutex);
	void delete_mutex(void *mutex);

	// Overlay
	virtual void show_overlay();
	virtual void hide_overlay();
	virtual void clear_overlay();
	virtual void grab_overlay(int16 *buf, int pitch);
	virtual void copy_rect_overlay(const int16 *buf, int pitch, int x, int y, int w, int h);
	virtual int16 get_height();
	virtual int16 get_width();

	// Methods that convert RBG to/from colors suitable for the overlay.
	virtual int16 RBGToColor(uint8 r, uint8 g, uint8 b);
	virtual void colorToRBG(int16 color, uint8 &r, uint8 &g, uint8 &b);

	static OSystem *create(int gfx_mode, bool full_screen);

protected:
	typedef void ScalerProc(uint8 *srcPtr, uint32 srcPitch, uint8 *deltaPtr,
								uint8 *dstPtr, uint32 dstPitch, int width, int height);

	OSystem_SDL_Common();
	virtual ~OSystem_SDL_Common();

	// unseen game screen
	SDL_Surface *_screen;
	int _screenWidth, _screenHeight;

	// temporary screen (for scalers/overlay)
	SDL_Surface *_tmpscreen;
	int _tmpScreenWidth;
	bool _overlayVisible;

	// CD Audio
	SDL_CD *_cdrom;
	int cd_track, cd_num_loops, cd_start_frame, cd_end_frame;
	Uint32 cd_end_time, cd_stop_time, cd_next_second;

	enum {
		DF_WANT_RECT_OPTIM			= 1 << 0,
		DF_UPDATE_EXPAND_1_PIXEL	= 1 << 3
	};

	bool _forceFull; // Force full redraw on next update_screen
	int _scaleFactor;
	int _mode;
	bool _full_screen;
	uint32 _mode_flags;

	enum {
		NUM_DIRTY_RECT = 100,

		MAX_MOUSE_W = 80,
		MAX_MOUSE_H = 80,
		MAX_SCALING = 3
	};

	// Dirty rect managment
	SDL_Rect _dirty_rect_list[100];
	int _num_dirty_rects;
	uint32 *_dirty_checksums;
	bool cksum_valid;
	int CKSUM_NUM;

	// Keyboard mouse emulation
	struct KbdMouse {	
		int16 x, y, x_vel, y_vel, x_max, y_max, x_down_count, y_down_count;
		uint32 last_time, delay_time, x_down_time, y_down_time;
	} km;

	struct MousePos {
		int16 x, y, w, h;
		MousePos() : x(0), y(0), w(0), h(0) {}
	};

	bool _mouseVisible;
	bool _mouseDrawn;
	byte *_mouseData;
	byte *_mouseBackup;
	MousePos _mouseCurState;
	MousePos _mouseOldState;
	int16 _mouseHotspotX;
	int16 _mouseHotspotY;

	// Shake mode
	int _currentShakePos;
	int _newShakePos;

	// Palette data
	SDL_Color *_currentPalette;
	uint _paletteDirtyStart, _paletteDirtyEnd;


	void add_dirty_rgn_auto(const byte *buf);
	void mk_checksums(const byte *buf);

	static void fill_sound(void *userdata, Uint8 * stream, int len);
	
	void add_dirty_rect(int x, int y, int w, int h);

	virtual void draw_mouse();
	virtual void undraw_mouse();

	virtual void load_gfx_mode() = 0;
	virtual void unload_gfx_mode() = 0;

	void setup_icon();
	void kbd_mouse();

	static OSystem_SDL_Common *create();
};


#endif
