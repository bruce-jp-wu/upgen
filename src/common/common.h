/*
    Upgen -- a scanner and parser generator.
    Copyright (C) 2016  Bruce Wu
    
    This file is a part of Upgen program

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef COMMON_COMMON_H__
#define COMMON_COMMON_H__

#define _TEST

// in-coming parameter label
#define IN
// out-going parameter label
#define OUT
// label indicates labeled parameter is both in-coming and out-going
#define IO

// define macro to avoid mistakes we may make about operator ==, !=
#if defined _EQ
#undef _EQ
#endif
#define _EQ(x, y) (x == y)

//// unified representation for char and wchar_t
//#define _T(x)	x
#define LEX_ESCAPE_CHAR		'`'

#define FALSE				0
#define TRUE				1

#define INVALID_INDEX		-1
// psudo-infinite number
#define INF					-1


#define FIRST_STATE			1
#define FIRST_ACTION		0
#define FIRST_RULE			0
#define FIRST_LINENO		0

#define INVALID_STATE		(FIRST_STATE - 1)
#define INVALID_ACTION		(FIRST_ACTION - 1)
#define INVALID_RULE		(FIRST_RULE - 1)
#define INVALID_LINENO		(FIRST_LINENO - 1)

// special parameter for macros without any parameters
#define SKEY_EMPTY_PARAM		"@@"
// suffix for macros to distinguish different version of macros with same name
#define SKEY_PARAM_SUFFIX		"@"
// prefix of left symbol of midrule 
#define MRLEFT_PREFIX			"@@"

// extended name of file of detailed information
#define DETAILED_LOG_EXT		".detailed"

// special key for expanding SV(semantic value)
#define SKEY_SVNAME_LEFT			"$SVNAME_LEFT"
#define SKEY_SVNAME_RIGHT			"$SVNAME_RIGHT"
#define SKEY_SVLOC_LEFT				"$SVLOC_LEFT"
#define SKEY_SVLOC_RIGHT			"$SVLOC_RIGHT"

#define SKEY_SVNAME_DSTRCT			"$SVNAME_DSTRCT"
#define SKEY_SVLOC_DSTRCT			"$SVLOC_DSTRCT"

// error code
#define WRNID_BASE			1024
#define ERRID_BASE			2048
#define ERRID_CMD			ERRID_BASE
#define ERRID_LEX			(ERRID_BASE + 1024)
#define ERRID_SYN			(ERRID_BASE + 2048)
#define ERRID_SEM			(ERRID_BASE + 3072)
#define ERRID_OTH			(ERRID_BASE + 4096)
#define FERRID				(ERRID_BASE + 5120)

#define ECMD(X)				(ERRID_CMD + X)
#define ELEX(X)				(ERRID_LEX + X)
#define ESYN(X)				(ERRID_SYN + X)
#define ESEM(X)				(ERRID_SEM + X)
#define EOTH(X)				(ERRID_OTH + X)

#include <cassert>

#endif //COMMON_COMMON_H__
