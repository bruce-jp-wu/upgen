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

#include <iomanip>
using std::setw;

#include <string>
using std::string;

#include <ostream>
using std::ostream;

#include "../common/uset.h"
using common_ns::charset_t;

#include "./dgarc.h"

namespace lexer_ns {

ostream& operator<<(ostream& os, const dgraph_arc_t& src) {
	
	ustring ustr;
	
	src.getCharSet().getSetChars(ustr);
	
	os << src.getToState() << " [";
	for(int i = 0; i < (int)ustr.size(); ++i) {
		
		os << ' ' << (int)(unsigned char)ustr[i];
	}
	os << ']';
	return os;
}

}
