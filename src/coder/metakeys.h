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

#ifndef CODER_METAKEYS_H__
#define CODER_METAKEYS_H__

// build-in macros
#define CKEY_GET_ACTION					"Action"
#define CKEY_GET_ACTLINE				"ActLine"
#define CKEY_GOFIRST					"GoFirst"
#define CKEY_GET_INDEX					"Index"
#define CKEY_GET_INT					"Integer"
#define CKEY_GET_SIZE					"Size"
#define CKEY_SKIP						"Skip"
#define CKEY_GET_SRCLINE				"SrcLine"
#define CKEY_GET_STR					"String"
#define CKEY_GET_TEXT					"Text"

#define INDEX_GET_ACTION				0
#define INDEX_GET_ACTLINE				1
#define INDEX_GOFIRST					2
#define INDEX_GET_INDEX					3
#define INDEX_GET_INT					4
#define INDEX_GET_SIZE					5
#define INDEX_SKIP						6
#define INDEX_GET_SRCLINE				7
#define INDEX_GET_STR					8
#define INDEX_GET_TEXT					9

#define ATOMIC_MACRO_NUM				10

/******************************************************
 * 
 * name of keys used in language SPECs
 * 
 * ***************************************************/

#define CKEY_SOFTWARE_NAME				"SoftwareName"
#define CVAL_SOFTWARE_NAME				"Upgen"
#define CKEY_SOFTWARE_VERSION			"SoftwareVersion"

#define CKEY_DEFINE_EXT					"DefinitionExtName"
#define CKEY_DECLARE_EXT				"DeclarationExtName"
#define CKEY_DECLARE_FILE				"DeclareFileName"
#define CKEY_DEFINE_FILE				"DefineFileName"
#define CKEY_SCRIPT_FILE				"ScriptFileName"
#define CKEY_FILE_BASENAME				"FileBaseName"

#define CKEY_NAME_PREFIX				"Prefix"
#define CVAL_DEFAULT_PREFIX				"yy"

#define CKEY_DECL_HEADER				"DeclareHeader"
#define CKEY_DEF_HEADER					"DefineHeader"
#define CKEY_USER_CODE					"UserCode"

#define CKEY_LEXVAR_DECL				"LexVarDecl"
#define CKEY_LEXVAR_INIT				"LexVarInit"
#define CKEY_PARSEVAR_DECL				"ParseVarDecl"
#define CKEY_PARSEVAR_INIT				"ParseVarInit"

#define CKEY_EOAF_ACTION				"EOAFAction"
#define CKEY_LEXINIT_ACTION				"LexInitAction"
#define CKEY_PREMATCH_ACTION			"PremacthAction"

#define CKEY_PARSEINIT_ACTION			"ParseInitAction"
#define CKEY_PARSEEXIT_ACTION			"ParseExitAction"

// keys of options(both internal and external options)
#define CKEY_LEX_DLEVEL					"LexDebugLevel"
#define CKEY_LEX_DMODE					"LexDebugMode"
#define CKEY_PARSE_DLEVEL				"ParseDebugLevel"
#define CKEY_PARSE_DMODE				"ParseDebugMode"
#define CKEY_ENABLE_LINENO				"EnableLineNo"
#define CKEY_ENABLE_LOCATION			"EnableLocation"
#define CKEY_ENABLE_COLUMN				"EnableColumn"
#define CKEY_ENABLE_DEFAULT_ACTION		"EnableDefaultAction"
#define CKEY_ENABLE_SCANNER				"EnableScanner"
#define CKEY_ENABLE_PARSER				"EnableParser"

#define CKEY_ENABLE_DECLARE				"EnableDeclare"
#define CKEY_ENABLE_INTERACTIVE			"EnableInteractive"

#define CKEY_FORMAL_PARAMETERS			"FormalParams"
#define CKEY_ACTUAL_PARAMETERS			"ActualParams"

#define CKEY_STYPE_DECLARE				"STypeDeclare"
#define CKEY_LTYPE_DECLARE				"LTypeDeclare"
#define CKEY_STYPE_NAME					"STypeName"
#define CKEY_LTYPE_NAME					"LTypeName"

// keys related to regular expressions and corresponding actions
#define CKEY_LEX_ACTIONS				"LexActions"
#define CKEY_LEX_RULE2ACTIONS			"LexRule2Actions"
#define CKEY_LEX_RULE2LINES				"LexRule2Lines"
#define CKEY_LEX_CHARMAP				"LexCharMap"
#define CKEY_LEX_METACHARS				"LexMetaChars"

// keys related to DFAs
#define CKEY_LEX_STARTS					"LexStartStates"
#define CKEY_LEX_STARTLABLES			"LexStartLabels"
#define CKEY_LEX_STARTINDEXES			"LexStartIndexes"
#define CKEY_LEX_DFA_NEXTS				"LexDFANexts"
#define CKEY_LEX_DFA_CHECKS				"LexDFAChecks"
#define CKEY_LEX_DFA_BASES				"LexDFABases"
#define CKEY_LEX_DFA_DEFAULTS			"LexDFADefaults"
#define CKEY_LEX_DFA_ACCEPTS			"LexDFAAccepts"
#define CKEY_LEX_DFA_LOOKAHEADS			"LexDFALookaheads"

#define CKEY_LEX_DEFAULT_STATE			"LexDefaultState"
#define CKEY_LEX_EOFRULEBASE			"LexEOFRuleBase"
#define CKEY_LEX_EOFARULEINDEX			"LexEOFARuleIndex"
#define CKEY_LEX_ERROR_STATE			"LexErrorState"
#define CKEY_LEX_ERROR_RULE				"LexErrorRule"


// keys related to parser
#define CKEY_DESTRUCTOR_ACTIONS			"DestructorActions"
#define CKEY_DESTRUCTOR_LINES			"DestructorLines"
#define CKEY_DESTRUCTOR_SYMBS			"DestructorSymbols"

#define CKEY_PARSE_STARTSTATE			"ParseStartState"
#define CKEY_PARSE_ACCEPTSTATE			"ParseAcceptState"
#define CKEY_PARSE_ERRORSTATE			"ParseErrorState"
#define CKEY_PARSE_ERRORRULE			"ParseErrorRule"

#define CKEY_PARSE_ENDTOKENID			"ParseEndSymbID"
#define CKEY_PARSE_ERRORTOKENID			"ParseErrorSymbID"
#define CKEY_PARSE_UNDEFTOKENID			"ParseUndefSymbID"
#define CKEY_PARSE_ENDSYMBIDX			"ParseEndSymbIdx"
#define CKEY_PARSE_ERRORSYMBIDX			"ParseErrorSymbIdx"
#define CKEY_PARSE_UNDEFSYMBIDX			"ParseUndefSymbIdx"

#define CKEY_PARSE_TOKENNAME			"ParseTokenNames"
#define CKEY_PARSE_TOKENVALUE			"ParseTokenValues"

// update 16/12/17
#define CKEY_PARSE_STRTOKENNAME         "ParseStrTokenNames"
#define CKEY_PARSE_STRTOKENVALUE        "ParseStrTokenValues"

#define CKEY_PARSE_TYPEBASE				"ParseTypeBase"
#define CKEY_PARSE_SYMBOLNUM			"ParseSymbolNum"
#define CKEY_PARSE_TOKENMAP				"ParseTokenMap"
#define CKEY_PARSE_TOKENID				"ParseTokenID"
#define CKEY_PARSE_RPNUM				"ParseRPNum"
#define CKEY_PARSE_LPID					"ParseLPID"
#define CKEY_PARSE_ACTENTRIES			"ParseActEntries"
#define CKEY_PACTION_CHECKS				"PActionChecks"
#define CKEY_PACTION_BASES				"PActionBases"
#define CKEY_PARSE_GOTOS				"ParseGotos"
#define CKEY_PGOTO_CHECKS				"PGotoChecks"
#define CKEY_PGOTO_BASES				"PGotoBases"

#define CKEY_PDEF_RULES					"ParseDefRules"
#define CKEY_PARSE_ACTIONS				"ParseActions"
#define CKEY_PARSE_RULE2ACTIONS			"PraseRule2Actions"

#define CKEY_PARSE_VALID_BMAP			"ParseValidBMap"
#define CKEY_PARSE_BMAP_ROWSIZE			"ParseBMapRowSize"
#define CKEY_PARSE_ACT_ROWVAL			"ParseActRowVal"
#define CKEY_PARSE_ACT_ROWNICE			"ParseActRowNice"
#define CKEY_PARSE_COLVAL				"ParseColVal"
#define CKEY_PARSE_COLNICE				"ParseColNice"
#define CKEY_PARSE_GOTO_ROWVAL			"ParseGotoRowVal"
#define CKEY_PARSE_GOTO_ROWNICE			"ParseGotoRowNice"

// only available when turn on parse-debug mode, that is global-setting's @m_nParseDLevel > 0
#define CKEY_PARSE_RPBASES				"ParseRPBases"
#define CKEY_PARSE_RPINDEXES			"ParseRPIndexes"
#define CKEY_PARSE_RULE2LINES			"ParseRule2Lines"
#define CKEY_PARSE_SYMNAME				"ParseSymName"

// miscellanuous
			
#endif // CODER_METAKEYS_H__
