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

#ifndef LEXER_DGRAPH_H__
#define LEXER_DGRAPH_H__

#include <iosfwd>
using std::ostream;
#include <string>
using std::string;

#include <algorithm>
using std::sort;

#include "../common/types.h"
#include "../common/uset.h"
using common_ns::charset_t;

#include "./dfafwd.h"
#include "./dtable.h"

#include "./dgarc.h"
#include "./dgnode.h"

namespace lexer_ns {
	
// Graph representation of DFA
class dgraph_t {
	
private:
	
	// representation type: arc or array
	bool m_bArcRep;
	vgnode_t m_vpNodes;
	vpvint_t m_vpvGenNodes;
	vgarc_t m_vpArcs;
	dtable_t &m_dTbl;

private:
	
	// minimization of DFA
	void minimize(void);
	// transform state representation from arc to array,
	// make it easier to compress table
	void transformRep(void);
	
	void fillTransitions(void);
	void fillAccepts(void);
	// rearrange states before compression
	// be sure dgraph is array-representative
	void updateAfterRearrange(void);
	
	// initialize before construction
	void init(void);
	
	// sort and update DFA graphs
	template <class CMP>
	void rearrangeStates(const CMP &strategy) {
		
		assert(!m_bArcRep);
		vgnode_it_t itBegin = m_vpNodes.begin();
		for(int i = 0; i < FIRST_STATE; ++i) {
			
			++itBegin;
		}
		
		sort(itBegin, m_vpNodes.end(), strategy);
		updateAfterRearrange();
	}
	
	// sort and update DFA graphs
	template <class CMP>
	void rearrangeStates(const CMP &strategy, int begin, int end) {
		
		assert(!m_bArcRep);
		assert(end >= begin);
		vgnode_it_t itBegin = m_vpNodes.begin(), itEnd = m_vpNodes.end();

		int i = 0;
		for(vgnode_it_t it = m_vpNodes.begin(); it != m_vpNodes.end(); ++it, ++i) {
			
			if(_EQ(i, begin)) {
				
				itBegin = it;
			}
			else if(_EQ(i, end)) {
				
				itEnd = it;
				break;
			}
		}
		sort(itBegin, itEnd, strategy);
		updateAfterRearrange();
	}
	
public:
	
	// compress DFA
	void compress(void);
	
	// convert graph representation to table representation
	void toDTables(void);
	
	void clear(void);
	
	friend class dgraph_arc_t;
	friend class dgraph_node_t;
	friend ostream& operator<<(ostream &os, const dgraph_t &src);
	
public:
	
	inline dgraph_t(int a_nFirstState, dtable_t &a_dTbl)
	: m_bArcRep(true)
	, m_dTbl(a_dTbl) {
		init();
	}
	
	inline ~dgraph_t(void) {
		clear();
	}
	
	inline const dtable_t& getDTable(void) const {
		return m_dTbl;
	}
	
	inline dtable_t& getDTable(void) {
		return m_dTbl;
	}
	
	void addStartIndex(const string &a_strSCName, int a_nFull, int a_nNoHat) {
		
		m_dTbl.addStartIndex(a_strSCName, a_nFull, a_nNoHat);
	}
	
	inline void setStartIndex(int a_nIdx, int a_nFull, int a_nNoHat) {
		m_dTbl.setStartIndex(a_nIdx, a_nFull, a_nNoHat);
	}
	
	inline string getStartName(int a_nIdx) const {
		return m_dTbl.getStartName(a_nIdx);
	}
	
	inline int getStartIndex(int a_nIdx, bool a_bFull) const {
		return m_dTbl.getStartIndex(a_nIdx, a_bFull);
	}
	
	inline int getStartCount(void) const {
		return m_dTbl.getStartCount();
	}
	
	inline int getLabelCount(void) const {
		return m_dTbl.getLabelCount();
	}
	
	inline dgraph_arc_t* newArc(void) {
		
		m_vpArcs.push_back(new dgraph_arc_t(
				INVALID_STATE,
				INVALID_STATE));
		return m_vpArcs[m_vpArcs.size() - 1];
	}
	
	inline dgraph_arc_t* newArc(int a_nFrom, int a_nTo) {
		
		m_vpArcs.push_back(new dgraph_arc_t(
				a_nFrom,
				a_nTo));
		return m_vpArcs[m_vpArcs.size() - 1];		
	}
	
	inline dgraph_arc_t* newArc(int a_nFrom, int a_nTo, const charset_t &src) {
		
		m_vpArcs.push_back(new dgraph_arc_t(
				a_nFrom,
				a_nTo,
				src));
		return m_vpArcs[m_vpArcs.size() - 1];		
	}
	
	inline dgraph_node_t* newNode(void) {
		
		m_vpNodes.push_back(
				new dgraph_node_t(m_vpNodes.size(), *this));
		return m_vpNodes[m_vpNodes.size() -1];
	}
	
	inline int getNextState(void) const {
		return (int)m_vpNodes.size();
	}

	inline dgraph_node_t* getNode(int a_nState) {
		return m_vpNodes[a_nState];
	}
	
	inline const dgraph_node_t* getNode(int a_nState) const {
		return m_vpNodes[a_nState];
	}
	
	inline int getStateCount(void) const {
		return (int)m_vpNodes.size();
	}
};

}
#endif // LEXER_DGRAPH_H__
