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

#ifndef CODER_CMMGR_H__
#define CODER_CMMGR_H__

#include <iosfwd>
using std::ostream;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <stack>
using std::stack;

#include <map>
using std::map;
using std::pair;
#include <iterator>
using std::iterator;

#include "./metakeys.h"
#include "./cmcond.h"
#include "./cmacro.h"
#include "./dmmap.h"

#define MAX_TEXT_COMPARE_SIZE		20

namespace coder_ns {

// cmacro manager class
class cmacro_mgr_t {
	
private:
	
	// built-in macro condition-items
	cond_empty_t m_condHas;
	cond_empty_t m_condEmpty;
	cond_end_t m_condEnd;
	cond_last_t m_condLast;
	cond_more_t m_condMore;
	cond_group_t m_condGroup;
	cond_groupfirst_t m_condGroupFirst;
	
	// array of user-defined macro condition-items
	vcond_item_t m_vcondItems;
	// array of user-defined macro condition
	vcond_t m_vConds;
	
	// map table: text(or macro name) --> index of the macro term to array of macro terms, say @m_str2TermIdx
	s2i_map_t m_str2TermIdx;
	// array of macro terms
	vterm_t m_vcmTerms;
	
	// map table: macro name --> macro object
	str2mac_map_t m_str2Macro;
	
	// stack of macro term objects, used by macro expansion
	stack<cmacro_term_t*> m_stMacTerms;
	// stack of macro parameters, used by macro expands
	stack<const vstr_t* > m_stMacParams;
	
	// @m_tempParams is used to store temporary parameters
	vector<vstr_t* > m_tempParams;
	// faked parameter used by macro without parameter
	vstr_t m_vstrEmptyParam;
	
	// name of built-in macros
	const char *m_pcchAtomicMacro[ATOMIC_MACRO_NUM];
	// built-in macro objects
	cmacro_term_t *m_pmAtomicMacro[ATOMIC_MACRO_NUM];

public:
	
	cmacro_mgr_t(void);	
	~cmacro_mgr_t(void);
	
private:
	
	// try to get built-in macro object,
	// if @a_strMacroName is name of one of built-in macro, then return corresponding macro object
	// otherwise return nullptr
	cmacro_term_t* tryBuildInMacro(const string &a_strMacroName);
	
public:

	// test macro expansion conditions
	bool testCond(dmmap_t &a_map, const string &a_strKey, int a_nVal) const;
	bool testCond(dmmap_t &a_map, const string &a_strKey, bool a_bVal) const;
	bool testCond(dmmap_t &a_map, const string &a_strKey, const string &a_strVal) const;
	bool testHas(dmmap_t &a_map, const string &a_strKey) const;
	bool testEmpty(dmmap_t &a_map, const string &a_strKey) const;
	
public:
	
	///////////////////////////////////////////////////////////////////////
	// add macro term: its type is @CTT_TEXT
	cmacro_term_t* newTextTerm(const string &a_strText, int a_nLines);
	// add macro term: its type is @CTT_MACRO, but without parameters
	cmacro_term_t* newMacroTerm(const string &a_strMacroName);
	// add macro term: its type is @CTT_MACRO
	cmacro_term_t* newMacroTerm(const string &a_strMacroName, const vint_t &a_vnParams);
	// add empty macro object, whose name is @a_strName
	cmacro_t* newMacro(const string &a_strName);
	
	// get macro object via its name
	cmacro_base_t* getMacro(const string &a_strName);
	// expand macros using stack
	bool expandWithStack(const string &a_strMacro, dmmap_t &a_map, const vstr_t &a_vstrKeys, ostream& os);
	
public:
	
	inline cond_empty_t* getHasCond(void) {
		return &m_condHas;
	}
	inline const cond_empty_t* getHasCond(void) const {
		return &m_condHas;
	}
	
	inline cond_empty_t* getEmptyCond(void) {
		return &m_condEmpty;
	}
	inline const cond_empty_t* getEmptyCond(void) const {
		return &m_condEmpty;
	}
	
	inline cond_end_t* getEndCond(void) {
		return &m_condEnd;
	}
	inline const cond_end_t* getEndCond(void) const {
		return &m_condEnd;
	}
	
	inline cond_last_t* getLastCond(void) {
		return &m_condLast;
	}
	inline const cond_last_t* getLastCond(void) const {
		return &m_condLast;
	}
	
	inline cond_more_t* getMoreCond(void) {
		return &m_condMore;
	}
	inline const cond_more_t* getMoreCond(void) const {
		return &m_condMore;
	}
	
	inline cond_group_t* getGroupCond(void) {
		return &m_condGroup;
	}
	inline const cond_group_t* getGroupCond(void) const {
		return &m_condGroup;
	}
	
	inline cond_groupfirst_t* getGroupFirstCond(void) {
		return &m_condGroupFirst;
	}
	inline const cond_groupfirst_t* getGroupFirstCond(void) const {
		return &m_condGroupFirst;
	}
	
	inline cond_mod_t* newModCond(int a_nElemsPerLine) {
		
		cond_mod_t *pcond = new cond_mod_t(a_nElemsPerLine);
		m_vcondItems.push_back(pcond);
		
		return pcond;
	}

	
public:
	
	inline cond_opteq_t* newOptCond(const string &a_strOptionName, int a_nVal) {
	
		cond_opteq_t *pcond = new cond_opteq_t(a_strOptionName, a_nVal);
		m_vcondItems.push_back(pcond);
		
		return pcond;
	}
	inline cond_opteq_t* newOptCond(const string &a_strOptionName, bool a_bVal) {
	
		cond_opteq_t *pcond = new cond_opteq_t(a_strOptionName, a_bVal);
		m_vcondItems.push_back(pcond);
		
		return pcond;
	}
	inline cond_opteq_t* newOptCond(const string &a_strOptionName, const string &a_strVal) {
	
		cond_opteq_t *pcond = new cond_opteq_t(a_strOptionName, a_strVal);
		m_vcondItems.push_back(pcond);
		
		return pcond;
	}
	
	inline cond_elemrel_t* newERelCond(cond_elemrel_t::eop_t a_nRop, int a_nVal) {
		
		cond_elemrel_t *pcond = new cond_elemrel_t(a_nRop, a_nVal);
		m_vcondItems.push_back(pcond);
		
		return pcond;
	}
	inline cond_elemrel_t* newERelCond(cond_elemrel_t::eop_t a_nRop, bool a_bVal) {
		
		cond_elemrel_t *pcond = new cond_elemrel_t(a_nRop, a_bVal);
		m_vcondItems.push_back(pcond);
		
		return pcond;
	}
	inline cond_elemrel_t* newERelCond(cond_elemrel_t::eop_t a_nRop, const string &a_strVal) {
		
		cond_elemrel_t *pcond = new cond_elemrel_t(a_nRop, a_strVal);
		m_vcondItems.push_back(pcond);
		
		return pcond;
	}
	inline cond_compound_t* newCompoundCond(void) {
		
		cond_compound_t *pcond = new cond_compound_t;
		m_vcondItems.push_back(pcond);
		
		return pcond;
	}
	
	inline cond_t* newCond(void) {
		
		cond_t* pcond = new cond_t;
		m_vConds.push_back(pcond);
		
		return pcond;
	}
	
public:
	
	inline void addExpandStack(cmacro_term_t* a_pTerm, const vstr_t *a_pParams) {
		
		m_stMacTerms.push(a_pTerm);
		m_stMacParams.push(a_pParams);
	}
	
	inline void addExpandStack(cmacro_term_t* a_pTerm, const string &a_strParam) {
		
		vstr_t* pvstr = newTempParams();
		pvstr->push_back(a_strParam);
		
		m_stMacTerms.push(a_pTerm);
		m_stMacParams.push(pvstr);
	}
	
	inline vstr_t* newTempParams(void) {
		
		vstr_t* pvs = new vstr_t;
		m_tempParams.push_back(pvs);
		
		return pvs;
	}
	inline const vstr_t& getEmptyParam(void) const {
		
		return m_vstrEmptyParam;
	}
};

}

#endif // CODER_CMMGR_H__
