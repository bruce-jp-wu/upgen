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

#ifndef LEXER_DFAFWD_H__
#define LEXER_DFAFWD_H__

#include <iosfwd>
using std::ostream;
#include "./defines.h"

namespace lexer_ns {

class dgraph_arc_t;
class dgraph_node_t;
class dgraph_t;
class dtable_t;

ostream& operator<<(ostream &os, const dgraph_node_t& src);
ostream& operator<<(ostream &os, const dgraph_t &src);
}

#endif // LEXER_DFAFWD_H__
