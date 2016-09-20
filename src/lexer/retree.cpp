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

#include <string>
using std::string;
#include <stack>
using std::stack;
using std::pair;

#include "../common/common.h"
#include "../common/types.h"
#include "../common/uset.h"
using common_ns::charset_t;
#include "../common/strhelper.h"
using common_ns::strhelper_t;
#include "./retree.h"

namespace lexer_ns {

inline static void setLookahead(renode_t *a_pNode, void *a_pLAFlag) {
	
	if(_EQ(renode_t::CHARSET, a_pNode->getType())) {
		
		a_pNode->setLookahead( *((bool*)a_pLAFlag));
	}
}

inline static void setTreeID(renode_t *a_pNode, void *a_pTreeID) {
	
	if(_EQ(renode_t::CHARSET, a_pNode->getType())) {
		
		a_pNode->setTreeID( *((int*)a_pTreeID));
	}	
}

inline static void setActionIndex(renode_t *a_pNode, void *a_pActionIndex) {
	
	if(_EQ(renode_t::CHARSET, a_pNode->getType())) {
		
		a_pNode->setActionIndex( *((int*)a_pActionIndex));
	}		
}

inline static void resetFollowPos(renode_t *a_pNode, void *pv) {
	
	a_pNode->followPos().clear();
}

inline static void calcFPos(renode_t *a_pNode, void *pv) {
	
	rnpool_t *pnp = (rnpool_t*) pv;
	
	switch(a_pNode->getType()) {
	
	case renode_t::STAR:    {
		
	    	for(iset_t::iterator it = a_pNode->lastPos().begin(); it != a_pNode->lastPos().end(); ++it) {
	    		
	    		((*pnp)[*it])->followPos().merge(a_pNode->firstPos());
	        }
	    }

	    break;
	    
	case renode_t::CONCAT:    {
		
	        assert(a_pNode->getLeft() && a_pNode->getRight());
	
	        for(iset_t::iterator it = a_pNode->getLeft()->lastPos().begin(); 
	        	it != a_pNode->getLeft()->lastPos().end(); ++it) {
	           
	        	((*pnp)[*it])->followPos().merge(a_pNode->getRight()->firstPos());
	        }
	    }
	
		break;
		
	default:
		;// do nothing
	}
}

inline static void delNode(renode_t *a_pNode, void *pv) {
	
	delete a_pNode;
}

static void outputNode(renode_t *a_pNode, void *pv) {
	
	string *pstr = (string*)pv;
	switch(a_pNode->getType()){
	
	case renode_t::CHARSET: {
		
			ustring ustr;
			a_pNode->getCharSet()->getSetChars(ustr);
			
			
			*pstr += "[";
			*pstr += strhelper_t::fromInt(a_pNode->getID());
			*pstr += ":";
			
			if(ustr.size() > 0) {
				
				*pstr += strhelper_t::fromUint((unsigned char)ustr[0], 10);
			}
			for(int i = 1; i < (int)ustr.size(); ++i) {
				
				*pstr += ',';
				*pstr += strhelper_t::fromUint((unsigned char)ustr[i], 10);
			}
			*pstr += "; follow:";						
			for(iset_t::const_iterator cit = a_pNode->followPos().begin();
				cit != a_pNode->followPos().end(); ++cit) {
				
				*pstr += ' ';
				*pstr += strhelper_t::fromInt(*cit);
			}	
			*pstr += ']';
			
		}
		break;
	case renode_t::STAR: {
		
			*pstr += "[*; first:";
			
			for(iset_t::const_iterator cit = a_pNode->firstPos().begin();
				cit != a_pNode->firstPos().end(); ++cit) {
				
				*pstr += ' ';
				*pstr += strhelper_t::fromInt(*cit);
			}
			
			*pstr += "; last:";						
			for(iset_t::const_iterator cit = a_pNode->lastPos().begin();
				cit != a_pNode->lastPos().end(); ++cit) {
				
				*pstr += ' ';
				*pstr += strhelper_t::fromInt(*cit);
			}
			
			*pstr += ']';
		}
		break;
	case renode_t::CONCAT: {

			*pstr += "[CAT; first:";
		
			for(iset_t::const_iterator cit = a_pNode->firstPos().begin();
				cit != a_pNode->firstPos().end(); ++cit) {
				
				*pstr += ' ';
				*pstr += strhelper_t::fromInt(*cit);
			}
			
			*pstr += "; last:";						
			for(iset_t::const_iterator cit = a_pNode->lastPos().begin();
				cit != a_pNode->lastPos().end(); ++cit) {
				
				*pstr += ' ';
				*pstr += strhelper_t::fromInt(*cit);
			}
			
			*pstr += ']';
		}
		break;
		
	case renode_t::UNION: {
		
			*pstr += "|; first:";
			for(iset_t::const_iterator cit = a_pNode->firstPos().begin();
				cit != a_pNode->firstPos().end(); ++cit) {
				
				*pstr += ' ';
				*pstr += strhelper_t::fromInt(*cit);
			}
			
			*pstr += "; last:";						
			for(iset_t::const_iterator cit = a_pNode->lastPos().begin();
				cit != a_pNode->lastPos().end(); ++cit) {
				
				*pstr += ' ';
				*pstr += strhelper_t::fromInt(*cit);
			}
			
			*pstr += ']';
		}
		break;
		
	default:
		assert(false);
	}
}

void treePreordTravel(renode_t* a_pRoot, renode_visitor_t a_vFunc, void *pv) {
	
	stack<renode_t*> stNode;
	stNode.push(a_pRoot);
	while(!stNode.empty()) {
		
		a_pRoot = stNode.top();
		stNode.pop();
		while(renode_t::CHARSET != a_pRoot->getType()) {
			
			renode_t *p = a_pRoot;
			if(_EQ(renode_t::STAR, a_pRoot->getType())) {
			
				// STAR node
				a_pRoot = a_pRoot->getChild();
			}			
			else {
				
				// CONCAT or UNION node
				stNode.push(a_pRoot->getRight());
				a_pRoot = a_pRoot->getLeft();				
			}
			
			a_vFunc(p, pv);
		}
		
		// push CHARSET node popped most recently
		stNode.push(a_pRoot);		
		while(!stNode.empty() && _EQ(stNode.top()->getType(), renode_t::CHARSET)) {
			
			a_vFunc(stNode.top(), pv);
			stNode.pop();
		}
	}
}

typedef pair<renode_t*, bool> rb_pair_t;

void treeInordTravel(renode_t* a_pRoot, renode_visitor_t a_vFunc, void *pv) {
	
	stack< rb_pair_t > stNode;
	
	stNode.push(rb_pair_t(a_pRoot, false));
	while(!stNode.empty()) {
		
		rb_pair_t rb = stNode.top();
		
		if(!rb.second) {
			
			switch(rb.first->getType()) {
			
			case renode_t::CHARSET: {
				
					stNode.pop();
					a_vFunc(rb.first, pv);				
					break;
				}	
			case renode_t::STAR: {
				
					stNode.top().second = true;
					stNode.push(rb_pair_t(rb.first->getChild(), false));
					break;
				}	
			case renode_t::CONCAT:
			case renode_t::UNION: {
				
					stNode.top().second = true;
					stNode.push(rb_pair_t(rb.first->getLeft(), false));				
					break;
				}
			}
		}
		else {
			
			stNode.pop();
			if(_EQ(renode_t::CONCAT, rb.first->getType())
					|| _EQ(renode_t::UNION, rb.first->getType())) {
				
				stNode.push(rb_pair_t(rb.first->getRight(), false));
			}
			
			a_vFunc(rb.first, pv);
		}
	}
}

void recusiveUpdateLookahead(renode_t *a_pNode, bool a_bFlag) {
	
	treePreordTravel(a_pNode, setLookahead, (void*)&a_bFlag);
}

void recursiveUpdateTreeID(renode_t *a_pNode, int a_nTreeID) {
	
	treePreordTravel(a_pNode, setTreeID, (void*)&a_nTreeID);
}

void recursiveUpdateActionIndex(renode_t *a_pNode, int a_nActionIndex) {
	
	treePreordTravel(a_pNode, setActionIndex, (void*)&a_nActionIndex);
}

void recursiveClearFollowPos(renode_t *a_pNode) {
	
	treePreordTravel(a_pNode, resetFollowPos, nullptr);
}

void recursiveCalcFollowPos(renode_t* a_pRoot, rnpool_t& a_rpLeaves) {
	
	treePreordTravel(a_pRoot, calcFPos, (void*)&a_rpLeaves);
}

void retree_t::clear(void) {
	
	treePreordTravel(getRoot(), delNode, nullptr);
	m_pRoot = nullptr;
}

void retree_t::toString(string &a_strOut) {
	
	treePreordTravel(getRoot(), outputNode, (void*)&a_strOut);
}

}
