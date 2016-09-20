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

#include "../main/upgmain.h"
#include <iostream>
#include <iosfwd>
using std::ostream;
#include <queue>
using std::queue;

#include "./lexer.h"

#include "../common/common.h"
#include "../common/strhelper.h"
#include "../common/reporter.h"
#include "../common/uset.h"
using common_ns::iset_t;

namespace lexer_ns {

ostream& operator<<(ostream& os, const lexer_t &src) {
	
	for(int i = 0; i < (int)src.m_rTrees.size(); ++i) {
		
		string strSC = "<";
		int j = 0;
		for(; j < (int)src.m_scMgr.size(); ++j) {
			
			if(src.m_scMgr.containTree(j, i)) {
				
				if(_EQ(strSC.size(), 1)) {
					
					strSC += src.m_scMgr.getName(j);
				}
				else {

					strSC += ',';
					strSC += src.m_scMgr.getName(j);
				}
			}
		}
		
		strSC += '>';
		
		os << strSC << std::endl;
		
		strSC = "";
		
		src.m_rTrees[i]->toString(strSC);
		
		os << strSC << std::endl << std::endl;
		
	}
	
	return os;
}

lexer_t::lexer_t(void)
: m_bIsLookahead(false)
, m_charMap()
, m_nCurLaNodes(0) {
}

lexer_t::~lexer_t(void) {
}

void lexer_t::clearup(void) {
	
	m_bIsLookahead = false;
	m_nCurLaNodes = 0;
	
	m_rLeaves.clear();
	m_rInners.clear();
	m_rTrees.clear();
	
	m_charMap.clear();
	m_scMgr.clear();
	m_macMgr.clear();
	m_vnTrail.clear();
	m_vnLaNodes.clear();
	m_tree2Rule.clear();
	m_sc2EOFRule.clear();
}

// copy recursively from the tree rooted at @a_pSrc to the tree rooted at @a_pDest
void lexer_t::recursiveCopy(renode_t*& a_pDest, renode_t *a_pSrc) {

	renode_t* pLeft = nullptr;
	renode_t* pRight = nullptr;
	
	assert(a_pSrc);
	
	switch(a_pSrc->getType()) {
	
	case renode_t::CHARSET:
		
		assert(a_pSrc->m_pCharSet);
		a_pDest = addLeafNode(a_pSrc->m_pCharSet->clone(), a_pSrc->getNullable());
		a_pDest->setTreeID(a_pSrc->getTreeID());
		a_pDest->setActionIndex(a_pSrc->getActionIndex());
		break;
		
	case renode_t::STAR:
		
		recursiveCopy(pLeft, a_pSrc->getChild());
		assert(pLeft);
		a_pDest = addStarNode(pLeft);
		break;
		
	case renode_t::CONCAT:
		
		recursiveCopy(pLeft, a_pSrc->getLeft());
		assert(pLeft);
		
		recursiveCopy(pRight, a_pSrc->getRight());
		assert(pRight);
		
		a_pDest = addConcatNode(pLeft, pRight);
		a_pDest->setNullable(a_pSrc->getNullable());
		break;
		
	case renode_t::UNION:
		
		recursiveCopy(pLeft, a_pSrc->getLeft());
		assert(pLeft);
		
		recursiveCopy(pRight, a_pSrc->getRight());
		assert(pRight);
		
		a_pDest = addUnionNode(pLeft, pRight);
		a_pDest->setNullable(a_pSrc->getNullable());
		break;
		
	default:
		assert(false);
	}
}


// calculate FollowPos set of all nodes of the forest
void lexer_t::calcFollowPos(void) {

	for(int i = 0; i < (int)m_rTrees.size(); ++i) {
		recursiveCalcFollowPos(m_rTrees[i]->getRoot(), m_rLeaves);
	}
}

// create DFAs for each of start-condition
// there are at most 2 DFAs for each start-condition:
// one is so-called DFA with hat(^),
// which is converted from those REs with a leading hat(^),
// the other is so-called full DFA, including both DFA with and without hat(^)
bool lexer_t::retrees2DGraph(int a_nSC, dgraph_t &a_dGrp, dstate_set_t& dset) {
	
	vint_t vtIdx;
	
	assert(a_nSC >= 0 && a_nSC < (int)m_scMgr.size());
	m_scMgr.getTrees(a_nSC, vtIdx);
	
	if(vtIdx.size() == 0) {
		// useless start-condition
		return false;
	}
	
	int nscEOFRule = INVALID_RULE;
	int i = 0, nHat = 0;
	iset_t *pisetFull = new iset_t;
	iset_t *pisetNohat = new iset_t;
	
	// identify start state for both DFAs
	for(i = 0; i < (int)vtIdx.size(); ++i)	{

		assert(m_rTrees[vtIdx[i]]);
		pisetFull->merge(m_rTrees[vtIdx[i]]->getRoot()->firstPos());
		
		if(m_rTrees[vtIdx[i]]->getBOL()) {
			++nHat;
		}
		else {
			pisetNohat->merge(m_rTrees[vtIdx[i]]->getRoot()->firstPos());
		}
	}
	
	queue < dstate_info_t*> quDState;
	dstate_info_t* pDState = nullptr;

	i = a_dGrp.getNextState();
	if(_EQ(0, nHat)) {// no DFA with hat need to create
		
		pDState = new dstate_info_t(i,	pisetFull, new charset_t);
		pair<dstate_it_t, bool > ret = dset.insert(pDState);
		
		if(ret.second) {
			quDState.push(pDState);
			a_dGrp.newNode();
		}
		else {
			delete pDState;
			i = (*(ret.first))->m_nID;
		}
		
		a_dGrp.addStartIndex(m_scMgr.getName(a_nSC), i, i);
		delete pisetNohat;
	}
	else if(_EQ((int)vtIdx.size(), nHat)) {// DFA with hat is equal to full DFA
	
		pDState = new dstate_info_t(i,	pisetFull, new charset_t);
		pair<dstate_it_t, bool > ret = dset.insert(pDState);
		
		if(ret.second) {
			quDState.push(pDState);			
			a_dGrp.newNode();
		}
		else {
			delete pDState;
			i = (*(ret.first))->m_nID;
		}
		
		a_dGrp.addStartIndex(m_scMgr.getName(a_nSC), i, INVALID_STATE);
		delete pisetNohat;
	}
	else {
		// DFA with hat is different from full DFA
		// there 2 DFAs for start-condition
		int j = i + 1;
		
		pDState = new dstate_info_t(i,	pisetFull, new charset_t);
		
		pair<dstate_it_t, bool > ret = dset.insert(pDState);
		if(ret.second) {
			quDState.push(pDState);
			a_dGrp.newNode();
		}
		else {
			delete pDState;
			i = (*(ret.first))->m_nID;
		}
		
		pDState = new dstate_info_t(j,	pisetNohat, new charset_t);
		ret = dset.insert(pDState);
		
		if(ret.second) {

			quDState.push(pDState);
			a_dGrp.newNode();
		}
		else {
			
			delete pDState;
			j = (*(ret.first))->m_nID;	
		}
		
		a_dGrp.addStartIndex(m_scMgr.getName(a_nSC), i, j);
	}
	
	// breadth-first search to create DFA graphs
	while( ! quDState.empty()) {
		
		pDState = quDState.front();
		quDState.pop();
		dgraph_node_t *pgNode = a_dGrp.getNode(pDState->m_nID);

		for(iset_t::const_iterator cit = pDState->m_psubSet->begin();
					cit != pDState->m_psubSet->end(); ++cit) {
			
			assert(*cit >= 0 && *cit < (int)m_rLeaves.size());
			renode_t *prNode = m_rLeaves[*cit];
			
			if(prNode->getEOF()) {
				// keep track of EOF-Rule for each start-condition
				if(_EQ(INVALID_RULE, nscEOFRule)
						|| tree2Rule(prNode->getTreeID()) < nscEOFRule) {
					
					if(INVALID_RULE != nscEOFRule) {
						a_dGrp.getDTable().decRuleRef(nscEOFRule);
					}
					
					nscEOFRule = tree2Rule(prNode->getTreeID());
					a_dGrp.getDTable().addRuleRef(nscEOFRule);
				}
			}
			else {
				
				(*pDState->m_ptransLabel) |= (*prNode->getCharSet());
			
				if(prNode->getEOR()) {// current state is accept state
					
					if(_EQ(INVALID_RULE, pgNode->getRule())
							|| (tree2Rule(prNode->getTreeID()) < pgNode->getRule())) {
						
						if(INVALID_RULE != pgNode->getRule()) {
							
							a_dGrp.getDTable().decRuleRef(pgNode->getRule());
						}
						
						pgNode->setRule(tree2Rule(prNode->getTreeID()));
						a_dGrp.getDTable().addRuleRef(pgNode->getRule());
					}

					if(_EQ(INVALID_ACTION, pgNode->getAction())
								|| (prNode->getActionIndex() < pgNode->getAction())) {
						
						pgNode->setAction(prNode->getActionIndex());
					}
				}
			}
		}

		ustring ucLabels;
		pDState->m_ptransLabel->getSetChars(ucLabels);
		
		// compute out-going node for DFA graph
		dstate_set_t priNext;
		for(i = 0; i < (int)ucLabels.size(); ++i) {
			
			dstate_info_t* priNew = new dstate_info_t(INVALID_STATE,
					new iset_t, nullptr);
			
			for(iset_t::const_iterator cit = pDState->m_psubSet->begin();
					cit != pDState->m_psubSet->end(); ++cit) {

				if(m_rLeaves[*cit]->getCharSet()->test((unsigned char)ucLabels[i])) {
					
					priNew->m_psubSet->merge(m_rLeaves[*cit]->followPos());
				}
			}
			
			if(priNew->m_psubSet->size() == 0) {
				
				delete priNew;
			}
			else {
				
				pair < dstate_it_t, bool > ret = priNext.insert(priNew);
				if(!ret.second) {
					
					(*ret.first)->m_ptransLabel->set((unsigned char)ucLabels[i]);
					
					delete priNew;
				}
				else {
					
					priNew->m_ptransLabel = new charset_t((unsigned char)ucLabels[i]);
				}
			}			
		}
	
		for(dstate_it_t it = priNext.begin(); it != priNext.end(); ++it) {
			
			// update char map information
			assert(*it && (*it)->m_ptransLabel);
			(*it)->m_ptransLabel->getSetChars(ucLabels);
			
			m_charMap.addGroup(ucLabels);
			
			dgraph_arc_t *parc = a_dGrp.newArc(pgNode->getID(), INVALID_STATE, *((*it)->m_ptransLabel));
			
			pgNode->addOutArc(parc);
			
			pair < dstate_it_t, bool > ret = dset.insert(*it);
			if(ret.second) {
				
				// find new state, add new graph node
				(*it)->m_nID = a_dGrp.getNextState();
				dgraph_node_t *pgNew = a_dGrp.newNode();
				
				parc->setToState(pgNew->getID());
				pgNew->addInArc(parc);
				
				quDState.push(*it);
			}
			else {
				
				// use existing state
				assert(*ret.first);
				
				int nOldIdx = (*ret.first)->m_nID;
				parc->setToState(a_dGrp.getNode(nOldIdx)->getID());
				a_dGrp.getNode(nOldIdx)->addInArc(parc);
				
				delete *it;
			}
		}
		
		priNext.clear();
	}
	
	m_sc2EOFRule.push_back(nscEOFRule);
	
	for(i = 0; i < (int)vtIdx.size(); ++i)	{
		
		if(_EQ(0, m_rTrees[vtIdx[i]]->decRef())) {
			
			m_rTrees[vtIdx[i]]->clear();
			delete m_rTrees[vtIdx[i]];
			m_rTrees[vtIdx[i]] = nullptr;
		}
	}
	
	return true;
}

void lexer_t::retrees2DTables(dtable_t &a_dTbl, gsetting_t &gsetup) {
	
	dgraph_t dg(FIRST_STATE, a_dTbl);
	dstate_set_t dset;
	
	// first compute all followpos sets
	calcFollowPos();
	
	// next, convert REs to DFA graphs
	int i;
	for(i = 0; i < (int)m_scMgr.size(); ++i) {
		
		if(!retrees2DGraph(i, dg, dset)) {
			
			string strMsg = "Start-condition ";
			strMsg += m_scMgr.getName(i);
			strMsg += " is not referenced.";
			_WARNING(strMsg);
		}
	}
	
	for(dstate_it_t it = dset.begin(); it != dset.end(); ++it) {
		delete (*it);
	}
	
	// compute charset
	vustr_t vustr;
	m_charMap.getGroups(vustr);
	a_dTbl.updateCharmap(vustr);
	
	// check if there are lookahead states in DFA tables
	if(m_vnTrail.size() > 0) {
		vint_t vnRules;
		for(i = 0; i < (int)m_vnTrail.size(); ++i) {
			vnRules.push_back(tree2Rule(m_vnTrail[i]));
		}
		a_dTbl.checkLookahead(vnRules, m_vnLaNodes);
	}

	// convert DFA graphs to DFA tables
	dg.toDTables();
	// update DFAs and report warning about redundant information if there is any
	assert(nullptr != gsetup.m_posLog);
	a_dTbl.updateRules(m_sc2EOFRule, *(ostream*)gsetup.m_posLog);
	a_dTbl.addAdditionChars();
	
	// free resources
	clearup();
}

}
