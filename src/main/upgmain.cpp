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

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#include <fstream>
using std::ofstream;

#include <sys/stat.h>
#include <cstring>

#include "./upgmain.h"

#include "../common/clhandler.h"
using common_ns::cl_handler_t;
#include "../common/strhelper.h"
using common_ns::strhelper_t;
#include "../common/filehelper.h"
using common_ns::filehelper_t;
#include "../common/reporter.h"
using common_ns::reporter_factory_t;
#include "./mreporter.h"

#include "../coder/metakeys.h"
#include "../coder/coder.h"
using coder_ns::coder_t;
#include "../coder/dmmap.h"
using coder_ns::dmmap_t;

#include "../lexer/lexer.h"
using lexer_ns::lexer_t;
using lexer_ns::dtable_t;

#include "../parser/grammar.h"
using parser_ns::grammar_t;
#include "../parser/ptable.h"
using parser_ns::ptable_t;

#include "../spec/spec.h"
using namespace spec_ns;

#include "../gcode/ulyac.h"

enum {
	
	OFT_DEFAULT = 0,
	OFT_STDOUT,
	OFT_CUSTOM
};

// only used by flex
typedef void* yyscan_t;

// entry function to parse script file, this function is originally generated with the help of Flex & Bison
// @lexer: container that contains all the stuff needed in the process of scanning
// @dtbl: DFA tables to be created
// @dtbl: DFA tables to be created
// @grmmr: augmented CFG(context-free grammar) object, container that contains all
//			the stuff needed in the process of parsing
// @ptbl: parse tables to be created
// @dmap: data map, used to guide coder to generate code
// @gsetup: global settings specified in command-line, used to control parsing and code generating
// @return-value: value returned by yyparse, succeeded if it is zero, there are some errors otherwise
extern int debug_yyparse(lexer_ns::lexer_t &lexer, lexer_ns::dtable_t &dtbl,
		parser_ns::grammar_t &grammar, parser_ns::ptable_t &ptbl,
		dmmap_t &dmap, gsetting_t &gsetup);

// scan and parse script file
// @a_pchFile: name of script file
// @lexer: container that contains all the stuff needed in the process of scanning
// @dtbl: DFA tables to be created
// @grmmr: augmented CFG(context-free grammar) object, container that contains all
//			the stuff needed in the process of parsing
// @ptbl: parse tables to be created
// @dmap: data map, used to guide coder to generate code
// @gsetut: global settings specified in commandline, used to control parsing and code generating
// @return-value: succeeded or not
bool analyze(const char *a_pchFile,
		lexer_t &lexer, dtable_t &dtbl,
		grammar_t &grmmr, ptable_t & ptbl,
		dmmap_t &dmap, gsetting_t &gsetup);

// scan and parse input from stdin
// @lexer: container that contains all the stuff needed in the process of scanning
// @dtbl: DFA tables to be created
// @grmmr: augmented CFG(context-free grammar) object, container that contains all
//			the stuff needed in the process of parsing
// @ptbl: parse tables to be created
// @dmap: data map, used to guide coder to generate code
// @gsetup: options specified in command-line, used to control parsing and code generating
bool analyze(lexer_t &lexer, dtable_t &dtbl,
		grammar_t &grmmr, ptable_t & ptbl,
		dmmap_t &dmap, gsetting_t &gsetup);


// show usage of this program
void usage(ostream &os) {
	
	os << "UPGEN: a scanner and LALR parser generator.\n\
Usage: upgen [OPTIONS] FILENAME\n\
\n\
Options and associated arguments\n\
-c                  enable column information computation, \n\
                    to take effect, -l must be set\n\
-d                  generate diagnosis information for scanner\n\
-D                  generate diagnosis information for parser\n\
-H                  generate declaration file(e.g. .h file for C or C++)\n\
-i                  patterns match input text case-insensitively\n\
-l                  enable locations computation\n\
-L                  generate `#line\' (or likewise) directives\n\
-m                  do not generate scanner\n\
-o FILENAME         specify name of generated file\n\
-p                  do not generate parser\n\
-P PREFIX           name prefix, the default is yy\n\
-s SPEC             specify programming language in which program is generated\n\
-S                  enable default action in pattern-matching\n\
-t                  output generated program to stdout\n\
-v                  report details on LALR grammar\n\
-V                  show version information\n\
-\?, -h              show help message\n\
\n\
Examples:\n\
Assume there is an Upgen script named `dcalc.upg\' for desktop calculator program.\n\
If we want the program in C++ language, here we simply use:\n\
\n\
    upgen -o dcalc.cpp dcalc.upg\n\
\n\
because C++ is considered as default target language.\n\
If we want the program in Object Pascal, we use:\n\
\n\
	upgen -o dcalc.pas -s pas dcalc.upg\n\
Both C++ and Object Pascal are built-in target languages.\n\
If we have a JAVA language SPEC, say `upgen-java-spec.xml\', and want to\n\
generate the program in JAVA, then we use the following command instead:\n\
\n\
    upgen -o dcalc.java -s upgen-java-spec.xml dcalc.upg\n"
	<< endl;
}

// show current version
void version(ostream &os) {
	
	os << "upgen " << VERSION_MAJOR << "."
		<< VERSION_MINOR << "."
		<< VERSION_RELEASE << endl;
}

// scan and parse script file
// @a_pchFile: name of script file
// @lexer: container that contains all the stuff needed in the process of scanning
// @dtbl: DFA tables to be created
// @grmmr: augmented CFG(context-free grammar) object, container that contains all
//			the stuff needed in the process of parsing
// @ptbl: parse tables to be created
// @dmap: data map, used to guide coder to generate code
// @gsetup: global settings specified in command-line, used to control parsing and code generating
// @return-value: succeeded or not
bool analyze(const char *a_pchFile,
		lexer_t &lexer, dtable_t &dtbl,
		grammar_t &grmmr, ptable_t & ptbl,
		dmmap_t &dmap, gsetting_t &gsetup) {
	
	assert(a_pchFile && strlen(a_pchFile) > 0);
	
	// test if a file is accessible
	if( ! filehelper_t::isAccessible(a_pchFile)) {
		
		string strMsg = "failed to open file: `";
		strMsg += a_pchFile;
		strMsg += "\'";
		_ERROR(strMsg, ECMD(1));
		return false;
	}

	// initialization before parsing
	if(yylexfile(a_pchFile, false)) {
		string strMsg = "failed to open the file: `";
		strMsg += a_pchFile;
		strMsg += "\'.";
		_ERROR(strMsg, ECMD(1));
		return false;
	}
	
	// parse script file
	// in the process, regular expression trees and grammar object are constructed,
	// and also user actions are extracted from the script file
	bool bret = (0 == debug_yyparse(lexer, dtbl, grmmr, ptbl, dmap, gsetup));
	
	if(bret) {
		
		if( ! gsetup.m_bNoScanner) {
			
			// convert from regular expression trees to DFA tables
			lexer.retrees2DTables(dtbl, gsetup);
		}
		
		if( ! gsetup.m_bNoParser) {
			// convert from grammar object to LALR tables
			grmmr.gram2PTable(ptbl, gsetup);
		}
		
	}
	
	return bret;
}

// scan and parse input from stdin
// @lexer: container that contains all the stuff needed in the process of scanning
// @dtbl: DFA tables to be created
// @grmmr: augmented CFG(context-free grammar) object, container that contains all
//			the stuff needed in the process of parsing
// @ptbl: parse tables to be created
// @dmap: data map, used to guide coder to generate code
// @gsetup: options specified in command-line, used to control parsing and code generating
bool analyze(lexer_t &lexer, dtable_t &dtbl,
		grammar_t &grmmr, ptable_t & ptbl,
		dmmap_t &dmap, gsetting_t &gsetup) {

	// initialization before parsing
	if(yylexfile(nullptr, false)) {
		return false;
	}
	
	// parse script file
	// in the process, regular expression trees and grammar object are constructed,
	// and also user actions are extracted from the script file
	bool bret = (0 == debug_yyparse(lexer, dtbl, grmmr, ptbl, dmap, gsetup));
	
	if(bret) {
		
		if( ! gsetup.m_bNoScanner) {
			// convert from regular expression trees to DFA tables
			lexer.retrees2DTables(dtbl, gsetup);
		}
		
		if( ! gsetup.m_bNoParser) {
			// convert from grammar object to LALR tables
			grmmr.gram2PTable(ptbl, gsetup);
		}
	}
	
	return bret;
}


int upgmain(int argc, char **argv) {
	
	cl_handler_t chdlr;
	
	reporter_factory_t::setReporter(mreporter_t::getOne(""));
	
	if(!chdlr.process(argc, argv, "acdDhHilLmopPsStvV?", "0000000000202200000")) {
		// failed to process command line
		usage(cerr);
		return - 1;
	}
	
	if(chdlr.hasOption('?') || chdlr.hasOption('h')) {
		// show help information
		usage(cout);
		version(cout);
		return 0;
	}
	
	if(chdlr.hasOption('V')) {
		// show version information
		version(cout);
		return 0;
	}
	
	if(chdlr.hasOption('t') && chdlr.hasOption('o')) {
		
		_ERROR("invalid option, cannot use `-o\' and `-t\' together.", ECMD(2));
		usage(cerr);
		return -1;
	}
	if(chdlr.hasOption('t') && chdlr.hasOption('H')) {
		_ERROR("invalid option, cannot use `-H\' and `-t\' together.", ECMD(2));
		usage(cerr);
		return -1;		
	}
	
	string str, strSpec, strOut;
	// initialize global setting
	gsetting_t gsetup;
	
	int nOutFileType = OFT_DEFAULT;
	
	if(chdlr.getOption('d', str)) {
		
//		if(str.empty()) {
			gsetup.m_nLexDLevel = 1;
//		}
//		else {
//			if(!strhelper_t::isNum(str, 10)) {
//				// illegal debug level number
//				_ERROR("invalid debug argument.", ECMD(3));
//				return -1;
//			}
//			strhelper_t::toSnum(str, gsetup.m_nLexDLevel);
//		}
	}
	if(chdlr.getOption('D', str)) {
		
//		if(str.empty()) {
			gsetup.m_nParseDLevel = 1;
//		}
//		else {
//			if(!strhelper_t::isNum(str, 10)) {
//				// illegal debug level number
//				_ERROR("invalid debug argument.", ECMD(3));
//				return -1;
//			}
//			strhelper_t::toSnum(str, gsetup.m_nParseDLevel);
//		}
	}
	
	if(chdlr.getOption('i', str)) {
		// no argument permitted for option 'i': case-insensitive
		if(!str.empty()) {
			
			_ERROR("invalid argument with `-i\'.", ECMD(4));
			usage(cerr);
			return -1;
		}
		gsetup.m_bCaseSensitive = false;
	}
	
	if(chdlr.getOption('l', str)) {
		// no argument permitted for option 'l': enable location computation
		if(!str.empty()) {
			
			_ERROR("invalid argument with `-l\'.", ECMD(4));
			usage(cerr);
			return -1;
		}
		gsetup.m_bLocCompute = true;
	}
	if(chdlr.getOption('L', str)) {
		// no argument permitted for option 'L': enable generating #line directives
		if(!str.empty()) {
			
			_ERROR("invalid argument with `-L\'.", ECMD(4));
			usage(cerr);
			return -1;
		}
		gsetup.m_bEnableLineNo = true;
	}
	if(chdlr.getOption('c', str)) {
		// no argument permitted for option 'c': enable column of location computation
		if(!str.empty()) {
			
			_ERROR("invalid argument with `-c\'.", ECMD(4));
			usage(cerr);
			return -1;
		}
		gsetup.m_bColCompute = true;
	}
	if(chdlr.getOption('m', str)) {
		// no argument permitted for option 'm': disable pattern-matching program generation
		if(!str.empty()) {
			
			_ERROR("invalid argument with `-m\'.", ECMD(4));
			usage(cerr);
			return -1;
		}
		gsetup.m_bNoScanner = true;
	}
	if(chdlr.getOption('p', str)) {
		// no argument permitted for option 'm': disable parsing program generation
		if(!str.empty()) {
			
			_ERROR("invalid argument with `-p\'.", ECMD(4));
			usage(cerr);
			return -1;
		}
		gsetup.m_bNoParser = true;
	}

	if(chdlr.getOption('S', str)) {
		// no argument permitted for option 'S': enable default action in pattern-matching
		if(!str.empty()) {
			
			_ERROR("invalid argument with `-S\'.", ECMD(4));
			usage(cerr);
			return -1;
		}
		gsetup.m_bDefaultAction = true;
	}
	if(chdlr.getOption('H', str)) {
		// no argument permitted for option 'H': generate declaration file
		if(!str.empty()) {
			
			_ERROR("invalid argument with `-H\'.", ECMD(4));
			usage(cerr);
			return -1;
		}
		gsetup.m_bEnableDeclare = true;
	}	
	
	// try to get name of language specification
	if(chdlr.getOption('s', strSpec)) {
		
		if(strSpec.empty()) {
			
			_ERROR("invalid option `-s\', file name expected.", ECMD(5));
			usage(cerr);
			return -1;
		}
	}
	
	// try to get name of script file
	const vstr_t& vstrIn = chdlr.getNPArgs();
	if(vstrIn.size() != 1) {
		
		_ERROR("if and only if one script file expected.", ECMD(8));
		usage(cerr);
		return -1;
	}
	
	if( ! filehelper_t::isAccessible(vstrIn[0])) {
		
		string strMsg = "failed to open file: `";
		strMsg += vstrIn[0];
		strMsg += "\'";
		_ERROR(strMsg, ECMD(1));
		return -2;
	}
	
	str = "";
	
	if(chdlr.getOption('o', strOut)) {
		
		if(strOut.empty()) {
			
			_ERROR("invalid option `-o\', file name expected.", ECMD(6));
			usage(cerr);
			return -1;
		}
		
		// program export to file
		nOutFileType = OFT_CUSTOM;
	}
	else if(chdlr.getOption('t', str)) {
		
		if(!str.empty()) {
			
			_ERROR("invalid argument with `-t\'.", ECMD(7));
			usage(cerr);
			return -1;
		}
		// program redirect to stdout
		nOutFileType = OFT_STDOUT;
	}
	else {
		
		_ERROR("output option `-o\' or `-t\' needed.", ECMD(6));
		usage(cerr);
		return -1;
	}
	
	if(chdlr.getOption('P', str)) {
		
		if(str.empty()) {
			
			_ERROR("invalid option `-P\', name prefix expected.", ECMD(17));
			usage(cerr);
			return -1;
		}
		
		gsetup.m_pchNamePrefix = new char[str.size() + 1];
		strcpy(gsetup.m_pchNamePrefix, str.c_str());
		gsetup.m_pchNamePrefix[str.size()] = '\0';
	}
	
	if(chdlr.getOption('v', str)) {
		// no argument permitted for option 'v': enable detailed information producing
		if(!str.empty()) {
			
			_ERROR("invalid argument with `-v\'.", ECMD(4));
			usage(cerr);
			return -1;
		}
		gsetup.m_bOutVerbose = true;
		
		string strName;
		filehelper_t::extractExt(vstrIn[0], strName, str);
		strName += DETAILED_LOG_EXT;
		
		ofstream *ofs = new ofstream(strName.c_str());
		if(!ofs || !(*ofs)) {
			str = "failed to create detailed log file `";
			str += strName;
			str += "\'.";
			_ERROR(str, ECMD(4));
			return -1;		
		}
		gsetup.m_posDetail = ofs;
	}
	
	lexer_t lexer;
	dtable_t dfa;
	dmmap_t dmap;
	
	grammar_t grammar;
	ptable_t lalr;
	
	if( ! analyze(vstrIn[0].c_str(), lexer, dfa, grammar, lalr, dmap, gsetup)) {
		
		string strMsg = "No code generated for script `";
		strMsg += vstrIn[0];
		
		if(_GETERR() >= ERRID_BASE) {
			strMsg += "\', due to some errors.";
			_ERROR(strMsg, EOTH(100));
			return -2;
		}
		else {
			strMsg += "\', be sure it is legal script.";
			_WARNING(strMsg);
			return 0;
		}
	}
	
	if(_GETERR() >= ERRID_BASE) {
		
		// some error(s) occur
		string strMsg = "No code generated for script `";
		strMsg += vstrIn[0];
		strMsg += "\', due to some errors.";
		_ERROR(strMsg, EOTH(100));
		
		return -3;
	}
	
	if(gsetup.m_bNoParser && gsetup.m_bNoScanner) {
		// both pattern-matching and parsing program
		// generation disabled
		string strMsg = "No code generated for script `";
		strMsg += vstrIn[0];
		strMsg += "',\nsince both pattern-matching and parsing program generation disabled.";
		_WARNING(strMsg);
		return 0;
	}
	
	if( !gsetup.m_bNoScanner && dfa.isEmpty()) {
		// regular express definition section of script is supposed to be empty
		_WARNING("No code generated for pattern-matching,\n"
				"since section 2 turns out to be empty.");
		gsetup.m_bNoScanner = true;
	}
	
	if( !gsetup.m_bNoParser && lalr.isEmpty()) {
		// grammar definition section of script is supposed to be empty
		_WARNING("No code generated for parsing,\n"
				"since section 3 turns out to be empty.");
		gsetup.m_bNoParser = true;
	}
	
	if(gsetup.m_bNoParser && gsetup.m_bNoScanner) {
		// both regular express definition section and 
		// grammar definition section of script are supposed to be empty
		
		_WARNING("No code generated, since section 2 and 3 turn out to be empty.");
		return 0;
	}
	
	dmap.insert(CKEY_SCRIPT_FILE, vstrIn[0]);
	
	string strPrefix, strSuffix;
	filehelper_t::extractDir(vstrIn[0], strPrefix, str);
	filehelper_t::extractExt(str, strPrefix, strSuffix);
	dmap.insert(CKEY_FILE_BASENAME, strPrefix);
	
	strSpec = strhelper_t::toUpper(strSpec);
	
	// now get ready to generate code
	coder_t coder(dfa, lalr, dmap, gsetup);
	char* pSPEC = nullptr;
	int nSPECSize;
	int nPLID = getLangSPEC(strSpec.empty()? nullptr : strSpec.c_str(), pSPEC, nSPECSize);
	
	if(sn_ppchSPECName[nPLID]) {
	
		int len = strlen(sn_ppchSPECName[nPLID]);
		gsetup.m_pchLangName = new char[len + 1];
		strcpy(gsetup.m_pchLangName, sn_ppchSPECName[nPLID]);
	}
	
	if(SPEC_NUM == nPLID) {
		// test if a file is accessible
		if( ! filehelper_t::isAccessible(strSpec)) {
			
			string strMsg = "failed to open language specification file: `";
			strMsg += strSpec;
			strMsg += "\'";
			_ERROR(strMsg, ECMD(20));
			return false;
		}

		switch(nOutFileType) {
		
		case OFT_DEFAULT:
		case OFT_CUSTOM:
			
			// export code to file
			if(!coder.gencode(strSpec, strOut)) {
				
				return -3;
			}
			break;
			
		default:
			
			// export code to stdout
			if(!coder.gencode(strSpec, "")) {
				
				return -3;
			}
			break;
		}
	}
	
	else {
		
		switch(nOutFileType) {
		
		case OFT_DEFAULT:
		case OFT_CUSTOM:
			
			// export code to file
			if(!coder.gencode(pSPEC, nSPECSize, strOut)) {
				
				return -3;
			}
			break;
			
		default:
			
			// export code to stdout
			if(!coder.gencode(pSPEC, nSPECSize, "")) {
				
				return -3;
			}
			break;
		}
		
	}

	return 0;
}

_gsetting_t::_gsetting_t(void) {
	
	m_bCaseSensitive = true;
	m_nLexDLevel = 0;
	m_nParseDLevel = 0;
	m_bNoScanner = false;
	m_bNoParser = false;
	m_bOutTables = false;
	m_bLocCompute = false;
	m_bColCompute = false;
	m_bOutVerbose = false;
	m_bDefaultAction = false;
	m_bEnableDeclare = false;
	m_bEnableLineNo = false;
	m_posLog = &cerr;
	m_posDetail = nullptr;
	m_pchLangName = nullptr;
	m_pchNamePrefix = nullptr;
}

_gsetting_t::~_gsetting_t(void) {
	
	if(m_pchLangName) {
		
		delete[] m_pchLangName;
	}
	if(m_pchNamePrefix) {
		delete[] m_pchNamePrefix;
	}
	
	if(m_posDetail) {
		ofstream *pfos = (ofstream*)m_posDetail;
		pfos->close();
		delete pfos;
		m_posDetail = nullptr;
	}
}
