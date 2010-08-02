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
 * $URL$
 * $Id$
 *
 */

#include "sci/engine/kernel.h"
#include "sci/engine/state.h"
#include "sci/engine/vm.h"
#include "sci/engine/workarounds.h"

#define SCI_WORKAROUNDENTRY_TERMINATOR { (SciGameId)0, -1, -1, 0, NULL, NULL, -1, 0, { WORKAROUND_NONE, 0 } }

namespace Sci {

//    gameID,           room,script,lvl,          object-name, method-name,    call,index,             workaround
const SciWorkaroundEntry opcodeDivWorkarounds[] = {
	{ GID_QFG1VGA,       301,   928,  0,              "Blink", "init",           -1,    0, { WORKAROUND_FAKE,   0 } }, // when entering inn, gets called with 1 parameter, but 2nd parameter is used for div which happens to be an object
	SCI_WORKAROUNDENTRY_TERMINATOR
};

//    gameID,           room,script,lvl,          object-name, method-name,    call,index,             workaround
const SciWorkaroundEntry opcodeOrWorkarounds[] = {
	{ GID_ECOQUEST2,       100,   0,  0,               "Rain", "points",      0xcc6,    0, { WORKAROUND_FAKE,   0 } }, // when giving the papers to the customs officer, gets called against a pointer instead of a number - bug #3034464
	SCI_WORKAROUNDENTRY_TERMINATOR
};

//    gameID,           room,script,lvl,          object-name, method-name,    call, index,            workaround
const SciWorkaroundEntry opcodeDptoaWorkarounds[] = {
	{ GID_LSL6,           360,  938,  0,               "ROsc", "cycleDone",      -1,    0, { WORKAROUND_FAKE,   1 } }, // when looking through tile in the shower room initial cycles get set to an object instead of 2, we fix this by setting 1 after decrease
	{ GID_LSL6HIRES,      360,64938,  0,               "ROsc", "cycleDone",      -1,    0, { WORKAROUND_FAKE,   1 } }, // when looking through tile in the shower room initial cycles get set to an object instead of 2, we fix this by setting 1 after decrease
	{ GID_SQ5,            200,  939,  0,                "Osc", "cycleDone",      -1,    0, { WORKAROUND_FAKE,   1 } }, // when going back to bridge the crew is goofing off, we get an object as cycle count
	SCI_WORKAROUNDENTRY_TERMINATOR
};

//    gameID,           room,script,lvl,          object-name, method-name,    call,index,  workaround
const SciWorkaroundEntry uninitializedReadWorkarounds[] = {
	{ GID_CNICK_KQ,      200,     0,  1,          "Character", "<noname 446>",   -1,  504, { WORKAROUND_FAKE,   0 } }, // checkers, like in hoyle 3
	{ GID_CNICK_KQ,      200,     0,  1,          "Character", "<noname 446>",   -1,  505, { WORKAROUND_FAKE,   0 } }, // checkers, like in hoyle 3
	{ GID_CNICK_KQ,       -1,   700,  0,           "gcWindow", "<noname 183>",   -1,   -1, { WORKAROUND_FAKE,   0 } }, // when entering control menu, like in hoyle 3
	{ GID_CNICK_LONGBOW,   0,     0,  0,          "RH Budget", "<noname 110>",   -1,    1, { WORKAROUND_FAKE,   0 } }, // when starting the game
    { GID_ECOQUEST,       -1,    -1,  0,                 NULL, "doVerb",         -1,    0, { WORKAROUND_FAKE,   0 } }, // almost clicking anywhere triggers this in almost all rooms
	{ GID_FREDDYPHARKAS,  -1,    24,  0,              "gcWin", "open",           -1,    5, { WORKAROUND_FAKE, 0xf } }, // is used as priority for game menu
	{ GID_FREDDYPHARKAS,  -1,    31,  0,            "quitWin", "open",           -1,    5, { WORKAROUND_FAKE, 0xf } }, // is used as priority for game menu
	{ GID_GK1,            -1, 64950,  1,            "Feature", "handleEvent",    -1,    0, { WORKAROUND_FAKE,   0 } }, // sometimes when walk-clicking
	{ GID_GK2,            -1,    11,  0,                   "", "export 10",      -1,    3, { WORKAROUND_FAKE,   0 } }, // called when the game starts
	{ GID_GK2,            -1,    11,  0,                   "", "export 10",      -1,    4, { WORKAROUND_FAKE,   0 } }, // called during the game
	{ GID_HOYLE1,          4,   104,  0,   "GinRummyCardList", "calcRuns",       -1,    4, { WORKAROUND_FAKE,   0 } }, // Gin Rummy / right when the game starts
	{ GID_HOYLE1,          5,   204,  0,            "tableau", "checkRuns",      -1,    2, { WORKAROUND_FAKE,   0 } }, // Cribbage / during the game
	{ GID_HOYLE3,         -1,     0,  1,          "Character", "say",            -1,  504, { WORKAROUND_FAKE,   0 } }, // when starting checkers or dominoes, first time a character says something
	{ GID_HOYLE3,         -1,     0,  1,          "Character", "say",            -1,  505, { WORKAROUND_FAKE,   0 } }, // when starting checkers or dominoes, first time a character says something
	{ GID_HOYLE3,         -1,   700,  0,           "gcWindow", "open",           -1,   -1, { WORKAROUND_FAKE,   0 } }, // when entering control menu
	{ GID_ISLANDBRAIN,   140,   140,  0,              "piece", "init",           -1,    3, { WORKAROUND_FAKE,   1 } }, // first puzzle right at the start, some initialization variable. bnt is done on it, and it should be non-0
	{ GID_ISLANDBRAIN,   200,   268,  0,          "anElement", "select",         -1,    0, { WORKAROUND_FAKE,   0 } }, // elements puzzle, gets used before super TextIcon
	{ GID_JONES,           1,   232,  0,        "weekendText", "draw",        0x3d3,    0, { WORKAROUND_FAKE,   0 } }, // jones/cd only - gets called during the game
	{ GID_JONES,           1,   255,  0,                   "", "export 0",       -1,   13, { WORKAROUND_FAKE,   0 } }, // jones/cd only - called when a game ends
	{ GID_JONES,           1,   255,  0,                   "", "export 0",       -1,   14, { WORKAROUND_FAKE,   0 } }, // jones/cd only - called when a game ends
	{ GID_JONES,         764,   255,  0,                   "", "export 0",       -1,   13, { WORKAROUND_FAKE,   0 } }, // jones/ega&vga only - called when the game starts
	{ GID_JONES,         764,   255,  0,                   "", "export 0",       -1,   14, { WORKAROUND_FAKE,   0 } }, // jones/ega&vga only - called when the game starts
	{ GID_KQ5,            -1,     0,  0,                   "", "export 29",      -1,    3, { WORKAROUND_FAKE,   0 } }, // called when playing harp for the harpies or when aborting dialog in toy shop, is used for kDoAudio - bug #3034700
	{ GID_KQ5,            25,    25,  0,              "rm025", "doit",           -1,    0, { WORKAROUND_FAKE,   0 } }, // inside witch forest, when going to the room where the walking rock is
	{ GID_KQ6,            -1,    30,  0,               "rats", "changeState",    -1,   -1, { WORKAROUND_FAKE,   0 } }, // rats in the catacombs (temps 1 - 5) - bugs #3034597, #3035495, #3035824
	{ GID_KQ6,           210,   210,  0,              "rm210", "scriptCheck",    -1,    0, { WORKAROUND_FAKE,   1 } }, // using inventory in that room - bug #3034565
	{ GID_KQ6,           500,   500,  0,              "rm500", "init",           -1,    0, { WORKAROUND_FAKE,   0 } }, // going to island of the beast
	{ GID_KQ6,           520,   520,  0,              "rm520", "init",           -1,    0, { WORKAROUND_FAKE,   0 } }, // going to boiling water trap on beast isle
	{ GID_KQ6,            -1,   903,  0,         "controlWin", "open",           -1,    4, { WORKAROUND_FAKE,   0 } }, // when opening the controls window (save, load etc)
	{ GID_KQ7,            30, 64996,  0,               "User", "handleEvent",    -1,    1, { WORKAROUND_FAKE,   0 } }, // called when pushing a keyboard key
	{ GID_LAURABOW,       44,   967,  0,             "myIcon", "cycle",          -1,    1, { WORKAROUND_FAKE,   0 } }, // second dialog box after the intro, when talking with Lillian - bug #3034985
	{ GID_LAURABOW2,      -1,    24,  0,              "gcWin", "open",           -1,    5, { WORKAROUND_FAKE, 0xf } }, // is used as priority for game menu
	{ GID_LAURABOW2,      -1,    21,  0,      "dropCluesCode", "doit",           -1,    1, { WORKAROUND_FAKE,   0 } }, // when asking some questions (e.g. the reporter about the burglary, or the policeman about Ziggy) - bugs #3035068, #3036274
	{ GID_LAURABOW2,     240,   240,  0,     "sSteveAnimates", "changeState",    -1,    0, { WORKAROUND_FAKE,   0 } }, // Steve Dorian's idle animation at the docks - bug #3036291
	{ GID_LONGBOW,        -1,   213,  0,              "clear", "handleEvent",    -1,    0, { WORKAROUND_FAKE,   0 } }, // When giving an aswer using the druid hand sign code in any room
	{ GID_LONGBOW,        -1,   213,  0,             "letter", "handleEvent",  0xa8,    1, { WORKAROUND_FAKE,   0 } }, // When using the druid hand sign code in any room - bug #3036601
	{ GID_LSL1,          250,   250,  0,           "increase", "handleEvent",    -1,    2, { WORKAROUND_FAKE,   0 } }, // casino, playing game, increasing bet
	{ GID_LSL1,          720,   720,  0,              "rm720", "init",           -1,    0, { WORKAROUND_FAKE,   0 } }, // age check room
	{ GID_LSL2,           38,    38,  0,        "cloudScript", "changeState",    -1,    1, { WORKAROUND_FAKE,   0 } }, // entering the room in the middle deck of the ship - bug #3036483
	{ GID_LSL3,          340,   340,  0,        "ComicScript", "changeState",    -1,   -1, { WORKAROUND_FAKE,   0 } }, // right after entering the 3 ethnic groups inside comedy club (temps 200, 201, 202, 203)
	{ GID_LSL3,           -1,   997,  0,         "TheMenuBar", "handleEvent",    -1,    1, { WORKAROUND_FAKE, 0xf } }, // when setting volume the first time, this temp is used to set volume on entry (normally it would have been initialized to 's')
	{ GID_LSL6,           -1,    85,  0,          "washcloth", "doVerb",         -1,    0, { WORKAROUND_FAKE,   0 } }, // washcloth in inventory
	{ GID_LSL6,           -1,   928, -1,           "Narrator", "startText",      -1,    0, { WORKAROUND_FAKE,   0 } }, // used by various objects that are even translated in foreign versions, that's why we use the base-class
	{ GID_LSL6HIRES,       0,    85,  0,             "LL6Inv", "init",           -1,    0, { WORKAROUND_FAKE,   0 } }, // on startup
	{ GID_LSL6HIRES,      -1, 64950,  1,            "Feature", "handleEvent",    -1,    0, { WORKAROUND_FAKE,   0 } }, // at least when entering swimming pool area
	{ GID_LSL6HIRES,      -1, 64964,  0,              "DPath", "init",           -1,    1, { WORKAROUND_FAKE,   0 } }, // during the game
	{ GID_MOTHERGOOSE,    18,   992,  0,             "AIPath", "init",           -1,    0, { WORKAROUND_FAKE,   0 } }, // DEMO: Called when walking north from mother goose's house two screens
	{ GID_MOTHERGOOSEHIRES,-1,64950,  1,            "Feature", "handleEvent",    -1,    0, { WORKAROUND_FAKE,   0 } }, // right when clicking on a child at the start and probably also later
	{ GID_MOTHERGOOSEHIRES,-1,64950,  1,               "View", "handleEvent",    -1,    0, { WORKAROUND_FAKE,   0 } }, // see above
	{ GID_QFG2,           -1,    71,  0,        "theInvSheet", "doit",           -1,    1, { WORKAROUND_FAKE,   0 } }, // accessing the inventory
	{ GID_QFG2,           -1,   701,  0,              "Alley", "at",             -1,    0, { WORKAROUND_FAKE,   0 } }, // when walking inside the alleys in the town - bug #3035835
	{ GID_QFG3,          330,   330,  0,        "rajahTeller", "doChild",        -1,    0, { WORKAROUND_FAKE,   0 } }, // when talking to King Rajah about "Tarna"
	{ GID_QFG3,          330,   330,  0,        "rajahTeller", "doChild",        -1,    1, { WORKAROUND_FAKE,   0 } }, // when talking to King Rajah about "Rajah" - bug #3036390
	{ GID_SQ1,           103,   103,  0,               "hand", "internalEvent",  -1,    1, { WORKAROUND_FAKE,   0 } }, // spanish (and maybe early versions?) only: when moving cursor over input pad
	{ GID_SQ1,           103,   103,  0,               "hand", "internalEvent",  -1,    2, { WORKAROUND_FAKE,   0 } }, // spanish (and maybe early versions?) only: when moving cursor over input pad
	{ GID_SQ1,            -1,   703,  0,                   "", "export 1",       -1,    0, { WORKAROUND_FAKE,   0 } }, // sub that's called from several objects while on sarien battle cruiser
	{ GID_SQ1,            -1,   703,  0,         "firePulsar", "changeState", 0x18a,    0, { WORKAROUND_FAKE,   0 } }, // export 1, but called locally (when shooting at aliens)
	{ GID_SQ4,            -1,   398,  0,            "showBox", "changeState",    -1,    0, { WORKAROUND_FAKE,   0 } }, // sq4cd: called when rummaging in Software Excess bargain bin
	{ GID_SQ4,            -1,   928,  0,           "Narrator", "startText",      -1, 1000, { WORKAROUND_FAKE,   1 } }, // sq4cd: method returns this to the caller
	{ GID_SQ6,           100,     0,  0,                "SQ6", "init",           -1,    2, { WORKAROUND_FAKE,   0 } }, // called when the game starts
	{ GID_SQ6,           100, 64950,  0,               "View", "handleEvent",    -1,    0, { WORKAROUND_FAKE,   0 } }, // called when pressing "Start game" in the main menu
	{ GID_SQ6,            -1, 64964,  0,              "DPath", "init",           -1,    1, { WORKAROUND_FAKE,   0 } }, // during the game
	SCI_WORKAROUNDENTRY_TERMINATOR
};

//    gameID,           room,script,lvl,          object-name, method-name,    call,index,                workaround
const SciWorkaroundEntry kAbs_workarounds[] = {
	{ GID_HOYLE1,          1,     1,  0,              "room1", "doit",           -1,    0, { WORKAROUND_FAKE,  0x3e9 } }, // crazy eights - called with objects instead of integers
	{ GID_HOYLE1,          2,     2,  0,              "room2", "doit",           -1,    0, { WORKAROUND_FAKE,  0x3e9 } }, // old maid - called with objects instead of integers
	{ GID_HOYLE1,          3,     3,  0,              "room3", "doit",           -1,    0, { WORKAROUND_FAKE,  0x3e9 } }, // hearts - called with objects instead of integers
	SCI_WORKAROUNDENTRY_TERMINATOR
};

//    gameID,           room,script,lvl,          object-name, method-name,    call,index,                workaround
const SciWorkaroundEntry kCelHigh_workarounds[] = {
	{ GID_SQ1,             1,   255,  0,              "DIcon", "setSize",        -1,    0, { WORKAROUND_STILLCALL, 0 } }, // DEMO: Called with 2nd/3rd parameters as objects when clicking on the menu
	SCI_WORKAROUNDENTRY_TERMINATOR
};

//    gameID,           room,script,lvl,          object-name, method-name,    call,index,                workaround
const SciWorkaroundEntry kCelWide_workarounds[] = {
	{ GID_SQ1,             1,   255,  0,              "DIcon", "setSize",        -1,    0, { WORKAROUND_STILLCALL, 0 } }, // DEMO: Called with 2nd/3rd parameters as objects when clicking on the menu - bug #3035720
	SCI_WORKAROUNDENTRY_TERMINATOR
};

//    gameID,           room,script,lvl,          object-name, method-name,    call,index,                workaround
const SciWorkaroundEntry kDisplay_workarounds[] = {
	{ GID_ISLANDBRAIN,   300,   300,  0,           "geneDude", "show",           -1,    0, { WORKAROUND_IGNORE,    0 } }, // when looking at the gene explanation chart - a parameter is an object
	{ GID_SQ4,           391,   391,  0,          "doCatalog", "mode",         0x84,    0, { WORKAROUND_IGNORE,    0 } }, // clicking on catalog in roboter sale - a parameter is an object
	{ GID_SQ4,           391,   391,  0,         "choosePlug", "changeState",    -1,    0, { WORKAROUND_IGNORE,    0 } }, // ordering connector in roboter sale - a parameter is an object
	SCI_WORKAROUNDENTRY_TERMINATOR
};

//    gameID,           room,script,lvl,          object-name, method-name,    call,index,                workaround
const SciWorkaroundEntry kDisposeScript_workarounds[] = {
	{ GID_LAURABOW,      777,   777,  0,             "myStab", "changeState",    -1,    0, { WORKAROUND_IGNORE,    0 } }, // DEMO: after the will is signed, parameter 0 is an object - bug #3034907
	{ GID_QFG1,           -1,    64,  0,               "rm64", "dispose",        -1,    0, { WORKAROUND_IGNORE,    0 } }, // when leaving graveyard, parameter 0 is an object
	{ GID_SQ4,           150,   151,  0,        "fightScript", "dispose",        -1,    0, { WORKAROUND_IGNORE,    0 } }, // during fight with vohaul, parameter 0 is an object
	{ GID_SQ4,           150,   152,  0,       "driveCloseUp", "dispose",        -1,    0, { WORKAROUND_IGNORE,    0 } }, // when choosing "beam download", parameter 0 is an object
	SCI_WORKAROUNDENTRY_TERMINATOR
};

//    gameID,           room,script,lvl,          object-name, method-name,    call,index,                workaround
const SciWorkaroundEntry kDoSoundFade_workarounds[] = {
	{ GID_CAMELOT,        -1,   989,  0,            "rmMusic", "fade",           -1,    0, { WORKAROUND_IGNORE,    0 } }, // gets called frequently with a NULL reference (i.e. 0:0) - bug #3035149
	{ GID_KQ1,            -1,   989,  0,          "gameSound", "fade",           -1,    0, { WORKAROUND_IGNORE,    0 } }, // gets called in several scenes (e.g. graham cracker) with 0:0
	{ GID_KQ6,           105,   989,  0,        "globalSound", "fade",           -1,    0, { WORKAROUND_STILLCALL, 0 } }, // floppy: during intro, parameter 4 is an object
	{ GID_KQ6,           460,   989,  0,       "globalSound2", "fade",           -1,    0, { WORKAROUND_STILLCALL, 0 } }, // after pulling the black widow's web on the isle of wonder, parameter 4 is an object - bug #3034567
	SCI_WORKAROUNDENTRY_TERMINATOR
};

//    gameID,           room,script,lvl,          object-name, method-name,    call,index,                workaround
const SciWorkaroundEntry kGetAngle_workarounds[] = {
	{ GID_KQ6,           740,   752,  0,        "throwDazzle", "changeState",    -1,    0, { WORKAROUND_STILLCALL, 0 } }, // after the Genie is exposed in the Palace (short and long ending), it starts shooting lightning bolts around. An extra 5th parameter is passed - bug #3034610
	SCI_WORKAROUNDENTRY_TERMINATOR
};

//    gameID,           room,script,lvl,          object-name, method-name,    call,index,                workaround
const SciWorkaroundEntry kFindKey_workarounds[] = {
	{ GID_ECOQUEST2,     100,   999,  0,            "myList", "contains",        -1,    0, { WORKAROUND_FAKE, 0 } }, // When Noah Greene gives Adam the Ecorder, and just before the game gives a demonstration, a null reference to a list is passed - bug #3035186
	SCI_WORKAROUNDENTRY_TERMINATOR
};

//    gameID,           room,script,lvl,          object-name, method-name,    call,index,                workaround
const SciWorkaroundEntry kGraphDrawLine_workarounds[] = {
	{ GID_ISLANDBRAIN,   300,   300,  0,         "dudeViewer", "show",           -1,    0, { WORKAROUND_STILLCALL, 0 } }, // when looking at the gene explanation chart, gets called with 1 extra parameter
	{ GID_SQ1,            43,    43,  0,        "someoneDied", "changeState",    -1,    0, { WORKAROUND_STILLCALL, 0 } }, // when ordering beer, gets called with 1 extra parameter
	SCI_WORKAROUNDENTRY_TERMINATOR
};

//    gameID,           room,script,lvl,          object-name, method-name,    call,index,                workaround
const SciWorkaroundEntry kGraphSaveBox_workarounds[] = {
	{ GID_CASTLEBRAIN,   420,   427,  0,          "alienIcon", "select",         -1,    0, { WORKAROUND_STILLCALL, 0 } }, // when selecting a card during the alien card game, gets called with 1 extra parameter
	{ GID_ISLANDBRAIN,   290,   291,  0,         "upElevator", "changeState",0x201f,    0, { WORKAROUND_STILLCALL, 0 } }, // when testing in the elevator puzzle, gets called with 1 argument less - 15 is on stack - bug #3034485
	{ GID_ISLANDBRAIN,   290,   291,  0,       "downElevator", "changeState",0x201f,    0, { WORKAROUND_STILLCALL, 0 } }, // see above
	{ GID_ISLANDBRAIN,   290,   291,  0,    "correctElevator", "changeState",0x201f,    0, { WORKAROUND_STILLCALL, 0 } }, // see above (when testing the correct solution)
	SCI_WORKAROUNDENTRY_TERMINATOR
};

//    gameID,           room,script,lvl,          object-name, method-name,    call,index,                workaround
const SciWorkaroundEntry kGraphRestoreBox_workarounds[] = {
	{ GID_LSL6,           -1,    85,  0,          "rScroller", "hide",           -1,    0, { WORKAROUND_STILLCALL, 0 } }, // happens when restoring (sometimes), same as the one below
	{ GID_LSL6,           -1,    85,  0,          "lScroller", "hide",           -1,    0, { WORKAROUND_STILLCALL, 0 } }, // happens when restoring (sometimes), same as the one below
	{ GID_LSL6,           -1,    86,  0,             "LL6Inv", "show",           -1,    0, { WORKAROUND_STILLCALL, 0 } }, // happens when restoring, is called with hunk segment, but hunk is not allocated at that time
	// ^^ TODO: check, if this is really a script error or an issue with our restore code
	{ GID_LSL6,           -1,    86,  0,             "LL6Inv", "hide",           -1,    0, { WORKAROUND_STILLCALL, 0 } }, // happens during the game, gets called with 1 extra parameter
	SCI_WORKAROUNDENTRY_TERMINATOR
};

//    gameID,           room,script,lvl,          object-name, method-name,    call,index,                workaround
const SciWorkaroundEntry kGraphFillBoxForeground_workarounds[] = {
	{ GID_LSL6,           -1,     0,  0,               "LSL6", "hideControls",   -1,    0, { WORKAROUND_STILLCALL, 0 } }, // happens when giving the bungee key to merrily (room 240) and at least in room 650 too - gets called with additional 5th parameter
	SCI_WORKAROUNDENTRY_TERMINATOR
};

//    gameID,           room,script,lvl,          object-name, method-name,    call,index,                workaround
const SciWorkaroundEntry kGraphFillBoxAny_workarounds[] = {
	{ GID_SQ4,            -1,   818,  0,     "iconTextSwitch", "show",           -1,    0, { WORKAROUND_STILLCALL, 0 } }, // game menu "text/speech" display - parameter 5 is missing, but the right color number is on the stack
	SCI_WORKAROUNDENTRY_TERMINATOR
};

//    gameID,           room,script,lvl,          object-name, method-name,    call,index,                workaround
const SciWorkaroundEntry kGraphRedrawBox_workarounds[] = {
	{ GID_SQ4,           405,   405,  0,       "swimAfterEgo", "changeState",    -1,    0, { WORKAROUND_STILLCALL, 0 } }, // skateOrama when "swimming" in the air - accidental additional parameter specified
	{ GID_SQ4,           406,   406,  0,       "swimAndShoot", "changeState",    -1,    0, { WORKAROUND_STILLCALL, 0 } }, // skateOrama when "swimming" in the air - accidental additional parameter specified
	{ GID_SQ4,           410,   410,  0,       "swimAfterEgo", "changeState",    -1,    0, { WORKAROUND_STILLCALL, 0 } }, // skateOrama when "swimming" in the air - accidental additional parameter specified
	{ GID_SQ4,           411,   411,  0,       "swimAndShoot", "changeState",    -1,    0, { WORKAROUND_STILLCALL, 0 } }, // skateOrama when "swimming" in the air - accidental additional parameter specified
	{ GID_SQ4,            -1,   704,  0,           "shootEgo", "changeState",    -1,    0, { WORKAROUND_STILLCALL, 0 } }, // When shot by Droid in Super Computer Maze (Rooms 500, 505, 510...) - accidental additional parameter specified
	{ GID_KQ5,            -1,   981,  0,           "myWindow",     "dispose",    -1,    0, { WORKAROUND_STILLCALL, 0 } }, // Happens in the floppy version, when closing any dialog box, accidental additional parameter specified - bug #3036331
	{ GID_KQ5,            -1,   995,  0,               "invW",        "doit",    -1,    0, { WORKAROUND_STILLCALL, 0 } }, // Happens in the floppy version, when closing the inventory window, accidental additional parameter specified
	SCI_WORKAROUNDENTRY_TERMINATOR
};

//    gameID,           room,script,lvl,          object-name, method-name,    call,index,                workaround
const SciWorkaroundEntry kIsObject_workarounds[] = {
	{ GID_GK1,           50,   999,  0,                "List", "eachElementDo",  -1,    0, { WORKAROUND_FAKE, 0 } }, // GK1 demo, when asking Grace for messages it gets called with an invalid parameter (type "error") - bug #3034519
	{ GID_ISLANDBRAIN,   -1,   999,  0,                "List", "eachElementDo",  -1,    0, { WORKAROUND_FAKE, 0 } }, // when going to the game options, choosing "Info" and selecting anything from the list, gets called with an invalid parameter (type "error") - bug #3035262
	{ GID_QFG3,          -1,   999,  0,                "List", "eachElementDo",  -1,    0, { WORKAROUND_FAKE, 0 } }, // when asking for something, gets called with type error parameter
	SCI_WORKAROUNDENTRY_TERMINATOR
};

//    gameID,           room,script,lvl,          object-name, method-name,    call,index,                workaround
const SciWorkaroundEntry kMemory_workarounds[] = {
	{ GID_LAURABOW2,      -1,   999,  0,                   "", "export 6",       -1,    0, { WORKAROUND_FAKE,    0 } }, // during the intro, when exiting the train, talking to Mr. Augustini, etc. - bug #3034490
	SCI_WORKAROUNDENTRY_TERMINATOR
};

//    gameID,           room,script,lvl,          object-name, method-name,    call,index,                workaround
const SciWorkaroundEntry kNewWindow_workarounds[] = {
	{ GID_ECOQUEST,       -1,   981,  0,          "SysWindow", "<noname 178>",   -1,    0, { WORKAROUND_STILLCALL, 0 } }, // EcoQuest 1 demo uses an in-between interpreter from SCI1 to SCI1.1. It's SCI1.1, but uses the SCI1 semantics for this call - bug #3035057
	SCI_WORKAROUNDENTRY_TERMINATOR
};

//    gameID,           room,script,lvl,          object-name, method-name,    call,index,                workaround
const SciWorkaroundEntry kPaletteUnsetFlag_workarounds[] = {
	{ GID_QFG4,          100,   100,  0,            "doMovie", "<noname 144>",   -1,    0, { WORKAROUND_IGNORE,    0 } }, // after the Sierra logo, no flags are passed, thus the call is meaningless - bug #3034506
	SCI_WORKAROUNDENTRY_TERMINATOR
};

//    gameID,           room,script,lvl,          object-name, method-name,    call,index,                workaround
const SciWorkaroundEntry kSetPort_workarounds[] = {
	{ GID_LSL6,          740,   740,  0,              "rm740", "drawPic",        -1,    0, { WORKAROUND_IGNORE,    0 } }, // ending scene, is called with additional 3 (!) parameters
	SCI_WORKAROUNDENTRY_TERMINATOR
};

//    gameID,           room,script,lvl,          object-name, method-name,    call,index,                workaround
const SciWorkaroundEntry kUnLoad_workarounds[] = {
	{ GID_CAMELOT,       921,   921,  1,             "Script", "changeState",  0x36,    0, { WORKAROUND_IGNORE, 0 } }, // DEMO: While showing Camelot (and other places), the reference is invalid - bug #3035000
	{ GID_CAMELOT,       921,   921,  1,             "Script", "init",         0x36,    0, { WORKAROUND_IGNORE, 0 } }, // DEMO: When being attacked by the boar (and other places), the reference is invalid - bug #3035000
	{ GID_CASTLEBRAIN,   320,   377,  0,              "SWord", "upDate",         -1,    0, { WORKAROUND_IGNORE, 0 } }, // after solving the cross-word-puzzle, trying to unload invalid reference
	{ GID_CASTLEBRAIN,   320,   377,  0,            "theWord", "show",           -1,    0, { WORKAROUND_IGNORE, 0 } }, // 2nd word puzzle, when exiting before solving, trying to unload invalid reference - bug #3034473
    { GID_ECOQUEST,      380,    61,  0,              "gotIt", "changeState",    -1,    0, { WORKAROUND_IGNORE, 0 } }, // after talking to the dolphin the first time
	{ GID_LAURABOW2,       1,     1,  0,           "sCartoon", "changeState",    -1,    0, { WORKAROUND_IGNORE, 0 } }, // DEMO: during the intro, a 3rd parameter is passed by accident - bug #3034902
	{ GID_LAURABOW2,       2,     2,  0,           "sCartoon", "changeState",    -1,    0, { WORKAROUND_IGNORE, 0 } }, // DEMO: during the intro, a 3rd parameter is passed by accident - bug #3034902
	{ GID_LAURABOW2,       4,     4,  0,           "sCartoon", "changeState",    -1,    0, { WORKAROUND_IGNORE, 0 } }, // DEMO: inside the museum, a 3rd parameter is passed by accident - bug #3034902
	{ GID_LAURABOW2,       6,     6,  0,           "sCartoon", "changeState",    -1,    0, { WORKAROUND_IGNORE, 0 } }, // DEMO: after the murder, a 3rd parameter is passed by accident - bug #3034902
	{ GID_LAURABOW2,       7,     7,  0,           "sCartoon", "changeState",    -1,    0, { WORKAROUND_IGNORE, 0 } }, // DEMO: after the logo is shown, a 3rd parameter is passed by accident - bug #3034902
	{ GID_LSL6,          130,   130,  0,    "recruitLarryScr", "changeState",    -1,    0, { WORKAROUND_IGNORE, 0 } }, // during intro, a 3rd parameter is passed by accident
	{ GID_LSL6,          740,   740,  0,        "showCartoon", "changeState",    -1,    0, { WORKAROUND_IGNORE, 0 } }, // during ending, 4 additional parameters are passed by accident
	{ GID_LSL6HIRES,     130,   130,  0,    "recruitLarryScr", "changeState",    -1,    0, { WORKAROUND_IGNORE, 0 } }, // during intro, a 3rd parameter is passed by accident
	{ GID_SQ1,            43,   303,  0,            "slotGuy", "dispose",        -1,    0, { WORKAROUND_IGNORE, 0 } }, // when leaving ulence flats bar, parameter 1 is not passed - script error
	SCI_WORKAROUNDENTRY_TERMINATOR
};

SciWorkaroundSolution trackOriginAndFindWorkaround(int index, const SciWorkaroundEntry *workaroundList, SciTrackOriginReply *trackOrigin) {
	EngineState *state = g_sci->getEngineState();
	ExecStack *lastCall = state->xs;
	Script *local_script = state->_segMan->getScriptIfLoaded(lastCall->local_segment);
	int curScriptNr = local_script->getScriptNumber();

	if (lastCall->debugLocalCallOffset != -1) {
		// if lastcall was actually a local call search back for a real call
		Common::List<ExecStack>::iterator callIterator = state->_executionStack.end();
		while (callIterator != state->_executionStack.begin()) {
			callIterator--;
			ExecStack loopCall = *callIterator;
			if ((loopCall.debugSelector != -1) || (loopCall.debugExportId != -1)) {
				lastCall->debugSelector = loopCall.debugSelector;
				lastCall->debugExportId = loopCall.debugExportId;
				break;
			}
		}
	}

	Common::String curObjectName = state->_segMan->getObjectName(lastCall->sendp);
	Common::String curMethodName;
	const SciGameId gameId = g_sci->getGameId();
	const int curRoomNumber = state->currentRoomNumber();

	if (lastCall->type == EXEC_STACK_TYPE_CALL) {
		if (lastCall->debugSelector != -1) {
			curMethodName = g_sci->getKernel()->getSelectorName(lastCall->debugSelector);
		} else if (lastCall->debugExportId != -1) {
			curObjectName = "";
			curMethodName = curMethodName.printf("export %d", lastCall->debugExportId);
		}
	}

	if (workaroundList) {
		// Search if there is a workaround for this one
		const SciWorkaroundEntry *workaround;
		int16 inheritanceLevel = 0;
		Common::String searchObjectName = curObjectName;
		reg_t searchObject = lastCall->sendp;
		do {
			workaround = workaroundList;
			while (workaround->methodName) {
				if (workaround->gameId == gameId
						&& ((workaround->scriptNr == -1) || (workaround->scriptNr == curScriptNr))
						&& ((workaround->roomNr == -1) || (workaround->roomNr == curRoomNumber))
						&& ((workaround->inheritanceLevel == -1) || (workaround->inheritanceLevel == inheritanceLevel))
						&& ((workaround->objectName == NULL) || (workaround->objectName == searchObjectName))
						&& workaround->methodName == curMethodName && workaround->localCallOffset == lastCall->debugLocalCallOffset
						&& ((workaround->index == -1) || (workaround->index == index))) {
					// Workaround found
					return workaround->newValue;
				}
				workaround++;
			}

			// Go back to the parent
			inheritanceLevel++;
			searchObject = state->_segMan->getObject(searchObject)->getSuperClassSelector();
			if (!searchObject.isNull())
				searchObjectName = state->_segMan->getObjectName(searchObject);
		} while (!searchObject.isNull()); // no parent left?
	}

	// give caller origin data
	trackOrigin->objectName = curObjectName;
	trackOrigin->methodName = curMethodName;
	trackOrigin->scriptNr = curScriptNr;
	trackOrigin->localCallOffset = lastCall->debugLocalCallOffset;

	SciWorkaroundSolution noneFound;
	noneFound.type = WORKAROUND_NONE;
	noneFound.value = 0;
	return noneFound;
}

} // End of namespace Sci