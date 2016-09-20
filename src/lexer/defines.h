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

#ifndef LEXER_INDEX_H__
#define LEXER_INDEX_H__


#define FIRST_CHAR			0
#define FIRST_META			0

#define INVALID_CHAR		(FIRST_CHAR - 1)
#define INVALID_META		(FIRST_META - 1)

#define WILD_STATE			(FIRST_STATE - 2)

#define MIN_SIZE_TO_MINIMIZE	10
#define MAX_SIZE_TO_MINIMIZE	1200
#define MIN_TOTAL_TO_COMPRESS	2
#define MIN_DENSE_TO_COMPRESS	1

#endif //LEXER_INDEX_H__
