/* ScummVM - Scumm Interpreter
 * Copyright (C) 2001  Ludvig Strigeus
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

#include "stdafx.h"
#include "scumm.h"

void Scumm_v4::loadCharset(int no)
{
	uint32 size;
	memset(_charsetData, 0, sizeof(_charsetData));

	checkRange(4, 0, no, "Loading illegal charset %d");
	openRoom(-1);

	openRoom(900 + no);

	size = fileReadDwordLE() + 11;

	fileRead(_fileHandle, createResource(6, no, size), size);
	openRoom(-1);
}
