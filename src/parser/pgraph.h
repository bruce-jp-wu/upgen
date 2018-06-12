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

#ifndef PARSER_PGRAPH_H__
#define PARSER_PGRAPH_H__
#include "../main/upgmain.h"

#include "./defines.h"
#include "../common/common.h"
#include "../common/types.h"
#include <vector>
using std::vector;
#include <iterator>
using std::iterator;

#include <iosfwd>
using std::ostream;
using std::endl;
#include <iomanip>
using std::setw;

#include "./lritem.h"
#include "./grammar.h"
#include "./ptable.h"

namespace parser_ns {

// parse graph arc class
struct pgarc_t {
	
public:
	
	typedef lr2si_map_t::iterator item_it_t;
	typedef lr2si_map_t::const_iterator item_cit_t;
	
public:
	
	// source node(state)
	int m_nfromState;
	// destination node(state)
	int m_ntoState;
	// symbol attached to the arc
	int m_nSymbol;
	// lookahead symbols
	lr2si_map_t m_lrp2Lookaheads;
	
public:
	
	inline pgarc_t(int a_nfromState,
			int a_ntoState = INVALID_INDEX,
			int a_nSymbol = INVALID_INDEX)
	: m_nfromState(a_nfromState)
	, m_ntoState(a_ntoState)
	, m_nSymbol(a_nSymbol) {
	}
	
	inline bool insertItem(int a_nProd, int a_nDot, int a_nLookahead) {
		return insertItem(lri_pair_t(a_nProd, a_nDot), a_nLookahead);
	}
	inline bool insertItem(const lri_pair_t &a_lrp, int a_nLookahead) {
		pair<lr2si_it_t, bool> pairRet = m_lrp2Lookaheads.insert(lr2si_pair_t(a_lrp, nullptr));
		if(pairRet.second) {
			pairRet.first->second = new sint_t;
		}
		return pairRet.first->second->insert(a_nLookahead).second;
	}
	inline bool insertItem(int a_nProd, int a_nDot, const sint_t &a_siLa) {
		return insertItem(lri_pair_t(a_nProd, a_nDot), a_siLa);
	}
	inline bool insertItem(const lri_pair_t &a_lrp, const sint_t &a_siLa) {
		pair<lr2si_it_t, bool> pairRet = m_lrp2Lookaheads.insert(lr2si_pair_t(a_lrp, nullptr));
		if(pairRet.second) {
			pairRet.first->second = new sint_t;
		}
		unsigned int nOldSize = pairRet.first->second->size();
		pairRet.first->second->insert(a_siLa.begin(), a_siLa.end());
		return pairRet.first->second->size() > nOldSize;
	}

    inline void insertItems(const lr2si_map_t& lrMap) {
        m_lrp2Lookaheads.insert(lrMap.begin(), lrMap.end());
    }
	
	inline item_it_t itemBegin(void) {
		return m_lrp2Lookaheads.begin();
	}
	inline item_cit_t itemBegin(void) const {
		return m_lrp2Lookaheads.begin();
	}
	inline item_it_t itemEnd(void) {
		return m_lrp2Lookaheads.end();
	}
	inline item_cit_t itemEnd(void) const {
		return m_lrp2Lookaheads.end();
	}
	
	inline item_it_t findItem(int a_nProd, int a_nDot) {
		return findItem(lri_pair_t(a_nProd, a_nDot));
	}
	inline item_it_t findItem(const lri_pair_t &a_lrp) {
		return m_lrp2Lookaheads.find(a_lrp);
	}
	inline item_cit_t findItem(int a_nProd, int a_nDot) const {
		return findItem(lri_pair_t(a_nProd, a_nDot));
	}
	inline item_cit_t findItem(const lri_pair_t &a_lrp) const {
		return m_lrp2Lookaheads.find(a_lrp);
	}
	
	inline int getItemCount(void) const {
		return (int)m_lrp2Lookaheads.size();
	}
};

typedef vector<pgarc_t* > vpgarc_t;
typedef map<int, pgarc_t* > i2arc_map_t;
typedef pair<int, pgarc_t* > i2arc_pair_t;
typedef i2arc_map_t::iterator i2arc_it_t;
typedef i2arc_map_t::const_iterator i2arc_cit_t;

class pgraph_t;

// parse graph node class
class pgnode_t {
	friend class pgraph_t;

private:

	// node ID
	int m_nID;
	// out-going arcs
	vpgarc_t m_arcOut;
	// in-coming arcs
	vpgarc_t m_arcIn;
	// map table: lookahead --> reduced rule IDs
	i2si_map_t m_la2Rules;
	
public:

	typedef vpgarc_t::iterator arc_it_t;
	typedef vpgarc_t::const_iterator arc_cit_t;
	
public:
	
	inline pgnode_t(int a_nID)
	: m_nID(a_nID) {
		
	}
	
public:
	
	inline int getID(void) const {
		return m_nID;
	}
	inline void setID(int a_nID) {
		m_nID = a_nID;
	}
public:
	
	inline void addInArc(pgarc_t *a_pArc) {
		m_arcIn.push_back(a_pArc);
	}
	inline void addOutArc(pgarc_t *a_pArc) {
		m_arcOut.push_back(a_pArc);
	}
	inline void addRRule(int a_nRule, int a_nLa) {
		
		pair<i2si_it_t, bool> pairRet = m_la2Rules.insert(i2si_pair_t(a_nLa, nullptr));
		if(pairRet.second) {
			pairRet.first->second = new sint_t;
		}
		pairRet.first->second->insert(a_nRule);
	}
	inline void addRRule(const sint_t &a_siRRule, int a_nLa) {
		pair<i2si_it_t, bool> pairRet = m_la2Rules.insert(i2si_pair_t(a_nLa, nullptr));
		if(pairRet.second) {
			pairRet.first->second = new sint_t;
		}
		pairRet.first->second->insert(a_siRRule.begin(), a_siRRule.end());
	}
	
	inline arc_cit_t inArcBegin(void) const {
		return m_arcIn.begin();
	}
	inline arc_it_t inArcBegin(void) {
		return m_arcIn.begin();
	}
	inline arc_cit_t inArcEnd(void) const {
		return m_arcIn.end();
	}
	inline arc_it_t inArcEnd(void) {
		return m_arcIn.end();
	}
	inline int getInArcCount(void) const {
		return (int)m_arcIn.size();
	}
	
	inline arc_cit_t outArcBegin(void) const {
		return m_arcOut.begin();
	}
	inline arc_it_t outArcBegin(void) {
		return m_arcOut.begin();
	}
	inline arc_cit_t outArcEnd(void) const {
		return m_arcOut.end();
	}
	inline arc_it_t outArcEnd(void) {
		return m_arcOut.end();
	}
	inline int getOutArcCount(void) const {
		return (int)m_arcOut.size();
	}
	
	inline i2si_it_t reduceBegin(void) {
		return m_la2Rules.begin();
	}
	inline i2si_cit_t reduceBegin(void) const {
		return m_la2Rules.begin();
	}
	inline i2si_it_t reduceEnd(void) {
		return m_la2Rules.end();
	}
	inline i2si_cit_t reduceEnd(void) const {
		return m_la2Rules.end();
	}
	inline int getReduceCount(void) const {
		return (int)m_la2Rules.size();
	}
};

typedef vector<pgnode_t*> vpgnode_t;

// parse graph class
class pgraph_t {
	
public:
	
	typedef vpgnode_t::iterator node_it_t;
	typedef vpgnode_t::const_iterator node_cit_t;

private:
	
	// graph nodes
	vpgnode_t m_vpNodes;
	// graph arcs
	vpgarc_t m_ioArcs;
	// referrence to grammar object
	const grammar_t& m_refGrammar;
	// referrence to parse-table
	ptable_t &m_refPTbl;
	
public:
	// convert parse-graph to parse-table
	void grp2PTbl(gsetting_t &gsetup);
	
private:
	// create full tables(action and goto table), that is, not compressed;
	// and computes shift/reduce, reduce/reduce conflicts, if any.
	i2i_pair_t createFullTable(vftbl_t &a_tAct, vftbl_t &a_tGoto, conflicts_t &a_cnft);
	// report conflicts(shift/reduce, reduce/reduce)
	void reportConflicts(const conflicts_t &a_cfts, const i2i_pair_t &rpair, ostream &oss);
	// compress parse tables
	void compress(vftbl_t &a_ftAct, vftbl_t &a_ftGoto);
	// fill action table using sparse matrix compression algorithm
	void toActTable(vftbl_t &a_ftAct);
	// fill goto table using sparse matrix compression algorithm
	void toGotoTable(vftbl_t &a_ftGoto);
	
public:
	
	inline pgraph_t(const grammar_t &a_refGrammar, ptable_t &a_ptbl)
	: m_refGrammar(a_refGrammar)
	, m_refPTbl( a_ptbl) {
		
		init();
	}
	
	inline ~pgraph_t(void) {
		
		clear();
	}
	
	void init(void);	
	void clear(void);
	
public:
	
	inline pgnode_t* fromID(int a_nID) {
		assert(a_nID >= FIRST_STATE && a_nID < (int)m_vpNodes.size());
		return m_vpNodes[a_nID];
	}
	
	inline void addArc(pgarc_t *a_pArc) {
		m_ioArcs.push_back(a_pArc);
	}
	
	inline pgnode_t* addEmptyNode(void) {
		
		pgnode_t* pgn = new pgnode_t((int)m_vpNodes.size());
		m_vpNodes.push_back(pgn);
		return pgn;
	}
	inline void addNode(pgnode_t* a_pNode) {
		assert(a_pNode);
		a_pNode->setID((int)m_vpNodes.size());
		m_vpNodes.push_back(a_pNode);
	}
	
	node_it_t nodeBegin(void) {
		node_it_t it = m_vpNodes.begin();
		for(int i = 0; i < FIRST_STATE; ++i, ++it);
		return it;
	}
	node_cit_t nodeBegin(void) const {
		node_cit_t cit = m_vpNodes.begin();
		for(int i = 0; i < FIRST_STATE; ++i, ++cit);
		return cit;
	}
	inline node_it_t nodeEnd(void) {
		return m_vpNodes.end();
	}
	inline node_cit_t nodeEnd(void) const {
		return m_vpNodes.end();
	}

	inline int getNodeTotalCount(void) const {
		return (int)m_vpNodes.size();
	}
	inline int getValidNodeCount(void) const {
		return (int)(m_vpNodes.size() -  FIRST_STATE);
	}
	inline int getNextNodeID(void) const {
		return (int)m_vpNodes.size();
	}
	
	inline const grammar_t& getGrammar(void) const {
		return m_refGrammar;
	}
	inline const ptable_t& getPTable(void) const {
		return m_refPTbl;
	}
	
public:
	
	friend ostream& operator<<(ostream& os, const pgraph_t& src);
};
}

#endif // PARSER_PGRAPH_H__
