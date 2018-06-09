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

#include "../main/upgmain.h"

//#define UPGEN_TEST_VERSION
#ifdef UPGEN_TEST_VERSION
// TODO: TBR
using std::cerr;
#endif

#include <iostream>
#include <iomanip>
using std::left;
using std::right;
using std::setw;
using std::endl;

#include "./grammar.h"

#include "../common/common.h"
#include "../common/reporter.h"

#include "../common/uset.h"
using common_ns::iset_t;
#include "../common/strhelper.h"
using common_ns::strhelper_t;

#include <queue>
using std::queue;

#include "../common/uniqueue.h"
using common_ns::uniqueue_t;

namespace parser_ns {

ostream& operator<<(ostream& os, const grammar_t &src) {
	
	os << endl;
	os << "---------------------- Grammar --------------------------------" << endl << endl;
	os << "Accept-symbol: " << src.getSName(src.getAcceptSymbol()) << endl << endl;
	os << "Start-symbol: " << src.getSName(src.getStartSymbol()) << endl << endl;
	os << "Terminals: " << endl;

	ostream::fmtflags ffs = os.flags();
	
	int i = 0, j = 0;
	string str;
	while(i < src.getTypeBase()) {
		
		const symbol_t *psymb = src.getSymbolObj(i);
		str = src.getSName(i);
		str += " (";
		str += src.getSVTName(psymb->getSVTID());
		str += ")";
		
		os << "\t";
		os << right << setw(4) << psymb->getID() << " " << left << setw(40) << str;
		
		++i;
		if(0 == i % 2) {
			os << endl;
		}
	}
	if( 1 == i % 2) {
		os << endl;
	}
	os << endl;
	
	os << "Variables: " << endl;
	while(i < src.getSymbolCount()) {
		
		
		const symbol_t *psymb = src.getSymbolObj(i);
		str = src.getSName(i);
		str += " (";
		str += src.getSVTName(psymb->getSVTID());
		str += ")";
		
		os << "\t";
		os << right << setw(4) << psymb->getID() << " " << left << setw(40) << str;

		++i;
		++j;
		if(j % 2 == 0) {
			os << endl;
		}
	}
	if(j % 2 == 1) {
		os << endl;
	}
	os << endl;
	
	os << "Productions: " << endl;
	for(i = 0; i < src.getRuleCount(); ++i) {
		
		const prod_t *pr = src.getProd(i);
		os << "\t" << right << setw(4) << pr->getID() << " " << src.getSName(pr->getLeft()) << ":";
		for(int j = 0; j < pr->getRightSize(); ++j) {
			
			os << " " << src.getSName(pr->getRightSymbol(j));
		}
		os << endl;
	}
	os << endl;
	
	os.flags(ffs);
	
	return os;
}
// clean up grammar object
void grammar_t::destroy(void) {

	int i = 0;
	m_vstrSName.clear();
	m_vstrSVT.clear();
	for(i = 0; i < (int)m_vsSymbol.size(); ++i) {
		delete m_vsSymbol[i];
	}
	m_vsSymbol.clear();
	m_sname2Idx.clear();
	m_svt2Idx.clear();
	for(i = 0; i < (int)m_vrRule.size(); ++i) {
		delete m_vrRule[i];
	}
	m_vrRule.clear();
	
	m_nMidRuleNum = 0;
	m_nStartSymbol = INVALID_INDEX;
	m_nAcceptSymbol = INVALID_INDEX;
	m_nAcceptRule = 0;
	m_nTypeBase = INVALID_INDEX;
	
	mt_nLastRuleIdx = INVALID_INDEX;
	mt_nFakeTypes = 1;
	
	mt_symb2Dstrct.clear();
	mt_svt2Dstrct.clear();
	m_vstrULSymb.clear();
	
	for(i = 0; i < (int)mt_vaBySymb.size(); ++i) {
		if(mt_vaBySymb[i]) {
			delete mt_vaBySymb[i];
		}
	}
	mt_vaBySymb.clear();
	
	for(i = 0; i < (int)mt_vaBySVT.size(); ++i) {
		if(mt_vaBySVT[i]) {
			delete mt_vaBySVT[i];
		}
	}
	mt_vaBySVT.clear();
}

// update information about %destructor action;
// the main job is to set connection between symbol table and %destructor action table
// by registering index of %destructor action into symbol table
bool grammar_t::initDstrct(gsetting_t &gsetup) {
	
	assert(mt_vaBySymb.size() > 0);
	if(mt_vaBySymb[0]) {
		// untagged symbol has no its semantic value type, so it's not corrent attempting
		// to invoke semantic value type in %destructor action of an untagged symbol 
		if(mt_vaBySymb[0]->getPHCount() > 0) {
			string strMsg = "%destructor at line ";
			strMsg += strhelper_t::fromInt(mt_vaBySymb[0]->getLineno());
			strMsg += "; description: untagged symbol has never semantic value type";
			_ERROR(strMsg, ESEM(1));
			
			return false;
		}
		// set %destructor action for symbol `$end', `$error', `$undef'
		m_vsSymbol[0]->setDstrctID(0);
		m_vsSymbol[1]->setDstrctID(0);
		m_vsSymbol[2]->setDstrctID(0);
	}
	
	int i, j, svtID;
	string strT;
	
	for(i = 3; i < (int)m_vstrSName.size(); ++i) {
			
		if(m_vsSymbol[i]->getDstrctID() != INVALID_INDEX) {
			// %destructor action is registered for the symbol
			continue;
		}
		
		svtID = m_vsSymbol[i]->getSVTID();
		
		s2i_cit_t cit = mt_symb2Dstrct.find(m_vstrSName[i]);
		if(cit != mt_symb2Dstrct.end()) {	// there is %destructor action declared for the symbol

			if(mt_vaBySymb[cit->second]->getPHCount() > 0) {

				if( INVALID_INDEX == svtID ) {	// but semantic value type is not declared for the symbol,
					// emit semantic error
								
					string strMsg = "%destructor at line ";
					strMsg += strhelper_t::fromInt(mt_vaBySymb[cit->second]->getLineno());
					strMsg += "; description: semantic value type of symbol `";
					strMsg += m_vstrSName[i];
					strMsg += "\' is not decleared";
					_ERROR(strMsg, ESEM(2));
					
					return false;
				}
				else {	// try to share %destructor action among symbols 
					
					mt_vaBySymb[cit->second]->getMacroPH(0, strT);
					if(strT.empty()) {	// the %destructor action is gone through for the first time
						for(j = 0; j < mt_vaBySymb[cit->second]->getPHCount(); ++j) {
							mt_vaBySymb[cit->second]->updateMacroPH(j, m_vstrSVT[svtID]);
						}
						m_vsSymbol[i]->setDstrctID(cit->second);
					}
					else if(strT == m_vstrSVT[svtID]){
						// both semantic value type and %destructor action are the same
						// so symbols can share the %destructor action
						m_vsSymbol[i]->setDstrctID(cit->second);
					}
					else { // different semantic value types have same %destructor,
						// each one has its own copy of %destructor action
						action_t *pnewAct = new action_t(*mt_vaBySymb[cit->second]);
						for(j = 0; j < pnewAct->getPHCount(); ++j) {
							pnewAct->updateMacroPH(j, m_vstrSVT[svtID]);
						}
						m_vsSymbol[i]->setDstrctID((int)mt_vaBySymb.size());
						mt_vaBySymb.push_back(pnewAct);
					}
				}
			}
			else { 	// no semantic value type invoked, it's safe to register it into symbol table
				m_vsSymbol[i]->setDstrctID(cit->second);
			}
		}
		else {	// no %destructor action declared for the symbol
			
			if(INVALID_INDEX != svtID) {
				i2i_cit_t citii = mt_svt2Dstrct.find(svtID);
				
				if(citii != mt_svt2Dstrct.end()) {	// there is a %destructor action
					// corresponding to the semantic value type
					
					assert( citii->second >= 0 && citii->second < (int)mt_vaBySVT.size());
					m_vsSymbol[i]->setDstrctID(-2 - citii->second);
				}
				else {
					if(mt_vaBySymb[1]) {	// there is default %destructor action for
						// tagged symbols
						
						// try to share default %destructor action among symbols 
						if(mt_vaBySymb[1]->getPHCount() > 0) {
							mt_vaBySymb[1]->getMacroPH(0, strT);
							if(strT.empty()) {	// the %destructor action is gone through for the first time
								for(j = 0; j < mt_vaBySymb[1]->getPHCount(); ++j) {
									mt_vaBySymb[1]->updateMacroPH(j, m_vstrSVT[svtID]);
								}
								m_vsSymbol[i]->setDstrctID(1);
							}
							else if(strT == m_vstrSVT[svtID]) {
								// both semantic value type and %destructor action are the same
								// so symbols can share the %destructor action
								m_vsSymbol[i]->setDstrctID(1);
							}
							else { // different semantic value types have same %destructor,
								// each one has its own copy of %destructor action
								action_t *pnewAct = new action_t(*mt_vaBySymb[1]);
								for(j = 0; j < pnewAct->getPHCount(); ++j) {
									pnewAct->updateMacroPH(j, m_vstrSVT[svtID]);
								}
								m_vsSymbol[i]->setDstrctID((int)mt_vaBySymb.size());
								mt_vaBySymb.push_back(pnewAct);
							}
						}
						else {
							m_vsSymbol[i]->setDstrctID(1);
						}
					}
				}
			}
			else {
				if(mt_vaBySymb[0]) {
					m_vsSymbol[i]->setDstrctID(0);
				}
			}
		}
	}
	
	// put all %destructor action to build a single table
	if(mt_vaBySVT.size() > 0) {
		
		for(i = 0; i < (int)m_vsSymbol.size(); ++i) {
			if(m_vsSymbol[i]->getDstrctID() < -1) {
				m_vsSymbol[i]->setDstrctID((int)mt_vaBySymb.size() - 2 - m_vsSymbol[i]->getDstrctID());
			}
		}
		
		// put all %destructor action to build a single table
		for(i = 0; i < (int)mt_vaBySVT.size(); ++i) {
			mt_vaBySymb.push_back(mt_vaBySVT[i]);
		}
		
	}
	
	assert(gsetup.m_posLog);
	updateDstrct(*(ostream*)gsetup.m_posLog);
	
	mt_vaBySVT.clear();
	mt_svt2Dstrct.clear();
	mt_symb2Dstrct.clear();
	
	return true;
}

// rearrange %destructor actions,
// remove  useless %destructor actions and feedback to user if any
void grammar_t::updateDstrct(ostream& oss) {

	assert(mt_vaBySymb.size() > 0);
	vint_t vnCnt(mt_vaBySymb.size(), 0);
	
	int i, j, k;
	for(i = 0; i < (int)m_vsSymbol.size(); ++i) {
		
		j = m_vsSymbol[i]->getDstrctID();
		if(INVALID_INDEX != j) {
			
			++vnCnt[j];
		}
	}
	
	j = 0;
	k = 0;
	for(i = 0; i < (int)mt_vaBySymb.size(); ++i) {
		
		if(mt_vaBySymb[i])  {
			
			if(vnCnt[i] > 0) {
				
				++j;
			}
			++k;
		}
	}
	
	if((int)mt_vaBySymb.size() > j) {
		
		if(k > j) {
			
			// there are some useless destructors, emit warnings
			for(i = 0; i < (int)mt_vaBySymb.size(); ++i) {
				
				if(mt_vaBySymb[i] && 0 == vnCnt[i]) {
					
					oss << "Warning: destructor at line "
						<<	mt_vaBySymb[i]->getLineno()
						<< " never referred.\n";
				}
			}
		}
		
		vint_t vnMap(mt_vaBySymb.size());
		k = 0;
		
		for(i = 0; i < (int)mt_vaBySymb.size(); ++i) {
			
			if(mt_vaBySymb[i] && vnCnt[i] > 0)  {
				
				mt_vaBySymb[k] = mt_vaBySymb[i];
				
				vnMap[i] = k;
				++k;
			}
			else {
				
				if(mt_vaBySymb[i]) {
					
					delete mt_vaBySymb[i];
					mt_vaBySymb[i] = nullptr;
				}
				vnMap[i] = j;
				++j;
			}
		}
		
		j = (int)mt_vaBySymb.size();
		for(; k < j; ++k) {
			
			mt_vaBySymb.pop_back();
		}
		
		for(i = 0; i < (int)m_vsSymbol.size(); ++i) {
			
			j = m_vsSymbol[i]->getDstrctID();
			if(INVALID_INDEX != j) {
				
				m_vsSymbol[i]->setDstrctID(vnMap[j]);
			}
		}
	}
	
}

// add tokens which are not defined in declaration section,
// but used in grammar definition section. 
void grammar_t::addTokens(const vstr_t & a_vstrSName, const vint_t &a_vnTokenID) {
	
	vint_t vnFlag(a_vstrSName.size(), -1);
	int n = (int)m_vstrSName.size();
	int k = (int)a_vstrSName.size();
	int i = 0;
	for(i = 0; i < k; ++i) {
		
		m_vstrSName.push_back("");
		m_vsSymbol.push_back(nullptr);
	}
	for(i = n - 1; i >= m_nTypeBase; --i) {
		
		s2i_it_t it = m_sname2Idx.find(m_vstrSName[i]);
		assert(it != m_sname2Idx.end());
		it->second += k;
		
		m_vstrSName[i + k] = m_vstrSName[i];
		m_vsSymbol[i + k] = m_vsSymbol[i];
		m_vsSymbol[i + k]->setID(i + k);
	}
	
	int s = m_nTypeBase;
	for(i = 0; i < (int)m_vrRule.size(); ++i) {
		
		prod_t *pr = m_vrRule[i];
		assert(pr);
		
		for(int j = -1; j < pr->getRightSize(); ++j) {
			
			prod_item_t *pri = pr->getRightItem(j);
			assert(pri);
			if(pri->m_nSymbol >= m_nTypeBase) {
				
				pri->m_nSymbol += k;
			}
			else if(pri->m_nSymbol < 0) {
				
				int m = - pri->m_nSymbol - 1;
				assert(m < k);
				
				if(_EQ(-1, vnFlag[m])) {
					
					assert(m_sname2Idx.find(a_vstrSName[m]) == m_sname2Idx.end());
//					assert( ! m_vsSymbol[s]);
					m_vsSymbol[s] = new symbol_t(s,	a_vnTokenID[m],
							 DEFAULT_SVT_ID, DEFAULT_PREC, ASSOC_UNKNOWN, false);
					m_sname2Idx.insert(s2i_pair_t(a_vstrSName[m], s));
					m_vstrSName[s] = a_vstrSName[m];					
					
					pri->m_nSymbol = s;
					vnFlag[m] = s;
					++s;
				}
				else {
					
					pri->m_nSymbol = vnFlag[m];
				}
			}
		}
	}
	
	m_nAcceptSymbol += k;
	m_nStartSymbol += k;
	m_nTypeBase += k;

	assert(_EQ(s, m_nTypeBase));
}

// compute nullable flag, FirstSet,
// and RLS set(RLS: Rules using the symbol as Left Symbol) for each symbol
void grammar_t::calcSymbSets(bool a_bClear) {
	
	int i, j;
	if(a_bClear) {
		for(i = 0; i < (int)m_vstrSName.size(); ++i) {
			symbol_t *psym = m_vsSymbol[i];
			psym->setNullable(false);
			if( ! psym->isToken()) {
				if(psym->m_pFirstSet) {
					psym->m_pFirstSet->clear();
				}
				if(psym->m_pRLSSet) {
					psym->m_pRLSSet->clear();
				}
			}
		}
	}
	
	bool bChanged = true;
	while(bChanged) {
		bChanged = false;
		for(i = 0; i < (int)m_vrRule.size(); ++i) {
			assert(m_vrRule[i]);
			assert(m_vrRule[i]->getLeft() >= m_nTypeBase);
			if( ! m_vsSymbol[m_vrRule[i]->getLeft()]->isNullable()) {
				int j = 0;
				for(; j < m_vrRule[i]->getRightSize(); ++j) {
					if( ! m_vsSymbol[m_vrRule[i]->getRightSymbol(j)]->isNullable()) {
						break;
					}
				}
				if(_EQ(j, m_vrRule[i]->getRightSize())) {
					
					m_vsSymbol[m_vrRule[i]->getLeft()]->setNullable(true);
					bChanged = true;
				}
			}
		}
	}

	bChanged = true;
	while(bChanged) {
		
		bChanged = false;
		for(i = 0; i < (int)m_vrRule.size(); ++i) {
			prod_t *pr = m_vrRule[i];
			int n = pr->getLeft();
			assert(n >= m_nTypeBase);
			
			if( ! m_vsSymbol[n]->m_pFirstSet) {
				m_vsSymbol[n]->m_pFirstSet = new iset_t;
			}
			
			j = 0;
			for(; j < pr->getRightSize(); ++j) {
				if(m_vsSymbol[pr->getRightSymbol(j)]->isToken()) {
					
					bChanged = m_vsSymbol[n]->m_pFirstSet->insert(pr->getRightSymbol(j)) || bChanged;
 				}
				else if(m_vsSymbol[pr->getRightSymbol(j)]->m_pFirstSet) {
					unsigned int oldSize = m_vsSymbol[n]->m_pFirstSet->size();
					m_vsSymbol[n]->m_pFirstSet->merge(*m_vsSymbol[pr->getRightSymbol(j)]->m_pFirstSet);
					bChanged = (m_vsSymbol[n]->m_pFirstSet->size() > oldSize) || bChanged;
				}
				if( ! m_vsSymbol[pr->getRightSymbol(j)]->isNullable()) {
					break;
				}
			}
		}
	}
	
	for(i = 0; i < (int)m_vrRule.size(); ++i) {
		prod_t *pr = m_vrRule[i];
		int nIdx = pr->getLeft();
		assert(nIdx >= 0 && nIdx < (int)m_vsSymbol.size());
		assert( ! m_vsSymbol[nIdx]->isToken());
		if( ! m_vsSymbol[nIdx]->m_pRLSSet) {
			m_vsSymbol[nIdx]->m_pRLSSet = new iset_t;
		}
		m_vsSymbol[nIdx]->m_pRLSSet->insert(i);
	}
}

// compute RCS set(RCS: rules which contain the symbol)
// for each symbol
void grammar_t::calcRCSSet(void) {

	for(int i = 0; i < (int)m_vrRule.size(); ++i) {
		prod_t *pr = m_vrRule[i];
		for(int j = -1; j < pr->getRightSize(); ++j) {
			int nIdx = pr->getRightSymbol(j);
			assert(nIdx >= 0 && nIdx < (int)m_vsSymbol.size());
			if( ! m_vsSymbol[nIdx]->m_pRCSSet) {
				m_vsSymbol[nIdx]->m_pRCSSet = new iset_t;
			}
			m_vsSymbol[nIdx]->m_pRCSSet->insert(i);
		}
	}
}

// delete RCS set of each symbol
void grammar_t::removeRCSSet(void) {
	for(int i = 0; i < (int)m_vsSymbol.size(); ++i) {
		if(m_vsSymbol[i]->m_pRCSSet) {
			delete m_vsSymbol[i]->m_pRCSSet;
			m_vsSymbol[i]->m_pRCSSet = nullptr;
		}
	}
}

void grammar_t::reportAdjustLog(const sint_t &a_siSymb, const sint_t &a_siRule, ostream& oss) {
	
	assert(a_siSymb.size() > 0);
	
	int i;
	if(a_siSymb.size() > 0) {
		
		i = 0;
		oss << "Warning: following symbols never referred" << endl;
		
		for(sint_cit_t citSymb = a_siSymb.begin(); citSymb != a_siSymb.end(); ++citSymb) {
			
			if((m_vstrSName[*citSymb])[0] != '\'') {
			
				m_vstrULSymb.push_back(m_vstrSName[*citSymb]);
			}
			
			oss << "\t" << m_vstrSName[*citSymb];
			++i;
			if(_EQ(0, i % 4)) {
				oss << endl;
			}
		}
		oss << endl;
	}
	
	if(a_siRule.size() > 0) {
		
		oss << "Warning: following grammar rules never referred" << endl;
		for(sint_cit_t citRule = a_siRule.begin(); citRule != a_siRule.end(); ++citRule) {
			
			oss << "\t" << setw(4) << m_vrRule[*citRule]->getID() + 1
				<< " " << m_vstrSName[m_vrRule[*citRule]->getLeft()];

			oss << " :";
			for(i = 0; i < m_vrRule[*citRule]->getRightSize(); ++i) {
				
				oss << " " << m_vstrSName[m_vrRule[*citRule]->getRightSymbol(i)];
			}
			oss << endl;
		}
	}
	
}

// adjust grammar by removing useless symbols, rules
void grammar_t::adjust(ptable_t & a_ptbl, gsetting_t &gsetup) {
	
	if(m_vrRule.size() < 1 || m_vstrSName.size() < 4) {
		// grammar is empty
		return;
	}
	
	calcSymbSets(false);
	calcRCSSet();
	
	int i = 0, j = 0;
	vbool_t vbUngen(m_vstrSName.size());
	for(i = 0; i < (int)m_vstrSName.size(); ++i) {
		vbUngen[i] = ( ! m_vsSymbol[i]->isToken());
	}

	//step 1: Compute ungeneratable variables
	bool bChanged = true;
	while(bChanged) {
		
		bChanged = false;
		for(i = 0; i < (int)m_vrRule.size(); ++i) {
			
			if( vbUngen[m_vrRule[i]->getLeft()]) {
				j = 0;
				for(; j < m_vrRule[i]->getRightSize()
					&& (! vbUngen[m_vrRule[i]->getRightSymbol(j)]);
					++j) {;}
				if(_EQ(m_vrRule[i]->getRightSize(), j)) {
					vbUngen[m_vrRule[i]->getLeft()] = false;
					bChanged = true;
				}
			}
		}
	}
	
	// step 2: Compute unreachable symbols
	vbool_t vbUnreach(m_vstrSName.size());
	uniqueue_t<int> quInt;

		quInt.push(m_nAcceptSymbol);
		// $end
		vbUnreach[0] = false;
		// $undefined
		vbUnreach[1] = false;
		// $error
		vbUnreach[2] = false;

	
	for(i = 3; i < (int)m_vstrSName.size(); ++i) {
		
		vbUnreach[i] = ( ! m_vsSymbol[i]->m_bUsed);
	}
	
	while(!quInt.isEmpty()) {
		
		int e;
		quInt.pop(e);
		
		if( ! vbUngen[e]) {
			
			vbUnreach[e] = false;
			if( ! m_vsSymbol[e]->isToken() && m_vsSymbol[e]->m_pRLSSet) {
				
				for(iset_t::const_iterator cit = m_vsSymbol[e]->m_pRLSSet->begin();
					cit != m_vsSymbol[e]->m_pRLSSet->end(); ++cit) {
					
					assert(*cit >= 0 && *cit < (int)m_vrRule.size());
					prod_t *pr = m_vrRule[*cit];
					for(i = 0; i < pr->getRightSize(); ++i) {
						
						int nsym = pr->getRightSymbol(i);
						if( ! vbUngen[nsym] && vbUnreach[nsym]) {
							
							quInt.push(nsym);
						}
					}
				}
			}
		}
	}
	
	// step 3: Compute useless symbols and rules
	// indices of useless symbols
	sint_t siULSymb;
	// indices of useless rules
	sint_t siULRule;

	for(i = 0; i < (int)m_vstrSName.size(); ++i) {
		
		if(vbUngen[i] || vbUnreach[i]) {
			
			siULSymb.insert(i);
			if( m_vsSymbol[i]->m_pRCSSet) {
				for(sint_cit_t cit = m_vsSymbol[i]->m_pRCSSet->begin();
					cit != m_vsSymbol[i]->m_pRCSSet->end(); ++cit) {
					
					siULRule.insert(*cit);
				}
			}
		}
	}
	
	removeRCSSet();
	
	if(siULSymb.size() > 0) {
		assert(nullptr != gsetup.m_posLog);
		reportAdjustLog(siULSymb, siULRule, *(ostream*)gsetup.m_posLog);
	}
	else {
		// need not to adjust grammar
		return;
	}
	
	if(siULSymb.size() ==  m_vstrSName.size()
			|| siULRule.size() == m_vrRule.size()) {
		
		destroy();
		return;
	}
	
	
	int nNewTypeBase = m_nTypeBase;
	vint_t idx2Symb, symb2Idx(m_vstrSName.size());
	vbool_t vbFlag;
	i = 0, j = 0;
	for(sint_cit_t citUL = siULSymb.begin(); citUL != siULSymb.end(); ++citUL, ++j) {
		
		for(; j < *citUL; ++i, ++j) {
			idx2Symb.push_back(j);
			symb2Idx[j] = i;
		}
		
		if(*citUL < m_nTypeBase) {
			--nNewTypeBase;
		}
	}
	
	for(; j < (int)m_vstrSName.size(); ++j, ++i) {
		idx2Symb.push_back(j);
		symb2Idx[j] = i;
	}
	
	int nNewSymbs = (int)idx2Symb.size();
	int nNewRules = (int)m_vrRule.size();
	for(sint_cit_t citY = siULSymb.begin(); citY != siULSymb.end(); ++citY, ++i) {
		
		idx2Symb.push_back(*citY);
		symb2Idx[*citY] = i;
	}
	
	// update start, accept and type base
	m_nStartSymbol = symb2Idx[m_nStartSymbol];
	m_nAcceptSymbol = symb2Idx[m_nAcceptSymbol];
	m_nTypeBase = nNewTypeBase;
	
	// update rules
	if(siULRule.size() > 0) {
		
		vint_t viRule;
		int nNewAcceptRule = m_nAcceptRule;
		
		i = 0;
		j = 0;
		for(sint_cit_t cit = siULRule.begin();
			cit != siULRule.end(); ++cit, ++j) {
			
			for(; j < *cit; ++i, ++j) {
				viRule.push_back(j);
			}
			if(*cit < m_nAcceptRule) {
				--nNewAcceptRule;
			}
		}
		for(; j < (int)m_vrRule.size(); ++j, ++i) {
			viRule.push_back(j);
		}
		
		nNewRules = (int)viRule.size();
		m_nAcceptRule = nNewAcceptRule;
		
		// update rule actions in parse table by using index map
		a_ptbl.updateRActions(viRule);
		
		for(sint_cit_t cit = siULRule.begin();
			cit != siULRule.end(); ++cit, ++i) {
			viRule.push_back(*cit);
		}
		
		vbFlag.resize(m_vrRule.size(), false);
		for(i = 0; i < (int)m_vrRule.size(); ++i) {
			if(vbFlag[i]) {
				continue;
			}
			prod_t *pr = m_vrRule[i];
			j = i;
			int k = viRule[j];
			while(i != k) {
				
				m_vrRule[j] = m_vrRule[k];
				m_vrRule[j]->setID(j);
				vbFlag[j] = true;
				j = k;
				k = viRule[k];
			}
			m_vrRule[j] = pr;
			vbFlag[j] = true;
		}
	}
	
	for(i = 0; i < (int)m_vrRule.size(); ++i) {
		
		prod_t *pr = m_vrRule[i];
		assert(pr);
		for(j = -1; j < pr->getRightSize(); ++j) {
			
			pr->setRightSymbol(j, symb2Idx[pr->getRightSymbol(j)]);
		}
	}
	
	vbFlag.clear();
	vbFlag.resize(m_vstrSName.size(), false);
	for(i = 0; i < (int)m_vstrSName.size(); ++i) {
		if(vbFlag[i]) {
			continue;
		}
		string strT = m_vstrSName[i];
		symbol_t *psym = m_vsSymbol[i];
		j = i;
		int k = idx2Symb[j];
		while(k != i) {
			m_vstrSName[j] = m_vstrSName[k];
			m_vsSymbol[j] = m_vsSymbol[k];
			m_vsSymbol[j]->setID(j);
			m_sname2Idx[m_vstrSName[j]] = j;
			
			vbFlag[j] = true;
			j = k;
			k = idx2Symb[k];
		}
		
		m_vstrSName[j] = strT;
		m_vsSymbol[j] = psym;
		m_vsSymbol[j]->setID(j);
		m_sname2Idx[m_vstrSName[j]] = j;
		vbFlag[j] = true;
	}
	
	for(i = (int)m_vstrSName.size() - 1; i >= nNewSymbs; --i) {
		
		m_sname2Idx.erase(m_vstrSName[i]);
		m_vstrSName.pop_back();
		delete m_vsSymbol[i];
		m_vsSymbol.pop_back();
	}
	
	for(i = (int)m_vrRule.size() - 1; i >= nNewRules; --i) {
		
		delete m_vrRule[i];
		m_vrRule.pop_back();
	}
	
	calcSymbSets(true);
}

// update token IDs
void grammar_t::updateTID(void) {
	
	for(int i = 3; i < m_nTypeBase; ++i) {
		if(_EQ(m_vsSymbol[i]->getTokenID(), INVALID_INDEX)) {
			m_vsSymbol[i]->setTokenID(m_nMaxTID);
			++m_nMaxTID;
		}
	}
}

// compute LR(1) closure
// @a_lrp: LR(0) kernel item
// @a_psiLa: lookahead symbols
// @a_la2Rules(a map): lookahead-symbol -> empty-rule
// @a_cls(a map): shift-symbol -> (Rule, Lookahead)s
void grammar_t::calcClosure(const lri_pair_t &a_lrp, const sint_t *a_psiLa,
		i2si_map_t &a_la2Rules, i2lrps_map_t &a_cls) const {
	
	assert(a_lrp.m_nRule >= 0 && a_lrp.m_nRule < (int)m_vrRule.size());
	assert(a_lrp.m_nDot >= 0 && a_lrp.m_nDot <= m_vrRule[a_lrp.m_nRule]->getRightSize());
	assert(a_cls.size() == 0);
	assert( ! m_vsSymbol[m_vrRule[a_lrp.m_nRule]->getRightSymbol(a_lrp.m_nDot)]->isToken());

	sint_t siMark;
	queue<lri_pair_t> quItem;
	queue<sint_t*> quSymb;
	sint_t si, *psi = const_cast<sint_t*>(a_psiLa);
	lri_pair_t rlpair;

	quItem.push(a_lrp);
	quSymb.push(psi);
	while( ! quItem.empty()) {
		
		rlpair = quItem.front();
		quItem.pop();
		psi = quSymb.front();
		quSymb.pop();
		
		prod_t *ppr = m_vrRule[rlpair.m_nRule];
		symbol_t *leftSymb = m_vsSymbol[ppr->getRightSymbol(rlpair.m_nDot)];
		si.clear();
		int i = rlpair.m_nDot + 1;
		for(; i < ppr->getRightSize(); ++i) {
			if(m_vsSymbol[ppr->getRightSymbol(i)]->isToken()) {
				si.insert(ppr->getRightSymbol(i));
				break;
			}
			else {
				si.insert(m_vsSymbol[ppr->getRightSymbol(i)]->m_pFirstSet->begin(), 
						m_vsSymbol[ppr->getRightSymbol(i)]->m_pFirstSet->end());
				if( ! m_vsSymbol[ppr->getRightSymbol(i)]->isNullable()) {
					break;
				}
			}
		}
		
		if(_EQ(i, ppr->getRightSize())) {
			si.insert(psi->begin(), psi->end());
		}
		
		assert(leftSymb->isType());
		
		for(iset_t::const_iterator cit = leftSymb->m_pRLSSet->begin();
			cit != leftSymb->m_pRLSSet->end(); ++cit) {

			assert(*cit >= 0 && *cit < (int)m_vrRule.size());

			if( m_vrRule[*cit]->getRightSize() <= 0) {
				
				for(sint_cit_t lacit = si.begin(); lacit != si.end(); ++lacit) {
					
					pair<i2si_it_t, bool> pairRet = a_la2Rules.insert(i2si_pair_t(*lacit, nullptr));
					if(pairRet.second) {
						pairRet.first->second = new sint_t;
					}
					pairRet.first->second->insert(*cit);
				}
			}
			else {

				symbol_t *firstSymb = m_vsSymbol[m_vrRule[*cit]->getRightSymbol(0)];
				int nSymbID = firstSymb->getID();

				pair<i2lrps_it_t, bool> pairRet = a_cls.insert(i2lrps_pair_t(nSymbID, nullptr));
				if(pairRet.second) {
					pairRet.first->second = new lrpair_set_t;
				}

				rlpair.m_nRule = *cit;
				for(sint_cit_t sit = si.begin(); sit != si.end(); ++sit) {
					
					rlpair.m_nLookahead = *sit;
					pairRet.first->second->insert(rlpair);
				}

				if(siMark.insert(*cit).second && ! firstSymb->isToken()) {

					rlpair.m_nDot = 0;
					quItem.push(rlpair);
					sint_t *psi2 = new sint_t;
					psi2->insert(si.begin(), si.end());
					quSymb.push(psi2);
				}
			}
		}

		if(a_psiLa != psi) {
			delete psi;
		}
	}
}

// compute propogated symbols
// @a_lrp: LR(0) kernel item
// @a_siRules: reduce rule ID(right part of the rule is empty)
// @a_shft2Rule(a map): symbol-to-be-shifted -> rule IDs
void grammar_t::calcPropogation(const lri_pair_t &a_lrp,
			sint_t &a_siRules, lrpair_set_t &a_shft2Rule) const {
	
	assert(a_lrp.m_nRule >= 0 && a_lrp.m_nRule < (int)m_vrRule.size());
	assert(a_lrp.m_nDot >= 0 && a_lrp.m_nDot <= m_vrRule[a_lrp.m_nRule]->getRightSize());
	assert(a_shft2Rule.size() == 0);
	assert( ! m_vsSymbol[m_vrRule[a_lrp.m_nRule]->getRightSymbol(a_lrp.m_nDot)]->isToken());
	
	sint_t siMark;
	queue<lri_pair_t> quItem;
	lri_pair_t rlpair;
	
	quItem.push(a_lrp);
	
	while( ! quItem.empty()) {
		
		rlpair = quItem.front();
		quItem.pop();
		
		int i = rlpair.m_nDot + 1;
		prod_t *ppr = m_vrRule[rlpair.m_nRule];
		symbol_t *leftSymb = m_vsSymbol[ppr->getRightSymbol(rlpair.m_nDot)];
		
		for(; i < ppr->getRightSize(); ++i) {
			if(m_vsSymbol[ppr->getRightSymbol(i)]->isToken()) {
				break;
			}
			else {
				if( ! m_vsSymbol[ppr->getRightSymbol(i)]->isNullable()) {
					break;
				}
			}
		}
		
		if(ppr->getRightSize() != i) {
			continue;
		}
		
		for(iset_t::const_iterator cit = leftSymb->m_pRLSSet->begin();
			cit != leftSymb->m_pRLSSet->end(); ++cit) {
			
			assert(*cit >= 0 && *cit < (int)m_vrRule.size());
			
			if( m_vrRule[*cit]->getRightSize() <= 0) {
				
				a_siRules.insert(*cit);
			}
			else {
				
				symbol_t *firstSymb = m_vsSymbol[m_vrRule[*cit]->getRightSymbol(0)];
				a_shft2Rule.insert(lri_pair_t(*cit, firstSymb->getID()));
				
				if(siMark.insert(*cit).second && ! firstSymb->isToken()) {

					rlpair.m_nRule = *cit;
					rlpair.m_nDot = 0;
					quItem.push(rlpair);
				}
			}
		}
	}
}

// convert from grammar object to parse graph
void grammar_t::gram2PGraph(pgraph_t &a_pgrp) {
	
	if(!isValid()) {
		return ;
	}
	
	/////////////////////////////////////////////////////////////////////
	// first create LR kernel items, build DFA graph  viable prefixes
	lrps2i_map_t lrs2Node;
	vlrps_t vlrPairs;
	i2i_map_t symb2Index;
	i2lrps_map_t lrkClosure;
	i2si_map_t symb2Rules;
	
	vpgarc_t varcTemp;
	queue< int > quNodeIndex;
	queue< pgarc_t* > quArc;
	
	// create start node(state)
	pgnode_t *pgn = nullptr, *pgNode = a_pgrp.addEmptyNode();
	lalr_item_t lalrItem(m_nAcceptRule, 0);
	pgarc_t *plrArc = new pgarc_t(INVALID_STATE, pgNode->getID(), m_nAcceptSymbol);
	plrArc->insertItem(lalrItem, END_SYMBOL_INDEX);
	pgNode->addInArc(plrArc);
	quNodeIndex.push(pgNode->getID());
	
	// breadth-first-search to build DFA graph for viable prefixes
	while(!quNodeIndex.empty()) {
		
		pgNode = a_pgrp.fromID(quNodeIndex.front());
		quNodeIndex.pop();

		// each node contains a set of out-going arcs and 
		// a set of in-coming arcs
		for(pgnode_t::arc_cit_t citArc = pgNode->inArcBegin();
			citArc != pgNode->inArcEnd();
			++citArc) {

			// each arc contains a set of LR kernel items
			for(pgarc_t::item_cit_t citItem = (*citArc)->itemBegin();
				citItem != (*citArc)->itemEnd(); ++citItem) {

				prod_t *ppr = m_vrRule[citItem->first.m_nRule];
				if(ppr->getRightSize() <= citItem->first.m_nDot) {
					// whole production is recognized, reduce it to non-terminal
					for(sint_cit_t sicit = citItem->second->begin();
						sicit != citItem->second->end();
						++sicit) {
						pgNode->addRRule(citItem->first.m_nRule, *sicit);
					}
					continue;
				}

				int nCurSymb = m_vrRule[citItem->first.m_nRule]->getRightSymbol(
						citItem->first.m_nDot);
				lalrItem.m_nRule = citItem->first.m_nRule;
				lalrItem.m_nDot = citItem->first.m_nDot + 1;
				
				i2i_cit_t citOld = symb2Index.find(nCurSymb);
				if(citOld != symb2Index.end()) {
					// this arc is back arc, that is, destination node is already created
					assert(citOld->second >= 0 && citOld->second < (int)varcTemp.size());
					assert(varcTemp[citOld->second]);
					vlrPairs[citOld->second]->insert(lalrItem);
					varcTemp[citOld->second]->insertItem(lalrItem, (*citItem->second));
				}
				else {
					lrpair_set_t *plrset = new lrpair_set_t;
					plrset->insert(lalrItem);
					symb2Index.insert(i2i_pair_t(nCurSymb, (int)vlrPairs.size()));
					vlrPairs.push_back(plrset);

					// add new arc
					plrArc = new pgarc_t(pgNode->getID(), (int)varcTemp.size(), nCurSymb);
					plrArc->insertItem(lalrItem, (*citItem->second));
					pgNode->addOutArc(plrArc);
					varcTemp.push_back(plrArc);
				}
				
				// current symbol is non-terminal, get non-kernel items by computing closure of LR item
				if( m_vsSymbol[nCurSymb]->isType()) {
					
					calcClosure(citItem->first, citItem->second, symb2Rules, lrkClosure);
					for(i2lrps_it_t itNKItem = lrkClosure.begin();
						itNKItem != lrkClosure.end(); ++itNKItem) {
						
						citOld = symb2Index.find(itNKItem->first);
						if(citOld != symb2Index.end()) {
							assert(citOld->second >= 0 && citOld->second < (int)varcTemp.size());
							assert(varcTemp[citOld->second]);
							
							for(lrpair_set_t::const_iterator  citLRP = itNKItem->second->begin();
								citLRP != itNKItem->second->end(); ++citLRP) {

								lalrItem.m_nRule = citLRP->m_nRule;
								lalrItem.m_nDot = 1;
								vlrPairs[citOld->second]->insert(lalrItem);
								varcTemp[citOld->second]->insertItem(lalrItem, citLRP->m_nLookahead);

							}
						}
						else {
							
							lrpair_set_t *plrset = new lrpair_set_t;
							symb2Index.insert(i2i_pair_t(itNKItem->first, (int)vlrPairs.size()));
							vlrPairs.push_back(plrset);

							// add new arc
							plrArc = new pgarc_t(pgNode->getID(), (int)varcTemp.size(), itNKItem->first);
							pgNode->addOutArc(plrArc);
							varcTemp.push_back(plrArc);
					
							for(lrpair_set_t::const_iterator  citLRP = itNKItem->second->begin();
								citLRP != itNKItem->second->end(); ++citLRP) {
								lalrItem.m_nRule = citLRP->m_nRule;
								lalrItem.m_nDot = 1;

								plrset->insert(lalrItem);
								plrArc->insertItem(lalrItem, citLRP->m_nLookahead);

							}
						}

						delete itNKItem->second;
					}

					lrkClosure.clear();
				}
			}
		}
		
		if(symb2Rules.size() > 0) {
			for(i2si_it_t itRRule = symb2Rules.begin(); itRRule != symb2Rules.end(); ++itRRule) {

				pgNode->addRRule(*itRRule->second, itRRule->first);				
				delete itRRule->second;
			}
			symb2Rules.clear();
		}
		
		for(pgnode_t::arc_it_t itArc = pgNode->outArcBegin();
			itArc != pgNode->outArcEnd(); ++itArc) {

			pair<lrps2i_it_t, bool > pairRet = lrs2Node.insert(
				lrps2i_pair_t(vlrPairs[(*itArc)->m_ntoState], a_pgrp.getNextNodeID()));
			
			if(pairRet.second) {
				// new node(state)
				pgn = a_pgrp.addEmptyNode();
				pgn->addInArc(*itArc);
				quNodeIndex.push(pgn->getID());
				(*itArc)->m_ntoState = pgn->getID();

				a_pgrp.addArc(*itArc);
			}
			else {
				
				delete vlrPairs[(*itArc)->m_ntoState];
				
				pgn = a_pgrp.fromID(pairRet.first->second);
				assert(pgn);
				(*itArc)->m_ntoState = pgn->getID();
				// current arc is cycle	
				if(_EQ(pgNode->getID(), pgn->getID())) {
					
					a_pgrp.addArc(*itArc);
					continue;
				}

				// prepare for propogation
				pgnode_t::arc_it_t itOldArc = pgn->inArcBegin();
				for(; itOldArc != pgn->inArcEnd(); ++itOldArc) {
					if(_EQ((*itOldArc)->m_nSymbol, (*itArc)->m_nSymbol)) {
						break;
					}
				}

				assert(itOldArc != pgn->inArcEnd());
				bool bDiff = false;
				pgarc_t::item_it_t itItem1 = (*itOldArc)->itemBegin();
				pgarc_t::item_it_t itItem2 = (*itArc)->itemBegin();
				for(; itItem1 != (*itOldArc)->itemEnd() && itItem2 != (*itArc)->itemEnd();
					++itItem1, ++itItem2) {

					assert(_EQ(itItem1->first.m_nRule, itItem2->first.m_nRule));
					assert(_EQ(itItem1->first.m_nDot, itItem2->first.m_nDot));
					for(sint_cit_t citLa = itItem1->second->begin();
						citLa != itItem1->second->end();
						++citLa) {
						itItem2->second->erase(*citLa);
					}

					if(itItem2->second->size() > 0) {
						bDiff = true;
						itItem1->second->insert(itItem2->second->begin(), itItem2->second->end());
					}
				}

				if(bDiff) {	
					quArc.push(*itArc);
				}
				*itArc = *itOldArc;
			}
		}
		
		varcTemp.clear();
		vlrPairs.clear();
		symb2Index.clear();
	}

	for(lrps2i_it_t it = lrs2Node.begin(); it != lrs2Node.end(); ++it) {
		delete it->first;
	}
	lrs2Node.clear();
	
	/////////////////////////////////////////////////////////////////
	// next, propogate lookahead symbols
	i2arc_map_t shift2Arc;
	sint_t siERule;
	lrpair_set_t shft2Rule;
	sint_t siT;

	while( ! quArc.empty()) {
		
		plrArc = quArc.front();
		quArc.pop();
		pgNode = a_pgrp.fromID(plrArc->m_ntoState);

		for(pgarc_t::item_it_t itItem = plrArc->itemBegin();
			itItem != plrArc->itemEnd(); ++itItem) {
			
			prod_t *ppr = m_vrRule[itItem->first.m_nRule];
			if(ppr->getRightSize() <= itItem->first.m_nDot) {
				
				for(sint_cit_t scitLa = itItem->second->begin();
					scitLa != itItem->second->end();
					++scitLa) {

					pgNode->addRRule(itItem->first.m_nRule, *scitLa);
				}
			}
			else {
				
				lalrItem.m_nRule = itItem->first.m_nRule;
				lalrItem.m_nDot = itItem->first.m_nDot + 1;
				int nCurSymb = m_vrRule[lalrItem.m_nRule]->getRightSymbol(itItem->first.m_nDot);
 
				bool bOK = false;
				pgnode_t::arc_it_t itArc = pgNode->outArcBegin();
				for(; itArc != pgNode->outArcEnd(); ++itArc) {

					pgarc_t::item_cit_t itItem2 = (*itArc)->findItem(lalrItem);
					if(itItem2 != (*itArc)->itemEnd()) {
						bOK = true;
						for(sint_cit_t citLa = itItem2->second->begin();
							citLa != itItem2->second->end();
							++citLa) {
							itItem->second->erase(*citLa);
						}
						if(itItem->second->size() > 0) {
							itItem2->second->insert(itItem->second->begin(), itItem->second->end());
						}
						break;
					}
				}
				
				if(bOK) {
					
					if(itItem->second->size() > 0) {
						pair<i2arc_it_t, bool > pairRet = shift2Arc.insert(i2arc_pair_t(nCurSymb, nullptr));
						if(pairRet.second) {
							pairRet.first->second = new pgarc_t((*itArc)->m_nfromState,
									(*itArc)->m_ntoState, (*itArc)->m_nSymbol);
						}
						pairRet.first->second->insertItem(lalrItem, *itItem->second);
					}
					
					if( m_vsSymbol[nCurSymb]->isType()) {
						calcPropogation(itItem->first, siERule, shft2Rule);	
	
						for(lrps_cit_t citSR = shft2Rule.begin(); citSR != shft2Rule.end(); ++citSR) {
							
							for(pgnode_t::arc_it_t itArc = pgNode->outArcBegin();
								itArc != pgNode->outArcEnd(); ++itArc) {

								lalrItem.m_nRule = citSR->m_nRule;
								lalrItem.m_nDot = 1;
								pgarc_t::item_cit_t itItem2 = (*itArc)->findItem(lalrItem);
								if(itItem2 != (*itArc)->itemEnd()) {
									for(sint_cit_t citLa = itItem->second->begin(); citLa != itItem->second->end(); ++citLa) {
										if(itItem2->second->insert(*citLa).second) {
											siT.insert(*citLa);
										}
									}
	
									if(siT.size() > 0) {
	
										nCurSymb = m_vrRule[lalrItem.m_nRule]->getRightSymbol(0);
										pair<i2arc_it_t, bool > pairRet = shift2Arc.insert(i2arc_pair_t(nCurSymb, nullptr));
										if(pairRet.second) {
											pairRet.first->second = new pgarc_t((*itArc)->m_nfromState,
													(*itArc)->m_ntoState, (*itArc)->m_nSymbol);
										}
										pairRet.first->second->insertItem(lalrItem, siT);
										siT.clear();
									}
								}
							}
						}
						
						if(siERule.size() > 0) {
							for(sint_cit_t citLa = itItem->second->begin(); citLa != itItem->second->end(); ++citLa) {
	
								pgNode->addRRule(siERule, *citLa);
							}
							siERule.clear();
						}
						
						shft2Rule.clear();
					}
				}
			}
		}
		for(i2arc_it_t itLA = shift2Arc.begin(); itLA != shift2Arc.end(); ++itLA) {
			
			quArc.push(itLA->second);
		}
		
		shift2Arc.clear();

		delete plrArc;
	}
}

// initialize before conversion from grammar object to parse-tables
void grammar_t::initPTable(ptable_t & a_ptbl) {

	int i, d;
	
	// constants
	a_ptbl.m_tTokenMap.resize(m_nMaxTID, UNDEFINED_SYMBOL_INDEX);
	a_ptbl.m_tTokenID.resize(m_nTypeBase);
	a_ptbl.m_nTypeBase = m_nTypeBase;
	a_ptbl.m_nSymbNum = (int)m_vstrSName.size();

	// token-map: external-ID -> internal-ID
	for(i = 0; i < m_nTypeBase; ++i) {
		a_ptbl.m_tTokenMap[m_vsSymbol[i]->getTokenID()] = i;
		a_ptbl.m_tTokenID[i] = m_vsSymbol[i]->getTokenID();
		
		if((m_vstrSName[i])[0] == '\'') {

			a_ptbl.m_vstrSName.push_back(m_vstrSName[i].substr(1, m_vstrSName[i].size() - 2));
		}
		else {
            if(m_vstrSName[i][0] == '\"') {

                std::string tokenText;
                auto& str = m_vstrSName[i];
                for(auto j = 1, e = (int)str.size(); j < e - 1; j++) {
                    auto packEscapeChar = [](char c) {
                        switch(c) {
                        case '\0':
                            return string("\\0");
                        case '\a':
                            return string("\\a");
                        case '\b':
                            return string("\\b");
                        case '\f':
                            return string("\\f");
                        case '\n':
                            return string("\\n");
                        case '\r':
                            return string("\\r");
                        case '\t':
                            return string("\\t");
                        case '\v':
                            return string("\\v");
                        case '\'':
                            return string("\\\'");
                        case '\"':
                            return string("\\\"");
                        case '?':
                            return string("\\?");
                        case '\\':
                            return string("\\\\");
                        }
                        std::string s;
                        s += c;
                        return s;
                    };

                    tokenText += packEscapeChar(str[j]);
                }

                a_ptbl.m_vstrSName.push_back(tokenText);
                a_ptbl.m_vstrStrTokenDef.push_back(tokenText);
                a_ptbl.m_tStrTokenDef.push_back(a_ptbl.m_tTokenID[i]);
            } else {
                a_ptbl.m_vstrSName.push_back(m_vstrSName[i]);
                if(i > UNDEFINED_SYMBOL_INDEX) {
                    a_ptbl.m_vstrTokenDef.push_back(m_vstrSName[i]);
                    a_ptbl.m_tTokenDef.push_back(a_ptbl.m_tTokenID[i]);
                }
            }
		}
	}
	
	// symbol names
	for(; i < (int)m_vsSymbol.size(); ++i) {
		a_ptbl.m_vstrSName.push_back(m_vstrSName[i]);
	}
	
	// useless symbols
	for(i = 0; i < (int)m_vstrULSymb.size(); ++i) {
        // update 18/06/09
        if(m_vstrULSymb[i][0] != '\"') {
            a_ptbl.m_vstrTokenDef.push_back(m_vstrULSymb[i]);
            a_ptbl.m_tTokenDef.push_back(UNDEFINED_SYMBOL_INDEX);
        }
	}

	
	// productions
	for(i = 0; i < (int)m_vrRule.size(); ++i) {
		a_ptbl.m_tRPartNum.push_back(m_vrRule[i]->getRightSize());
		
		a_ptbl.m_tLPartIdx.push_back(m_vrRule[i]->getLeft() - m_nTypeBase);
		a_ptbl.m_tRPBase.push_back((int)a_ptbl.m_tRPartIdx.size());
		for(int j = 0; j < m_vrRule[i]->getRightSize(); ++j) {
			a_ptbl.m_tRPartIdx.push_back(m_vrRule[i]->getRightSymbol(j));
		}
		a_ptbl.m_tRPartIdx.push_back(a_ptbl.m_nSymbNum);
	}
	
	// %destructor actions
	if(mt_vaBySymb.size() > 0) {
		
		for(i = 0; i < (int)mt_vaBySymb.size(); ++i) {
			a_ptbl.m_vaDstrct.push_back(mt_vaBySymb[i]);
		}
		
		for(i = 0; i < (int)m_vsSymbol.size(); ++i) {
			d = m_vsSymbol[i]->getDstrctID();
			if(d >= 0) {
				a_ptbl.m_tDstrctIdx.push_back(d);
				a_ptbl.m_tDstrctSymb.push_back(i);
			}
		}
		
		mt_vaBySymb.clear();
	}
}

// convert grammar object to parse-tables
void grammar_t::gram2PTable(ptable_t & a_ptbl, gsetting_t &gsetup) {
	
	if(!this->isValid()) {
		return ;
	}
	// create parse-graph object
	pgraph_t grp(*this, a_ptbl);
	
	// initialize before conversion from grammar object to parse-tables
	initPTable(a_ptbl);
	// convert from grammar object to parse-graph object
	gram2PGraph(grp);

	// convert from parse-graph object to parse-tables
	grp.grp2PTbl(gsetup);	
}

}
