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

#ifndef LEXER_DGARC_H__
#define LEXER_DGARC_H__

#include <iosfwd>
using std::ostream;
#include <vector>
using std::vector;

#include "../common/uset.h"
using common_ns::charset_t;

#include "./dfafwd.h"

namespace lexer_ns {

/////////////////////////////////////////////////////////////////////////////////
// DFA graph arc definition
class dgraph_arc_t {    

	friend class dgraph_t;
	
private:
	
    int m_nfromState;
    int m_ntoState;
    charset_t m_chSet;
	
private:
	
	inline dgraph_arc_t(int a_nFrom, int a_nTo)
	: m_nfromState(a_nFrom)
	, m_ntoState(a_nTo)
	, m_chSet() {
		
	}
	
	inline dgraph_arc_t(int a_nFrom, int a_nTo, const charset_t &src)
	: m_nfromState(a_nFrom)
	, m_ntoState(a_nTo)
	, m_chSet(src) {
		
	}
	
public:
	
	inline int getFromState(void) const {
		
		return m_nfromState;
	}
	inline void setFromState(int a_nIdx) {
		
		m_nfromState = a_nIdx;
	}
	
	inline int getToState(void) const {
		
		return m_ntoState;
	}
	
	inline void setToState(int a_nIdx) {
		
		m_ntoState = a_nIdx;
	}
	
	inline charset_t& getCharSet(void) {
		
		return m_chSet;
	}
	
	inline const charset_t& getCharSet(void) const {
		
		return m_chSet;
	}
	
	friend ostream& operator<<(ostream& os, const dgraph_arc_t& src);
};
	// graph arc definition
////////////////////////////////////////////////////////////////////////////////////

typedef vector<dgraph_arc_t* > vgarc_t;
typedef vgarc_t::iterator vgarc_it_t;
}

#endif //LEXER_DGARC_H__
