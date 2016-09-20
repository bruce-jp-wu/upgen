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

#ifndef LEXER_DGNODE_H__
#define LEXER_DGNODE_H__

#include <iosfwd>
using std::ostream;
#include <vector>
using std::vector;
#include <iterator>
using std::iterator;
#include <string>
using std::string;

#include <functional>
using std::binary_function;

#include "../common/types.h"
#include "../common/uset.h"
using common_ns::charset_t;

#include "./dfafwd.h"
#include "./dgarc.h"

namespace lexer_ns {

/////////////////////////////////////////////////////////////////////////////////
// graph node definition
class dgraph_node_t	{

	friend class dgraph_t;
	friend class dgnode_less_t;
	friend class dgnode_greater_t;
	
private:
	
	int m_nID;
	int m_nDefault;
	
	struct {
		// indicates if there are cycled transitions
		bool m_bCycled:	1;
		// indicates which type of representation of transitions
		bool m_bArc:	1;
		int m_nRule: 	15;
		int m_nAction: 	15;
	};
	
//	sint_t m_siLookahead;
    
	union {
	
		// arc representation of transitions
		struct
		{
			vgarc_t *m_pinArc;
			vgarc_t *m_poutArc;
		};
		
		// array representive of transitions
		struct {
			
			struct {
				
				int m_nLabels: 	16;
				int m_nTargets: 16;
			};
			vint_t *m_pvnLabels;
		};
	};
	
	dgraph_t& m_dgRef;

private:

	inline dgraph_node_t(int a_nID, dgraph_t& a_dg)
	: m_nID(a_nID)
	, m_nDefault(INVALID_STATE)
	, m_bCycled(false)
	, m_bArc(true)
	, m_nRule(INVALID_RULE)
	, m_nAction(INVALID_ACTION)
	, m_pinArc(nullptr)
	, m_poutArc(nullptr)
	, m_dgRef(a_dg) {
		
		m_pinArc = new vgarc_t;
		m_poutArc = new vgarc_t;
	}
	
public:
	
	inline ~dgraph_node_t(void) {
		
		if(m_bArc) {
			
			delete m_pinArc;
			delete m_poutArc;
		}
		else {
			
			delete m_pvnLabels;
		}
	}
	
public:	
	
	inline int getID(void) const {
		
		return m_nID;
	}
	
	inline int getDefault(void) const {
		
		return  m_nDefault;
	}
	
	inline void setDefault(int a_nDefault) {
		
		m_nDefault =  a_nDefault;
	}
	
	inline bool getCycled(void) const {
		
		return m_bCycled;
	}
	
	inline void setCycled(bool a_bCycled) {
		
		m_bCycled = a_bCycled;
	}
	
	inline int getRule(void) const {
		
		return m_nRule;
	}
	
	inline void setRule(int a_nRule) {
		
		m_nRule = a_nRule;
	}
	
	inline int getAction(void) const {
		
		return m_nAction;
	}
	
	inline void setAction(int a_nAction) {
		
		m_nAction = a_nAction;
	}
	
	inline bool isArcRep(void) const {
		
		return m_bArc;
	}
	
	inline void addInArc(dgraph_arc_t* a_pArc) {
		
		assert(a_pArc && m_bArc);
		if(_EQ(a_pArc->getFromState(), a_pArc->getToState())) {
				
			if(m_pinArc->size() > 0) {
			
				m_pinArc->push_back((*m_pinArc)[0]);
				(*m_pinArc)[0] = a_pArc;
			}
			else {
				
				m_pinArc->push_back(a_pArc);
			}
			
			m_bCycled = true;
		}
		else {
				
			m_pinArc->push_back(a_pArc);
		}
	}
	
	inline void addOutArc(dgraph_arc_t* a_pArc) {
		
		assert(a_pArc && m_bArc);
		if(_EQ(a_pArc->getFromState(), a_pArc->getToState())) {
			
			if(m_poutArc->size() > 0) {
				
				m_poutArc->push_back((*m_poutArc)[0]);
				(*m_poutArc)[0] = a_pArc;
			}
			else {
				
				m_poutArc->push_back(a_pArc);
			}
			
			m_bCycled = true;
		}
		else {
				
			m_poutArc->push_back(a_pArc);
		}
	}
	
	inline int getInArcCount(void) const {
		
		assert(m_bArc);
		return (int)m_pinArc->size();
	}
	
	inline int getOutArcCount(void) const {
		
		assert(m_bArc);
		return (int)m_poutArc->size();
	}
	
	inline dgraph_arc_t* getInArc(int a_nIdx) const {
		
		assert(m_bArc);
		assert(a_nIdx >= 0 && a_nIdx < (int)m_pinArc->size());
		return (*m_pinArc)[a_nIdx];
	}
	
	inline dgraph_arc_t* getOutArc(int a_nIdx) const {
		
		assert(m_bArc);
		assert(a_nIdx >= 0 && a_nIdx < (int)m_poutArc->size());
		return (*m_poutArc)[a_nIdx];
	}
	
	inline int getValidLabelCount(void) const {
		
		assert(!m_bArc);
		return m_nLabels;
	}
	
	int getTotalLabelCount(void) const;
	
	inline int getTargetState(int a_nLabel) const {
		
		assert(!m_bArc);
		assert(a_nLabel >= 0 && a_nLabel < getTotalLabelCount());
		return (*m_pvnLabels)[a_nLabel];
	}
	
	inline const dgraph_t& getDGraph(void) const {
		
		return m_dgRef;
	}
	inline dgraph_t& getDGraph(void) {
		
		return m_dgRef;
	}
	
	friend ostream& operator<<(ostream &os, const dgraph_node_t& src);
};

// graph node definition
/////////////////////////////////////////////////////////////////////////////////

typedef vector<dgraph_node_t* > vgnode_t;
typedef vgnode_t::iterator vgnode_it_t;

typedef binary_function < dgraph_node_t*, dgraph_node_t*, bool > dgnode_compare_t;

class dgnode_greater_t: public dgnode_compare_t{
		
public:
	inline bool operator()(const dgraph_node_t* a_pLeft, const dgraph_node_t* a_pRight) const {
		
		assert(a_pLeft && a_pRight);
//		assert(!a_pLeft->m_bArc && !a_pRight->m_bArc);
		
		return (a_pLeft->m_nLabels > a_pRight->m_nLabels)
			|| (_EQ(a_pLeft->m_nLabels, a_pRight->m_nLabels) && a_pLeft->m_nTargets > a_pRight->m_nTargets);
	}
};


class dgnode_less_t: public dgnode_compare_t {
	
public:
	inline bool operator()(const dgraph_node_t* a_pLeft, const dgraph_node_t* a_pRight) const {
		
		assert(a_pLeft && a_pRight);
//		assert(!a_pLeft->m_bArc && !a_pRight->m_bArc);
		
		return (a_pLeft->m_nLabels < a_pRight->m_nLabels)
			|| (_EQ(a_pLeft->m_nLabels, a_pRight->m_nLabels) && a_pLeft->m_nTargets < a_pRight->m_nTargets);
	}
};
}

#endif //LEXER_DGNODE_H__
