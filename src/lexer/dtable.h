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

#ifndef LEXER_DFA_H__
#define LEXER_DFA_H__
#include <iosfwd>
#include <string>

using std::ostream;
using std::pair;
using std::string;

#include "../common/common.h"
#include "../common/types.h"
#include "../common/action.h"
#include "../common/rulemgr.h"
using common_ns::action_t;
using common_ns::vaction_t;
using common_ns::rule_mgr_t;

#include "./dfafwd.h"

namespace lexer_ns {

class dtable_t {
	
private:

	rule_mgr_t m_ruleMgr;
	table_t m_tRuleRef;
	
	// start states of DFAs
	table_t m_tStarts;
	// names of start-conditions
	vstr_t m_vsSCName;
	// indexes of start-conditions
	table_t m_vnSCNameIndex;
	
	// map from inputted char to index which is used as offset when consulting tables
	table_t m_tCharMap;
	table_t m_tMeta;
	// transition table 
	table_t m_tNext;
	// check if corresponding entry of transition table is correct
	table_t m_tCheck;
	// the base address of the entries for each state stored in the transition table
	table_t m_tBase;
	// alternative base address when current base address is invalid
	table_t m_tDefault;
	// accept table: for each accept state, the value is matched rule ID,
	// for others, the value is invalid rule ID
	table_t m_tAccept;
	
	// lookahead table, determine what rules can be matched in lookahead way for each state
	table_t m_tLookahead;

	// a special trap state,
	int m_nDefaultState;
	// base address of SC-EOF Rules, which only match rule `<<EOF>>'
	int m_nSCEOFRuleBase;
	// count of groups of chars
	int m_nLabelCount;
	// number of meta-chars
	int m_nMetaCount;
	
	// store %lex-init action
	action_t *m_pactLexInit;
	// store %yywrap action
	action_t *m_pactWrap;
	// store %pre-match action
	action_t *m_pactPrematch;
	action_t *m_pCurAction;

public:
	
	dtable_t(void)
	: m_ruleMgr()
	, m_nDefaultState(INVALID_STATE)
	, m_nSCEOFRuleBase(INVALID_RULE)
	, m_nLabelCount(0)
	, m_nMetaCount(0)
	, m_pactLexInit(nullptr)
	, m_pactWrap(nullptr)
	, m_pactPrematch(nullptr)
	, m_pCurAction(nullptr) {

		m_tCharMap.resize(256, 0);
	}
	
	inline void clear(void) {
		
		m_ruleMgr.clear();
		
		m_tCharMap.clear();
		m_tCharMap.resize(256, 0);
		m_tMeta.clear();
		m_tStarts.clear();
		m_vsSCName.clear();
		m_tNext.clear();
		m_tCheck.clear();
		m_tBase.clear();
		m_tDefault.clear();
		m_tAccept.clear();
		
		m_nSCEOFRuleBase = INVALID_RULE;
		m_nDefaultState = INVALID_STATE;
		m_nLabelCount = 0;
		m_nMetaCount = 0;
		m_tLookahead.clear();
		m_pCurAction = nullptr;
		m_tRuleRef.clear();

		if(m_pactLexInit) {
			
			delete m_pactLexInit;
			m_pactLexInit = nullptr;
		}
		
		if(m_pactWrap) {
			
			delete m_pactWrap;
			m_pactWrap = nullptr;
		}
		
		if(m_pactPrematch) {
			
			delete m_pactPrematch;
			m_pactPrematch = nullptr;
		}
	}
	
public:
	
	// check if scanner's DFAs is empty
	bool isEmpty(void) const {

		return m_tCharMap.size() == 0 || m_tMeta.size() == 0 
			|| m_tNext.size() == 0 || m_tBase.size() == 0
			|| m_tAccept.size() == 0 || m_ruleMgr.getActions().size() == 0;
	}
	void updateCharmap(const vustr_t &a_vustrMap);
	// check if there are lookahead states for DFA table
	void checkLookahead(const vint_t& a_vnRules, const vint_t& a_vnLaNodes);
	// update rules by adding End-Of-File rules
	void updateRules(vint_t &a_sc2Rules, ostream &os);
	void addAdditionChars(void);

	inline void addRuleRef(int a_nRuleIndex) {
		assert(a_nRuleIndex >= 0 && a_nRuleIndex < (int)m_tRuleRef.size());
		++m_tRuleRef[a_nRuleIndex];
	}
	
	inline void decRuleRef(int a_nRuleIndex) {
		assert(a_nRuleIndex >= 0 && a_nRuleIndex < (int)m_tRuleRef.size());
		--m_tRuleRef[a_nRuleIndex];		
	}
	
	inline int addRule(void) {
		m_tRuleRef.push_back(0);
		return (int)m_tRuleRef.size() - 1;
	}
	
	inline void mapRule2Action(int a_nRule, int a_nActionIndex) {
		m_ruleMgr.mapRule2Action(a_nRule, a_nActionIndex);
	}
	
	inline void mapRule2Line(int a_nRule, int a_nLineNo) {
		m_ruleMgr.mapRule2Line(a_nRule, a_nLineNo);
	}
	
	inline int addEmptyAction(int a_nLine) {
		int idx = m_ruleMgr.addEmptyAction(a_nLine);
		m_pCurAction = m_ruleMgr.getCurAction();
		return idx;
	}
	
	inline void addActionText(char c) {
		if(m_pCurAction ) {
			m_pCurAction->addText(c);
		}
	}
	inline void addActionText(const string &a_strText) {
		if(m_pCurAction) {
			m_pCurAction->addText(a_strText);
		}
	}
	inline void addActionMacro(const string &a_strMacro) {
		if(m_pCurAction) {
			m_pCurAction->addMacro(a_strMacro);
		}
	}
	inline void addActionMacro(const string &a_strMacro, const string &a_strParam) {
		if(m_pCurAction) {
			m_pCurAction->addMacro(a_strMacro, a_strParam);
		}
	}
	inline bool beginWrap(int a_nLine) {
		
		if(! m_pactWrap) {
			
			m_pCurAction = m_pactWrap = new action_t(a_nLine);	
		}
		else {
			
			m_pCurAction = nullptr;
		}

		return m_pCurAction == m_pactWrap;
	}
	inline bool beginLexInit(int a_nLine) {
		
		if(! m_pactLexInit) {
			
			m_pCurAction = m_pactLexInit = new action_t(a_nLine);
		}
		else {
			m_pCurAction = nullptr;
		}
		
		return m_pactLexInit == m_pCurAction;
	}
	
	inline bool beginPrematch(int a_nLine) {
		
		if(!m_pactPrematch) {
			
			m_pCurAction = m_pactPrematch = new action_t(a_nLine);
		}
		else {
			m_pCurAction = nullptr;
		}
		return m_pCurAction == m_pactPrematch;
	}
	
	inline vaction_t& getActions(void) {
		return m_ruleMgr.getActions();
	}
	inline const vaction_t& getActions(void) const {
		return m_ruleMgr.getActions();
	}
	
	inline action_t* getLexInitAction(void) {
		return m_pactLexInit;
	}
	inline const action_t* getLexInitAction(void) const {
		return m_pactLexInit;
	}
	
	inline action_t* getEOAFAction(void) {
		return m_pactWrap;
	}
	inline const action_t* getEOAFAction(void) const {
		return m_pactWrap;
	}
	inline action_t* getPrematchAction(void) {
		return m_pactPrematch;
	}
	inline const action_t* getPrematchAction(void) const {
		return m_pactPrematch;
	}
	
	inline vint_t& getRule2Actions(void) {
		return m_ruleMgr.getRule2Actions();
	}
	inline const vint_t& getRule2Actions(void) const {
		return m_ruleMgr.getRule2Actions();
	}

	inline vint_t& getRule2LineNos(void) {
		return m_ruleMgr.getRule2LineNos();
	}

	inline const vint_t& getRule2LineNos(void) const {
		return m_ruleMgr.getRule2LineNos();
	}
	
private:
	
	friend class dgraph_t;
	friend class dgraph_node_t;
	friend ostream& operator<<(ostream &os, const dgraph_node_t& src);
	friend ostream& operator<<(ostream &os, const dgraph_t &src);
	friend ostream& operator<<(ostream& os, const dtable_t &src);
	
private:
	// TODO: only for dgraph_t, because no index transformation is involved	
	inline void addStartIndex(const string &a_strSCName, int a_nFull, int a_nNoHat) {
		assert(!a_strSCName.empty());
		m_vnSCNameIndex.push_back((int)m_vsSCName.size() * 2);
		m_vsSCName.push_back(a_strSCName);
		m_tStarts.push_back(a_nFull);
		m_tStarts.push_back(a_nNoHat);
	}

	inline void setStartIndex(int a_nIdx, int a_nFull, int a_nNoHat) {
		assert(a_nIdx >= 0 && a_nIdx < (int)m_vsSCName.size());
		m_tStarts[a_nIdx * 2] = a_nFull;
		m_tStarts[a_nIdx * 2 + 1] = a_nNoHat;
	}
	
	inline string getStartName(int a_nIdx) const {
		assert(a_nIdx >= 0 && a_nIdx < (int)m_vsSCName.size());
		return m_vsSCName[a_nIdx];
	}
	
	inline int getStartIndex(int a_nIdx, bool a_bFull) const {
		assert(a_nIdx >= 0 && a_nIdx < (int)m_vsSCName.size());
		return (a_bFull? 
					m_tStarts[a_nIdx * 2] 
					          : m_tStarts[a_nIdx * 2 + 1]);
	}
	inline int getStartCount(void) const {
		return (int)m_vsSCName.size();
	}

public:

	inline const vint_t& getCharMap(void) const {	
		return m_tCharMap;
	}
	inline const vint_t& getMeta(void) const {
		return m_tMeta;
	}
	inline const table_t& getStarts(void) const {
		return m_tStarts;
	}
	inline const vstr_t& getStartLabels(void) const {
		return m_vsSCName;
	}
	inline const table_t& getStartIndexes(void) const {
		return m_vnSCNameIndex;
	}
	inline const table_t& getNexts(void) const {
		return m_tNext;
	}
	inline const table_t& getChecks(void) const {
		return m_tCheck;
	}
	inline const table_t& getBases(void) const {
		return m_tBase;
	}
	inline const table_t& getDefaults(void) const {
		return m_tDefault;
	}
	inline const table_t& getAccepts(void) const {
		return m_tAccept;
	}
	inline bool hasLookahead(void) const {
		return m_tLookahead.size() > 0;
	}
	inline const table_t& getLookaheads(void) const {
		return m_tLookahead;
	}
	inline int getDefaultState(void) const {
		return m_nDefaultState;
	}
	inline int getEOFRuleBase(void) const {
		return m_nSCEOFRuleBase;
	}
	inline int getLabelCount(void) const {
		return m_nLabelCount;
	}
	inline void setMetaCount(int a_nMetas) {
		m_nMetaCount = a_nMetas;
	}
};

}

#endif // LEXER_DFA_H__
