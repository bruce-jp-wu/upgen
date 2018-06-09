/*
    Upgen -- a scanner and parser generator.
    Copyright (C) 2009-2018 Bruce Wu
    
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

#ifndef PARSER_INDEX_H__
#define PARSER_INDEX_H__

#define INTERNAL_END_TOKEN   "$end"
#define INTERNAL_ERROR_TOKEN "$error"
#define EXTERNAL_ERROR_TOKEN "error"
#define INTERNAL_UNDEFINED_TOKEN "$undefined"
#define INTERNAL_ACCEPT_TYPE "$accept"

#define GRAMMAR_DEFAULT_SVT	"$untagged"
#define FAKE_SYMBOL			"$FAKE_SYMBOL"

#define HIGHEST_NICE		0x20

enum {
	INVALID_SYMBOL_INDEX2 = -3,
    INVALID_SYMBOL_INDEX = -2,
    EMPTY_SYMBOL_INDEX = -1,
    END_SYMBOL_INDEX = 0,
    ERROR_SYMBOL_INDEX = 1,
    UNDEFINED_SYMBOL_INDEX = 2
};
enum {
	DEFAULT_SVT_ID = -1,
	DEFAULT_PREC = 0,
    END_SYMBOL_ID = 0,
    ERROR_SYMBOL_ID = 256,
    UNDEFINED_SYMBOL_ID = 257,
	INIT_TOKEN_ID = 258,
};
enum {
    ASSOC_UNKNOWN = 0,
    ASSOC_LEFT,
    ASSOC_RIGHT,
    ASSOC_NO
};

enum {
	UNTAG_DSTRCT = -1,
	OTHER_DSTRCT = 0,
};

#endif // PARSER_INDEX_H__
