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


#ifndef UPGEN_UPGEN_H__
#define UPGEN_UPGEN_H__

// version information
// number of major version
#define VERSION_MAJOR	0
// number of minor version
#define VERSION_MINOR	5
// build number
#define VERSION_RELEASE	3

// update 14/12/17
//#define VERSION_DESC	"0.5.2"

#define UPGEN_TEST_VERSION	1
#undef UPGEN_TEST_VERSION

// global setting for upgen
typedef struct _gsetting_t {
	
public:
	
	// @m_bCaseSensitive: flag indicating whether or not 
	// 		input information is treated case-sensitively
	// option: -i
	bool m_bCaseSensitive;
	// @m_bNoScanner: flag indicating whether or not
	//		generated program includes pattern-matching code
	bool m_bNoScanner;
	// @m_bNoParser: flag indicating whether or not
	//		generated program includes parsing code
	bool m_bNoParser;
	// @m_bOutTables: flag indicating whether or not
	// 		automations (in form of table) needs to be exported
	bool m_bOutTables;
	// @m_bLocCompute: flag indicating whether or not
	// 		location information needs to be computed
	bool m_bLocCompute;
	// @m_bColCompute: flag indicating whether or not
	//		column of location information needs to be computed
	bool m_bColCompute;
	// @m_bOutVerbos: flag indicating whether or not
	// 		detailed information of grammar and LR(1) needs to be exported
	bool m_bOutVerbose;
	// @m_bDefaultAction: flag indicating whether or not
	//		default action can be matched in the process of pattern matching
	bool m_bDefaultAction;
	// @m_bEnableDeclare: flag indication whether or not
	//		declaration should be seperated from definition
	bool m_bEnableDeclare;
	// @m_nParseDLevel: debug level for parsing program, if it is zero,
	//		then no any debug information about parsing is produced
	bool m_bEnableLineNo;
	// @m_bEnableLineNo:  flag indicating whether
	//		generate #line directives or not
	int m_nParseDLevel;
	// @m_nLexDLevel: debug level for pattern-matching program, if it is
	//		then no any debug information about pattern-matching is produced
	int m_nLexDLevel;
	// @m_posLog: a pointer to output stream, used to prompt warning information
	// to user; in default case, it points to cerr
	void *m_posLog;
	// @m_posDetail: a pointer to output stream, used to report detailed information
	// on grammar to user; in default case, it points to nullptr
	void *m_posDetail;
	
	// language name of generated program
	char *m_pchLangName;
	
	// name prefix
	char *m_pchNamePrefix;
public:
	
	_gsetting_t(void);
	~_gsetting_t(void);

} gsetting_t;


#endif // UPGEN_UPGEN_H__
