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

#ifndef CODER_DMODEL_H__
#define CODER_DMODEL_H__

#include <string>
using std::string;
#include <map>
using std::map;
#include <iterator>
using std::iterator;
using std::pair;

#include <cstring>

#include "../common/types.h"
#include "../common/action.h"
using common_ns::action_t;
using common_ns::vaction_t;

// data model definition. data model is set of data independent of programming languages
namespace coder_ns {

enum {
	
	GROUP_DELIMITER = -1
};

class dmodel_t {

private:
	
	enum {
		NT_INT = 0,
		NT_BYTE,
		NT_PSTR,
		NT_ACTION
	};
	
private:
	
	struct {
		
		bool m_bOwn:	1;
		int m_nType:	5;
		int m_nCur:		29;
		int m_nSize:	29;
	};
	
	union {
		
		int* m_pvnVal;
		byte_t *m_pvbVal;
		string* m_pvstrVal;
		const action_t** m_ppactVal;
	};

public:
	
	inline ~dmodel_t(void) {
		
		switch(m_nType) {
		
		case NT_INT:
			delete[] m_pvnVal;
			break;
		case NT_BYTE:
			delete[] m_pvbVal;
			break;
		case NT_PSTR:
			delete[] m_pvstrVal;
			break;
		case NT_ACTION:
			delete[] m_ppactVal;
		default:
			assert(false);
			break;
		}
	}
	
	inline bool isInt(void) const {
		
		return NT_INT == m_nType;
	}
	
	inline bool isString(void) const {
		
		return NT_PSTR == m_nType;
	}
	
	inline bool isAction(void) const {
		
		return NT_ACTION == m_nType;
	}
	
	inline bool isEnd(void) const {
		
		return m_nCur == m_nSize;
	}
	
	inline bool isLast(void) const {
		
		return m_nCur == m_nSize - 1;
	}
	
	inline bool isMore(void) const {
		
		return m_nCur < m_nSize - 1;
	}
	
	inline bool isMod(int a_nElemsPerLine) const {
		
		assert(a_nElemsPerLine > 0);
		return (m_nCur + 1) % a_nElemsPerLine == 0;
	}
	
	inline bool isGroupFlag(void) const {
		
		assert(NT_INT == m_nType);
		return (m_nCur < m_nSize) && GROUP_DELIMITER == m_pvnVal[m_nCur];
	}
	
	inline bool isGroupFirst(void) const {
		
		assert(NT_INT == m_nType);
		return (m_nCur < m_nSize)
			&& (m_nCur == 0 || GROUP_DELIMITER == m_pvnVal[m_nCur - 1]);
	}
		
	inline int getIndex(void) const {
		
		return m_nCur;
	}
	
	inline void skip(void) {
		
		if(m_nCur < m_nSize) {
			
			++m_nCur;
		}
	}
	
	inline void goFirst(void) {
		
		m_nCur = 0;
	}
	
	inline int getSize(void) const {
		
		return m_nSize;
	}
	
	inline int nextInt(void) {
		
		assert(NT_INT == m_nType);
		m_nCur = m_nCur % m_nSize;
		
		return m_pvnVal[m_nCur++];
	}
	inline int nextByte(void) {
		
		assert(NT_BYTE == m_nType);
		m_nCur = m_nCur % m_nSize;
		
		return m_pvbVal[m_nCur++];
	}
	inline const string& nextString(void) {
		
		assert(NT_PSTR == m_nType);
		m_nCur = m_nCur % m_nSize;
		
		return m_pvstrVal[m_nCur++];
	}
	inline const action_t* nextAction(void) {
		assert(NT_ACTION == m_nType);
		m_nCur = m_nCur % m_nSize;
		return m_ppactVal[m_nCur++];
	}
	
public:
	
	inline int getInt(void) const {
		
		assert(NT_INT == m_nType);
		
		return m_pvnVal[m_nCur % m_nSize];
	}
	inline int getByte(void) const {
		
		assert(NT_BYTE == m_nType);
		return m_pvbVal[m_nCur % m_nSize];
	}
	inline const string& getString(void) const {
		
		assert(NT_PSTR == m_nType);
		
		return m_pvstrVal[m_nCur % m_nSize];
	}
	inline const action_t* getAction(void) const {
		
		assert(NT_ACTION == m_nType);
		return m_ppactVal[m_nCur % m_nSize];
	}
	
public:
	
	inline dmodel_t(int a_nVal)
	: m_nType(NT_INT)
	, m_nCur(0)
	, m_nSize(1)  {
		
		m_pvnVal = new int[1];
		m_pvnVal[0] = a_nVal;
	}
	inline dmodel_t(const string &a_strVal)
	: m_nType(NT_PSTR)
	, m_nCur(0)
	, m_nSize(1) {
		
		m_pvstrVal = new string[1];
		m_pvstrVal[0] = a_strVal;
	}
	
	dmodel_t(const vint_t &a_vnLst)
	: m_nType(NT_INT)
	, m_nCur(0)
	, m_nSize(a_vnLst.size()) {
		
		assert(m_nSize > 0);
		m_pvnVal = new int[m_nSize];
		for(int i = 0; i < m_nSize; ++i) {
			
			m_pvnVal[i] = a_vnLst[i];
		}
	}
	dmodel_t(const int *a_pnLst, int a_nSize)
	: m_nType(NT_INT)
	, m_nCur(0)
	, m_nSize(a_nSize) {
		
		assert(a_pnLst && a_nSize > 0);
		m_pvnVal = new int[m_nSize];
		for(int i = 0; i < m_nSize; ++i) {
			
			m_pvnVal[i] = a_pnLst[i];
		}
	}
	dmodel_t(const bit_vect_t &a_bitVect)
	: m_nType(NT_INT)
	, m_nCur(0)
	, m_nSize((int)a_bitVect.size()) {
		
		assert(m_nSize > 0);
		m_pvnVal = new int[m_nSize];
		for(int i = 0; i < m_nSize; ++i) {
			m_pvnVal[i] = a_bitVect[i];
		}
	}
	dmodel_t(const vstr_t &a_vstrLst)
	: m_nType(NT_PSTR)
	, m_nCur(0)
	, m_nSize((int)a_vstrLst.size()) {
		
		assert(m_nSize > 0);
		m_pvstrVal = new string[m_nSize];
		for(int i = 0; i < m_nSize; ++i) {
			
			m_pvstrVal[i] = a_vstrLst[i];
		}
	}
	dmodel_t(const vaction_t &a_va)
	: m_nType(NT_ACTION)
	, m_nCur(0)
	, m_nSize((int)a_va.size()) {
		
		assert(m_nSize > 0);
		m_ppactVal = new const action_t*[m_nSize];
		for(int i = 0; i < m_nSize; ++i) {
			
			m_ppactVal[i] = a_va[i];
		}
	}
	inline dmodel_t(const action_t *a_pact)
	: m_nType(NT_ACTION)
	, m_nCur(0)
	, m_nSize(1) {
		
		m_ppactVal = new const action_t*[1];
		m_ppactVal[0] = a_pact;
	}
};

typedef map < string, dmodel_t* > str2dm_map_t;
typedef str2dm_map_t::iterator str2dm_it_t;
typedef str2dm_map_t::const_iterator str2dm_cit_t;
typedef pair< string, dmodel_t* > str2dm_pair_t;
}

#endif // CODER_DMODEL_H__
