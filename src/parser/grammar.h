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

#ifndef PARSER_GRAMMAR_H__
#define PARSER_GRAMMAR_H__

#include "../main/upgmain.h"

#include <iosfwd>
#include <iostream>
using std::ostream;
using std::endl;
#include <iomanip>
using std::setw;
#include <string>
using std::string;

#include "../common/types.h"
#include "../common/common.h"
#include "../common/strhelper.h"
using common_ns::strhelper_t;

#include "../common/action.h"
using common_ns::action_t;
using common_ns::vaction_t;

#include "./defines.h"
#include "./parserfwd.h"
#include "./symbol.h"
#include "./prod.h"
#include "./pgraph.h"
#include "./ptable.h"
#include "./lalrtrp.h"
#include "./lritem.h"

namespace parser_ns {

class grammar_t {

private:
	// array of symbol's names
	vstr_t m_vstrSName;
	// array of symbol's semantic value types
	vstr_t m_vstrSVT;
	// array of symbol objects
	vsymbol_t m_vsSymbol;
	// map table: symbol's name -> index of symbol to array of symbol's names.
	// make it easier to find registered symbols
	s2i_map_t m_sname2Idx;
	// map table: semantic value type  -> index of SVT to array of semantic value types
	s2i_map_t m_svt2Idx;
	// array of rules
	vprod_t m_vrRule;
	
	// start symbol ID
	int m_nStartSymbol;
	// accept symbol ID
	int m_nAcceptSymbol;
	// accept rule ID
	int m_nAcceptRule;
	
	// index of first variable to symbol array
	int m_nTypeBase;
	int m_nMaxTID;
	
	int m_nMidRuleNum;
	
	// name of useless symbols
	vstr_t m_vstrULSymb;
	
private:
	
	// temporary variables, only use during parsing
	int mt_nLastRuleIdx;
	string mt_strMidSVT;

	vaction_t mt_vaBySymb;
	vaction_t mt_vaBySVT;
	i2i_map_t mt_svt2Dstrct;
	s2i_map_t mt_symb2Dstrct;
	
	int mt_nFakeTypes;

private:
	
	void destroy(void);

	// compute nullable flag, FirstSet,
	// and RLS set(RLS: Rules using the symbol as Left Symbol) for each symbol
	void calcSymbSets(bool a_bClear);
	// compute RCS set(RCS: rules which contain the symbol)
	// for each symbol
	void calcRCSSet(void);
	// delete RCS set of each symbol
	void removeRCSSet(void);
	
	// report result of grammar adjustment
	void reportAdjustLog(const sint_t &a_siSymb, const sint_t &a_siRule, ostream& oss);
	
	// compute LR(1) closure
	// @a_lrp: LR(0) kernel item
	// @a_psiLa: lookahead symbols
	// @a_la2Rules(a map): lookahead-symbol -> empty-rule
	// @a_cls(a map): shift-symbol -> (Rule, Lookahead)s
	void calcClosure(const lri_pair_t &a_lrp, const sint_t *a_psiLa,
			i2si_map_t &a_la2Rules, i2lrps_map_t &a_cls) const;
	// compute propogated symbols
	// @a_lrp: LR(0) kernel item
	// @a_siRules: reduce rule ID(right part of the rule is empty)
	// @a_shft2Rule(a map): symbol-to-be-shifted -> rule IDs
	void calcPropogation(const lri_pair_t &a_lrp, sint_t &a_siRules, lrpair_set_t &a_shft2Rule) const;
	
	// initialize before conversion from grammar object to parse-tables
	void initPTable(ptable_t & a_ptbl);
	// convert from grammar object to parse graph
	void gram2PGraph(pgraph_t &a_pgrp);
	
public:
	
	// update token IDs
	void updateTID(void);
	
	// update information about %destructor action;
	// the main job is to set connection between symbol table and %destructor action table
	// by registering index of %destructor action into symbol table
	bool initDstrct(gsetting_t &gsetup);
	// rearrange %destructor actions,
	// remove  useless %destructor actions and feedback to user if any
	void updateDstrct(ostream& oss);
	
	// adjust grammar by removing useless symbols, rules
	void adjust(ptable_t & a_ptbl, gsetting_t &gsetup);
	
	// convert grammar object to parse-tables
	void gram2PTable(ptable_t & a_ptbl, gsetting_t &gsetup);
	
	friend ostream& operator<<(ostream& os, const grammar_t &src);
	
public:
	
	inline grammar_t(void)
	: m_nStartSymbol(INVALID_INDEX)
	, m_nAcceptSymbol(INVALID_INDEX)
	, m_nAcceptRule(0)
	, m_nTypeBase(INVALID_INDEX)
	, m_nMaxTID(UNDEFINED_SYMBOL_ID)
	, m_nMidRuleNum(INVALID_INDEX)
	, mt_nLastRuleIdx(INVALID_INDEX)
	, mt_nFakeTypes(1) {
		
		// keep a slot for %destructor <>
		mt_vaBySymb.push_back(nullptr);
		
		// keep a slot for %destructor <*>
		mt_vaBySymb.push_back(nullptr);
	}
	
	inline ~grammar_t(void) {
		
		destroy();
	}

	// reset grammar object
	inline void reset(void) {
		
		destroy();
		
		// add token `$end'
		addToken(INTERNAL_END_TOKEN, END_SYMBOL_ID,
				DEFAULT_SVT_ID, DEFAULT_PREC, ASSOC_UNKNOWN, false);
		// add token `$error'(external name is `error')
		int nErrID = addToken(INTERNAL_ERROR_TOKEN, ERROR_SYMBOL_ID,
				DEFAULT_SVT_ID, DEFAULT_PREC, ASSOC_UNKNOWN, false).first;
		m_sname2Idx.insert(s2i_pair_t(EXTERNAL_ERROR_TOKEN, nErrID));
		// add token `$undef'
		addToken(INTERNAL_UNDEFINED_TOKEN, UNDEFINED_SYMBOL_ID,
				DEFAULT_SVT_ID, DEFAULT_PREC, ASSOC_UNKNOWN, false);
		
		// keep a slot for %destructor <>
		mt_vaBySymb.push_back(nullptr);
		
		// keep a slot for %destructor <*>
		mt_vaBySymb.push_back(nullptr);
	}
	
public:
	
	// predictors: is--
	
	inline bool isValid(void) const {
		
	    return (m_vsSymbol.size() > 4
	        && m_vrRule.size() > 0
	        && m_nTypeBase > 2
	        && m_nStartSymbol >= m_nTypeBase);
	}
	
	inline bool isToken(const string &a_strSName) const {
		s2i_cit_t cit = m_sname2Idx.find(a_strSName);
		return cit != m_sname2Idx.end()
			&& m_vsSymbol[cit->second]->m_bIsToken;
	}
	inline bool isToken(int a_nIdx) const {
		assert(a_nIdx >= 0);
		return a_nIdx < m_nTypeBase;
	}
	inline bool isType(const string &a_strSName) const {
		s2i_cit_t cit = m_sname2Idx.find(a_strSName);
		return cit != m_sname2Idx.end()
			&& !m_vsSymbol[cit->second]->m_bIsToken;
	}
	inline bool isType(int a_nIdx) const {
		assert(a_nIdx >= 0 && a_nIdx < (int)m_vstrSName.size());
		return a_nIdx >= m_nTypeBase;
	}

public:
	// add tokens which are not defined in declaration section,
	// but used in grammar definition section. 
	void addTokens(const vstr_t & a_vstrSName, const vint_t &a_vnTokenID);
	
	inline i2b_pair_t addToken(const string &a_strSName, int a_nTokenID,
			int a_nSVT, int a_nPrec, int a_nAssoc, bool a_bUserDefined = true) {

		assert(!a_strSName.empty());
		s2i_cit_t cit = m_sname2Idx.find(a_strSName);
		if(cit != m_sname2Idx.end()) {
			return i2b_pair_t(cit->second, false);
		}
		symbol_t* psym = new symbol_t((int)m_vstrSName.size(),	a_nTokenID,
				a_nSVT, a_nPrec, a_nAssoc, a_bUserDefined);
		if(a_nTokenID >= m_nMaxTID) {
			m_nMaxTID = a_nTokenID + 1;
		}
		m_sname2Idx.insert(s2i_pair_t(a_strSName, (int)m_vstrSName.size()));
		m_vstrSName.push_back(a_strSName);
		m_vsSymbol.push_back(psym);
		
		return i2b_pair_t((int)m_vstrSName.size() - 1, true);
	}
	inline i2b_pair_t addToken(const string &a_strSName,
			int a_nSVT, int a_nPrec, int a_nAssoc, bool a_bUserDefined = true) {
		assert(!a_strSName.empty());
		s2i_cit_t cit = m_sname2Idx.find(a_strSName);
		if(cit != m_sname2Idx.end()) {
			return i2b_pair_t(cit->second, false);
		}
		symbol_t* psym = new symbol_t((int)m_vstrSName.size(), INVALID_INDEX,
				a_nSVT, a_nPrec, a_nAssoc, a_bUserDefined);
		
		m_sname2Idx.insert(s2i_pair_t(a_strSName, (int)m_vstrSName.size()));
		m_vstrSName.push_back(a_strSName);
		m_vsSymbol.push_back(psym);
		
		return i2b_pair_t((int)m_vstrSName.size() - 1, true);
	}
	
	inline i2b_pair_t addSVT(const string &a_strSVT) {
		assert(!a_strSVT.empty());
		s2i_cit_t cit = m_svt2Idx.find(a_strSVT);
		if(cit != m_svt2Idx.end()) {
			return i2b_pair_t(cit->second, false);
		}
		m_svt2Idx.insert(s2i_pair_t(a_strSVT, (int)m_vstrSVT.size()));
		m_vstrSVT.push_back(a_strSVT);
		return i2b_pair_t((int)m_vstrSVT.size() - 1, true);
	}
	inline i2b_pair_t addType(const string &a_strSName, int a_nSVT, bool a_bUserDefined = true) {
		assert(!a_strSName.empty());
		s2i_cit_t cit = m_sname2Idx.find(a_strSName);
		if(cit != m_sname2Idx.end()) {
			return i2b_pair_t(cit->second, false);
		}
		symbol_t *psym = new symbol_t((int)m_vstrSName.size(), a_nSVT, a_bUserDefined);
		m_sname2Idx.insert(s2i_pair_t(a_strSName, (int)m_vstrSName.size()));
		m_vstrSName.push_back(a_strSName);
		m_vsSymbol.push_back(psym);
		return i2b_pair_t((int)m_vstrSName.size() - 1, true);
	}
	
	inline prod_t* addEmptyRule(const string &a_strSName) {
		
		s2i_cit_t cit = m_sname2Idx.find(a_strSName);
		if(cit != m_sname2Idx.end()) {
			mt_nLastRuleIdx = (int)m_vrRule.size();
			prod_t* prule = new prod_t(mt_nLastRuleIdx);
			prule->setLeft(cit->second);
			m_vrRule.push_back(prule);
			
			return prule;
		}
		return nullptr;
	}
	inline prod_t* addEmptyRule(int a_nLeftIdx) {
		
		assert(a_nLeftIdx >= m_nTypeBase && a_nLeftIdx < (int)m_vsSymbol.size());
		mt_nLastRuleIdx = (int)m_vrRule.size();
		prod_t* prule = new prod_t(mt_nLastRuleIdx);
		prule->setLeft(a_nLeftIdx);
		m_vrRule.push_back(prule);
		return prule;
	}
	inline prod_t* addMidRule(int a_nActionIndex) {
		
		prod_t* prule = new prod_t((int)m_vrRule.size());
		prule->setActionIndex(a_nActionIndex);
		
		string strSymb = MRLEFT_PREFIX;
		strSymb += strhelper_t::fromInt(++m_nMidRuleNum);
		assert(m_sname2Idx.find(strSymb) == m_sname2Idx.end());
		
		int svtID = DEFAULT_SVT_ID;
		if( ! mt_strMidSVT.empty()) {
			svtID = addSVT(mt_strMidSVT).first;
		}
		i2b_pair_t pairRet = addType(strSymb, svtID, false);
		assert(pairRet.second);
		prule->setLeft(pairRet.first);
		
		m_vrRule.push_back(prule);
		
		mt_strMidSVT = "";
		
		return prule;
	}

public:
	
	// this method only used during parsing
	inline int getLastProdIndex(void) const {
		return mt_nLastRuleIdx;
	}
	inline bool setMidSVT(const string &a_strMSVT) {
		
		bool bret = mt_strMidSVT.empty();
		mt_strMidSVT = a_strMSVT;
		return bret;	
	}
	inline string getMidSVT(void) const {
		return mt_strMidSVT;
	}
	inline void clearMidSVT(void) {
		mt_strMidSVT = "";
	}
	
	//accessors: get/set
	inline int getSymbolIndex(const string &a_strSName) const {
		s2i_cit_t cit = m_sname2Idx.find(a_strSName);
		if(cit == m_sname2Idx.end()) {
			return INVALID_INDEX;
		}
		return cit->second;
	}
	inline const string& getSName(int a_nIdx) const {
		assert(a_nIdx >= 0 && a_nIdx < (int)m_vstrSName.size());
		return m_vstrSName[a_nIdx];
	}
	inline const string& getRuleLName(int a_nRule) const {
		assert(a_nRule >= 0 && a_nRule < (int)m_vrRule.size());
		return m_vstrSName[m_vrRule[a_nRule]->getLeft()];
	}
	inline prod_t* getProd(int a_nIdx) {
		assert(a_nIdx >= 0 && a_nIdx < (int)m_vrRule.size());
		return m_vrRule[a_nIdx];
	}
	inline const prod_t* getProd(int a_nIdx) const {
		assert(a_nIdx >= 0 && a_nIdx < (int)m_vrRule.size());
		return m_vrRule[a_nIdx];
	}
	inline const symbol_t* getSymbolObj(const string &a_strSName) const {
		s2i_cit_t cit = m_sname2Idx.find(a_strSName);
		if(cit == m_sname2Idx.end()) {
			return nullptr;
		}
		return m_vsSymbol[cit->second];
	}
	inline const symbol_t* getSymbolObj(int a_nIdx) const {
		assert(a_nIdx >= 0 && a_nIdx < (int)m_vstrSName.size());
		return m_vsSymbol[a_nIdx];
	}
	inline string getSVTName(int a_nIdx) const {
		assert(a_nIdx >= DEFAULT_SVT_ID && a_nIdx < (int)m_vstrSName.size());
		if(a_nIdx < 0) {
			return GRAMMAR_DEFAULT_SVT;
		}
		return m_vstrSVT[a_nIdx];
	}
	inline bool getSymbolSVT(int a_nRule, int a_nDot, string &a_strSVT) const {
		
		assert(a_nRule < (int)m_vrRule.size());
		prod_t * pr = m_vrRule[a_nRule];
		if(a_nDot < -1 || a_nDot >= pr->getRightSize()) {
			return false;
		}
		
		a_strSVT = "";
		int nID = pr->getRightSymbol(a_nDot);
		if(nID >= 0) {
			nID = m_vsSymbol[nID]->getSVTID();
			if(nID >= 0) {
				a_strSVT = m_vstrSVT[nID];
			}
		}
		return true;
	}
	inline int getTokenCount(void) const {
		return (int)m_nTypeBase;
	}
	inline int getTypeCount(void) const {
		return (int)((int)m_vstrSName.size() - m_nTypeBase);
	}
	inline int getSymbolCount(void) const {
		return (int)m_vstrSName.size();
	}
	inline int getRuleCount(void) const {
		return (int)m_vrRule.size();
	}
	inline int getStartSymbol(void) const {
		return m_nStartSymbol;
	}
	inline void setStartSymbol(int a_nIdx) {
		assert(a_nIdx >= 0 && a_nIdx < (int)m_vstrSName.size());
		m_nStartSymbol = a_nIdx;
	}
	inline void setStartSymbol(const string &a_strSName) {
		s2i_cit_t cit = m_sname2Idx.find(a_strSName);
		if(cit != m_sname2Idx.end()) {
			m_nStartSymbol = cit->second;
		}
	}
	inline int getAcceptSymbol(void) const {
		return m_nAcceptSymbol;
	}
	inline void setAcceptSymbol(int a_nIdx) {
		assert(a_nIdx >= 0 && a_nIdx < (int)m_vstrSName.size());
		m_nAcceptSymbol = a_nIdx;
	}
	inline int getAcceptRule(void) const {
		return m_nAcceptRule;
	}
	inline void setAcceptRule(int a_nIdx) {
		assert(a_nIdx >= 0 && a_nIdx < (int)m_vrRule.size());
		m_nAcceptRule = a_nIdx;
	}
	inline int getTypeBase(void) const {
		return m_nTypeBase;
	}
	inline void setTypeBase(void) {
		m_nTypeBase = (int)m_vsSymbol.size();
	}
	inline void setTypeBase(int a_nIdx) {
		assert(a_nIdx > 2 && a_nIdx < (int)m_vsSymbol.size());
		m_nTypeBase = a_nIdx;
	}
	
	bool addDstrctBySymbs(action_t *a_pDstrct, const vstr_t &a_vstrSymb) {
		assert(a_pDstrct);
		int ncnt = 0;
		for(int i = 0; i < (int)a_vstrSymb.size(); ++i) {
			if(mt_symb2Dstrct.insert(s2i_pair_t(a_vstrSymb[i], (int)mt_vaBySymb.size())).second) {
				++ncnt;
			}
		}
		if(ncnt > 0) {
			mt_vaBySymb.push_back(a_pDstrct);
		}
		return ncnt > 0;
	}
	inline bool addDstrctBySymb(action_t *a_pDstrct, const string &a_strSymb) {
		assert(!a_strSymb.empty() && a_pDstrct);
		if(mt_symb2Dstrct.insert(s2i_pair_t(a_strSymb, (int)mt_vaBySymb.size())).second) {
			mt_vaBySymb.push_back(a_pDstrct);
			return true;
		}
		return false;
	}

	inline bool addDstrctBySVT(action_t *a_pDstrct, const string &a_strSVT) {
		
		assert(!a_strSVT.empty() && a_pDstrct);
		int svtIdx = addSVT(a_strSVT).first;
		assert(svtIdx >= 0);
		if(mt_svt2Dstrct.insert(i2i_pair_t(svtIdx ,(int)mt_vaBySVT.size())).second) {
			mt_vaBySVT.push_back(a_pDstrct);
			return true;
		}
		return false;
	}

	inline bool setUntagDstrct(action_t *a_pDstrct) {
		
		if( ! mt_vaBySymb[0]) {
			mt_vaBySymb[0] = a_pDstrct;
			return mt_vaBySymb[0] != nullptr;
		}
		return false;
	}
	inline bool setOtherDstrct(action_t *a_pDstrct) {
		
		if( ! mt_vaBySymb[1]) {
			mt_vaBySymb[1] = a_pDstrct;
			return mt_vaBySymb[1] != nullptr;
		}
		return false;
	}
	
	inline string getFakeSymbol(void) {
		
		string strSymb = FAKE_SYMBOL;
		++mt_nFakeTypes;
		
		strSymb += strhelper_t::fromInt(mt_nFakeTypes);
		
		return strSymb;
	}
};

}

#endif // PARSER_GRAMMAR_H__
