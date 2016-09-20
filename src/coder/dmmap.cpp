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
#include <string>
using std::string;
#ifdef UPGEN_TEST_VERSION
// TODO: fix bug
#include <iostream>
using std::cerr;
using std::endl;
// TODO
#endif

#include "../common/common.h"
#include "../common/reporter.h"

#include "../lexer/defines.h"
#include "../lexer/dtable.h"
using lexer_ns::dtable_t;

#include "../parser/defines.h"
#include "../parser/ptable.h"
using parser_ns::ptable_t;


#include "./metakeys.h"
#include "./dmmap.h"

namespace coder_ns {

dmmap_t::~dmmap_t(void) {
	
	for(str2dm_it_t it = m_sdEntries.begin(); it != m_sdEntries.begin(); ++it) {
		
		if(it->second) {
			
			delete it->second;
		}
	}
}

// group array by value of its elements, and
// insert group delimiters between groups
void dmmap_t::map2Groups(const vint_t& a_map, vint_t& a_grp) const {
	
	if(a_map.size() == 0) {
		
		return ;
	}
	
	// first, get maximum value of elements of the array
	int nMax = GROUP_DELIMITER;
	for(int i = 0; i < (int)a_map.size(); ++i) {
		
		if(nMax < a_map[i]) {
			
			nMax = a_map[i];
		}
	}
	if(nMax == GROUP_DELIMITER) { return;}
	
	++nMax;
	
	// then adjust array so that elements of the same group are together
	// by introducing new array
	a_grp.clear();
	a_grp.resize(a_map.size() + nMax);
	vint_t vnBase(nMax + 1, 1);
	for(int i = 0; i < (int)a_map.size(); ++i) {
		
		if(a_map[i] >= 0) {
			
			++vnBase[a_map[i]];
		}
	}
	vnBase[nMax] = (int)a_grp.size();
	
	// next, insert group a delimiters between adjacent groups
	for(int i = nMax - 1; i >= 0; --i) {
		
		vnBase[i] = vnBase[i + 1] - vnBase[i];
		a_grp[vnBase[i + 1] - 1] = GROUP_DELIMITER;
	}
	assert(vnBase[0] == 0);
	for(int i = 0; i < (int)a_map.size(); ++i) {
		
		if(a_map[i] != GROUP_DELIMITER) {
			
			a_grp[vnBase[a_map[i]]++] = i;
		}
	}

}

// build data model map, which contains all keys and its values used in generating code
// there are two types of keys according the way dealing its value,
// the first is the one whose value is text, and will be copied literally to output stream,
// the other is macro-name, to get its value, macro expansion is needed.
// but all detail is hidden, we can treat them no difference
void dmmap_t::buildMap(const dtable_t &a_dTbl, const ptable_t &a_pTbl, const gsetting_t &a_gsetup) {

	/********************************************************************
	 * Outline of script
	 * 
	 * 		declaration section
	 * 		
	 * 		%%
	 * 		
	 * 		RED section: regular express definition section
	 * 
	 * 		%%
	 * 
	 * 		GD section: grammar definition section
	 * 
	 * 		%%
	 * 
	 * 		user-code section
	 * 
	 * Remarks
	 *  
	 * 1 declaration section
	 * 	you may want to give some of following declarations here
	 * 		1) regular expression names,
	 * 		2) start-conditions
	 * 		3) tokens,
	 * 		4) variables(aka types),
	 * 	  	5) start symbol of grammar,
	 * 		6) options,
	 * 		7) built-in option beginning with '%', such as %formal-param, %actual-param, etc
	 * 		8) header code, which is placed at top of generated source file.
	 * 
	 * 2 RED section
	 * 	Lexical pattern in the manner of RE, and its corresponding action if it has, define
	 *  in this section. 
	 * 
	 * 3 GD section
	 *  CFG(context-free grammar), and its corresponding actions if it has any, define
	 *  here.
	 * 
	 * 4 user-code section
	 *  All code in user-code section are literally copied to generated source file
	 * 
	 * Last note: all section can be empty.
	 * *****************************************************************/
	vint_t vi, vi2;
	int i;
	
	bool b1 = isAction(CKEY_FORMAL_PARAMETERS);
	bool b2 = isAction(CKEY_ACTUAL_PARAMETERS);
	if((b1 && !b2) || (!b1 && b2)) {
		_WARNING("formal parameters and actual parameters are not consistent.");
	}
	
	// global settings
	insert(CKEY_ENABLE_LOCATION, a_gsetup.m_bLocCompute ? TRUE : FALSE);
	insert(CKEY_ENABLE_COLUMN, (a_gsetup.m_bColCompute && a_gsetup.m_bLocCompute) ? TRUE : FALSE);
	insert(CKEY_ENABLE_LINENO, a_gsetup.m_bEnableLineNo? TRUE : FALSE);
	if(! isString(CKEY_NAME_PREFIX)) {
		if(a_gsetup.m_pchNamePrefix) {
			
			insert(CKEY_NAME_PREFIX, a_gsetup.m_pchNamePrefix);
		}
		else {
			insert(CKEY_NAME_PREFIX, CVAL_DEFAULT_PREFIX);
		}
	}
	insert(CKEY_ENABLE_SCANNER, a_gsetup.m_bNoScanner ? FALSE : TRUE);
	insert(CKEY_ENABLE_PARSER, a_gsetup.m_bNoParser ? FALSE : TRUE);

	insert(CKEY_SOFTWARE_NAME, CVAL_SOFTWARE_NAME);
	insert(CKEY_SOFTWARE_VERSION, VERSION_DESC);

	// settings for scanner
	if( ! a_gsetup.m_bNoScanner) {
		// lexical actions
		insert(CKEY_LEX_ACTIONS, a_dTbl.getActions());
		// flag: is there an EOS action gived by user
		if(a_dTbl.getEOAFAction() != nullptr) {
		
			insert(CKEY_EOAF_ACTION, a_dTbl.getEOAFAction());
		}
		// flag: is there lexical init-action provided by user
		// lexical init-action will be performed at beginning stage of lexical analysis
		if(a_dTbl.getLexInitAction() != nullptr) {
			
			insert(CKEY_LEXINIT_ACTION, a_dTbl.getLexInitAction());
		}
		// flag: is there pre-match action provided by user
		// pre-match action is performed whenever a pattern(or RE) matched
		if(a_dTbl.getPrematchAction() != nullptr) {
			
			insert(CKEY_PREMATCH_ACTION, a_dTbl.getPrematchAction());
		}
	
		// before insert the key, map rules to actions just for the case of multiple rules
		// share same action,
		map2Groups(a_dTbl.getRule2Actions(), vi);
		insert(CKEY_LEX_RULE2ACTIONS, vi);
		// character map, and meta-character, to minimize number of invalid characters 
		insert(CKEY_LEX_CHARMAP, a_dTbl.getCharMap());
		insert(CKEY_LEX_METACHARS, a_dTbl.getMeta());
	
	// keys related to DFAs
		// start state of DFAs
		insert(CKEY_LEX_STARTS, a_dTbl.getStarts());
		// indexes of start-conditions, INITIAL is always 0
		insert(CKEY_LEX_STARTINDEXES, a_dTbl.getStartIndexes());
		// start-condition names
		insert(CKEY_LEX_STARTLABLES, a_dTbl.getStartLabels());
		// DFA transition table
		// here, we use next/check/base/default scheme to store DFA
		// for more information, refer the book
		//	Compiler: Principles, Techniques, and Tools
		// at page 144-146, chapter 3
		insert(CKEY_LEX_DFA_NEXTS, a_dTbl.getNexts());
		// check table for next table (transition table)
		insert(CKEY_LEX_DFA_CHECKS, a_dTbl.getChecks());
		// store offset for each state to lookup it's transitions
		// in next table and check table
		insert(CKEY_LEX_DFA_BASES, a_dTbl.getBases());
		// optional offset for each state when the current offset is invalid
		insert(CKEY_LEX_DFA_DEFAULTS, a_dTbl.getDefaults());
		// store rule accepted in each state
		insert(CKEY_LEX_DFA_ACCEPTS, a_dTbl.getAccepts());
		// indicate which state should go to, if no valid transition available
		insert(CKEY_LEX_DEFAULT_STATE, a_dTbl.getDefaultState());
		// trap state, no out-going transitions
		insert(CKEY_LEX_ERROR_STATE, INVALID_STATE);
		// invalid rule
		insert(CKEY_LEX_ERROR_RULE, INVALID_RULE);
		// this constant used by computing index of a rule that matches EOF
		// for example, in start-condition INITIAL, index of the rule that
		// matches EOF is 
		//	DEOFRULE_BASE + INITIAL
		insert(CKEY_LEX_EOFRULEBASE, a_dTbl.getEOFRuleBase());
		// index of the rule that matches EOS (end of stream)
		insert(CKEY_LEX_EOFARULEINDEX, (int)a_dTbl.getRule2Actions().size());
		// flag: are there any lookahead states
		if(a_dTbl.hasLookahead()) {
			
			insert(CKEY_LEX_DFA_LOOKAHEADS, a_dTbl.getLookaheads());
		}
		
		// debug options for scanner 
		if(a_gsetup.m_nLexDLevel > 0) {
			insert(CKEY_LEX_DLEVEL, a_gsetup.m_nLexDLevel);
			insert(CKEY_LEX_DMODE, TRUE);
			// map: rule index --> lineno
			insert(CKEY_LEX_RULE2LINES, a_dTbl.getRule2LineNos());
		}
		else {
			insert(CKEY_LEX_DLEVEL, 0);
			insert(CKEY_LEX_DMODE, FALSE);
		}
		
		insert(CKEY_ENABLE_DEFAULT_ACTION, a_gsetup.m_bDefaultAction ? TRUE : FALSE);
	}

	// built-in token IDs and its indices
	insert(CKEY_PARSE_ENDTOKENID, END_SYMBOL_ID);
	insert(CKEY_PARSE_ERRORTOKENID, ERROR_SYMBOL_ID);
	insert(CKEY_PARSE_UNDEFTOKENID, UNDEFINED_SYMBOL_ID);
	insert(CKEY_PARSE_ENDSYMBIDX, END_SYMBOL_INDEX);
	insert(CKEY_PARSE_ERRORSYMBIDX, ERROR_SYMBOL_INDEX); 
	insert(CKEY_PARSE_UNDEFSYMBIDX, UNDEFINED_SYMBOL_INDEX);

	// settings for parser
	if( ! a_gsetup.m_bNoParser) {
		// token names, provided by %token, %left, %right, %nonassoc
		if(a_pTbl.getTokenDefNames().size() > 0) {
			insert(CKEY_PARSE_TOKENNAME, a_pTbl.getTokenDefNames());
		}
		// token values, auto-generated by upgen
		if(a_pTbl.getTokenDefValues().size() > 0) {
			insert(CKEY_PARSE_TOKENVALUE, a_pTbl.getTokenDefValues());
		}
		// start state of LALR
		insert(CKEY_PARSE_STARTSTATE, a_pTbl.getStartState());
		// accept state of LALR
		assert(a_pTbl.getAcceptState() != INVALID_STATE);
		insert(CKEY_PARSE_ACCEPTSTATE, a_pTbl.getAcceptState());
		// trap state of LALR
		insert(CKEY_PARSE_ERRORSTATE, a_pTbl.getErrorState());
		// invalid rule of LALR
		insert(CKEY_PARSE_ERRORRULE, a_pTbl.getErrorRule());
		// type-base: index of first type(non-terminal) to symbol table
		// those symbols whose indices are less than type-base
		// are tokens(terminals); otherwise, types(non-terminals)
		insert(CKEY_PARSE_TYPEBASE, a_pTbl.getTypeBase());

		// number of symbols
		insert(CKEY_PARSE_SYMBOLNUM, a_pTbl.getSymNum());
		// token map: token ID --> index of token
		insert(CKEY_PARSE_TOKENMAP, a_pTbl.getTokenMap());
		// token IDs
		insert(CKEY_PARSE_TOKENID, a_pTbl.getTIDTable());
		// numbers of symbols in right part of grammar rules
		insert(CKEY_PARSE_RPNUM, a_pTbl.getRPNumTable());
		// symbol ID at left part of grammar rules
		insert(CKEY_PARSE_LPID, a_pTbl.getLPartIndex());
		// action table
		insert(CKEY_PARSE_ACTENTRIES, a_pTbl.getActionTable());
		// the base address of the entries for each state stored in action table
		insert(CKEY_PACTION_BASES, a_pTbl.getActionBase());
		// goto table
		insert(CKEY_PARSE_GOTOS, a_pTbl.getGotoTable());
		// the base address of the entries for each state stored in goto table
		insert(CKEY_PGOTO_BASES, a_pTbl.getGotoBase());

		// tables used to compress action table and goto table
		insert(CKEY_PARSE_VALID_BMAP, a_pTbl.getValidBMap().getBVect());
		insert(CKEY_PARSE_BMAP_ROWSIZE, a_pTbl.getValidBMap().getRowSize());
		insert(CKEY_PARSE_ACT_ROWVAL, a_pTbl.getActRowVal());
		insert(CKEY_PARSE_ACT_ROWNICE, a_pTbl.getActRowNice());
		insert(CKEY_PARSE_COLVAL, a_pTbl.getColVal());
		insert(CKEY_PARSE_COLNICE, a_pTbl.getColNice());
		insert(CKEY_PARSE_GOTO_ROWVAL, a_pTbl.getGotoRowVal());
		insert(CKEY_PARSE_GOTO_ROWNICE, a_pTbl.getGotoRowNice());

		// user-defined parse-actions
		insert(CKEY_PARSE_ACTIONS, a_pTbl.getActions());
		vi.clear();
		map2Groups(a_pTbl.getRule2Actions(), vi);
		insert(CKEY_PARSE_RULE2ACTIONS, vi);

		// debug settings for parser
		if(a_gsetup.m_nParseDLevel > 0) {
			
			insert(CKEY_PARSE_DLEVEL, a_gsetup.m_nParseDLevel);
			insert(CKEY_PARSE_DMODE, TRUE);
			// symbol names
			insert(CKEY_PARSE_SYMNAME, a_pTbl.getSName());
			// 
			insert(CKEY_PARSE_RPBASES, a_pTbl.getRPBase());
			insert(CKEY_PARSE_RPINDEXES, a_pTbl.getRPartIndex());
			insert(CKEY_PARSE_RULE2LINES, a_pTbl.getRule2LineNos());
		}
		else {
			insert(CKEY_PARSE_DLEVEL, 0);
			insert(CKEY_PARSE_DMODE, FALSE);
		}

		// settings for destructor
		if(a_pTbl.getDstrctActions().size() > 0) {
			
			vi.clear();
			map2Groups(a_pTbl.getDstrctIndex(), vi);
			for(i = 0; i < (int)vi.size(); ++i) {
				if(vi[i] != GROUP_DELIMITER) {
					vi2.push_back(a_pTbl.getDstrctSymb()[vi[i]]);
				}
				else {
					vi2.push_back(GROUP_DELIMITER);
				}
			}
			insert(CKEY_DESTRUCTOR_ACTIONS, a_pTbl.getDstrctActions());
			insert(CKEY_DESTRUCTOR_SYMBS, vi2);
		}
	}
}

}
