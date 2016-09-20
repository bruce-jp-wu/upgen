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

#ifndef LEXER_LEXER_H__
#define LEXER_LEXER_H__

#include "../main/upgmain.h"

#include <string>
using std::string;

#include <iosfwd>
using std::ostream;

#include <set>
using std::set;

#include <functional>
using std::binary_function;

#include "../common/common.h"
#include "../common/types.h"
#include "../common/reporter.h"
#include "../common/charmap.h"
using common_ns::charmap_t;

#include "./defines.h"
#include "./retree.h"
#include "./dtable.h"
#include "./dgraph.h"
#include "./scmgr.h"
#include "./macmgr.h"

namespace lexer_ns {

class dstate_info_t {

	friend class lexer_t;
	
	int m_nID;
	iset_t *m_psubSet;
	charset_t *m_ptransLabel;

public:
	
	inline dstate_info_t(int a_nID, iset_t *a_psubSet, charset_t *a_pLabel)
	: m_nID(a_nID)
	, m_psubSet(a_psubSet)
	, m_ptransLabel(a_pLabel){
		
	}
	
	inline ~dstate_info_t(void) {
		
		if(m_psubSet) {
			delete m_psubSet;
		}
		if(m_ptransLabel) {
			delete m_ptransLabel;
		}
	}

public:
	
	inline bool operator < (const dstate_info_t &src) const {
		assert(m_psubSet && src.m_psubSet);
		return (*m_psubSet) < (*src.m_psubSet);
	}
};

class dstate_less_t: public binary_function < dstate_info_t*, dstate_info_t*, bool > {
	
public:
	inline bool operator () (const dstate_info_t* a_pLeft, const dstate_info_t* a_pRight) const {
		assert(a_pLeft && a_pRight);
		return *a_pLeft < *a_pRight;
	}
};

typedef set < dstate_info_t*, dstate_less_t > dstate_set_t;
typedef dstate_set_t::iterator dstate_it_t;
typedef dstate_set_t::const_iterator dstate_cit_t;

class lexer_t {

private:
	
	// flag indicating if lexer is scanning lookahead nodes
	bool m_bIsLookahead;
	
	rnpool_t m_rLeaves;
	rnpool_t m_rInners;
	rtpool_t m_rTrees;
	
	charmap_t m_charMap;
	sc_mgr_t m_scMgr;
	mac_mgr_t m_macMgr;
	
	vint_t m_sc2EOFRule;
	
	// keep track of trees that contain trailing context ('/')
	vint_t m_vnTrail;
	// keep track of RE nodes after trailing context ('/')
	vint_t m_vnLaNodes;
	int m_nCurLaNodes;
	
	// map from tree ID to rule ID
	vint_t m_tree2Rule;

private:
	
/////////////////////////////////////////////////////////////////////////////////////
	// reset the lexer object
	void clearup(void);
	// copy recursively from the tree rooted at @a_pSrc to the tree rooted at @a_pDest
	void recursiveCopy(renode_t*& a_pDest, renode_t *a_pSrc);
	// calculate FollowPos set of all nodes of the forest
	void calcFollowPos(void);
	// convert RE trees associated with specific start-condition to DFA graph
	bool retrees2DGraph(int a_nSC, dgraph_t &a_dGrp, dstate_set_t& dset);

	friend ostream& operator<<(ostream& os, const lexer_t &src);
	
public:
	
	lexer_t(void);
	~lexer_t(void);
	
public:

/////////////////////////////////////////////////////////////////////////////////	
	// convert all RE trees to DFA tables
	void retrees2DTables(dtable_t &a_dTbl, gsetting_t &gsetup);	

/////////////////////////////////////////////////////////////////////////////////
	// RE node methods
	// add node
	inline renode_t* addLeafNode(charset_t *a_pSet, bool a_bNullable = false) {
		
		assert(a_pSet);
		renode_t* pnode = new renode_t(a_pSet, m_rLeaves.size(),
				INVALID_RULE, getLookaheadFlag(), a_bNullable);
		m_rLeaves.push_back(pnode);
		
		if(m_bIsLookahead) {	
			++m_nCurLaNodes;
		}
		
		return pnode;
	}
	
	inline renode_t* addLeafNode(char a_c, bool a_bExcluded = false,
			bool a_bNullable = false) {
		
		charset_t *pcs = new charset_t(a_c, a_bExcluded);
		return addLeafNode(pcs, a_bNullable);
	}
	
	inline renode_t* addLeafNode(const char *a_pch, bool a_bExcluded = false,
			bool a_bNullable = false) {
		
		assert(a_pch && strlen(a_pch) > 0);
		charset_t *pcs = new charset_t(a_pch, a_bExcluded);
		return addLeafNode(pcs, a_bNullable);
	}
	
	inline renode_t* addLeafNode(const string &a_str, bool a_bExcluded = false,
			bool a_bNullable = false) {
		
		assert(a_str.size() > 0);
		charset_t *pcs = new charset_t(a_str, a_bExcluded);
		return addLeafNode(pcs, a_bNullable);
	}
	
	inline renode_t* addLeafNode(charset_t::posix_char_t a_nType,
			bool a_bExcluded = false) {
		
		charset_t *pcs = new charset_t(a_nType);
		
		if(a_bExcluded) {
			pcs->flip();
		}
		return addLeafNode(pcs, false);
	}
	
	inline renode_t* addStarNode(renode_t *a_pChild) {
		
		assert(a_pChild);
		renode_t* pnode = new renode_t(a_pChild, m_rInners.size());
		m_rInners.push_back(pnode);
		
		return pnode;
	}
	
	inline renode_t* addConcatNode(renode_t *a_pLeft, renode_t *a_pRight) {
		
		assert(a_pLeft && a_pRight);
		renode_t* pnode = new renode_t(a_pLeft, a_pRight, renode_t::CONCAT,
				m_rInners.size());
		m_rInners.push_back(pnode);
		
		return pnode;		
	}
	
	inline renode_t* addUnionNode(renode_t *a_pLeft, renode_t *a_pRight) {
		
		assert(a_pLeft && a_pRight);
		
		renode_t* pnode = new renode_t(a_pLeft, a_pRight, renode_t::UNION,
				m_rInners.size());
		m_rInners.push_back(pnode);
		
		return pnode;
	}
	
	inline renode_t* addEolNode(void) {
		
		charset_t *pcs = new charset_t('\r');
		renode_t *pr = addLeafNode(pcs, true);
		
		pcs = new charset_t('\n');
		renode_t *pn = addLeafNode(pcs, false);
		
		return addConcatNode(pr, pn);
	}
	
	inline renode_t* addEofNode(void) {
		
		charset_t *pcs = new charset_t;
		renode_t* pnode = addLeafNode(pcs, false);
		pnode->setEOF(true);
		m_rLeaves.push_back(pnode);
		
		return pnode;
	}
	
	inline renode_t* addEorNode(bool a_bNullable) {
		
		charset_t *pcs = new charset_t;
		renode_t* pNode = addLeafNode(pcs, a_bNullable);
		pNode->setEOR(true);
		
		if(m_bIsLookahead) {
			--m_nCurLaNodes;
		}
		return pNode;
	}	

///////////////////////////////////////////////////////////////////////////////
	// RE tree and action associated methods
	inline void mapTree2Rule(int a_nTreeID, int a_nRuleID) {
		
		while( a_nTreeID >= (int)m_tree2Rule.size() ) {
			
			m_tree2Rule.push_back(0);
		}
		
		m_tree2Rule[a_nTreeID] = a_nRuleID;
	}
	
	// get ID of the rule corresponding to @a_nTreeID
	inline int tree2Rule(int a_nTreeID) {
	
		assert(a_nTreeID >= 0 && a_nTreeID < (int)m_tree2Rule.size());
		
		return m_tree2Rule[a_nTreeID];
	}
	
	inline retree_t* addTree(int a_nRuleID, renode_t *a_pRoot, bool a_bBOL = false, bool a_bEOF = false) {
		
		int nCurTreeID = (int)m_rTrees.size();
		mapTree2Rule(nCurTreeID, a_nRuleID);
		
		retree_t* pTree = new retree_t(a_pRoot, nCurTreeID, a_bBOL, a_bEOF);
		m_rTrees.push_back(pTree);
		recursiveUpdateTreeID(a_pRoot, nCurTreeID);
		
		if(getLookaheadFlag()) {
			
			m_vnTrail.push_back(nCurTreeID);
			m_vnLaNodes.push_back(m_nCurLaNodes);
			m_nCurLaNodes = 0;
		}
		
		return pTree;
	}
	
	inline void attachAction(retree_t *a_pTree, int a_nActionIndex) {
		
		assert(a_pTree);
		a_pTree->setActionIndex(a_nActionIndex);
		recursiveUpdateActionIndex(a_pTree->getRoot(), a_nActionIndex);
	}
	
	// get/set flag of lookahead, when you see trailing context ('/'),
	// set the flag of lookahead, then reset it at the end of the regular expression
	inline bool getLookaheadFlag(void) const {
		return m_bIsLookahead;
	}
	inline void setLookaheadFlag(bool a_bLA) {
		
		m_bIsLookahead = a_bLA;
		if(m_bIsLookahead) {
			// seeing trailing context ('/'),
			// begin to count number of nodes
			m_nCurLaNodes = 0;
		}
	}

///////////////////////////////////////////////////////////////////////////////
	// RE macro methods
	inline bool addMacro(const string &a_strMName, renode_t *a_pMRoot) {
		
		return m_macMgr.add(a_strMName, a_pMRoot);
	}
	
	inline bool findMacro(const string &a_strMName, renode_t*& a_rpMroot) {
		
		return m_macMgr.find(a_strMName, a_rpMroot);
	}
	
	inline renode_t* copySubtree(renode_t *a_pNode) {
		
		assert(a_pNode);
		renode_t *pNode = nullptr;
		
		recursiveCopy(pNode, a_pNode);
		
		return pNode;
	}
	
	inline renode_t* copyMacrotree(const string &a_strMName) {
		
		if(0 == m_macMgr.addRef(a_strMName)) {
			
			return m_macMgr.getRoot(a_strMName);
		}
		
		return copySubtree(m_macMgr.getRoot(a_strMName));
	}	
	
///////////////////////////////////////////////////////////////////////////////////
	// methods that dealing with start-condition
	inline int getSCIdx(const string &a_strSCName) {
		
		return m_scMgr.getIdx(a_strSCName);
	}
	
	inline bool addISC(const string &a_strSCName) {

		return m_scMgr.addISC(a_strSCName);
	}
	
	inline bool addXSC(const string &a_strSCName) {

		return m_scMgr.addXSC(a_strSCName);
	}
	
	inline int addSCTree(int a_nIdx, int a_nTreeID) {
		
		if(a_nIdx < INITIAL_INDEX || a_nIdx >= (int)m_scMgr.size()) {
			
			return 0;
		}
		
		int num = 0;
		if(m_scMgr.addTree(a_nIdx, a_nTreeID)) {
			
			++num;
			m_rTrees[a_nTreeID]->incRef();
		}
		
		if(!m_scMgr.isExclusive(a_nIdx) && m_scMgr.addTree(INITIAL_INDEX, a_nTreeID)) {

			++num;
			m_rTrees[a_nTreeID]->incRef();			
		}
		
		return num;
	}
	
	inline int addSCTree(const string &a_strSCName, int a_nTreeID) {
		
		assert(a_nTreeID >= 0 && a_nTreeID < (int)m_rTrees.size());
		
		int idx = m_scMgr.getIdx(a_strSCName);
		if(_EQ(INVALID_SCINDEX, idx)) {
			
			return 0;
		}
		
		return addSCTree(idx, a_nTreeID);
	}
	
	inline int addSCTree(int a_nTreeID) {
		
		assert(a_nTreeID >= 0 && a_nTreeID < (int)m_rTrees.size());
		
		int num = 0;
		
		for(int i = 0; i < (int)m_scMgr.size(); ++i) {
			
			if(m_scMgr.addTree(i, a_nTreeID)) {
				
				++num;
				m_rTrees[a_nTreeID]->incRef();
			}			
		}
		
		return num;
	}
};

}
#endif //LEXER_LEXER_H__
