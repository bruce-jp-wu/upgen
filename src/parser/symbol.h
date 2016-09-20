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

#ifndef PARSER_SYMBOL_H__
#define PARSER_SYMBOL_H__

#include <vector>
using std::vector;
#include "parserfwd.h"
#include "../common/uset.h"
using common_ns::iset_t;

namespace parser_ns {

class symbol_t {
	
	friend class grammar_t;
	
private:
	
	struct {
		// symbol unique ID
		int m_nID;
		// ID of symbol's semantic value type
		int m_nSVTID:		30;
		// indicates if the symbol is token or not
		bool m_bIsToken:	1;
		// indicates if the symbol is user-defined, which may be used for scanner
		bool m_bUserDefined:	1;
		int m_nDstrctID:	30;
		// indicates if the symbol is nullable or not
		bool m_bNullable:	1;
		// indicate if the symbol is used by some rules but not a part of rule
		bool m_bUsed:		1;
		
		union {
			// for token
			struct {
				int m_nTokenID;
				// operator precedence, when the token is treated as an operator
				int m_nPrec;
				// operator association type, when the token is treated as operator
				int m_nAssoc;
			};
			
			// for type
			struct {
				iset_t *m_pFirstSet;
				// RLS: Rules using the symbol as Left Symbol
				iset_t *m_pRLSSet;
			};
		};
	};
	
	// RCS: rules which contain the symbol
	iset_t *m_pRCSSet;
	
private:
	
	inline symbol_t(int a_nID, int a_nTokenID, int a_nSVT, int a_nPrec, int a_nAssoc, bool a_bUserDefined)
	: m_nID(a_nID)
	, m_nSVTID(a_nSVT)
	, m_bIsToken(true)
	, m_bUserDefined(a_bUserDefined)
	, m_nDstrctID(INVALID_INDEX)
	, m_bNullable(false)
	, m_bUsed(false)
	, m_nTokenID(a_nTokenID)
	, m_nPrec(a_nPrec)
	, m_nAssoc(a_nAssoc)
	, m_pRCSSet(nullptr) {
		
	}
	inline symbol_t(int a_nID, int a_nSVT, bool a_bUserDefined)
	: m_nID(a_nID)
	, m_nSVTID(a_nSVT)
	, m_bIsToken(false)
	, m_bUserDefined(a_bUserDefined)
	, m_nDstrctID(INVALID_INDEX)
	, m_bNullable(false)
	, m_bUsed(false)
	, m_pFirstSet(nullptr)
	, m_pRLSSet(nullptr)
	, m_pRCSSet(nullptr) {
		
	}
	
public:
	
	~symbol_t(void) {
	
		reset();
	}
	inline void reset(void) {
		
		if(!m_bIsToken) {
			
			if(m_pFirstSet) {
				delete m_pFirstSet;
				m_pFirstSet = nullptr;
			}
			if(m_pRLSSet) {
				delete m_pRLSSet;
				m_pRLSSet = nullptr;
			}
		}
		
		if(m_pRCSSet) {
			
			delete m_pRCSSet;
			m_pRCSSet = nullptr;
		}
	}
	
	inline int getID(void) const {
		return m_nID;
	}
	inline void setID(int a_nID) {
		assert(a_nID >= 0);
		m_nID = a_nID;
	}
	inline int getSVTID(void) const {
		return m_nSVTID;
	}
	inline void setSVTID(int a_nSVTID) {
		m_nSVTID = a_nSVTID;
	}
	inline int getDstrctID(void) const {
		return m_nDstrctID;
	}
	inline void setDstrctID(int a_nDstrctID) {
		m_nDstrctID = a_nDstrctID;
	}
	inline bool isToken(void) const {
		return m_bIsToken;
	}
	inline bool isType(void) const {
		return ! m_bIsToken;
	}
	inline void setToken(bool a_bIsToken) {
		m_bIsToken = a_bIsToken;
	}
	inline bool isUserDefined(void) const {
		return m_bUserDefined;
	}
	inline void setUserDefined(bool a_bUserDefined) {
		m_bUserDefined = a_bUserDefined;
	}
	inline bool isNullable(void) const {
		return m_bNullable;
	}
	inline void setNullable(bool a_bNull) {
		m_bNullable = a_bNull;
	}
	inline bool getUsed(void) const {
		return m_bUsed;
	}
	inline void setUsed(bool a_bUsed) {
		m_bUsed = a_bUsed;
	}
	inline int getTokenID(void) const {
		assert(m_bIsToken);
		return m_nTokenID;
	}
	inline void setTokenID(int a_nTokenID) {
		assert(m_bIsToken);
		m_nTokenID = a_nTokenID;
	}
	inline int getPrec(void) const {
		assert(m_bIsToken);
		return m_nPrec;
	}
	inline void setPrec(int a_nPrec) {
		assert(m_bIsToken);
		m_nPrec = a_nPrec;
	}
	inline int getAssoc(void) const {
		assert(m_bIsToken);
		return m_nAssoc;
	}
	inline void setAssoc(int a_nAssoc) {
		
		assert(m_bIsToken);
		m_nAssoc = a_nAssoc;
	}
	inline iset_t* getFirstSet(void) {
		assert( ! m_bIsToken);
		return m_pFirstSet;
	}
	inline const iset_t* getFirstSet(void) const {
		assert( ! m_bIsToken);
		return m_pFirstSet;
	}

};

typedef vector <symbol_t * >  vsymbol_t;

}

#endif // PARSER_SYMBOL_H__
