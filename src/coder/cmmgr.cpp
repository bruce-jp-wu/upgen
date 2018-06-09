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


#include <cstring>
#include "./cmmgr.h"
#include "../common/strhelper.h"
using common_ns::strhelper_t;

namespace coder_ns {

// constructor: initialize macro manager
cmacro_mgr_t::cmacro_mgr_t(void)
: m_condHas(false)
, m_condEmpty(true) {
	
	m_vstrEmptyParam.push_back(SKEY_EMPTY_PARAM);
	////////////////////////////////////////////////////////////////////////////
	// initialize built-in macros
	m_pcchAtomicMacro[INDEX_GET_ACTION] = CKEY_GET_ACTION;
	m_pcchAtomicMacro[INDEX_GET_ACTLINE] = CKEY_GET_ACTLINE;
	m_pcchAtomicMacro[INDEX_GOFIRST] = CKEY_GOFIRST;
	m_pcchAtomicMacro[INDEX_GET_INDEX] = CKEY_GET_INDEX;
	m_pcchAtomicMacro[INDEX_GET_INT] = CKEY_GET_INT;
	m_pcchAtomicMacro[INDEX_GET_SIZE] = CKEY_GET_SIZE;
	m_pcchAtomicMacro[INDEX_SKIP] = CKEY_SKIP;
	m_pcchAtomicMacro[INDEX_GET_SRCLINE] = CKEY_GET_SRCLINE;
	m_pcchAtomicMacro[INDEX_GET_STR] = CKEY_GET_STR;
	m_pcchAtomicMacro[INDEX_GET_TEXT] = CKEY_GET_TEXT;
	
	m_pmAtomicMacro[INDEX_GET_ACTION] = new cmacro_term_t(CTT_GET_ACTION, *this);
	m_pmAtomicMacro[INDEX_GET_ACTLINE] = new cmacro_term_t(CTT_GET_ACTLINE, *this);
	m_pmAtomicMacro[INDEX_GOFIRST] = new cmacro_term_t(CTT_GOFIRST, *this);
	m_pmAtomicMacro[INDEX_GET_INDEX] = new cmacro_term_t(CTT_GET_INDEX, *this);
	m_pmAtomicMacro[INDEX_GET_INT] = new cmacro_term_t(CTT_GET_INT, *this);
	m_pmAtomicMacro[INDEX_GET_SIZE] = new cmacro_term_t(CTT_GET_SIZE, *this);
	m_pmAtomicMacro[INDEX_SKIP] = new cmacro_term_t(CTT_SKIP, *this);
	m_pmAtomicMacro[INDEX_GET_STR] = new cmacro_term_t(CTT_GET_PSTR, *this);
	m_pmAtomicMacro[INDEX_GET_SRCLINE] = new cmacro_term_t(CTT_GET_SRCLINE, *this);
	m_pmAtomicMacro[INDEX_GET_TEXT] = new cmacro_term_t(CTT_GET_TEXT, *this);

	cmacro_t * pmac;
	
	pmac = new cmacro_t(*this);
	pmac->addTerm(m_pmAtomicMacro[INDEX_GET_ACTION], nullptr);
	m_str2Macro.insert(str2mac_pair_t(CKEY_GET_ACTION, pmac));
	
	pmac = new cmacro_t(*this);
	pmac->addTerm(m_pmAtomicMacro[INDEX_GET_ACTLINE], nullptr);
	m_str2Macro.insert(str2mac_pair_t(CKEY_GET_ACTLINE, pmac));
	
	pmac = new cmacro_t(*this);
	pmac->addTerm(m_pmAtomicMacro[INDEX_GOFIRST], nullptr);
	m_str2Macro.insert(str2mac_pair_t(CKEY_GOFIRST, pmac));
	
	pmac = new cmacro_t(*this);
	pmac->addTerm(m_pmAtomicMacro[INDEX_GET_INDEX], nullptr);
	m_str2Macro.insert(str2mac_pair_t(CKEY_GET_INDEX, pmac));
	
	pmac = new cmacro_t(*this);
	pmac->addTerm(m_pmAtomicMacro[INDEX_GET_INT], nullptr);
	m_str2Macro.insert(str2mac_pair_t(CKEY_GET_INT, pmac));
	
	pmac = new cmacro_t(*this);
	pmac->addTerm(m_pmAtomicMacro[INDEX_GET_SIZE], nullptr);
	m_str2Macro.insert(str2mac_pair_t(CKEY_GET_SIZE, pmac));
	
	pmac = new cmacro_t(*this);
	pmac->addTerm(m_pmAtomicMacro[INDEX_SKIP], nullptr);
	m_str2Macro.insert(str2mac_pair_t(CKEY_SKIP, pmac));
	
	pmac = new cmacro_t(*this);
	pmac->addTerm(m_pmAtomicMacro[INDEX_GET_STR], nullptr);
	m_str2Macro.insert(str2mac_pair_t(CKEY_GET_STR, pmac));

	pmac = new cmacro_t(*this);
	pmac->addTerm(m_pmAtomicMacro[INDEX_GET_SRCLINE], nullptr);
	m_str2Macro.insert(str2mac_pair_t(CKEY_GET_SRCLINE, pmac));
	
	pmac = new cmacro_t(*this);
	pmac->addTerm(m_pmAtomicMacro[INDEX_GET_TEXT], nullptr);
	m_str2Macro.insert(str2mac_pair_t(CKEY_GET_TEXT, pmac));

}

cmacro_mgr_t::~cmacro_mgr_t(void) {
	
	for(int i = 0; i < ATOMIC_MACRO_NUM; ++i) {
		
		delete m_pmAtomicMacro[i];
		m_pmAtomicMacro[i] = nullptr;
	}
	
	for(int i = 0; i < (int)m_vConds.size(); ++i) {
		
		if(m_vConds[i]) {
			
			delete m_vConds[i];
		}
	}
	
	for(int i = 0; i < (int)m_vcondItems.size(); ++i) {
		
		if(m_vcondItems[i]) {
			
			delete m_vcondItems[i];
		}
	}
	
	for(int i = 0; i < (int)m_vcmTerms.size(); ++i) {
		
		if(m_vcmTerms[i]) {
			
			delete m_vcmTerms[i];
		}
	}
	
	for(str2mac_it_t it = m_str2Macro.begin(); it != m_str2Macro.end(); ++it) {
		
		if(it->second) {
			
			delete it->second;
		}
	}
}

// try to get built-in macro object,
// if @a_strMacroName is name of one of built-in macro, then return corresponding macro object
// otherwise return nullptr
cmacro_term_t* cmacro_mgr_t::tryBuildInMacro(const string &a_strMacroName) {
	
	int high = ATOMIC_MACRO_NUM - 1, low = 0, n;
	while(high >= low) {
		
		int mid = (high + low) / 2;
		if((n = strcmp(a_strMacroName.c_str(), m_pcchAtomicMacro[mid])) == 0) {
			
			return m_pmAtomicMacro[mid];
		}
		else if(n > 0) {
			
			high = mid - 1;
		}
		else {
			
			low = mid + 1;
		}
	}
	
	return nullptr;
}

// test in-place expansion conditions
bool cmacro_mgr_t::testCond(dmmap_t &a_map, const string &a_strKey, int a_nVal) const {
	
	cond_opteq_t coe(a_strKey, a_nVal);
	return coe.satisfied(a_map, "");
}
bool cmacro_mgr_t::testCond(dmmap_t &a_map, const string &a_strKey, bool a_bVal) const {
	
	cond_opteq_t coe(a_strKey, a_bVal);
	return coe.satisfied(a_map, "");
}
bool cmacro_mgr_t::testCond(dmmap_t &a_map, const string &a_strKey, const string &a_strVal) const {
	
	cond_opteq_t coe(a_strKey, a_strVal);
	return coe.satisfied(a_map, "");
}
bool cmacro_mgr_t::testHas(dmmap_t &a_map, const string &a_strKey) const {
	return getHasCond()->satisfied(a_map, a_strKey);
}
bool cmacro_mgr_t::testEmpty(dmmap_t &a_map, const string &a_strKey) const {
	return getEmptyCond()->satisfied(a_map, a_strKey);
}

// add macro term: its type is @CTT_TEXT
cmacro_term_t* cmacro_mgr_t::newTextTerm(const string &a_strText, int a_nLines) {
	
	if(a_strText.size() > MAX_TEXT_COMPARE_SIZE) {
		// text is too long to compare with
		m_vcmTerms.push_back(new cmacro_term_t(a_strText, a_nLines, *this));
	}
	else {
		
		s2i_cit_t cit = m_str2TermIdx.find(a_strText);
		if(cit == m_str2TermIdx.end()) {
		
			m_str2TermIdx.insert(s2i_pair_t(a_strText, (int)m_vcmTerms.size()));
			m_vcmTerms.push_back(new cmacro_term_t(a_strText, a_nLines, *this));
		}
		else {
			
			assert(cit->second >= 0 && cit->second < (int)m_vcmTerms.size());
			return m_vcmTerms[cit->second];
		}
	}
	return m_vcmTerms[m_vcmTerms.size() - 1];
}

// add macro term: its type is @CTT_MACRO, but without parameters
cmacro_term_t* cmacro_mgr_t::newMacroTerm(const string &a_strMacroName) {
	
	cmacro_term_t* pterm = nullptr;
	// first, try to match it with built-in macro
	pterm = tryBuildInMacro(a_strMacroName);
	
	if(!pterm) {
		// if it is user-defined macro
		string str = a_strMacroName;
		str += SKEY_PARAM_SUFFIX;
		str += "0";
		s2i_cit_t cit = m_str2TermIdx.find(str);
		if(cit == m_str2TermIdx.end()) {
		
			m_str2TermIdx.insert(s2i_pair_t(str, (int)m_vcmTerms.size()));
			pterm = new cmacro_term_t(a_strMacroName, *this);
			m_vcmTerms.push_back(pterm);
		}
		else {
			
			assert(cit->second >= 0 && cit->second < (int)m_vcmTerms.size());
			pterm = m_vcmTerms[cit->second];
		}
	}
	
	return pterm;
}

// add macro term: its type is @CTT_MACRO
cmacro_term_t* cmacro_mgr_t::newMacroTerm(const string &a_strMacroName, const vint_t &a_vnParams) {
	
	cmacro_term_t* pterm = nullptr;
	string str = a_strMacroName;
	// first, try to match it with built-in macro
	pterm = tryBuildInMacro(a_strMacroName);
	
	if(pterm) {
		
		if(a_vnParams.size() > 1) {
			
			// TODO: emit error
			assert(false);
		}
		str += SKEY_PARAM_SUFFIX;
		str += strhelper_t::fromInt(a_vnParams[0]);
		s2i_cit_t cit = m_str2TermIdx.find(str);
		if(cit == m_str2TermIdx.end()) {
			
			cmacro_t *pmac = newMacro(str);
			pmac->addTerm(pterm, nullptr);
			
			m_str2TermIdx.insert(s2i_pair_t(str, (int)m_vcmTerms.size()));
			pterm = new cmacro_term_t(str, *this);
			pterm->getParamIndexes().push_back(a_vnParams[0]);
			m_vcmTerms.push_back(pterm);
		}
		else {
			
			assert(cit->second >= 0 && cit->second < (int)m_vcmTerms.size());
			pterm = m_vcmTerms[cit->second];
		}
	}
	else {
		
		for(int i = 0; i < (int)a_vnParams.size(); ++i) {
			
			str += SKEY_PARAM_SUFFIX;
			str += strhelper_t::fromInt(a_vnParams[i]);
		}
		s2i_cit_t cit = m_str2TermIdx.find(str);
		if(cit == m_str2TermIdx.end()) {
		
			m_str2TermIdx.insert(s2i_pair_t(str, (int)m_vcmTerms.size()));
			pterm = new cmacro_term_t(a_strMacroName, *this);
			for(int j = 0; j < (int)a_vnParams.size(); ++j) {
			
				pterm->getParamIndexes().push_back(a_vnParams[j]);
			}
			m_vcmTerms.push_back(pterm);
		}
		else {
			
			assert(cit->second >= 0 && cit->second < (int)m_vcmTerms.size());
			pterm = m_vcmTerms[cit->second];
		}
	}
	
	return pterm;
}

// add empty macro object, whose name is @a_strName
cmacro_t* cmacro_mgr_t::newMacro(const string &a_strName) {
	
	str2mac_it_t it = m_str2Macro.find(a_strName);
	if(it != m_str2Macro.end()) {
		
		// TODO: warning duplicated macro
		assert(it->second);
		return (cmacro_t*)it->second;
	}
	cmacro_t* pmac = new cmacro_t(*this);
	m_str2Macro.insert(str2mac_pair_t(a_strName, pmac));
	
	return pmac;
}

// get macro object by its name
cmacro_base_t* cmacro_mgr_t::getMacro(const string &a_strName) {
	
	str2mac_it_t it = m_str2Macro.find(a_strName);
	if(it != m_str2Macro.end()) {
		
		assert(it->second);
		return it->second;
	}
	return nullptr;
}

// expand macros using stack
bool cmacro_mgr_t::expandWithStack(const string &a_strMacro, dmmap_t &a_map, const vstr_t &a_vstrKeys, ostream& os) {
	
	cmacro_base_t* pmac = getMacro(a_strMacro);
	bool bret = true;
	if(pmac && pmac->expandWithStack(a_map, &a_vstrKeys, os)) {
		
		cmacro_term_t *pt;
		const vstr_t *pvs;
		
		while(!m_stMacTerms.empty()) {
			
			pt = m_stMacTerms.top();
			m_stMacTerms.pop();
			
			pvs = m_stMacParams.top();
			m_stMacParams.pop();
			
			if( ! pt->expandWithStack(a_map, pvs, os)) {
				
				bret = false;
				break;
			}
		}
		
		for(int i = 0; i < (int)m_tempParams.size(); ++i) {
			
			delete m_tempParams[i];
		}
		m_tempParams.clear();
	}
	else {
		return false;
	}
	
	return bret;
}

}
