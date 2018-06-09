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

#ifndef PARSER_LALRTRIPLE_H__
#define PARSER_LALRTRIPLE_H__

#include "../common/types.h"

namespace parser_ns{

// triple <left-symbol, first-right-symbol, rule-id>
class lalrtrp_t {
	
private:
	
	mutable sint_t *m_psi;
	sint_t m_siEmpty;
	i2si_map_t m_left2Firsts;
	i2si_map_t m_first2Rules;
	
public:
	
	typedef sint_it_t iterator;
	typedef sint_cit_t const_iterator;
public:
	
	inline lalrtrp_t(void) {
		m_psi = &m_siEmpty;
	}
	inline ~lalrtrp_t(void) {
		clear();
	}
	void clear(void) {
		
		m_psi = &m_siEmpty;
		i2si_it_t it;
		for(it = m_left2Firsts.begin(); it != m_left2Firsts.end(); ++it) {
			delete it->second;
		}
		m_left2Firsts.clear();
		for(it = m_first2Rules.begin(); it != m_first2Rules.end(); ++it) {
			delete it->second;
		}
		m_first2Rules.clear();
	}
	inline int size() const {
		return (int)m_left2Firsts.size();
	}
	
	inline bool addTriple(int a_nLeft, int a_nFirst, int a_nRule) {
		
		bool bRet = false;
		pair<i2si_it_t, bool> pairRet = m_left2Firsts.insert(i2si_pair_t(a_nLeft, nullptr));
		
		if( ! pairRet.second) {
			bRet = pairRet.first->second->insert(a_nFirst).second;
		}
		else {
			bRet = true;
			pairRet.first->second = new sint_t;
			pairRet.first->second->insert(a_nFirst);			
		}
		
		pair<i2si_it_t, bool> pairRet2 = m_first2Rules.insert(i2si_pair_t(a_nFirst, nullptr));
		if( ! pairRet2.second) {
			bRet = pairRet2.first->second->insert(a_nRule).second || bRet;
		}
		else {
			bRet = true;
			pairRet2.first->second = new sint_t;
			pairRet2.first->second->insert(a_nRule);
		}

		return bRet;
	}
	
	inline bool addTriples(int a_nDestLeft, int a_nSrcLeft) {
		
		i2si_cit_t citSrc = m_left2Firsts.find(a_nSrcLeft);
		if(citSrc != m_left2Firsts.end()) {
			
			i2si_it_t itDest = m_left2Firsts.find(a_nDestLeft);
			if(itDest != m_left2Firsts.end()) {
				
				unsigned int nOldCnt = itDest->second->size();
				itDest->second->insert(citSrc->second->begin(), citSrc->second->end());
				return itDest->second->size() > nOldCnt;
			}
		}
		
		return false;
	}
	
	inline iterator firstBegin(int a_nLeft) {
		
		i2si_it_t it = m_left2Firsts.find(a_nLeft);
		if(it != m_left2Firsts.end()) {
			m_psi = it->second;
		}
		else {
			m_psi = &m_siEmpty;
		}
		return m_psi->begin();
	}
	inline const_iterator firstBegin(int a_nLeft) const {

		i2si_cit_t cit = m_left2Firsts.find(a_nLeft);
		if(cit != m_left2Firsts.end()) {
			m_psi = cit->second;
		}
		else {
			m_psi = const_cast<sint_t*>(&m_siEmpty);
		}
		return m_psi->begin();
	}
	
	inline iterator firstEnd(int a_nLeft) {
		
		return m_psi->end();
	}
	inline const_iterator firstEnd(int a_nLeft) const {
		
		return m_psi->end();
	}
	
	inline sint_t* getRules(int a_nLeft, int a_nFirst) {
		
		if(m_left2Firsts.find(a_nLeft) != m_left2Firsts.end()) {
			
			i2si_it_t it = m_first2Rules.find(a_nFirst);
			if(it != m_first2Rules.end()) {
				return it->second;
			}
		}
		return nullptr;
	}
	inline const sint_t* getRules(int a_nLeft, int a_nFirst) const {
		
		if(m_left2Firsts.find(a_nLeft) != m_left2Firsts.end()) {
			
			i2si_cit_t cit = m_first2Rules.find(a_nFirst);
			if(cit != m_first2Rules.end()) {
				return cit->second;
			}
		}
		return nullptr;
	}
};
}

#endif // PARSER_LALRTRIPLE_H__
