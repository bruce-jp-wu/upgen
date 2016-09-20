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
//#define UPGEN_TEST_VERSION
#ifdef UPGEN_TEST_VERSION
// TODO: to be removed
using std::cerr;
#endif

#include <iostream>
#include <iomanip>
using std::left;
using std::right;
using std::setw;
using std::endl;

#include <algorithm>
using std::sort;

#include "../common/unifind.h"
using common_ns::unifind_t;

#include "../main/upgmain.h"
#include "./pgraph.h"
#include "./prod.h"
#include "./lritem.h"
#include "./grammar.h"
#include "./ptable.h"

namespace parser_ns {

void pgraph_t::init(void) {
	
	for(int i = 0; i < FIRST_STATE; ++i) {
		m_vpNodes.push_back(nullptr);
	}
}

void pgraph_t::clear(void) {
	
	int i;
	for(i = FIRST_STATE; i < (int)m_vpNodes.size(); ++i) {
		delete m_vpNodes[i];
	}
	m_vpNodes.clear();
	
	for(i = 0; i < (int)m_ioArcs.size(); ++i) {
		delete m_ioArcs[i];
	}
	m_ioArcs.clear();
	
	for(i = 0; i < FIRST_STATE; ++i) {			
		m_vpNodes.push_back(nullptr);
	}
}

// create full tables(action and goto table), that is, not compressed;
// and computes shift/reduce, reduce/reduce conflicts, if any.
i2i_pair_t pgraph_t::createFullTable(vftbl_t &a_tAct, vftbl_t &a_tGoto, conflicts_t &a_cnft) {

	if(m_vpNodes.size() == 0) {
		return i2i_pair_t(0, 0);
	}
	
	a_tAct.resize(m_vpNodes.size(), nullptr);
	a_tGoto.resize(m_vpNodes.size(), nullptr);
	a_cnft.resize(m_vpNodes.size(), nullptr);
	int nTokens = m_refGrammar.getTokenCount();
	int nTypes = m_refGrammar.getTypeCount();
	
	m_refPTbl.m_bitValid.resize((int)m_vpNodes.size(), nTokens);
	
	sint_t siRule;
	int nMaxPrec, nDefRule;
	int i;
	i2i_pair_t rpair;
	rpair.first = 0;
	rpair.second = 0;
	
	// allocate full action, goto tables
	for(i = FIRST_STATE; i < (int)m_vpNodes.size(); ++i) {
		a_tAct[i] = new frow_t(i, nTokens, FT_CELL_ERROR, m_refPTbl.m_nERule);
		a_tGoto[i] = new frow_t(i, nTypes, FT_CELL_ERROR, m_refPTbl.m_nERule);
	}
	
	for(i = FIRST_STATE; i < (int)m_vpNodes.size(); ++i) {
		
		// first, fill shift-action or goto-action
		for(pgnode_t::arc_cit_t citArc = m_vpNodes[i]->outArcBegin();
			citArc != m_vpNodes[i]->outArcEnd();
			++citArc) {
			
			if(m_refGrammar.isToken((*citArc)->m_nSymbol)) {
				(*a_tAct[i])[(*citArc)->m_nSymbol].m_nCellType = FT_CELL_SHIFT;
				(*a_tAct[i])[(*citArc)->m_nSymbol].m_nState = (*citArc)->m_ntoState;
				a_tAct[i]->incValid(1);
				m_refPTbl.m_bitValid.set(i, (*citArc)->m_nSymbol);
			}
			else {
				(*a_tGoto[i])[(*citArc)->m_nSymbol - nTokens].m_nCellType = FT_CELL_GOTO;
				(*a_tGoto[i])[(*citArc)->m_nSymbol - nTokens].m_nGoto = (*citArc)->m_ntoState;
				a_tGoto[i]->incValid(1);
			}
		}
	
		// next, fill reduce-action, and compute shift/reduce,
		// reduce/reduce conflicts, if any
		for(i2si_cit_t citReduce = m_vpNodes[i]->reduceBegin();
			citReduce != m_vpNodes[i]->reduceEnd();
			++citReduce) {
			
			assert(citReduce->first < nTokens);
			assert(citReduce->second->size() > 0);
			
			nDefRule = *citReduce->second->begin();
			if(_EQ(FT_CELL_ERROR, (*a_tAct[i])[citReduce->first].m_nCellType)) {
				
				if(citReduce->second->size() > 1) {
					
					nMaxPrec = DEFAULT_PREC;					
					for(sint_cit_t scit = citReduce->second->begin();
						scit != citReduce->second->end();
						++scit) {
						
						if(m_refGrammar.getProd(*scit)->getPrec() > nMaxPrec) {
							siRule.clear();
							siRule.insert(*scit);
							nMaxPrec = m_refGrammar.getProd(*scit)->getPrec();
						}
						else if(_EQ(m_refGrammar.getProd(*scit)->getPrec(), nMaxPrec)) {
							siRule.insert(*scit);
						}
					}
					
					assert(siRule.size() > 0);
					nDefRule = *siRule.begin();
					if(siRule.size() > 1) {
						
						rpair.second += (int)siRule.size();
						// emit reduce/reduce conflict warning information
						if( ! a_cnft[i]) {
							a_cnft[i] = new i2cnft_map_t;
						}
						
						cnft_item_t* pcnft = new cnft_item_t(siRule);
						a_cnft[i]->insert(i2cnft_pair_t(citReduce->first, pcnft));
						
					}
					siRule.clear();
				}

				(*a_tAct[i])[citReduce->first].m_nCellType = FT_CELL_REDUCE;
				(*a_tAct[i])[citReduce->first].m_nProd = nDefRule;
				a_tAct[i]->incValid(1);
				
				m_refPTbl.m_bitValid.set(i, citReduce->first);
				
				if(m_refGrammar.getAcceptRule() == nDefRule) {
					assert(m_refPTbl.m_nAState == INVALID_STATE || m_refPTbl.m_nAState == i);
					m_refPTbl.m_nAState = i;
				}
			}
			else {
				
				assert(_EQ(FT_CELL_SHIFT, (*a_tAct[i])[citReduce->first].m_nCellType));
				
				const prod_t *pr = nullptr;
				if(citReduce->second->size() > 1) {
					
					int nCurPrec = -1;
					for(sint_cit_t scit = citReduce->second->begin();
						scit != citReduce->second->end();
						++scit) {
						
						if(m_refGrammar.getProd(*scit)->getPrec() > nCurPrec) {
							siRule.clear();
							siRule.insert(*scit);
						}
						else if(_EQ(m_refGrammar.getProd(*scit)->getPrec(), nCurPrec)) {
							siRule.insert(*scit);
						}
					}
					
					pr = m_refGrammar.getProd(*siRule.begin());
					if(siRule.size() > 1) {
						
						rpair.second += (int)siRule.size();
						// emit reduce/reduce conflict warning information
						if( ! a_cnft[i]) {
							a_cnft[i] = new i2cnft_map_t;
						}
						
						cnft_item_t* pcnft = new cnft_item_t(siRule);
						a_cnft[i]->insert(i2cnft_pair_t(citReduce->first, pcnft));

					}
				}
				else {
					
					pr = m_refGrammar.getProd(*citReduce->second->begin());
				}
				
				assert(pr);
				
				int j = pr->getRightSize() - 1;
				for(; j >= 0; --j) {
					
					if(m_refGrammar.getSymbolObj(pr->getRightSymbol(j))->isToken()
							&& (pr->getRightItem(j)->m_nPrec > DEFAULT_PREC
							|| pr->getRightItem(j)->m_nAssoc == ASSOC_NO)) {
						
						break;
					}
				}
				
				const symbol_t *pToken = m_refGrammar.getSymbolObj(citReduce->first);
				int nRID = INVALID_INDEX;
				if(pToken->getPrec() > DEFAULT_PREC) {
					
					if(j >= 0) {
						
						int nPrevPrec = pr->getRightItem(j)->m_nPrec;
					
						if(nPrevPrec > pToken->getPrec()) {
							
							nRID = pr->getID();
						}
						else if(nPrevPrec == pToken->getPrec()) {
							
							switch(pr->getRightItem(j)->m_nAssoc) {
							
							case ASSOC_UNKNOWN: {
								// emit shift/reduce conflict warning information
								if( ! a_cnft[i]) {
									a_cnft[i] = new i2cnft_map_t;
								}
								
								cnft_item_t* pcnft = new cnft_item_t(
										(*a_tAct[i])[citReduce->first].m_nState, *citReduce->second);
								
								a_cnft[i]->insert(i2cnft_pair_t(citReduce->first, pcnft));
								rpair.first += 2;
							}
								break;
							case ASSOC_RIGHT:
								// shift, change nothing
								break;	
							case ASSOC_LEFT:
								
								nRID = pr->getID();
								
								break;

							default:
								assert(false);
								break;
								
							}// switch(pr->getRightItem(j)->m_nAssoc)
						}
					}//if(j >= 0)
				} //pToken->getPrec() > DEFAULT_PREC
				else {// 	shift is default action, change nothing
					
					if(j >= 0) {
						if(pr->getRightItem(j)->m_nSymbol == pToken->getID() && pToken->getAssoc() == ASSOC_NO) {
							string strMsg;
							strMsg = "associativity of the symbol `";
							strMsg += m_refGrammar.getSName(citReduce->first);
							strMsg += "\' is forbidden.";
							_ERROR(strMsg, ESEM(3));
						}
						else {
							
							if(pr->getRightItem(j)->m_nPrec > DEFAULT_PREC) {
								nRID = pr->getID();
							}
							else {
								// emit shift/reduce conflict warning information
								if( ! a_cnft[i]) {
									a_cnft[i] = new i2cnft_map_t;
								}
								
								cnft_item_t* pcnft = new cnft_item_t(
										(*a_tAct[i])[citReduce->first].m_nState, *citReduce->second);
								
								a_cnft[i]->insert(i2cnft_pair_t(citReduce->first, pcnft));
								rpair.first += 2;
							}
						}
					}
					else {
						// emit shift/reduce conflict warning information
						if( ! a_cnft[i]) {
							a_cnft[i] = new i2cnft_map_t;
						}
						
						cnft_item_t* pcnft = new cnft_item_t(
								(*a_tAct[i])[citReduce->first].m_nState, *citReduce->second);
						
						a_cnft[i]->insert(i2cnft_pair_t(citReduce->first, pcnft));
						rpair.first += 2;
					}
				}
				
				if(nRID != INVALID_INDEX) {
					(*a_tAct[i])[citReduce->first].m_nCellType = FT_CELL_REDUCE;
					(*a_tAct[i])[citReduce->first].m_nProd = nRID;
				}
			}
		}
	}
	
	return rpair;
}

// compress parse tables
void pgraph_t::compress(vftbl_t &a_ftAct, vftbl_t &a_ftGoto) {
	
	table_t& trv = m_refPTbl.m_tActRowVal;
	table_t& trn = m_refPTbl.m_tActRowNice;
	table_t& tcv = m_refPTbl.m_tColVal;
	table_t& tcn = m_refPTbl.m_tColNice;
	table_t& tgrv = m_refPTbl.m_tGotoRowVal;
	table_t& tgrn = m_refPTbl.m_tGotoRowNice;
	
	trv.resize(a_ftAct.size(), m_refPTbl.m_nERule);
	trn.resize(a_ftAct.size(), HIGHEST_NICE);
	
	tcv.resize(m_refGrammar.getSymbolCount(), m_refPTbl.m_nERule);
	tcn.resize(m_refGrammar.getSymbolCount(), HIGHEST_NICE);
	
	tgrv.resize(a_ftGoto.size(), m_refPTbl.m_nERule);
	tgrn.resize(a_ftGoto.size(), HIGHEST_NICE);
	
	int nTokens = m_refGrammar.getTokenCount();
	int nTypes = m_refGrammar.getTypeCount();
	int nTypeBase = m_refGrammar.getTypeBase();
	int nRuleBase = m_refPTbl.m_nEState + 1;
	
	int i, j, nices = 0;
	fcell_t actCell;
	bool bFound = true;	
	while(bFound && nices < HIGHEST_NICE) {
		
		bFound = false;
		
		for(i = FIRST_STATE; i < (int)a_ftAct.size(); ++i) {
		
			if( ! a_ftAct[i]) {
				continue;
			}
			
			actCell.m_nCellType = FT_CELL_ERROR;
			for(j = 0; j < nTokens; ++j) {
				if((*a_ftAct[i])[j].m_nCellType != FT_CELL_ERROR) {
					
					if(actCell.m_nCellType == FT_CELL_ERROR) {
						actCell = (*a_ftAct[i])[j];
					}
					if((*a_ftAct[i])[j] != actCell) {
						break;
					}
				}
			}
			
			if(j < nTokens) {
				continue;
			}
			
			if(actCell.m_nCellType != FT_CELL_ERROR) {
			
				trv[i] = (actCell.m_nCellType == FT_CELL_SHIFT)?
						actCell.m_nState: (actCell.m_nState + nRuleBase);
				trn[i] = nices;
				bFound = true;
			}
			
			delete a_ftAct[i];
			a_ftAct[i] = nullptr;
		}
		
		++nices;
		
		for(j = 0; j < nTokens; ++j) {
			
			actCell.m_nCellType = FT_CELL_ERROR;
			
			for(i = FIRST_STATE; i < (int)a_ftAct.size(); ++i) {
				
				if(a_ftAct[i]) {
					
					if((*a_ftAct[i])[j].m_nCellType != FT_CELL_ERROR) {
						
						if(actCell.m_nCellType == FT_CELL_ERROR) {
							actCell = (*a_ftAct[i])[j];
						}
						if((*a_ftAct[i])[j] != actCell) {
							break;
						}
					}
				}
			}
			
			if(i < (int)a_ftAct.size()) {
				continue;
			}
			
			if(actCell.m_nCellType != FT_CELL_ERROR) {
			
				tcv[j] = (actCell.m_nCellType == FT_CELL_SHIFT)?
						actCell.m_nState: (actCell.m_nState + nRuleBase);
				tcn[j] = nices;
				
				bFound = true;
				
				for(i = FIRST_STATE; i < (int)a_ftAct.size(); ++i) {
					if(a_ftAct[i]) {
						actCell = (*a_ftAct[i])[j].m_nCellType = FT_CELL_ERROR;
					}
				}
			}
		}
		
		++nices;
	}
	
	nices = 0;
	bFound = true;
	
	while(bFound && nices < HIGHEST_NICE) {
		
		bFound = false;
		
		for(i = FIRST_STATE; i < (int)a_ftGoto.size(); ++i) {
			if( ! a_ftGoto[i]) {
				continue;
			}
			
			actCell.m_nCellType = FT_CELL_ERROR;
			for(j = 0; j < nTypes; ++j) {
				if((*a_ftGoto[i])[j].m_nCellType != FT_CELL_ERROR) {
					if(actCell.m_nCellType == FT_CELL_ERROR) {
						actCell = (*a_ftGoto[i])[j];
					}
					if((*a_ftGoto[i])[j] != actCell) {
						break;
					}
				}
			}
			if(j < nTypes) {
				continue;
			}
			if(actCell.m_nCellType != FT_CELL_ERROR) {
			
				tgrv[i] = actCell.m_nGoto;
				tgrn[i] = nices;
				bFound = true;
			}

			delete a_ftGoto[i];
			a_ftGoto[i] = nullptr;
		}

		++nices;
		for(j = 0; j < nTypes; ++j) {
			actCell.m_nCellType = FT_CELL_ERROR;
			for(i = FIRST_STATE; i < (int)a_ftGoto.size(); ++i) {
				if(a_ftGoto[i]) {
					if((*a_ftGoto[i])[j].m_nCellType != FT_CELL_ERROR) {
						
						if(actCell.m_nCellType == FT_CELL_ERROR) {
							actCell = (*a_ftGoto[i])[j];
						}
						if((*a_ftGoto[i])[j] != actCell) {
							break;
						}
					}
				}
			}

			if( i < (int)a_ftGoto.size()) {
				continue;
			}

			if(actCell.m_nCellType != FT_CELL_ERROR) {
				
				tcv[j + nTypeBase] = actCell.m_nGoto;
				tcn[j + nTypeBase] = nices;
				
				bFound = true;
				
				for(i = FIRST_STATE; i < (int)a_ftGoto.size(); ++i) {
					if(a_ftGoto[i]) {
						(*a_ftGoto[i])[j].m_nCellType = FT_CELL_ERROR;
					}
				}
			}
		}
		++nices;
	}
}

// fill action table using sparse matrix compression algorithm
void pgraph_t::toActTable(vftbl_t &a_ftAct) {
	
	int i, j, k;
	int nRows = m_refPTbl.m_nEState;
	int nRuleBase = nRows + 1;
	int nTokens = m_refGrammar.getTokenCount();
	
	table_t& tNext = m_refPTbl.m_tParse;
	table_t& tBase = m_refPTbl.m_tBase;

	int nInvalid_State = m_refPTbl.m_nERule;
	
	tBase.resize(nRows, 0);
	
	int nCur = 0;
	for(i = FIRST_STATE; i < nRows; ++i) {
		
		if( a_ftAct[i])  {

			int nLow = 0, nHigh = nTokens - 1;
			while(nLow < nHigh && _EQ((*a_ftAct[i])[nLow].m_nCellType, FT_CELL_ERROR)) {
				++nLow;
			}
			while(nHigh > nLow && _EQ((*a_ftAct[i])[nHigh].m_nCellType, FT_CELL_ERROR)){
				--nHigh;
			}
			
			++nHigh;
			
			if(nHigh <= nLow) {
				tBase[i] = 0;
				
				delete a_ftAct[i];
				a_ftAct[i] = nullptr;
				continue;
			}
//			j = (nCur > nLow)? nCur: nLow;
			j = nCur;
			while(j < (int)tNext.size()) {
				
				k = nLow;
				for(; k < nHigh; ++k) {
					
					int idx = j + k - nLow;
					if(idx < (int)tNext.size()
							&& tNext[idx] != nInvalid_State
							&& (*a_ftAct[i])[k].m_nCellType != FT_CELL_ERROR) {
						
						break;
					}
				}
				
				if(_EQ(k, nHigh)) {
					
					break;
				}
				
				++j;
			}
	
			tBase[i] = j - nLow;
			while((int)tNext.size() < nLow) {
				tNext.push_back(nInvalid_State);
			}
			int m = nLow;
			for(k = j; k < (int)tNext.size() && m < nHigh; ++k, ++m) {
				
				switch((*a_ftAct[i])[m].m_nCellType) {
				case FT_CELL_SHIFT:
					assert(_EQ(tNext[k], nInvalid_State));
					tNext[k] = (*a_ftAct[i])[m].m_nState;
					break;
				case FT_CELL_REDUCE:
					assert(_EQ(tNext[k], nInvalid_State));
					tNext[k] = (*a_ftAct[i])[m].m_nProd + nRuleBase;					
					break;
				default:
					break;// do nothing
				}
			}
			
			for(; m < nHigh; ++m) {
				
				switch((*a_ftAct[i])[m].m_nCellType) {
				case FT_CELL_SHIFT:
					tNext.push_back((*a_ftAct[i])[m].m_nState);
					break;
				case FT_CELL_REDUCE:
					tNext.push_back((*a_ftAct[i])[m].m_nProd + nRuleBase);
					break;
				default:
					tNext.push_back(nInvalid_State);
					break;
				}
			}
	
			while(nCur < (int)tNext.size() && tNext[nCur] != nInvalid_State) {
				
				++nCur;
			}
			
			delete a_ftAct[i];
			a_ftAct[i] = nullptr;
		}
	}
	
	if(tNext.size() == 0) {
		tNext.push_back(0);
	}
}

// fill goto table using sparse matrix compression algorithm
void pgraph_t::toGotoTable(vftbl_t &a_ftGoto) {
	
	int i, j, k;
	int nRows = m_refPTbl.m_nEState;
	int nTypes = m_refGrammar.getTypeCount();
	
	table_t& tNext = m_refPTbl.m_tGoto;
	table_t& tBase = m_refPTbl.m_tGBase;
	
	int nInvalid_State = m_refPTbl.m_nERule;
	
	tBase.resize(nRows, 0);
	
	int nCur = 0;
	for(i = FIRST_STATE; i < nRows; ++i) {
		
		if( a_ftGoto[i]) {
			
			int nLow = 0, nHigh = nTypes - 1;
			while(nLow < nHigh && _EQ((*a_ftGoto[i])[nLow].m_nCellType, FT_CELL_ERROR)) {
				++nLow;
			}
			while(nHigh > nLow && _EQ((*a_ftGoto[i])[nHigh].m_nCellType, FT_CELL_ERROR)) {
				--nHigh;
			}
			
			++nHigh;
			
			if(nHigh <= nLow) {
				tBase[i] = 0;
				delete a_ftGoto[i];
				a_ftGoto[i] = nullptr;
				continue;
			}
			
//			j = (nCur > nLow)? nCur: nLow;
			j = nCur;
			while(j < (int)tNext.size()) {
				
				k = nLow;
				for(; k < nHigh; ++k) {
					
					int idx = j + k - nLow;
					if(idx < (int)tNext.size()
							&& tNext[idx] != nInvalid_State
							&& (*a_ftGoto[i])[k].m_nCellType != FT_CELL_ERROR) {
						
						break;
					}
				}
				
				if(_EQ(k, nHigh)) {
					
					break;
				}
				++j;
			}
		
			tBase[i] = j - nLow;
			
			while((int)tNext.size() < nLow) {
				
				tNext.push_back(nInvalid_State);
			}
			int m = nLow;
			for(k = j; k < (int)tNext.size() && m < nHigh; ++k, ++m) {
				
				if((*a_ftGoto[i])[m].m_nCellType == FT_CELL_GOTO) {
					assert(_EQ(tNext[k], nInvalid_State));
					tNext[k] = (*a_ftGoto[i])[m].m_nState;
				}
			}
			for(; m < nHigh; ++m) {
				
				if((*a_ftGoto[i])[m].m_nCellType == FT_CELL_GOTO) {
					tNext.push_back((*a_ftGoto[i])[m].m_nState);
				}
				else {
					tNext.push_back(nInvalid_State);
				}
			}
	
			while(nCur < (int)tNext.size() && tNext[nCur] != nInvalid_State) {
				
				++nCur;
			}
			
			delete a_ftGoto[i];
			a_ftGoto[i] = nullptr;
		}
	}
	
	if(tNext.size() == 0) {
		tNext.push_back(0);
	}
}

// convert parse-graph to parse-table, report conflicts if any
void pgraph_t::grp2PTbl(gsetting_t &gsetup) {
	
	conflicts_t cfts;
	vftbl_t vtAct, vtGoto;
	
	m_refPTbl.m_nEState = (int)m_vpNodes.size();
	m_refPTbl.m_nERule = m_refPTbl.m_nEState + 1 + m_refPTbl.getRule2Actions().size();
	
	i2i_pair_t rpair = createFullTable(vtAct, vtGoto, cfts);
	
	if(gsetup.m_posDetail) {
		
		if(rpair.first > 0 || rpair.second > 0) {
	
				reportConflicts(cfts, rpair , *(ostream*)gsetup.m_posDetail);
		}
		*(ostream*)gsetup.m_posDetail << *this;
	}

	if(rpair.first > 0 || rpair.second > 0) {
		
		ostream *posLog = (ostream*)gsetup.m_posLog;
		*posLog << "conflicts:";
		if(rpair.first > 0) {
			*posLog << " " << rpair.first << " shift/reduce";
		}
		if(rpair.second > 0) {
			if(rpair.first > 0) {
				*posLog << "; " << rpair.second << " reduce/reduce";
			}
			else {
				*posLog << " " << rpair.second << " reduce/reduce";
			}
		}
		*posLog << "." << endl;
	}
	
	
	clear();
	
	compress(vtAct, vtGoto);
	
	toActTable(vtAct);

	toGotoTable(vtGoto);
}

// report conflicts(shift/reduce, reduce/reduce)
void pgraph_t::reportConflicts(const conflicts_t &a_cfts, const i2i_pair_t &rpair,  ostream &oss) {
	
	const vint_t& vnLine = m_refPTbl.getRule2LineNos();
	
	oss << endl;
	oss << "---------------------- Conflicts --------------------------------" << endl << endl;
	if( rpair.first > 0) {
		oss << "shift/reduce conflicts: " << rpair.first << "." << endl << endl;
	}
	if( rpair.second > 0) {
		oss << "reduce/reduce conflicts: " << rpair.second << "." << endl << endl;
	}
	for(int i = FIRST_STATE; i < (int)a_cfts.size(); ++i) {
		
		if(a_cfts[i]) {
			
			int nSRState = 0;
			int nRRState = 0;
			
			for(i2cnft_cit_t citCnft = a_cfts[i]->begin(); citCnft != a_cfts[i]->end(); ++citCnft) {
				if(citCnft->second->getShift() != INVALID_STATE) {
					nSRState += citCnft->second->getCount();
				}
				else {
					nRRState += citCnft->second->getCount();
				}
			}
			if(nSRState > 0) {
				oss << "state " << i << ", " << nSRState << " shift/reduce conflicts" << endl;
			}
			if(nRRState > 0) {
				oss << "state " << i << ", " << nRRState << " reduce/reduce conflicts" << endl;
			}
			
			if(nSRState + nRRState > 0) {
	
				for(i2cnft_cit_t citCnft = a_cfts[i]->begin(); citCnft != a_cfts[i]->end(); ++citCnft) {
					
					if(citCnft->second->getShift() != INVALID_STATE) {
						
						oss << "\tshift " << m_refGrammar.getSName(citCnft->first)
						    << ", go to " << citCnft->second->getShift()
						    << endl;
					}
	
					int nRule = citCnft->second->getDefRule();
					oss << "\tlook ahead " <<  m_refGrammar.getSName(citCnft->first)
						    << ", reduce by rule " << nRule
						    << "(default) at line " << vnLine[nRule]
						    << "." << endl;
					for(sint_cit_t citRule = citCnft->second->ruleBegin();
						citRule != citCnft->second->ruleEnd(); ++citRule) {
						oss << "\tlook ahead " <<  m_refGrammar.getSName(citCnft->first)
						    << ", reduce by rule " << *citRule
						    << " at line " << vnLine[*citRule] << endl;
					}
				}
			
				oss << endl;
			}
		}
	}
}


ostream& operator<<(ostream& os, const pgraph_t& src) {
	
	int i, ncnt;
	int ntok = 0, ntyp = 0;
	sint_t siRule;
	sint_t siToken;
	string str, str2;
	
	os << src.getGrammar() << endl;
	
	ostream::fmtflags ffs = os.flags();
	
	os << endl;
	os << "---------------------- LALR Items -----------------------------" << endl;
	os << "Number of states: " << src.getValidNodeCount() << endl;
	
	for(pgraph_t::node_cit_t cit = src.nodeBegin(); cit != src.nodeEnd(); ++cit) {
		
		os << "State " << (*cit)->getID() << endl;

		siToken.clear();
		
		ntok = ntyp = 0;
		ncnt = 0;
		
		for(pgnode_t::arc_cit_t citArc = (*cit)->outArcBegin();
			citArc != (*cit)->outArcEnd(); ++citArc) {
			
			for(pgarc_t::item_cit_t citItem = (*citArc)->itemBegin();
				citItem != (*citArc)->itemEnd(); ++citItem) {
				
				if(citItem->first.m_nDot == 1 && citItem->first.m_nRule != src.getGrammar().getAcceptRule()) {
					continue;
				}
				
				if(ncnt == 0) {
					os << endl;
				}
				++ncnt;
				
				const prod_t* ppr = src.getGrammar().getProd(citItem->first.m_nRule);
				
				os << "\t" << left << setw(4) << ppr->getID()
					<< src.getGrammar().getSName(ppr->getLeft());
				os << ":";
				
				for(i = 0; i < citItem->first.m_nDot - 1; ++i) {
					os << " " << src.getGrammar().getSName(ppr->getRightSymbol(i));
				}
				os << " .";
				for(; i < ppr->getRightSize(); ++i) {
					os << " " << src.getGrammar().getSName(ppr->getRightSymbol(i));
				}

				os << endl << endl;
			}
			
			if((*citArc)->getItemCount() > 0) {
				
				if((*citArc)->m_nSymbol < src.getGrammar().getTypeBase()) {
					++ntok;
					siToken.insert((*citArc)->m_nSymbol);
				}
				else {
					++ntyp;
				}
			}
		}
		
		if((*cit)->getReduceCount() > 0) {
		
			if(ncnt == 0) {
				os << endl;
			}
			siRule.clear();
			
			for(i2si_cit_t citReduce = (*cit)->reduceBegin();
				citReduce != (*cit)->reduceEnd(); ++citReduce) {
				
				for(sint_cit_t citRule = citReduce->second->begin();
					citRule != citReduce->second->end();
					++citRule) {
					
					siRule.insert(*citRule);
				}
			}
			
			for(sint_cit_t citRule = siRule.begin(); citRule != siRule.end(); ++citRule) {
	
				const prod_t* ppr = src.getGrammar().getProd(*citRule);
				
				os << "\t" << left << setw(4) << ppr->getID()
					<< src.getGrammar().getSName(ppr->getLeft());
				os << ":";
				for(i = 0; i < ppr->getRightSize(); ++i) {
					os << " " << src.getGrammar().getSName(ppr->getRightSymbol(i));
				}
				os << " .";

				os << endl << endl;
			}
		}
		
		if(ntok > 0) {
			os << endl;
			os << "\tshift action" << endl << endl;
		}
		for(pgnode_t::arc_cit_t citArc = (*cit)->outArcBegin();
			citArc != (*cit)->outArcEnd(); ++citArc) {
			
			if((*citArc)->m_nSymbol < src.getGrammar().getTypeBase()) {
				os << "\t\t" << left << setw(10) << src.getGrammar().getSName((*citArc)->m_nSymbol)
					<< "    ---->    " << "state " << (*citArc)->m_ntoState << endl << endl;
			}
		}
		
		if(ntyp > 0) {
			os << endl;
			os << "\tgoto action" << endl << endl;
		}
		
		for(pgnode_t::arc_cit_t citArc = (*cit)->outArcBegin();
			citArc != (*cit)->outArcEnd(); ++citArc) {
			
			if((*citArc)->m_nSymbol >= src.getGrammar().getTypeBase()) {
				os << "\t\t" << left << setw(10) << src.getGrammar().getSName((*citArc)->m_nSymbol)
					<< "    ---->    " << "state " << (*citArc)->m_ntoState << endl << endl;
			}
		}
		
		if(siRule.size() > 0) {
			
			os << endl;
			os << "\treduce action" << endl << endl;
			
			if(siToken.size() == 0) {
				
				sint_cit_t citRule = siRule.begin();
				
				str = "[";
				str += src.getGrammar().getRuleLName(*citRule);
				str += "]";
				
				os << "\t\t($default), reduce to " << setw(12) << str
					<< " by rule " << *citRule /*<< " at line "
					<< src.getPTable().getRule2LineNos()[*citRule]*/
					<< endl << endl;
				
				++citRule;
				
				for(; citRule != siRule.end(); ++citRule) {
					
					str = " ";
					str += src.getGrammar().getRuleLName(*citRule);
					str += " ";
					
					os << "\t\t($default), reduce to  " << setw(12) << str
						<< "  by rule " << *citRule /*<< " at line "
						<< src.getPTable().getRule2LineNos()[*citRule]*/
						<< endl << endl;
				}
			}
			else {
				
				for(i2si_cit_t citReduce = (*cit)->reduceBegin();
					citReduce != (*cit)->reduceEnd(); ++citReduce) {
					
					sint_cit_t citRule = citReduce->second->begin();
					
					str2 = src.getGrammar().getSName(citReduce->first);
					str2 += ",";
					
					str = "[";
					str += src.getGrammar().getRuleLName(*citRule);
					str += "]";
					
					os << "\t\t" << setw(12) << str2
						<< " reduce to " << setw(12) << str
						<< " by rule " << *citRule /*<< " at line "
						<< src.getPTable().getRule2LineNos()[*citRule]*/
						<< endl << endl;
					
					++citRule;
					
					for(; citRule != citReduce->second->end(); ++citRule) {
						
						str2 = src.getGrammar().getSName(citReduce->first);
						str2 += ",";
						
						str = " ";
						str += src.getGrammar().getRuleLName(*citRule);
						str += " ";
						
						os << "\t\t" << setw(12) << str2
							<< " reduce to " << setw(12) << str
							<< " by rule " << *citRule /*<< " at line "
							<< src.getPTable().getRule2LineNos()[*citRule]*/
							<< endl << endl;
					}
				}
			}
		}
		
		siRule.clear();
		
		os << endl;
	}
	
	os.flags(ffs);
	
	return os;
}

}
