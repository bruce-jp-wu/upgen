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

#ifndef LEXER_SCMGR_H__
#define LEXER_SCMGR_H__

#include "../common/common.h"
#include "../common/types.h"
#include "../common/uset.h"
using common_ns::viset_t;

namespace lexer_ns {

enum {
	INVALID_SCINDEX = -1,
	INITIAL_INDEX = 0
};

	// start-conditions' manager 
class sc_mgr_t {
	
private:
	s2i_map_t m_siSCIdx;
	vstr_t m_vsSCName;
	vbool_t m_vbExclusive;
	viset_t m_visTreeSet;

public:
	
	sc_mgr_t(void) {
		
		addXSC("INITIAL");
	}
	
	~sc_mgr_t(void) {
		
		clear();
	}
	
	void clear(void) {
		
		for(int i = 0; i < (int)m_visTreeSet.size(); ++i) {
			
			if(m_visTreeSet[i]) {
			
				delete m_visTreeSet[i];
			}
		}
		
		m_siSCIdx.clear();
		m_vsSCName.clear();
		m_vbExclusive.clear();
		m_visTreeSet.clear();
		addXSC("INITIAL");
	}
	
private:
	
	inline bool addSC(const string &a_strSCName, bool a_bExclusive) {
		
		
		pair<s2i_it_t, bool > ret = m_siSCIdx.insert(
				s2i_pair_t(a_strSCName, m_vbExclusive.size()));
		
		if(!ret.second) {
			
			return false;
		}
		
		m_vsSCName.push_back(a_strSCName);
		m_vbExclusive.push_back(a_bExclusive);
		m_visTreeSet.push_back(nullptr);
		
		return true;		
	}
	
public:
	
	inline int getIdx(const string &a_strSCName) const {
		
		s2i_cit_t it = m_siSCIdx.find(a_strSCName);
		if(_EQ(m_siSCIdx.end(), it)) {
			
			return INVALID_INDEX;
		}
		
		return it->second;
	}
	
	inline int size(void) const {
		
		return (int)m_vbExclusive.size();
	}
	
	inline bool isExclusive(int a_nIdx) const {
		
		assert(a_nIdx >= 0 && a_nIdx < (int)m_vbExclusive.size());
		return m_vbExclusive[a_nIdx];
	}
	
	inline bool addISC(const string &a_strSCName) {

		return addSC(a_strSCName, false);
	}
	
	inline bool addXSC(const string &a_strSCName) {

		return addSC(a_strSCName, true);
	}
	
	inline bool addTree(int a_nIdx, int a_nTreeID) {
		
		assert(a_nIdx >= 0 && a_nIdx < (int)m_vbExclusive.size());
		assert(a_nIdx < (int)m_visTreeSet.size());
		
		if(!m_visTreeSet[a_nIdx]) {
			
			m_visTreeSet[a_nIdx] = new iset_t;
		}
		
		return m_visTreeSet[a_nIdx]->insert(a_nTreeID);
	}
	
	inline void getTrees(int a_nIdx, vint_t &a_viIdx) const {
		
		a_viIdx.clear();
		assert(a_nIdx >= 0 && a_nIdx < (int)m_vbExclusive.size());
		
		if(!m_visTreeSet[a_nIdx]) {
			
			return;
		}
		
		for(iset_t::const_iterator it = m_visTreeSet[a_nIdx]->begin(); it != m_visTreeSet[a_nIdx]->end(); ++it) {
			
			a_viIdx.push_back(*it);
		}
	}
	
	inline void getTrees(const string &a_strSCName, vint_t &a_viIdx) const {
		
		s2i_cit_t it = m_siSCIdx.find(a_strSCName);
		if(_EQ(m_siSCIdx.end(), it)) {
			
			a_viIdx.clear();
			return ;
		}
		
		getTrees(it->second, a_viIdx);
	}
	
	inline bool containTree(int a_nSCIdx, int a_nTreeIdx) const {
		
		assert(a_nSCIdx >= 0 && a_nSCIdx < (int)m_vbExclusive.size());
		assert(a_nTreeIdx >= 0);
		
		if(_EQ(nullptr, m_visTreeSet[a_nSCIdx])) {
			
			return false;
		}
		
		return m_visTreeSet[a_nSCIdx]->include(a_nTreeIdx);
	}
	
	inline string getName(int a_nIdx) const {
		
		assert(a_nIdx >= 0 && a_nIdx < (int)m_vbExclusive.size());
	
		return m_vsSCName[a_nIdx];
	}
};
	
}
	
#endif //LEXER_SCMGR_H__
