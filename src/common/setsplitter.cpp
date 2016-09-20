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

#include <algorithm>
#include "./uset.h"
#include "./setsplitter.h"

namespace common_ns {

superset_t::superset_t(int a_nBegin, int a_nEnd)
: m_nBegin(a_nBegin)
, m_nEnd(a_nEnd)
, m_sbsOther(a_nEnd - a_nBegin + 1) {
	
	// all elements belong a set
	assert(a_nBegin < a_nEnd);
	for(int i = m_nBegin; i < m_nEnd; ++i) {
		
		m_vnPrnt.push_back(0);
	}
}

// get leaders of all subsets
void superset_t::getLeaders(vint_t &a_vnLeaders) const {
	
	a_vnLeaders.clear();
	
	for(int i = 0; i < (int)m_vnPrnt.size(); ++i) {
		
		if(_EQ(i, m_vnPrnt[i])) {
			
			a_vnLeaders.push_back(toOuterIndex(i));
		}
	}
}

// get all elements represented by a given leader
void superset_t::getSubset(int a_nLeader, vint_t &a_vnElems) const {
	
	a_vnElems.clear();
	a_nLeader = toInnerIndex(a_nLeader);
	
	for(int i = 0; i < (int)m_vnPrnt.size(); ++i) {
		
		if(_EQ(a_nLeader, m_vnPrnt[i])) {
			
			a_vnElems.push_back(toOuterIndex(i));
		}
	}
}

// get all subsets
void superset_t::getSubsets(vsubset_t &a_vsSubsets) const {
	
	int i = 0;
	for(i = 0; i < (int)a_vsSubsets.size(); ++i) {
		
		delete a_vsSubsets[i];
	}
	a_vsSubsets.clear();
	
	vsubset_t newVsub;
	
	//find subsets and number of elements for each subset
	for(i = 0; i < (int)m_vnPrnt.size(); ++i) {
		
		if(_EQ(i, m_vnPrnt[i])) {
			// find new subset
			newVsub.push_back(new subset_t);
		}
		else {
			
			newVsub.push_back(nullptr);
		}
	}
	
	for(i = 0; i < (int)m_vnPrnt.size(); ++i) {
				
		newVsub[m_vnPrnt[i]]->push_back(toOuterIndex(i));
	}
	
	for(i = 0; i < (int)newVsub.size(); ++i) {
		
		if(nullptr != newVsub[i]) {
			
			a_vsSubsets.push_back(newVsub[i]);
		}
	}
}

// divide superset by adding a new subset
// elements of new subset is chained by list;
// @p: a sorted list
// @return-value: if new subset generated, return true
// 		otherwise return false
bool superset_t::split(_node_t *p) {
	
	int i;
	bool bChanged = false;
	while(p) {

		_node_t *r = p;
		_node_t *q = r->p;	
		
		if(_EQ(p->n, m_vnPrnt[p->n])) {
			
			while(q) {
				
				if(_EQ(p->n, m_vnPrnt[q->n])) {
					
					// remove q from list
					r->p = q->p;
				}
				else {
				
					r = q;
				}
				q = r->p;
			}
			
			int newLeader = 0;
			for(i = p->n + 1; i < (int)m_vnPrnt.size(); ++i) {

				if(_EQ(m_vnPrnt[i], p->n) && !m_sbsOther.test(i)) {
					
					newLeader = i;
					m_vnPrnt[i] = newLeader;
					bChanged = true;
					break;
				}
			}

			for(; i < (int)m_vnPrnt.size(); ++i) {

				if(_EQ(m_vnPrnt[i], p->n) && !m_sbsOther.test(i)) {
					
					assert(newLeader > 0);
					m_vnPrnt[i] = newLeader;
					bChanged = true;
				}
			}
		}
		else {
			
			int np = m_vnPrnt[p->n];
			if(np != p->n) {
				
				m_vnPrnt[p->n] = p->n;
				bChanged = true;
			}
			
			while(q) {
				
				if(_EQ(np, m_vnPrnt[q->n])) {
					
					m_vnPrnt[q->n] = p->n;
					
					// remove q from list
					r->p = q->p;
				}
				else {
				
					r = q;
				}
				q = r->p;
			}
		}
		
		p = p->p;
	}
	
	return bChanged;
}

// @a_nElem is considered as an element of singleton set,
// split original set which contains @a_nElem
bool superset_t::split(int a_nElem) {
	
	assert(a_nElem >= m_nBegin && a_nElem < m_nEnd);
	
	a_nElem = toInnerIndex(a_nElem);
	
	if(_EQ(m_vnPrnt[a_nElem], a_nElem)) {
		
		bool bChanged = false;
		int newLeader = 0;
		int i = a_nElem + 1;
		for(; i < (int)m_vnPrnt.size(); ++i) {
			
			if(_EQ(m_vnPrnt[i], a_nElem)) {
				
				
				newLeader = i;
				m_vnPrnt[i] = newLeader;
				bChanged = true;
				break;
			}
		}
		
		for(; i < (int)m_vnPrnt.size(); ++i) {
			
			if(_EQ(m_vnPrnt[i], a_nElem)) {
				
				assert(newLeader != 0);
				m_vnPrnt[i] = newLeader;
				bChanged = true;
			}
		}
		
		return bChanged;
	}
	
	m_vnPrnt[a_nElem] = a_nElem;
	return true;
}

// divide superset by adding a new subset
// assume a_subset is sorted ascendingly
bool superset_t::split(const subset_t &a_subset) {
		
	if(_EQ(0, a_subset.size())) {
		
		return false;
	}

	_node_t *pn = new _node_t[a_subset.size()];
	
	m_sbsOther.reset();
	int i = 0;
	for(i = 0; i < ((int)a_subset.size()) - 1; ++i) {
		
		pn[i].n = toInnerIndex(a_subset[i]);
		pn[i].p = &pn[i + 1];

		m_sbsOther.set(pn[i].n);
	}
	pn[i].n = toInnerIndex(a_subset[i]);
	pn[i].p = nullptr;
	m_sbsOther.set(pn[i].n);
	
	bool bret = split(&pn[0]);
	delete[] pn;
	
	return bret;
}

// divide superset by adding a new subset
// assume a_subset is sorted increasingly
bool superset_t::split(const sint_t &a_si) {
	
	if(_EQ(0, a_si.size())) {
		
		return false;
	}

	_node_t *pn = new _node_t[a_si.size()];
	
	m_sbsOther.reset();
	int i = 0;
	sint_cit_t cit = a_si.begin();
	for(; i < ((int)a_si.size()) - 1; ++i, ++cit) {
		
		pn[i].n = toInnerIndex(*cit);
		pn[i].p = &pn[i + 1];
		m_sbsOther.set(pn[i].n);
	}
	
	assert(cit != a_si.end());
	pn[i].n = toInnerIndex(*cit);
	pn[i].p = nullptr;
	m_sbsOther.set(pn[i].n);
	
	bool bret = split(&pn[0]);
	delete[] pn;
	
	return bret;
}

}
