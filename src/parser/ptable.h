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

#ifndef PARSER_PTABLE_H_
#define PARSER_PTABLE_H_
#include "../main/upgmain.h"

#include "../common/common.h"
#include "../common/types.h"
#include "../common/action.h"
#include "../common/bitmap.h"
using common_ns::bitmap_t;
using common_ns::action_t;
using common_ns::vaction_t;

#include "../common/rulemgr.h"
using common_ns::rule_mgr_t;

#include "./parserfwd.h"

namespace parser_ns {

// compressed table
class ptable_t {

private:
	
	// start state
	int m_nSState;
	// accept state;
	int m_nAState;
	// error state
	int m_nEState;
	// error rule
	int m_nERule;
	// symbol tables
	vstr_t m_vstrSName;
	// 
	int m_nTypeBase;
	// total number of symbols
	int m_nSymbNum;
	
	// user-defined semantic value type
	string m_strSType;
	// user-defined location type
	string m_strLType;
	
	// to define tokens as constants
	// e.g.
	// #define NUM		258
	// in this case, string 'NUM' is in  @m_vstrTokenDef
	// and value 258 in correspoding entry in @m_tTokenDef
	vstr_t m_vstrTokenDef;
	table_t m_tTokenDef;

    // update 16/12/17
    // to define string tokens as string->int map
    // e.g.
    // yyslexemID{{"==", 300}, {">=", 301}}
    vstr_t m_vstrStrTokenDef;
    table_t m_tStrTokenDef;
	
	// array of token IDs
	table_t m_tTokenID;
	// map: token ID -> index
	table_t m_tTokenMap;
	table_t m_tRPartNum;
	
	// only available for debug mode
	table_t m_tLPartIdx;
	table_t m_tRPBase;
	table_t m_tRPartIdx;
	
	// action tables
	table_t m_tParse;
	table_t m_tBase;
	
	// goto tables
	table_t m_tGoto;
	table_t m_tGBase;
	
	// tables which are used to compress parse tables
	// bit map indicating if cell(state, symbol) is valid or not
	bitmap_t m_bitValid;
	table_t m_tActRowVal;
	table_t m_tActRowNice;
	table_t m_tColVal;
	table_t m_tColNice;
	table_t m_tGotoRowVal;
	table_t m_tGotoRowNice;
	
	// grammar rule manager
	rule_mgr_t m_ruleMgr;
	// pointer pointing to action being scanned
	action_t *m_pCurAction;
	
	// destructors for symbols, which are declared by
	// `%destructor symbols', `%destructor <>', `%destructor <*>'
	// or `%destructor <semantic-value-type>'
	vaction_t m_vaDstrct;
	// symbol indices
	table_t m_tDstrctSymb;
	// destructor IDs
	table_t m_tDstrctIdx;

	
	friend class grammar_t;
	friend class pgraph_t;
	friend ostream& operator<<(ostream &os, const ptable_t &src);

public:
	void clear(void);

public:
	
	inline ptable_t(void)
	: m_nSState(FIRST_STATE)
	, m_nAState(INVALID_STATE)
	, m_nEState(FIRST_STATE)
	, m_nERule(INVALID_RULE)
	, m_nTypeBase(0)
	, m_nSymbNum(0)	{
		
	}
	
public:
	
	// check if lalr tables are empty
	inline bool isEmpty(void) const {
	
		return m_vstrSName.size() == 0 || 2 >= m_nTypeBase
			|| m_tTokenMap.size() == 0 || m_tRPartNum.size() == 0
			|| m_tParse.size() == 0 || m_tGoto.size() == 0
			|| getActions().size() == 0;
	}
	
	inline int getStartState(void) const {
		return m_nSState;
	}
	inline int getAcceptState(void) const {
		return m_nAState;
	}
	inline int getErrorState(void) const {
		return m_nEState;
	}
	inline int getErrorRule(void) const {
		return m_nERule - 1 - m_nEState;
	}
	inline const vstr_t& getSName(void) const {
		return m_vstrSName;
	}
	inline int getTypeBase(void) const {
		return m_nTypeBase;
	}
	inline int getSymNum(void) const {
		return m_nSymbNum;
	}
	
	inline const table_t& getTIDTable(void) const {
		return m_tTokenID;
	}
	inline const table_t& getTokenMap(void) const {
		return m_tTokenMap;
	}
	inline const table_t& getRPNumTable(void) const {
		return m_tRPartNum;
	}
	
	// only available for debug mode
	inline const table_t& getLPartIndex(void) const {
		return m_tLPartIdx;
	}
	inline const table_t& getRPBase(void) const {
		return m_tRPBase;
	}
	inline const table_t& getRPartIndex(void) const {
		return m_tRPartIdx;
	}
	
	// action tables
	inline const table_t& getActionTable(void) const {
		return m_tParse;
	}
	inline const table_t& getActionBase(void) const {
		return m_tBase;
	}
	
	// goto tables
	inline const table_t& getGotoTable(void) const {
		return m_tGoto;
	}
	inline const table_t& getGotoBase(void) const {
		return m_tGBase;
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
	inline void addActionMacro(const string &a_strMacro,
			const string &a_strParam1, const string &a_strParam2) {
		if(m_pCurAction) {
			m_pCurAction->addMacro(a_strMacro, a_strParam1, a_strParam2);
		}
	}
	
	inline void updateRActions(const vint_t &viRule) {
		m_ruleMgr.updateRActions(viRule);
	}
	
	inline vaction_t& getActions(void) {
		return m_ruleMgr.getActions();
	}
	inline const vaction_t& getActions(void) const {
		return m_ruleMgr.getActions();
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

	inline const vstr_t& getTokenDefNames(void) const {
		return 	m_vstrTokenDef;
	}
	inline const table_t& getTokenDefValues(void) const {
		return m_tTokenDef;
	}
    // update 16/12/17
    inline const vstr_t& getStrTokenDefNames(void) const {
        return m_vstrStrTokenDef;
    }
    inline const table_t& getStrTokenDefValues(void) const {
        return m_tStrTokenDef;
    }
	
	inline const vaction_t& getDstrctActions(void) const {
		return m_vaDstrct;
	}
	inline const table_t& getDstrctSymb(void) const {
		return m_tDstrctSymb;
	}
	inline const table_t& getDstrctIndex(void) const {
		return m_tDstrctIdx;
	}
	
	inline const bitmap_t& getValidBMap(void) const {
		return m_bitValid;
	}
	inline const table_t& getActRowVal(void) const {
		return m_tActRowVal;
	}
	inline const table_t& getActRowNice(void) const {
		return m_tActRowNice;
	}
	inline const table_t& getColVal(void) const {
		return m_tColVal;
	}
	inline const table_t& getColNice(void) const {
		return m_tColNice;
	}
	inline const table_t& getGotoRowVal(void) const {
		return m_tGotoRowVal;
	}
	inline const table_t& getGotoRowNice(void) const {
		return m_tGotoRowNice;
	}
};

}
#endif // PARSER_PTABLE_H_
