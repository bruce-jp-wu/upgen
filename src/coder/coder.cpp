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

#include <iostream>
using std::cout;
#include <iosfwd>
using std::ostream;
#include <fstream>
using std::ofstream;

#include "../common/reporter.h"
#include "../common/filehelper.h"
using common_ns::filehelper_t;
#include "./metakeys.h"
#include "./coder.h"
#include "./dmmap.h"
#include "./cmmgr.h"
#include "../gcode/coderyac.h"
using namespace coder_nsx;

namespace coder_ns {

coder_t::~coder_t(void) {
	
	if( ! m_strDefName.empty()) {
		if(m_posDecl && m_posDef != m_posDecl) {
			
			m_posDecl->flush();
			((ofstream*)m_posDecl)->close();
			delete m_posDecl;
		}
		if(m_posDef) {
			
			m_posDef->flush();
			((ofstream*)m_posDef)->close();
			delete m_posDef;
		}	
	}
}

// prepare code generation
bool coder_t::openStream(dmmap_t &dmap) {
	
	if(m_strDefName.empty()) {
		
		// code redirect to standard output
		m_posDecl = &cout;
		m_posDef = &cout;
	}
	else {
		
		string strMsg;
		
		m_posDecl = nullptr;
		m_posDef = new ofstream(m_strDefName.c_str());
		
		if(!m_posDef || !(*m_posDef)) {
			// given file name is illegal, or access denied
			strMsg = "failed to create file `";
			strMsg += m_strDefName;
			strMsg += "\'.";
			_ERROR(strMsg, ECMD(10));
			
			if(m_posDef) {
			
				delete m_posDef;
			}
			m_posDef = nullptr;
			m_posDecl = nullptr;
			
			return false;
		}
		
		if(m_gSetup.m_bEnableDeclare) {
			
			string strName, strHExt, strExt, strDir;
			
			// try to get declaration file name
			s2s_cit_t cit = m_specSetupMap.find(CKEY_DECLARE_EXT);
			if(cit != m_specSetupMap.end()) {
				
				strHExt = cit->second;
			}
			else {
				strHExt = "";
			}
			
			if(strHExt.empty()) {
				// no declaration file for given language SPEC
				_WARNING("extended name for declaration file not specified in SPEC, merge declaration into definition.");
				m_gSetup.m_bEnableDeclare = false;
				
				m_posDecl = m_posDef;
			}
			else {
				
				filehelper_t::extractExt(m_strDefName, strName, strExt);
				strName += strHExt;
				
				// check if declaration file name is the same as definition file name
				if(strName == m_strDefName) {
					m_gSetup.m_bEnableDeclare = false;
					m_posDecl = m_posDef;
				}
				else {
				
					m_posDecl = new ofstream(strName.c_str());
					
					if(!m_posDecl || !(*m_posDecl)) {
						
						strMsg = "failed to create file `";
						strMsg += strName;
						strMsg += "\'.";
						_ERROR(strMsg, ECMD(10));
						
						delete m_posDef;
						m_posDef = nullptr;
						
						if(m_posDecl) {
						
							delete m_posDecl;
							m_posDecl = nullptr;
						}
						
						return false;
					}
					
					// declaration file name is different from definition file name
					filehelper_t::extractDir(strName, strDir, strExt);
					// add declaration file name to data map in case of referrenced some place
					dmap.insert(CKEY_DECLARE_FILE, strExt);
				}
			}
		}
	}
	
	// add declaration file name to data map in case of referrenced some place
	dmap.insert(CKEY_DEFINE_FILE, m_strDefName);
	if(! m_gSetup.m_bEnableDeclare) {
		dmap.insert(CKEY_DECLARE_FILE, m_strDefName);
	}

	dmap.insert(CKEY_ENABLE_DECLARE, m_gSetup.m_bEnableDeclare ? TRUE : FALSE);
	
	return true;
}

// generate code
// @a_inFileName: name of language SPEC file
// @a_outFileName: name of outputted file
bool coder_t::gencode(const string &a_inFileName, const string &a_outFileName) {

	cmacro_mgr_t cmmgr;
	m_dmap.buildMap(m_dTbl, m_pTbl, m_gSetup);
	
	m_strDefName = a_outFileName;
	
	bool bret = false;
	
	do {
	
        if(0 != coder_nsx::yylexfile(a_inFileName.c_str(), false)) {
			_ERROR("failed to SPEC file.", ECMD(11));
			break;
		}
	
        if(0 != coder_nsx::yyparse(*this, m_dmap, cmmgr)) {
			
			break;
		}
		
		bret = true;
		
	}while(0);

	return bret;
}

// generate code
// @a_strSpec: language SPEC in form of string buffer
// @a_nSize: size of @a_strSpec, including ending nullptr
// @a_outFileName: name of outputted file
bool coder_t::gencode(char *a_strSpec, int a_nSize, const string &a_outFileName) {
	
	cmacro_mgr_t cmmgr;
	dmmap_t dmap;
	m_dmap.buildMap(m_dTbl, m_pTbl, m_gSetup);
	
	m_strDefName = a_outFileName;
	
	bool bret = false;
	
	do {

        if(0 != coder_nsx::yylexstr(a_strSpec, a_nSize)) {
			
			_ERROR("failed to open buffer", ECMD(11));
			break;	
		}
	
        if(0 != coder_nsx::yyparse(*this, m_dmap,cmmgr)) {
			
			break;
		}
		
		bret = true;
		
	}while(0);

	return bret;
}

}
