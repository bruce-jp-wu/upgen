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

#ifndef PARSER_PROD_H__
#define PARSER_PROD_H__

#include <cassert>
#include <vector>
using std::vector;
#include "../common/types.h"
#include "../common/common.h"
#include "./parserfwd.h"
#include "./defines.h"

namespace parser_ns {

// production item class
struct prod_item_t {

public:
	// symbol ID
	int m_nSymbol;
	struct {
		// precedence
		int m_nPrec: 	28;
		// associativity
		int m_nAssoc:	4;
	};

public:
	inline prod_item_t(void)
	: m_nSymbol(INVALID_INDEX)
	, m_nPrec(DEFAULT_PREC)
	, m_nAssoc(ASSOC_UNKNOWN) {
	}
	inline prod_item_t(int a_nSym, int a_nPrec = DEFAULT_PREC, int a_nAssoc = ASSOC_UNKNOWN)
	: m_nSymbol(a_nSym)
	, m_nPrec(a_nPrec)
	, m_nAssoc(a_nAssoc) {
	}
	inline prod_item_t(const prod_item_t &src)
	: m_nSymbol(src.m_nSymbol)
	, m_nPrec(src.m_nPrec)
	, m_nAssoc(src.m_nAssoc) {
	}
	inline prod_item_t& operator = (const prod_item_t &src) {
		if(&src != this) {
			m_nSymbol = src.m_nSymbol;
			m_nPrec = src.m_nPrec;
			m_nAssoc = src.m_nAssoc;
		}
		return *this;
	}
	inline bool operator == (const prod_item_t &src) const {
		
		return _EQ(m_nSymbol, src.m_nSymbol)
				&& _EQ(m_nPrec, src.m_nPrec)
				&& _EQ(m_nAssoc, src.m_nAssoc);
	}
};

typedef vector<prod_item_t> vpitem_t;

// production class
class prod_t {

	friend class grammar_t;
	
private:
	// production ID
	int m_nID;
	// left symbol in production
	prod_item_t m_riLeft;
	// production precedence
	int m_nPrec;
	// right symbols in production
	vpitem_t m_vnRight;
	// parse-action attached
	int m_nActionIndex;

public:
	inline prod_t(int a_nID)
	: m_nID(a_nID)
	, m_riLeft(INVALID_INDEX)
	, m_nPrec(0)
	, m_nActionIndex(INVALID_INDEX) {
	}
	
	inline int getID(void) const {
		return m_nID;
	}
	inline void setID(int a_nID) {
		assert(a_nID >= 0);
		m_nID = a_nID;
	}
	inline int getLeft(void) const {
		return m_riLeft.m_nSymbol;
	}
	inline void setLeft(int a_nIdx) {
		assert(a_nIdx >= 0);
		m_riLeft.m_nSymbol = a_nIdx;
	}
	inline int getPrec(void) const {
		return m_nPrec;
	}
	inline void setPrec(int a_nPrec) {
		m_nPrec = a_nPrec;
	}
	inline prod_item_t* getRightItem(int a_nIdx) {
		assert(a_nIdx >= -1 && a_nIdx < (int)m_vnRight.size());
		if(a_nIdx < 0) {
			return &m_riLeft;
		}
		return &m_vnRight[a_nIdx];
	}
	inline const prod_item_t* getRightItem(int a_nIdx) const {
		assert(a_nIdx >= -1 && a_nIdx < (int)m_vnRight.size());
		if(a_nIdx < 0) {
			return &m_riLeft;
		}
		return &m_vnRight[a_nIdx];
	}
	inline int addRight(int a_nSymb, int a_nPrec = DEFAULT_PREC, int a_nAssoc = ASSOC_UNKNOWN) {
		//assert(a_nSymb >= 0);
		m_vnRight.push_back(prod_item_t(a_nSymb, a_nPrec, a_nAssoc));
		return (int)m_vnRight.size() - 1;
	}
	inline int getRightSymbol(int a_nIdx) const {
		assert(a_nIdx >= -1 && a_nIdx < (int)m_vnRight.size());
		
		if(a_nIdx < 0) {
			return m_riLeft.m_nSymbol;
		}
		return m_vnRight[a_nIdx].m_nSymbol;
	}
	inline void setRightSymbol(int a_nIdx, int a_nSymb
//			, int a_nPrec = DEFAULT_PREC, int a_nAssoc = ASSOC_UNKNOWN
			) {
		assert(a_nIdx >= -1 && a_nIdx < (int)m_vnRight.size() && a_nSymb >= 0);
		if(a_nIdx < 0) {
			m_riLeft.m_nSymbol = a_nSymb;
		}
		else {
			m_vnRight[a_nIdx].m_nSymbol = a_nSymb;
		}
	}
	inline int getActionIndex(void) const {
		return m_nActionIndex;
	}
	inline void setActionIndex(int a_nIdx) {
		m_nActionIndex = a_nIdx;
	}
	inline int getRightSize(void) const {
		return (int)m_vnRight.size();
	}
};

typedef vector<prod_t*> vprod_t;

}
#endif // PARSER_PROD_H__
