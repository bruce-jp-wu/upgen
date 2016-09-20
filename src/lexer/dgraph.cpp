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
#ifdef UPGEN_TEST_VERSION
// TODO: to be removed
using std::cerr;
using std::endl;
#include <iomanip>
using std::setw;
#endif

#include <cmath>
#include <string>
using std::string;

#include <iosfwd>
using std::ostream;

#include <algorithm>
using std::sort;
#include <queue>
using std::queue;

#include "./dgraph.h"
#include "../common/setsplitter.h"
using common_ns::superset_t;
using common_ns::vsubset_t;

#include "../common/unifind.h"
using common_ns::unifind_t;

namespace lexer_ns {

#define MAY_IN_GROUP(total, diff)				(diff * diff * diff < total)
//#define IN_GROUP(t, x, m)						(m * m * m * m < t * x)
#define IN_GROUP(total, label, diff)			(diff * diff * 4 < total * label)
#define IS_DENSE_STATE(t, x)					(x * 2 >= t)
#define IS_COMPRESSION_GOOD(t, x, m)			(IS_DENSE_STATE(t, x) && IS_DENSE_STATE(x, m))

// fill data(compressed next/check/base/default array) into transition tables
void dgraph_t::fillTransitions(void) {
	
	// filter states whose tranistion target are less than 2,
	// because they need not to compress
	
	int nBase = FIRST_STATE;
	while(nBase < (int)m_vpNodes.size() && m_vpNodes[nBase]->m_nLabels < 1) {
		
		++nBase;
	}
	if(_EQ((int)m_vpNodes.size(), nBase)) {
		
		assert(_EQ(m_vpvGenNodes.size(), 0));
		return;
	}
	
	// sort states so that states are arranged in non-increasing order
	rearrangeStates(dgnode_less_t(), nBase, (int)m_vpNodes.size());
	
	int nLabels = m_dTbl.getLabelCount();
	int i;
	table_t& tNext = m_dTbl.m_tNext;
	table_t& tCheck = m_dTbl.m_tCheck;
	table_t& tBase = m_dTbl.m_tBase;
	table_t& tDefault = m_dTbl.m_tDefault;
	
	int nTotalStates = (int)(m_dTbl.m_nDefaultState + m_vpvGenNodes.size() + 1);
	int nInvalidTarget = m_dTbl.m_nDefaultState;
	
	tBase.resize(nTotalStates, INVALID_STATE);
	tDefault.resize(nTotalStates, INVALID_STATE);
	
	nBase = FIRST_STATE;
	while(nBase < (int)m_vpNodes.size() && m_vpNodes[nBase]->m_nLabels < 1) {
		
		++nBase;
	}
	assert(nBase < (int)m_vpNodes.size());
	
	int nEnd = (int)m_vpNodes.size();
	int nCur = (int)tNext.size();
	for(i = nEnd - 1; i >= nBase; --i) {
		
		int nLow = 0, nHigh = nLabels - 1;
		while(nLow < nHigh && _EQ((*m_vpNodes[i]->m_pvnLabels)[nLow], INVALID_STATE)) {
			
			++nLow;
		}
		
		while(nHigh > nLow && _EQ((*m_vpNodes[i]->m_pvnLabels)[nHigh], INVALID_STATE)) {
			
			--nHigh;
		}
		
		++nHigh;
		
		assert(nHigh > nLow);
		
		int j = (nCur > nLow)? nCur: nLow;
		for(; /*j < ((int)tNext.size()) - (nHigh - nLow)*/; ++j) {
			
			int k = nLow;
			for(; k < nHigh; ++k) {
				
				int idx = j + k - nLow;
				if(idx < (int)tNext.size()
						&& tNext[idx] != INVALID_STATE
						&& (*m_vpNodes[i]->m_pvnLabels)[k] != INVALID_STATE) {
					
					break;
				}
			}
			
			if(_EQ(k, nHigh) || j >= (int)tNext.size()) {
				
				break;
			}
		}
	
		tBase[i] = j - nLow;
		tDefault[i] = m_vpNodes[i]->m_nDefault;
		
		while((int)tNext.size() < nLow) {
			
			tNext.push_back(INVALID_STATE);
			//tCheck.push_back(i);
			tCheck.push_back(nInvalidTarget);
		}
		int m = nLow;
		for(int k = j; k < (int)tNext.size() && m < nHigh; ++k, ++m) {
			
			if((*m_vpNodes[i]->m_pvnLabels)[m] != INVALID_STATE) {
				
				assert(_EQ(tNext[k], INVALID_STATE));
				tNext[k] = (*m_vpNodes[i]->m_pvnLabels)[m];
				tCheck[k] = i;
			}
		}
		for(; m < nHigh; ++m) {
			
			tNext.push_back((*m_vpNodes[i]->m_pvnLabels)[m]);
			if((*m_vpNodes[i]->m_pvnLabels)[m] != INVALID_STATE) {
				
				tCheck.push_back(i);
			}
			else {
			
				tCheck.push_back(nInvalidTarget);
			}
		}

		while(nCur < (int)tNext.size() && tNext[nCur] != INVALID_STATE) {
			
			++nCur;
		}
	}
	
	if(m_vpvGenNodes.size() > 0) {
		
		nLabels = (int)m_vpvGenNodes[0]->size();
		// compression generated states
		
		for(i = ((int)m_vpvGenNodes.size()) - 1; i >= 0; --i) {
			
			int nState = nInvalidTarget + 1 + i;
			int nLow = 0, nHigh = nLabels - 1;
			while(nLow < nHigh && _EQ((*m_vpvGenNodes[i])[nLow], INVALID_STATE)) {
				
				++nLow;
			}
			
			while(nHigh > nLow && _EQ((*m_vpvGenNodes[i])[nHigh], INVALID_STATE)) {
				
				--nHigh;
			}
			
			++nHigh;
			
			assert(nHigh > nLow);
			
			int j = (nCur > nLow)?nCur: nLow;
			for(; /*j < ((int)tNext.size()) - (nHigh - nLow)*/; ++j) {
				
				int k = nLow;
				for(; k < nHigh; ++k) {
					
					int idx = j + k - nLow;
					if(idx < (int)tNext.size()
							&& tNext[idx] != INVALID_STATE
							&& (*m_vpvGenNodes[i])[k] != INVALID_STATE) {
						
						break;
					}
				}
				if(_EQ(k, nHigh) || j >= (int)tNext.size()) {
					
					break;
				}
			}

			tBase[nState] = j - nLow;
			tDefault[nState] = INVALID_STATE;
			
			int m = nLow;
			for(int k = j; k < (int)tNext.size() && m < nHigh; ++k, ++m) {
				
				if((*m_vpvGenNodes[i])[m] != INVALID_STATE) {
					
					assert(_EQ(tNext[k], INVALID_STATE));
					tNext[k] = (*m_vpvGenNodes[i])[m];
					tCheck[k] = nState;
				}
			}
			for(; m < nHigh; ++m) {
				
				tNext.push_back((*m_vpvGenNodes[i])[m]);
				tCheck.push_back(nState);
			}

			while(nCur < (int)tNext.size() && tNext[nCur] != INVALID_STATE) {
				
				++nCur;
			}
		}
	}
	
	// add special invalid target
	tBase[nInvalidTarget] = (int)tNext.size();
	tDefault[nInvalidTarget] = INVALID_STATE;
	// add additional entry for match any invalid chars
	for(i = 0; i <= m_dTbl.getLabelCount(); ++i) {
		
		tNext.push_back(INVALID_STATE);
		tCheck.push_back(nInvalidTarget);
	}
	
	// fill information for states without transitions
	for(i = 0; i < FIRST_STATE; ++i) {
		
		tBase[i] = tBase[nInvalidTarget];
		tDefault[i] = nInvalidTarget;
	}
	for(i = FIRST_STATE; i < nBase; ++i) {
		
		tBase[i] = tBase[nInvalidTarget];
		tDefault[i] = m_vpNodes[i]->m_nDefault;
	}
}

// fill in accept table
void dgraph_t::fillAccepts(void) {
	
	table_t& tAccept = m_dTbl.m_tAccept;
	
	int i = 0;
	for(i = 0; i < FIRST_STATE; ++i) {
		
		tAccept.push_back(INVALID_RULE);
	}
	for(i = FIRST_STATE; i < (int)m_vpNodes.size(); ++i) {
		
		tAccept.push_back(m_vpNodes[i]->m_nRule);
	}
}

// transform state representation from arc to array,
// make it easier to compress table
// during this time, default state is also computed
void dgraph_t::transformRep(void) {
	
	int i;
	
	const vint_t& vnMap = m_dTbl.m_tCharMap;
	int nLabels = m_dTbl.getLabelCount();
	assert(nLabels >= 0 && nLabels <= 256);
	
	for(i = FIRST_STATE; i < (int)m_vpNodes.size(); ++i) {
		
		assert(m_vpNodes[i]->m_poutArc);
		vgarc_t& arc = *(m_vpNodes[i]->m_poutArc);
		
		vint_t *pvn = new vint_t(nLabels, INVALID_STATE);
		int m = 0;
		sint_t si;
		for(int j = 0; j < (int)arc.size(); ++j) {
			
			ustring ustr;
			(arc[j])->getCharSet().getSetChars(ustr);
			
			for(int k = 0; k < (int)ustr.size(); ++k) {
			
				if((*pvn)[vnMap[(unsigned char)ustr[k]]] != INVALID_STATE) {
				
					assert(_EQ((*pvn)[vnMap[(unsigned char)ustr[k]]],  (arc[j])->m_ntoState));
				}
				else {
					
					++m;
					(*pvn)[vnMap[(unsigned char)ustr[k]]] = (arc[j])->m_ntoState;
					si.insert((arc[j])->m_ntoState);
				}
			}
		}
		
		// WARNING: THE SEQUENCE OF DELETING BEFORE SWITHING IS ESSENTIAL
		delete m_vpNodes[i]->m_pinArc;
		delete m_vpNodes[i]->m_poutArc;
		
		// switch representation
		m_vpNodes[i]->m_bArc = false;
		m_vpNodes[i]->m_pvnLabels = pvn;
		m_vpNodes[i]->m_nLabels = m;
		m_vpNodes[i]->m_nTargets = (int)si.size();		
		
	}
	
	// arcs are not needed any more
	for(i = 0; i < (int)m_vpArcs.size(); ++i) {
		
		delete m_vpArcs[i];
	}
	
	m_vpArcs.clear();
	m_bArcRep = false;
}

void dgraph_t::updateAfterRearrange(void) {
	
	vint_t vnIdx(m_vpNodes.size());
	int i;
	// first we get index table from old index to new index
	for(i = FIRST_STATE; i < (int)m_vpNodes.size(); ++i) {
		
		vnIdx[m_vpNodes[i]->m_nID] = i;
	}
	
	int nLabels = m_dTbl.getLabelCount();
	// update state information
	for(i = FIRST_STATE; i < (int)m_vpNodes.size(); ++i) {
		
		for(int j = 0; j < nLabels; ++j) {
			
			if(INVALID_STATE != (*m_vpNodes[i]->m_pvnLabels)[j]) {
				
				(*m_vpNodes[i]->m_pvnLabels)[j] = vnIdx[(*m_vpNodes[i]->m_pvnLabels)[j]];
			}
		}
		
		m_vpNodes[i]->m_nID = i;
		
		// NOTE: it is possible that index of default state is greater than
		// the greatest index in m_vpNodes
		if(m_vpNodes[i]->m_nDefault != INVALID_STATE
				&& m_vpNodes[i]->m_nDefault < (int)m_vpNodes.size()) {
		
			m_vpNodes[i]->m_nDefault = vnIdx[m_vpNodes[i]->m_nDefault];
		}
	}
	// update start-condition information
	for(i = 0; i < m_dTbl.getStartCount(); ++i) {
		
		int j = m_dTbl.getStartIndex(i, true);
		assert(j != INVALID_STATE);
		j = vnIdx[j];
		int k = m_dTbl.getStartIndex(i, false);
		if(k != INVALID_STATE) {
			
			k = vnIdx[k];
		}
		m_dTbl.setStartIndex(i, j, k);
	}
	
	for(i = 0; i < (int)m_vpvGenNodes.size(); ++i) {
		
		for(int j = 0; j < (int)m_vpvGenNodes[0]->size(); ++j) {
			
			if(INVALID_STATE != (*m_vpvGenNodes[i])[j]) {
				
				(*m_vpvGenNodes[i])[j] = vnIdx[(*m_vpvGenNodes[i])[j]];
			}
		}		
	}
}

// minimization of DFA
void dgraph_t::minimize(void) {
	
	superset_t stateSets(FIRST_STATE, (int)m_vpNodes.size());
	vsubset_t vsStates;
	int i;

	i2si_map_t rule2Stats;
	for(i = FIRST_STATE; i < (int)m_vpNodes.size(); ++i) {
		
		if(m_vpNodes[i]->getRule() != INVALID_RULE) {
			
			i2si_it_t it = rule2Stats.find(m_vpNodes[i]->getRule());
			if(it != rule2Stats.end()) {
				
				it->second->insert(i);
			}
			else {
				
				sint_t *psi = new sint_t;
				psi->insert(i);
				rule2Stats.insert(
						i2si_pair_t(m_vpNodes[i]->getRule(), psi));
			}
		}
	}
	
	for(i2si_it_t it = rule2Stats.begin(); it != rule2Stats.end(); ++it) {
		
		assert(it->second);
		stateSets.split(*it->second);
		delete it->second;
	}
	rule2Stats.clear();
	
	vint_t& vnMap = m_dTbl.m_tCharMap;
	int nLabels = m_dTbl.getLabelCount();
	assert(nLabels > 0);
	bool *pbFlags = new bool[nLabels];
	sint_t **psiStats = new sint_t*[nLabels];
	memset(psiStats, 0, sizeof(sint_t*) * nLabels);
	
	bool bChanged = true;
	while(bChanged) {
		
		bChanged = false;
		
		stateSets.getSubsets(vsStates);
		for(i = ((int)vsStates.size()) - 1; i >= 0; --i) { // sets of states

			for(int j = ((int)vsStates[i]->size()) - 1; j >= 0; --j) { // set of states
				
				dgraph_node_t *pNode = m_vpNodes[(*vsStates[i])[j]];
				for(int k = 0; k < (int)pNode->getInArcCount(); ++k) { // set of arcs

					dgraph_arc_t* parc = pNode->getInArc(k);
					ustring ustr;
					parc->getCharSet().getSetChars(ustr);
					memset(pbFlags, 0, sizeof(bool) * nLabels);
					for(int m = 0; m < (int)ustr.size(); ++m) {
						
						pbFlags[vnMap[(unsigned char)ustr[m]]] = true;
					}
					for(int m = 0; m < nLabels; ++m) {
						
						if(pbFlags[m]) {
							
							if(psiStats[m]) {
								
								psiStats[m]->insert(parc->m_nfromState);
							}
							else {
								psiStats[m] = new sint_t;
								psiStats[m]->insert(parc->m_nfromState);
							}
						}
					}
				}
			}
			
			for(int m = 0; m < nLabels; ++m) {
				
				if(psiStats[m]) {
					
					if(stateSets.split(*psiStats[m])) {
						
						bChanged = true;
					}
					delete psiStats[m];
					psiStats[m] = nullptr;
				}
			}
		}

		for(i = 0; i < (int)vsStates.size(); ++i) {
			
			delete vsStates[i];
		}
		vsStates.clear();
	}
	delete[] pbFlags;
	delete[] psiStats;
	
	stateSets.getSubsets(vsStates);

	// merge states, update transition labels
	if(m_vpNodes.size() - FIRST_STATE > vsStates.size()) {
		
		vint_t vnStateMap(m_vpNodes.size());
		// representative for undistingishable set of states
		vgnode_t repNodes;
		
		for(i = 0; i < FIRST_STATE; ++i) {
			
			vnStateMap[i] = i;
			repNodes.push_back(nullptr);
		}
		
		// merge states, compute representative for each undistingishable set of states
		for(i = 0; i < (int)vsStates.size(); ++i) {
			
			assert((*vsStates[i])[0] >=  FIRST_STATE);
			vnStateMap[(*vsStates[i])[0]] = (int)repNodes.size();
			repNodes.push_back(m_vpNodes[(*vsStates[i])[0]]);
			repNodes[FIRST_STATE + i]->m_nID = FIRST_STATE + i;
			for(int j = 1; j < (int)vsStates[i]->size(); ++j) {
				
				vnStateMap[(*vsStates[i])[j]] = FIRST_STATE + i;
				
				for(int k = 0; k < m_vpNodes[(*vsStates[i])[j]]->getInArcCount(); ++k) {
					
					repNodes[FIRST_STATE + i]->addInArc(m_vpNodes[(*vsStates[i])[j]]->getInArc(k));
				}
				for(int k = 0; k < m_vpNodes[(*vsStates[i])[j]]->getOutArcCount(); ++k) {
					
					repNodes[FIRST_STATE + i]->addOutArc(m_vpNodes[(*vsStates[i])[j]]->getOutArc(k));
				}
				delete m_vpNodes[(*vsStates[i])[j]];
			}
		}
		
		m_vpNodes.clear();
		m_vpNodes.assign(repNodes.begin(), repNodes.end());

		//update transition labels 
		for(i = 0; i < (int)m_vpArcs.size(); ++i) {
			
			assert(m_vpArcs[i]->m_nfromState >= 0);
			assert(m_vpArcs[i]->m_ntoState >= 0);
			
			m_vpArcs[i]->m_nfromState = vnStateMap[m_vpArcs[i]->m_nfromState];
			m_vpArcs[i]->m_ntoState = vnStateMap[m_vpArcs[i]->m_ntoState];
		}
		
		// update start-condition information
		for(i = 0; i < m_dTbl.getStartCount(); ++i) {
		
			int j = vnStateMap[m_dTbl.getStartIndex(i, true)];
			int k = m_dTbl.getStartIndex(i, false);

			if (k != INVALID_STATE) {

				k = vnStateMap[k];
			}
			
			m_dTbl.setStartIndex(i, j, k);
		}
		
	}
	
	for(i = 0; i < (int)vsStates.size(); ++i) {
		delete vsStates[i];
	}
}

// compress DFA
void dgraph_t::compress(void) {
	
	if(m_vpNodes.size() < MIN_TOTAL_TO_COMPRESS) {
		
		return;
	}
	
	// sort states so that states are arranged in non-increasing order
	rearrangeStates(dgnode_less_t());
	
	// filter states whose tranistion target are less than 2,
	// because they need not to compress
	
	int nLabels = m_dTbl.getLabelCount();
	int i = 0;
	vint_t vnDiff;
	vpvint_t vvi, vnTempStates;
	i2si_map_t mapDiffLabels;
	
	int nBase = FIRST_STATE;
	while(nBase < (int)m_vpNodes.size() && m_vpNodes[nBase]->m_nLabels <= 2) {
		
		++nBase;
	}
	if((int)m_vpNodes.size() - nBase < MIN_DENSE_TO_COMPRESS) {
		
		return;
	}
	
	int nStates = (int)m_vpNodes.size() - nBase;
	
	// keep it simple for following algorithm with a little loss of efficiency
	vgnode_t vgn;
	for(i = nBase; i < (int)m_vpNodes.size(); ++i) {
		
		vgn.push_back(m_vpNodes[i]);
	}
	
	unifind_t identStates(nStates);
	
	// find identical states
	for(i = 0; i < nStates; ++i) {
		
		for(int j = i + 1; j < nStates; ++j) {
			
			int k = 0;
			for(; k < nLabels; ++k) {
				
				if((*vgn[i]->m_pvnLabels)[k] != (*vgn[j]->m_pvnLabels)[k]) {
					
					break;
				}
			}
			
			if(_EQ(k, nLabels)) {
				
				identStates.merge(i, j);
			}
		}
	}

	identStates.getClasses(vvi);
	
	// merge identical states
	for(i = 0; i < (int)vvi.size(); ++i) {
		
		assert(vvi[i]);
		for(int j = 1; j < (int)vvi[i]->size(); ++j) {
			for(int k = 0; k < nLabels; ++k) {
				
				(*vgn[(*vvi[i])[j]]->m_pvnLabels)[k] = INVALID_STATE;
			}
			vgn[(*vvi[i])[j]]->m_nLabels = 0;
			vgn[(*vvi[i])[j]]->m_nTargets = 0;
			vgn[(*vvi[i])[j]]->m_nDefault = vgn[(*vvi[i])[0]]->m_nID;
		}
		vnDiff.push_back((*vvi[i])[0]);
	}
	
	for(i = 0; i < (int)vvi.size(); ++i) {
		
		delete vvi[i];
	}
	vvi.clear();
	
	vpsint_t vsi(nStates);
	vvi.resize(nStates, nullptr);
	for(i = 0; i < nStates; ++i) {
		vsi[i] = nullptr;
	}
	// find differences between states, and partition into groups of states, 
	// states in each group have few different transitions
	for(i = (int)vnDiff.size() - 1; i > 0; --i) {
		
		if(vsi[i]) {
			continue;
		}
		dgraph_node_t &nodi = *vgn[vnDiff[i]];
		int j = 0;
		while(j < i ) {
			
			int k = nodi.m_nLabels - vgn[vnDiff[j]]->m_nLabels;
			if(MAY_IN_GROUP(nLabels, k)) {
				
				break;
			}
			++j;
		}
		
		int nPrev = INVALID_INDEX;
		int nDiff = (nLabels + 1) * (nLabels + 1);
		for(; j < i; ++j) {
			
			int m = 0;
			int step = 1;
			if(!vsi[vnDiff[j]]) {
				
				step = (int)sqrt(nLabels + 1);
				for(int k = 0; k < nLabels; ++k) {
					
					if((*nodi.m_pvnLabels)[k] != (*vgn[vnDiff[j]]->m_pvnLabels)[k]) {
						
						m += step;
					}
				}
			}
			else {
				
				step = (int)sqrt((nLabels + 1) / ((int)vsi[vnDiff[j]]->size() + 1));
				if(!step) {
					step = 1;
				}
				
				for(int k = 0; k < nLabels; ++k) {
					
					if((*nodi.m_pvnLabels)[k] != (*vgn[vnDiff[j]]->m_pvnLabels)[k]) {
						
						m += step;
					}
					else {
						
						for(sint_cit_t cit = vsi[vnDiff[j]]->begin();
							cit != vsi[vnDiff[j]]->end(); ++cit) {
							
							if((*nodi.m_pvnLabels)[k] != (*vgn[*cit]->m_pvnLabels)[k]) {
								
								m += step;
								break;
							}
						}
					}
				}
			}

			if(nDiff > m) {
				
				nPrev = vnDiff[j];
				nDiff = m;
			}
		}
		
		if(IN_GROUP(nLabels, nodi.m_nLabels, nDiff)) {
			
			if(!vvi[vnDiff[i]]) {
				
				vvi[vnDiff[i]] = new vint_t;
			}
			vvi[vnDiff[i]]->push_back(nPrev);
			
			if(!vsi[nPrev]) {
				
				vsi[nPrev] = new sint_t;
			}
			vsi[nPrev]->insert(vnDiff[i]);
		}
	}
	
	for(i = nStates - 1; i >= 0; --i) {
		
		if(vvi[i]) {

			assert(vvi[i]->size() == 1);
			if(vsi[i]) {

				vsi[(*vvi[i])[0]]->erase(i);
			}
		}
	}

	// reduce number of transitions of states in each group by introducing a new state
	for(i = 0; i < nStates; ++i) {
		
		if(0 == vgn[i]->m_nLabels) {
			
			continue;
		}
		
		if(!vsi[i] && !vvi[i]) {
			
			// figure out a state is 'good' enough to compress,
			// that is, many valid labels and few different kinds of labels
			if(IS_DENSE_STATE(nLabels, vgn[i]->m_nLabels)) {
				
				i2si_map_t target2Labels;
				
				for(int k = 0; k < nLabels; ++k) {
					
					i2si_it_t it = target2Labels.find((*vgn[i]->m_pvnLabels)[k]);
					if(it != target2Labels.end()) {
						
						it->second->insert(k);
					}
					else {
						
						sint_t *psi = new sint_t;
						psi->insert(k);
						target2Labels.insert(i2si_pair_t((*vgn[i]->m_pvnLabels)[k], psi));
					}
				}
				
				int nMax = 0, nMax2 = 0, nTarget = 0, nTarget2 = INVALID_STATE;
				for(i2si_cit_t cit = target2Labels.begin(); cit != target2Labels.end(); ++cit) {
					
					if(cit->first != INVALID_STATE && cit->second && (int)cit->second->size() > nMax) {
						
						nMax2 = nMax;
						nTarget2 = nTarget;
						
						nMax = (int)cit->second->size();
						nTarget = cit->first;
					}
				}
				
				if(nMax2 * 2 < nMax || _EQ(nMax, nMax2)) {
					
					if(IS_COMPRESSION_GOOD(nLabels, vgn[i]->m_nLabels, nMax)) {
						
						vint_t *pvi = new vint_t(nLabels, INVALID_STATE);
						for(int k = 0; k < nLabels; ++k) {
							
							if(_EQ(nTarget, (*vgn[i]->m_pvnLabels)[k])) {
							
								(*pvi)[k] = nTarget;
								(*vgn[i]->m_pvnLabels)[k] = INVALID_STATE;
							}
							else if((*vgn[i]->m_pvnLabels)[k] != INVALID_STATE) {
								
								(*pvi)[k] = WILD_STATE;
							}
						}
						
						vgn[i]->m_nLabels -= nMax;
						vgn[i]->m_nTargets -= 1;
						vgn[i]->m_nDefault = (int)(m_dTbl.m_nDefaultState + 1 + vnTempStates.size());
						vnTempStates.push_back(pvi);
					}
				}
				else {
					
					if(IS_COMPRESSION_GOOD(nLabels, vgn[i]->m_nLabels, (nMax + nMax2) * 2 / 3)) {
						
						vint_t *pvi = new vint_t(nLabels, INVALID_STATE);
						for(int k = 0; k < nLabels; ++k) {
							
							if(_EQ(nTarget, (*vgn[i]->m_pvnLabels)[k])) {
							
								(*pvi)[k] = nTarget;
								(*vgn[i]->m_pvnLabels)[k] = INVALID_STATE;
							}
							else if(_EQ(nTarget2, (*vgn[i]->m_pvnLabels)[k])) {
								
								(*pvi)[k] = nTarget2;
								(*vgn[i]->m_pvnLabels)[k] = INVALID_STATE;
							}
							else if((*vgn[i]->m_pvnLabels)[k] != INVALID_STATE) {
								
								(*pvi)[k] = WILD_STATE;
							}
						}
						
						vgn[i]->m_nLabels -= nMax + nMax2;
						vgn[i]->m_nTargets -= 2;
						vgn[i]->m_nDefault = (int)(m_dTbl.m_nDefaultState + 1 + vnTempStates.size());
						vnTempStates.push_back(pvi);
					}
				}

				
				for(i2si_it_t it = target2Labels.begin(); it != target2Labels.end(); ++it) {
					
					if(it->second) {
						
						delete it->second;
					}
				}
				target2Labels.clear();
			}
		}
		else if(vsi[i]) {
						
			bool bEmpty = true;
			bool bWild = true;
			sint_t &si = *vsi[i];
			vint_t *pvi = new vint_t(nLabels, INVALID_STATE);
			for(int k = 0; k < nLabels; ++k) {
				
				int ntarg = (*vgn[i]->m_pvnLabels)[k];
				bWild = (ntarg != INVALID_STATE);
				sint_cit_t cit = si.begin();
				for(; cit != si.end(); ++cit) {
					
					if((*vgn[*cit]->m_pvnLabels)[k] != ntarg) {
						
						bWild = bWild && ((*vgn[*cit]->m_pvnLabels)[k] != INVALID_STATE);
						break;
					}
				}
				if(cit != si.end()) {
					
					if(bWild) {
						
						for(; cit != si.end(); ++cit) {
							
							bWild = bWild && ((*vgn[*cit]->m_pvnLabels)[k] != INVALID_STATE);
						}
					}
					
					if(bWild) {
						
						(*pvi)[k] = WILD_STATE;
					}
					continue;
				}
				
				bEmpty = bEmpty && (ntarg == INVALID_STATE);
				(*pvi)[k] = ntarg;
				if((*vgn[i]->m_pvnLabels)[k] != INVALID_STATE) {
					
					--vgn[i]->m_nLabels;
				}
				(*vgn[i]->m_pvnLabels)[k] = INVALID_STATE;
				for(cit = si.begin(); cit != si.end(); ++cit) {
					
					if((*vgn[*cit]->m_pvnLabels)[k] != INVALID_STATE) {
						
						--vgn[*cit]->m_nLabels;
					}
					(*vgn[*cit]->m_pvnLabels)[k] = INVALID_STATE;
				}
			}
			
			if(bEmpty) {
				
				delete pvi;
			}
			else {
				
				vgn[i]->m_nDefault = (int)(m_dTbl.m_nDefaultState + 1 + vnTempStates.size());
				for(sint_cit_t cit = si.begin(); cit != si.end(); ++cit) {
					
					vgn[*cit]->m_nDefault = vgn[i]->m_nDefault;
				}
				vnTempStates.push_back(pvi);
			}
		}
	}
	
	for(i = 0; i < (int)vvi.size(); ++i) {
		
		if(vvi[i]) {
			
			delete vvi[i];
		}
	}
	vvi.clear();
	for(i = 0; i < (int)vsi.size(); ++i) {
		
		if(vsi[i]) {
			
			delete vsi[i];
		}
	}
	vsi.clear();
	
	// compress new states by merging identical columns
	identStates.destroy();
	identStates.init(nLabels);
	for(i = 0; i < nLabels - 1; ++i) {
		
		for(int j = i + 1; j < nLabels; ++j) {
			
			int k = 0;
			vint_t vnChanged;
			for(; k < (int)vnTempStates.size(); ++k) {
				
				if((*vnTempStates[k])[j] != (*vnTempStates[k])[i]) {
				
					if(_EQ((*vnTempStates[k])[i], WILD_STATE)) {
						
						vnChanged.push_back(k + 1);
					}
					else if(_EQ((*vnTempStates[k])[j], WILD_STATE)) {
						
						vnChanged.push_back(-k - 1);

					}
					else {
						
						break;
					}
				}
			}
			
			if(_EQ(k, (int)vnTempStates.size())) {
				
				for(int m = 0; m < (int)vnChanged.size(); ++m) {
					
					if(vnChanged[m] > 0) {
						
						(*vnTempStates[vnChanged[m] - 1])[i] = (*vnTempStates[vnChanged[m] - 1])[j];
						
					}
					else {
						
						(*vnTempStates[-1 - vnChanged[m]])[j] = (*vnTempStates[-1 - vnChanged[m]])[i];
					}
				}
				identStates.merge(i, j);
			}
		}
	}
	
	vint_t& tMeta = m_dTbl.m_tMeta;
	identStates.getClasses(vvi);
	// 0: invalid label
	int nCnt = 0;
	for(i = 0; i < (int)vvi.size(); ++i) {
			
		for(int j = 0; j < (int)vvi[i]->size(); ++j) {
			
			tMeta[(*vvi[i])[j]] = nCnt;
		}
		++nCnt;
	}
	m_dTbl.setMetaCount(nCnt);
	
	for(i = 0; i < (int)vnTempStates.size(); ++i) {
		
		vint_t *pv = new vint_t;
		m_vpvGenNodes.push_back(pv);
		
		for(int j = 0; j < (int)vvi.size(); ++j) {
			
			if(_EQ((*vnTempStates[i])[(*vvi[j])[0]], WILD_STATE)) {
				
				pv->push_back(INVALID_STATE);
			}
			else {
			
				pv->push_back((*vnTempStates[i])[(*vvi[j])[0]]);
			}
		}
	}

	for(i2si_it_t it = mapDiffLabels.begin(); it != mapDiffLabels.end(); ++it) {
		
		if(it->second) {
			
			delete it->second;
		}
	}
	
	for(i = 0; i < (int)vvi.size(); ++i) {
		
		if(vvi[i]) {
			
			delete vvi[i];
		}
	}
	
	for(i = 0; i < (int)vnTempStates.size(); ++i) {
		
		if(vnTempStates[i]) {
			
			delete vnTempStates[i];
		}
	}
}

// convert graph representation to table representation
void dgraph_t::toDTables(void) {

////	if(m_vpNodes.size() < MIN_SIZE_TO_MINIMIZE || m_vpNodes.size() > MAX_SIZE_TO_MINIMIZE) {
//		
//		minimize();
////	}

	m_dTbl.m_nDefaultState = (int)m_vpNodes.size();
	
	transformRep();
	
	compress();

	fillTransitions();

	fillAccepts();

}

void dgraph_t::init(void) {

	for(int i = 0; i < FIRST_STATE; ++i) {
		m_vpNodes.push_back(nullptr);
	}
}

void dgraph_t::clear(void) {
	
	int i;
	for(i = FIRST_STATE; i < (int)m_vpNodes.size(); ++i) {
		
		delete m_vpNodes[i];
	}
	m_vpNodes.clear();
	for(i = 0; i < FIRST_STATE; ++i) {
		
		m_vpNodes.push_back(nullptr);
	}
	
	for(i = 0; i < (int)m_vpvGenNodes.size(); ++i) {
		
		delete m_vpvGenNodes[i];
	}
	m_vpvGenNodes.clear();
	
	for(i = 0; i < (int)m_vpArcs.size(); ++i) {
		
		delete m_vpArcs[i];
	}
	m_vpArcs.clear();
}


ostream& operator<<(ostream &os, const dgraph_t &src) {

	os << "\n\n";
	
	if(src.m_vpNodes.size() == 0) {
		
		return os;
	}
	
	vbool_t vbVisited(src.m_vpNodes.size(), false);

	int i;
	
	if(src.m_bArcRep) {
		
		for(i = 0; i < src.getStartCount(); ++i) {
			
			os << src.getStartName(i) << "-Full\n";
			
			int nState;
			queue<int> quStat;
			nState = src.getStartIndex(i, true);
			vbVisited[nState] = true;
			
			quStat.push(nState);		
			
			while(!quStat.empty()) {
				
				nState = quStat.front();
				quStat.pop();
				
				os << (*src.getNode(nState)) << "\n";
				
				for(int j = 0; j < src.getNode(nState)->getOutArcCount(); ++j) {
					
					dgraph_arc_t* parc = src.getNode(nState)->getOutArc(j);
					int newState = parc->getToState();
					if(!vbVisited[newState]) {
						
						quStat.push(newState);
						vbVisited[newState] = true;
					}
				}
			}
			
			os << "\n" << src.getStartName(i) << "-Nohat" << "\n";
			nState = src.getStartIndex(i, false);
			if(INVALID_STATE != nState && !vbVisited[nState]) {
				
				vbVisited[nState] = true;
				quStat.push(nState);
			}
			while(!quStat.empty()) {
				
				nState = quStat.front();
				quStat.pop();
				
				os << (*src.getNode(nState)) << "\n";			
				
				for(int j = 0; j < src.getNode(nState)->getOutArcCount(); ++j) {
					
					dgraph_arc_t* parc = src.getNode(nState)->getOutArc(j);
					int newState = parc->getToState();
					if(!vbVisited[newState]) {
						
						quStat.push(newState);
						vbVisited[newState] = true;
					}
				}			
			}
		}
	}
	else {
		
		int nLabels = src.m_dTbl.getLabelCount();
		
		for(i = 0; i < src.getStartCount(); ++i) {
			
			os << src.getStartName(i) << "-Full\n";
			
			int nState;
			queue<int> quStat;
			nState = src.getStartIndex(i, true);
			vbVisited[nState] = true;
			
			quStat.push(nState);
			
			while(!quStat.empty()) {
				
				nState = quStat.front();
				quStat.pop();
				
				const dgraph_node_t *pNode = src.getNode(nState);
				os << (*pNode) << "\n";
				
				for(int j = 0; j < nLabels; ++j) {

					int newState = pNode->getTargetState(j);
					if(newState != INVALID_STATE 
							&& !vbVisited[newState]) {
						
						quStat.push(newState);
						vbVisited[newState] = true;
					}
				}
			}
			
			os << "\n" << src.getStartName(i) << "-Nohat" << "\n";
			nState = src.getStartIndex(i, false);
			if(INVALID_STATE != nState && !vbVisited[nState]) {
				
				vbVisited[nState] = true;
				quStat.push(nState);
			}

			while(!quStat.empty()) {
				
				nState = quStat.front();
				quStat.pop();
				
				const dgraph_node_t *pNode = src.getNode(nState);
				os << (*pNode) << "\n";
				
				for(int j = 0; j < nLabels; ++j) {

					int newState = pNode->getTargetState(j);
					if(newState != INVALID_STATE 
							&& !vbVisited[newState]) {
						
						quStat.push(newState);
						vbVisited[newState] = true;
					}
				}
			}
		}
	}

	return os;
}

}
