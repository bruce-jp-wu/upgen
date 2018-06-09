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

#ifndef LEXER_RETREE_H__
#define LEXER_RETREE_H__

#include <vector>
using std::vector;

#include "../common/common.h"
#include "../common/uset.h"
using common_ns::charset_t;
using common_ns::iset_t;

#include "./defines.h"

namespace lexer_ns {

// node of regular expression tree
class renode_t {

public:
	enum node_type_t{
		CHARSET,
		STAR,
		CONCAT,
		UNION
	};
	
private:
	
	node_type_t m_nType;
	int m_nID;
	
	bool m_bNullable;
	iset_t m_isFirstPos;
	iset_t m_isLastPos;
	iset_t m_isFollowPos;
	
	union{
		
		// when m_nType == CHARSET
		struct {	// leaf node
			charset_t *m_pCharSet;
			struct {
				// flag of lookahead
				bool m_bLookahead:	1;
				bool m_bEOR:		1;
				bool m_bEOF:		1;
				int m_nTreeID: 		31;
				int m_nActionIndex: 30;
			};
		};
		
		// when m_nType == STAR
		struct {
			renode_t *m_pChild;
		};
		
		// when m_nType == CONCAT or m_nType == UNION
		struct {
			
			renode_t *m_pLeft;
			renode_t *m_pRight;
		};
	};
	
	friend class lexer_t;
	
public:
	
	inline renode_t(charset_t *a_pSet, int a_nID, int a_nTreeID = INVALID_RULE,
			bool a_bLookahead = false, bool a_bNullable = false)
	: m_nType(CHARSET)
	, m_nID(a_nID)
	, m_bNullable(a_bNullable)
	, m_pCharSet(a_pSet)
	, m_bLookahead(a_bLookahead)
	, m_bEOR(false) 
	, m_bEOF(false)
	, m_nTreeID(a_nTreeID)
	, m_nActionIndex(INVALID_ACTION) {
		
		m_isFirstPos.insert(a_nID);
		m_isLastPos.insert(a_nID);
	}
	
	inline renode_t(renode_t *a_pChild, int a_nID)
	: m_nType(STAR)
	, m_nID(a_nID)
	, m_bNullable(true)
	, m_pChild(a_pChild) {
	
		m_isFirstPos.merge(a_pChild->m_isFirstPos);
		m_isLastPos.merge(a_pChild->m_isLastPos);
		
	}
	
	inline renode_t(renode_t *a_pLeft, renode_t *a_pRight, node_type_t a_nType, int a_nID)
	: m_nType(a_nType)
	, m_nID(a_nID)
	, m_pLeft(a_pLeft)
	, m_pRight(a_pRight) {
		
		switch(m_nType) {
		case CONCAT:
			
			m_bNullable = m_pLeft->m_bNullable && m_pRight->m_bNullable;
			m_isFirstPos.merge(m_pLeft->m_isFirstPos);
			m_isLastPos.merge(m_pRight->m_isLastPos);
			
			if(m_pLeft->m_bNullable) {
				
				m_isFirstPos.merge(m_pRight->m_isFirstPos);
			}
			if(m_pRight->m_bNullable) {
				
				m_isLastPos.merge(m_pLeft->m_isLastPos);
			}
			break;
			
		case UNION:
			
			m_bNullable = m_pLeft->m_bNullable || m_pRight->m_bNullable;
			
			m_isFirstPos.merge(m_pLeft->m_isFirstPos);
			m_isFirstPos.merge(m_pRight->m_isFirstPos);
			
			m_isLastPos.merge(m_pLeft->m_isLastPos);
			m_isLastPos.merge(m_pRight->m_isLastPos);			
			
			break;
			
		default:
			
			assert(false);
		}
		
	}
	
public:
	
	inline bool getLookahead(void) const {
		
		assert(_EQ(CHARSET, m_nType));
		return m_bLookahead;
	}
	
	inline void setLookahead(bool a_bLA) {
		
		assert(_EQ(CHARSET, m_nType));
		m_bLookahead = a_bLA;
	}
	
	inline bool getEOR(void) const {
		
		assert(_EQ(CHARSET, m_nType));
		return m_bEOR;
	}
	
	inline void setEOR(bool a_bEOR) {
		
		assert(_EQ(CHARSET, m_nType));
		m_bEOR = a_bEOR;
	}
	
	inline bool getEOF(void) const {
		
		assert(_EQ(CHARSET, m_nType));
		return m_bEOF;
	}
	
	inline void setEOF(bool a_bEOF) {
		
		assert(_EQ(CHARSET, m_nType));
		m_bEOF = a_bEOF;
	}
	
	inline int getTreeID(void) const {
		
		assert(_EQ(CHARSET, m_nType));
		return m_nTreeID;
	}
	
	inline void setTreeID(int a_nTreeID) {
		
		assert(_EQ(CHARSET, m_nType));
		m_nTreeID = a_nTreeID;
	}
	
	inline int getActionIndex(void) const {
		
		assert(_EQ(CHARSET, m_nType));
		return m_nActionIndex;
	}
	
	inline void setActionIndex(int a_nActionIndex) {
		
		assert(_EQ(CHARSET, m_nType));
		m_nActionIndex = a_nActionIndex;
	}
	
	inline bool getNullable(void) const {
		
		return m_bNullable;
	}
	
	inline void setNullable(bool a_bNullable) {
			
		m_bNullable = a_bNullable;
	}
	
	inline iset_t& firstPos(void) {
		
		return m_isFirstPos;
	}
	
	inline iset_t& lastPos(void) {
		
		return m_isLastPos;
	}
	
	inline iset_t& followPos(void) {
		
		return m_isFollowPos;
	}
	
	inline node_type_t getType(void) const {
		
		return m_nType;
	}
	
	inline int getID(void) const {
		
		return m_nID;
	}
	
	inline charset_t* getCharSet(void) const {
		
		assert(_EQ(CHARSET, m_nType));
		return m_pCharSet;
	}
	
	inline renode_t* getChild(void) const {
		
		assert(_EQ(STAR, m_nType));
		return m_pChild;
	}
	
	inline renode_t* getLeft(void) const {
		
		assert(_EQ(CONCAT, m_nType) || _EQ(UNION, m_nType));
		return m_pLeft;
	}
	
	inline renode_t* getRight(void) const {
		assert(_EQ(CONCAT, m_nType) || _EQ(UNION, m_nType));
		return m_pRight;
	}	
	
	~renode_t(void) {
		
		if(_EQ(CHARSET, m_nType)) {
			
			delete m_pCharSet;
		}
	}

};

// regular expression tree
class retree_t {
	
private:
	// root of tree
	renode_t *m_pRoot;
	
	// unique ID
	int m_nID;
	
	// index of code block associated with RE
	int m_nActionIndex;
	
	struct {
		// flag indicate if the regular expression must be matched at beginning of line
		bool m_bBOL:		1;
		// flag indicate if the regular expression is only matched with End-Of-File
		bool m_bEOF:		1;
		
		// reference count
		int m_nRef:			30;
	};
	
	friend class lexer_t;
	
public:
	
	inline retree_t(renode_t *a_pRoot, int a_nID, bool a_bBOL, bool a_bEOF, int a_nActionIndex = INVALID_ACTION)
	: m_pRoot(a_pRoot)
	, m_nID(a_nID)
	, m_nActionIndex(a_nActionIndex)
	, m_bBOL(a_bBOL)
	, m_bEOF(a_bEOF)
	, m_nRef(0) {
		
		assert(a_pRoot);
	}
	
	inline int getID(void) const {
		
		return m_nID;
	}
	
	inline renode_t* getRoot(void) const {
		
		return m_pRoot;
	}
	
	inline int incRef(void) {
		
		return ++m_nRef;
	}
	
	inline int decRef(void) {
		
		return --m_nRef;
	}
	
	inline bool getBOL(void) const {
		
		return m_bBOL;
	}
	
	inline void setBOL(bool a_bBOL) {
		
		m_bBOL = a_bBOL;
	}
	
	inline bool getEOF(void) const {
		
		return m_bEOF;
	}
	
	inline void setEOF(bool a_bEOF) {
		
		m_bEOF = a_bEOF;
	}
	
	inline int getActionIndex(void) const {
		
		return m_nActionIndex;
	}
	
	inline void setActionIndex(int a_nActionIndex) {
		
		m_nActionIndex = a_nActionIndex;
	}
	
	// output RE tree to a string
	void toString(string &a_strOut);
	
	// delete all nodes of the tree
	void clear(void);
};

typedef vector< renode_t* > rnpool_t;
typedef vector< retree_t* > rtpool_t;
typedef void (*renode_visitor_t)(renode_t*, void*);

// methods for RE subtree information updating, traveling each node of the tree
// generic algorithm for preorder tree-traversing
void treePreordTravel(renode_t* a_pRoot, renode_visitor_t a_vFunc, void *pv = nullptr);
void treeInordTravel(renode_t* a_pRoot, renode_visitor_t a_vFunc, void *pv = nullptr);


// update lookahead flags of all leaf nodes of the tree
void recusiveUpdateLookahead(renode_t *a_pNode, bool a_bFlag);
// update tree ID of all leaf nodes of the tree
void recursiveUpdateTreeID(renode_t *a_pNode, int a_nTreeID);
// update code index of all leaf nodes of the tree
void recursiveUpdateActionIndex(renode_t *a_pNode, int a_nActionIndex);
// clear FollowPos set of all nodes of the subtee
void recursiveClearFollowPos(renode_t *a_pNode);
// calculate FollowPos set of all nodes of the tree
void recursiveCalcFollowPos(renode_t* a_pRoot, rnpool_t& a_rpLeaves);

}

#endif //LEXER_RETREE_H__
