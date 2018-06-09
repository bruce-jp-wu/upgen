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

#ifndef CODER_CODER_H__
#define CODER_CODER_H__
#include <cassert>
#include <string>
using std::string;

#include <iostream>
#include <iosfwd>
using std::ostream;

#include "../main/upgmain.h"

#include "../common/types.h"

#include "../lexer/dtable.h"
using lexer_ns::dtable_t;

#include "../parser/ptable.h"
using parser_ns::ptable_t;

#include "./dmmap.h"

namespace coder_ns {
// code generator class
class coder_t {
	
private:
	
	// name definition file(source file for C/C++) 
	string m_strDefName;
	
	// referrence to DFA table object
	dtable_t &m_dTbl;
	// referrence to parser table object
	ptable_t &m_pTbl;
	// referrence to data map
	dmmap_t &m_dmap;
	// referrrence to global setting
	gsetting_t &m_gSetup;
	
	// output stream for declaration file(header file for C/C++)
	ostream *m_posDecl;
	// output stream for definition file(source file for C/C++).
	// it might be the same value as @m_posDecl
	ostream *m_posDef;
	// current output stream, can be either @m_posDecl or @m_posDef
	ostream *m_posCur;
	
	// setting(a set of (key, value) pairs) in language SPEC
	s2s_map_t m_specSetupMap;
	
private:
	
	// temporary variables used only in the process of parsing
	int mt_nFalses;
	bool mt_bsentoTrash;

public:
	
	// prepare code generation
	bool openStream(dmmap_t &dmap);
	
	// generate code
	// @a_inFileName: name of language SPEC file
	// @a_outFileName: name of output file
	bool gencode(const string &a_inFileName, const string &a_outFileName);
	// generate code
	// @a_strSpec: language SPEC in form of string buffer
	// @a_nSize: size of @a_strSpec, including ending nullptr
	// @a_outFileName: name of output file
	bool gencode(char *a_strSpec, int a_nSize, const string &a_outFileName);
	
public:
	
	coder_t(dtable_t &a_dtbl, ptable_t& a_ptbl, dmmap_t &a_dmap, gsetting_t &a_gsetup)
	: m_strDefName("")
	, m_dTbl(a_dtbl)
	, m_pTbl(a_ptbl)
	, m_dmap(a_dmap)
	, m_gSetup(a_gsetup)
	, m_posDecl(nullptr)
	, m_posDef(nullptr)
	, m_posCur(nullptr)
	, mt_nFalses(0)
	, mt_bsentoTrash(false) {
	}

	~coder_t(void);
	
	// output code to declaration file
	inline void switchToDecl(void) {
		
		if( m_gSetup.m_bEnableDeclare) {
			
			m_posCur = m_posDecl;
			m_dmap.switchtoDecl();
		}
		else {
			
			mt_bsentoTrash = true;
		}
	}
	
	/// output code to definition file
	inline void switchToDef(void) {

		mt_bsentoTrash = false;
		m_posCur = m_posDef;
		m_dmap.switchtoDef();
	}
	
	// stream flush
	inline void flush(void) {
		
		if(m_posDecl) {
			
			m_posDecl->flush();
		}
		
		if(m_posDef) {
			
			m_posDef->flush();
		}
	}
	
	// get current stream object
	inline ostream& getStream(void) {
		
		assert(m_posCur);
		return *m_posCur;
	}
	
	// add SPEC setting
	inline bool addSetupItem(const string &a_strKey, const string &a_strVal) {
		
		return m_specSetupMap.insert(s2s_pair_t(a_strKey, a_strVal)).second;
	}
	
	// determine if following code block need to output
	inline bool isCondTrue(void) const {
		
		return 0 == mt_nFalses;
	}
	inline bool isThrowAway(void) const {
				
		return (mt_nFalses > 0)	|| mt_bsentoTrash;
	}
	
	inline void addIPCond(bool a_bSatified) {
		
		if( ! a_bSatified) {
			++mt_nFalses;
		}
	}
	inline void subIPCond(bool a_bSatified) {
		
		if( ! a_bSatified) {
			--mt_nFalses;
		}
	}
};

}

#endif // CODER_CODER_H__
