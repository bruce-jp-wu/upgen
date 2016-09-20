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

#ifndef CODER_DMMAP_H__
#define CODER_DMMAP_H__

#include <map>
using std::map;
using std::pair;
#include <string>
using std::string;

#include "../main/upgmain.h"
#include "../lexer/dtable.h"
using lexer_ns::dtable_t;
#include "../parser/ptable.h"
using parser_ns::ptable_t;

#include "./dmodel.h"
#include "../common/action.h"
using common_ns::action_t;
using common_ns::vaction_t;

namespace coder_ns {
// data (model) map
class dmmap_t {
	
private:
	
	// collection of pairs of (key, value)
	str2dm_map_t m_sdEntries;
	// keep track of line number of definition file in the process of parsing
	int m_nSrcLineNo;
	// keep track of line number of declaration file in the process of parsing
	int m_nHeaderLineNo;
	// pointer to either @m_nSrcLineNo or @m_nHeaderLineNo
	int *m_pnLineNo;

private:
	
	// group array by value of its elemnets, and
	// insert group delimiters between adjacent groups
	void map2Groups(const vint_t& a_map, vint_t& a_grp) const;
public:
	
	inline dmmap_t(void)
	: m_nSrcLineNo(1)
	, m_nHeaderLineNo(1)
	, m_pnLineNo(&m_nSrcLineNo){
		
	}
	~dmmap_t(void);
	void buildMap(const dtable_t &a_dTbl, const ptable_t &a_pTbl, const gsetting_t &a_gsetup);
	
	inline bool insert(const string& a_strKey, int a_nVal) {
		
		pair < str2dm_it_t, bool > ret = m_sdEntries.insert(str2dm_pair_t(a_strKey, nullptr));
		if(ret.second) {
			
			ret.first->second = new dmodel_t(a_nVal);
		}
		
		return ret.second;
	}

	inline bool insert(const string& a_strKey, const string &a_strVal) {
		
		pair < str2dm_it_t, bool > ret = m_sdEntries.insert(str2dm_pair_t(a_strKey, nullptr));
		if(ret.second) {
			
			ret.first->second = new dmodel_t(a_strVal);
		}
		
		return ret.second;
	}
	
	inline bool insert(const string& a_strKey, const vint_t &a_vnLst) {
		
		pair < str2dm_it_t, bool > ret = m_sdEntries.insert(str2dm_pair_t(a_strKey, nullptr));
		if(ret.second) {
			
			ret.first->second = new dmodel_t(a_vnLst);
		}
		
		return ret.second;
	}
	inline bool insert(const string& a_strKey, const int *a_pnLst, int a_nSize) {
		
		pair < str2dm_it_t, bool > ret = m_sdEntries.insert(str2dm_pair_t(a_strKey, nullptr));
		if(ret.second) {
			
			ret.first->second = new dmodel_t(a_pnLst, a_nSize);
		}
		
		return ret.second;
	}
	
	inline bool insert(const string& a_strKey, const bit_vect_t& a_bitVect) {
		
		pair < str2dm_it_t, bool > ret = m_sdEntries.insert(str2dm_pair_t(a_strKey, nullptr));
		if(ret.second) {
			
			ret.first->second = new dmodel_t(a_bitVect);
		}
		
		return ret.second;
	}

	inline bool insert(const string& a_strKey, const vstr_t &a_vstrLst) {
		
		pair < str2dm_it_t, bool > ret = m_sdEntries.insert(str2dm_pair_t(a_strKey, nullptr));
		if(ret.second) {
			
			ret.first->second = new dmodel_t(a_vstrLst);
		}
		
		return ret.second;
	}
	inline bool insert(const string& a_strKey, const vaction_t &a_vact) {
		
		pair < str2dm_it_t, bool > ret = m_sdEntries.insert(str2dm_pair_t(a_strKey, nullptr));
		if(ret.second) {
			
			ret.first->second = new dmodel_t(a_vact);
		}
		
		return ret.second;
	}
	inline bool insert(const string& a_strKey, const action_t *a_pact) {
		
		pair < str2dm_it_t, bool > ret = m_sdEntries.insert(str2dm_pair_t(a_strKey, nullptr));
		if(ret.second) {
			
			ret.first->second = new dmodel_t(a_pact);
		}
		
		return ret.second;
	}
	
public:
	
	inline bool isValid(const string &a_strKey) const {
		
		return getDModel(a_strKey) != nullptr;
	}
	
	inline bool isEmpty(const string &a_strKey) const {
		
		const dmodel_t *pdm = getDModel(a_strKey);
		return (! pdm);
	}
	
	inline bool isEnd(const string &a_strKey) const {
		const dmodel_t *pdm = getDModel(a_strKey);
		return  ( ! pdm) || pdm->isEnd();
	}
	
	inline bool isLast(const string &a_strKey) const {
		
		const dmodel_t *pdm = getDModel(a_strKey);
		return pdm && pdm->isLast();
	}
	
	inline bool isMore(const string &a_strKey) const {
		
		const dmodel_t *pdm = getDModel(a_strKey);
		return pdm && pdm->isMore();
	}
	
	inline bool isMod(const string& a_strKey, int a_nElemsPerLine) const {
		
		const dmodel_t *pdm = getDModel(a_strKey);
		return pdm && pdm->isMod(a_nElemsPerLine);
	}
	
	inline bool isGroupFlag(const string& a_strKey) const {
		
		const dmodel_t *pdm = getDModel(a_strKey);
		return pdm && pdm->isGroupFlag();
	}
	
	inline bool isGroupFirst(const string& a_strKey) const {
		
		const dmodel_t *pdm = getDModel(a_strKey);
		return pdm && pdm->isGroupFirst();
	}	
	
	inline bool isInt(const string &a_strKey) const {
		
		const dmodel_t *pdm = getDModel(a_strKey);
		return pdm && pdm->isInt();
	}
	
	inline bool isString(const string &a_strKey) const {
		
		const dmodel_t *pdm = getDModel(a_strKey);
		return pdm && pdm->isString();
	}
	inline bool isAction(const string &a_strKey) const {
		
		const dmodel_t *pdm = getDModel(a_strKey);
		return pdm && pdm->isAction();
	}
public:
	
	inline int nextInt(const string& a_strKey) {
		
		dmodel_t* pdm = getDModel(a_strKey);
		assert(pdm);
		
		return pdm->nextInt();
	}
	inline const string& nextString(const string& a_strKey) {
		
		dmodel_t* pdm = getDModel(a_strKey);
		assert(pdm);
		return pdm->nextString();
	}
	inline const action_t* nextAction(const string &a_strKey) {
		
		dmodel_t* pdm = getDModel(a_strKey);
		assert(pdm);
		return pdm->nextAction();
	}
	
	inline void skip(const string& a_strKey) {
		
		dmodel_t* pdm = getDModel(a_strKey);
		if(pdm) {
			
			pdm->skip();
		}
	}
	
	inline void goFirst(const string& a_strKey) {
		
		dmodel_t* pdm = getDModel(a_strKey);
		if(pdm) {
			
			pdm->goFirst();
		}
	}
public:
	
	inline bool getSize(const string &a_strKey, int &a_rn) const {
		
		const dmodel_t* pdm = getDModel(a_strKey);
		if(pdm) {
			
			a_rn = pdm->getSize();
			return true;
		}
		return false;
	}
	
	inline bool getIndex(const string &a_strKey, int &a_rn) const {
		
		const dmodel_t* pdm = getDModel(a_strKey);
		if(pdm) {
			
			a_rn = pdm->getIndex();
			return true;
		}
		return false;
	}
	
	inline bool getInt(const string& a_strKey, int &a_rn) const {
		
		const dmodel_t* pdm = getDModel(a_strKey);
		if(pdm) {
			
			a_rn = pdm->getInt();
			return true;
		}
		return false;
	}
	inline bool getString(const string& a_strKey, string &a_rstr) const {
		
		const dmodel_t* pdm = getDModel(a_strKey);
		if(pdm) {
			
			a_rstr = pdm->getString();
			return true;
		}
		return false;
	}
	inline bool getAction(const string& a_strKey, const action_t* &a_rAct) const {
		
		const dmodel_t* pdm = getDModel(a_strKey);
		if(pdm) {
			
			a_rAct = pdm->getAction();
			return true;
		}
		return false;
	}
	
	inline int getSrcLineNo(void) const {
		return *m_pnLineNo;
	}
	inline void incSrcLineNo(void) {
		++(*m_pnLineNo);
	}
	inline void addSrcLineNo(int a_nLines) {
		*m_pnLineNo += a_nLines;
	}
	inline void switchtoDef(void) {
		m_pnLineNo = &m_nSrcLineNo;
	}
	inline void switchtoDecl(void) {
		m_pnLineNo = &m_nHeaderLineNo;
	}
	
public:
	
	inline const dmodel_t* getDModel(const string& a_strKey) const {
		
		str2dm_cit_t cit = m_sdEntries.find(a_strKey);
		if(cit == m_sdEntries.end()) {
			
			return nullptr;
		}
		
		return cit->second;
	}
	
	inline dmodel_t* getDModel(const string& a_strKey) {
		
		str2dm_it_t it = m_sdEntries.find(a_strKey);
		if(it == m_sdEntries.end()) {
			
			return nullptr;
		}
		
		return it->second;
	}
};

}

#endif //CODER_DMMAP_H__

