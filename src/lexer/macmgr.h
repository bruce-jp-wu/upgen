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

#ifndef LEXER_MACMGR_H__
#define LEXER_MACMGR_H__

#include "../common/common.h"
#include "../common/types.h"
#include "../common/reporter.h"

#include "./retree.h"

namespace lexer_ns {

// RE macro manager class.
// RE macro is a RE with a name,
// we use RE macro just for convenience during design scanner
class mac_mgr_t {

private:
	
	s2i_map_t m_siMacroIdx;
	rnpool_t m_rMacroNode;
	vint_t m_viMacroRef;

public:
	// RE macro methods
	inline bool add(const string &a_strMName, renode_t *a_pMRoot) {
		
		pair<s2i_it_t, bool > ret = m_siMacroIdx.insert(
				s2i_pair_t(a_strMName, m_rMacroNode.size()));
		
		if(!ret.second) {
			// Macro name duplicated
			string strMsg = "Macro name duplicated: ";
			strMsg += a_strMName;
			_WARNING(strMsg);
			
			return false;
		}
		
		m_rMacroNode.push_back(a_pMRoot);
		m_viMacroRef.push_back(0);
		
		return true;
	}
	
	// get root of RE tree by its name
	inline bool find(const string &a_strMName, renode_t*& a_rpMroot) {
		
		s2i_it_t it = m_siMacroIdx.find(a_strMName);
		
		if(_EQ(m_siMacroIdx.end(), it)) {
			
			return false;
		}
		
		assert(it->second >= 0 && it->second < (int)m_rMacroNode.size());
		
		a_rpMroot = m_rMacroNode[it->second];
		
		return true;
	}
	
	// use referrence counter in order to decide if its RE tree need to clone
	// when RE macro is referred.
	inline int addRef(const string &a_strMName) {
		
		s2i_it_t it = m_siMacroIdx.find(a_strMName);
		
		if(_EQ(m_siMacroIdx.end(), it)) {
			
			return 0;
		}
		assert(it->second >= 0 && it->second < (int)m_rMacroNode.size());
				
		++m_viMacroRef[it->second];
		
		return m_viMacroRef[it->second] - 1;
	}
	
	inline renode_t* getRoot(const string &a_strMName) {
		
		s2i_it_t it = m_siMacroIdx.find(a_strMName);
		
		if(_EQ(m_siMacroIdx.end(), it)) {
			
			return nullptr;
		}
		
		assert(it->second >= 0 && it->second < (int)m_rMacroNode.size());
		
		return m_rMacroNode[it->second];
	}

	inline void clear(void) {
		
		m_siMacroIdx.clear();
		m_rMacroNode.clear();
		m_viMacroRef.clear();
	}

};

}

#endif //LEXER_MACMGR_H__
