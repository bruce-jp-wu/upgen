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

#ifndef COMMON_SETSPLITTER_H__
#define COMMON_SETSPLITTER_H__

#include "./common.h"
#include "./types.h"
#include "./uset.h"

namespace common_ns {

// supperset_t: set splitter class

typedef vint_t subset_t;

typedef vector < subset_t* > vsubset_t;

class superset_t {
	// inner list node
	struct _node_t {
		
		int n;
		_node_t *p;
	};
	
private:
	
	int m_nBegin;
	int m_nEnd;
	vint_t m_vnPrnt;
	sbitset_t m_sbsOther;
	
private:
	// map @m_nBegin-base from to 0-base
	inline int toInnerIndex(int a_nOuter) const {
		
		return a_nOuter - m_nBegin;
	}
	// map from 0-base to @m_nBegin-base
	inline int toOuterIndex(int a_nInner) const {
		
		return a_nInner + m_nBegin;
	}
	
	bool split(_node_t *ph);
	
public:
	
	superset_t(int a_nLow, int a_nHigh);
	// get leaders of all subsets
	void getLeaders(vint_t &a_vnLeaders) const;
	// get all elements representend by a given leader
	void getSubset(int a_nLeader, vint_t &a_vnElems) const;
	// get all subsets
	void getSubsets(vsubset_t &a_vsSubsets) const;
	// @a_nElem is considered as an element of singleton set,
	// split original set which contains @a_nElem
	bool split(int a_nElem);
	// divide superset by adding a new subset
	// assume a_subset is sorted ascendingly
	bool split(const subset_t &a_subset);
	
	// divide superset by adding a new subset
	// assume a_subset is sorted ascendingly
	bool split(const sint_t &a_si);
	
public:
	
	// get leader of @a_nElem
	inline int getLeader(int a_nElem) const {
		
		return m_vnPrnt[toInnerIndex(a_nElem)];
	}	
};

}

#endif //COMMON_SETSPLITTER_H__
