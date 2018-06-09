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

#include "../common/action.h"
using common_ns::action_t;

#include "./cmacro.h"
#include "./cmmgr.h"

namespace coder_ns {

/*
 expand macro term recursively and output code
 @a_map: data map, get value via its key
 @a_pvstrParams: macro in-coming parameters
 @os: output stream
 */
bool cmacro_term_t::expand(dmmap_t &a_map, const vstr_t *a_pvstrParams, ostream &os) {
	
	int i = 0;
	
	static vstr_t vstr;
	
	switch(m_nType) {
	
	case CTT_GET_INT:
	
		assert(a_pvstrParams->size() == 1);
		assert(a_map.isInt((*a_pvstrParams)[0]));

		os << a_map.nextInt((*a_pvstrParams)[0]);
		
		break;
		
	case CTT_GET_PSTR:
		
		assert(a_pvstrParams->size() == 1);
		assert(a_map.isString((*a_pvstrParams)[0]));

		os << a_map.nextString((*a_pvstrParams)[0]);

		break;

	case CTT_GET_SIZE:
		
		assert(a_pvstrParams->size() == 1);
		assert(a_map.isValid((*a_pvstrParams)[0]));
		
		a_map.getSize((*a_pvstrParams)[0], i);
		os << i;
		
		break;

	case CTT_GET_INDEX:
		
		assert(a_pvstrParams->size() == 1);
		assert(a_map.isValid((*a_pvstrParams)[0]));

		a_map.getIndex((*a_pvstrParams)[0], i);
		os << i;
		
		break;
		
	case CTT_SKIP:
		
		for(i = 0; i < (int)a_pvstrParams->size(); ++i) {
		
			a_map.skip((*a_pvstrParams)[i]);
		}
		
		break;
		
	case CTT_GOFIRST:

		for(i = 0; i < (int)a_pvstrParams->size(); ++i) {
		
			a_map.goFirst((*a_pvstrParams)[i]);
		}
			
		break;
		
	case CTT_GET_TEXT:
		
		assert(a_pvstrParams->size() == 1);
		os << (*a_pvstrParams)[0];
		
		{
			for(i = 0; i < (int)(*a_pvstrParams)[0].size(); ++i) {
				if(((*a_pvstrParams)[0])[i] == '\n') {
					a_map.incSrcLineNo();
				}
			}
			
		}
		
		break;
		
	case CTT_TEXT:
		
		os << m_strText;
		a_map.addSrcLineNo(m_vnParams[0]);
		
		break;
		
	case CTT_GET_ACTLINE: {
		
		const action_t *pact = nullptr;
		
		if(a_map.getAction((*a_pvstrParams)[0], pact)) {

			assert(pact);
			
			os << pact->getLineno();
		}
	}
		break;
		
	case CTT_GET_SRCLINE: {
		
		os << a_map.getSrcLineNo();
	}
		break;

	case CTT_GET_ACTION:
		
		if(a_map.isAction((*a_pvstrParams)[0])) {
			
			const action_t *pact = a_map.nextAction((*a_pvstrParams)[0]);
			assert(pact);
			a_map.addSrcLineNo(pact->getLineCount());
			
			string strText;
			int nType;
			
			i = 0;
			while(i < pact->size()) {
				
				if( ! pact->get(i, strText, nType)) {
					
					return false;
				}
				
				assert(action_t::BLT_PARAM != nType);
					
				if( action_t::BLT_TEXT == nType) {
					
					os << strText;
				}
				else {	// action_t::BLT_MACRO == nType
					
					cmacro_base_t *pmac = m_cmMgr.getMacro(strText);
					assert(pmac);
						
					vstr.clear();
						
					while(pact->get(i + 1, strText, nType)
							&& nType == action_t::BLT_PARAM) {
						
						vstr.push_back(strText);
						++i;
					}
					
					if(vstr.size() > 0) {
						if( ! pmac->expand(a_map, &vstr, os)) {
							return false;
						}
					}
					else {
						if( ! pmac->expand(a_map, &m_cmMgr.getEmptyParam(), os)) {
							return false;
						}
					}
				}
				
				++i;
			}
		}
		
		break;
		
	case CTT_MACRO:
		{
			cmacro_base_t *pmac = m_cmMgr.getMacro(m_strText);
			assert(pmac);
			
			if(m_vnParams.size() > 0 && m_vnParams.size() < a_pvstrParams->size()) {
				
				vstr.clear();
				
				for(i = 0; i < (int)m_vnParams.size(); ++i) {
					
					assert(m_vnParams[i] >= 0 && m_vnParams[i] < (int)a_pvstrParams->size());
					vstr.push_back((*a_pvstrParams)[m_vnParams[i]]);
				}

				if( ! pmac->expand(a_map, &vstr, os)) {
					return false;
				}
			}
			else {
				
				if( ! pmac->expand(a_map, a_pvstrParams, os)) {
					return false;
				}
			}
		}
		
		break;
	}
	
	return true;
}

/*
 expand macro term (using stack to avoid recursion) and output code
 @a_map: data map, get value via its key
 @a_pvstrParams: macro in-coming parameters
 @os: output stream
 */
bool cmacro_term_t::expandWithStack(dmmap_t &a_map, const vstr_t *a_pvstrParams, ostream &os) {
	
	int i = 0;
	
	static vstr_t vstr;
	
	switch(m_nType) {
	
	case CTT_GET_INT:
	
		assert(a_pvstrParams->size() == 1);
		assert(a_map.isInt((*a_pvstrParams)[0]));
			
		os << a_map.nextInt((*a_pvstrParams)[0]);
		
		break;
		
	case CTT_GET_PSTR:
		
		assert(a_pvstrParams->size() == 1);
		assert(a_map.isString((*a_pvstrParams)[0]));
		
		os << a_map.nextString((*a_pvstrParams)[0]);
		
		break;

	case CTT_GET_SIZE:
		
		assert(a_pvstrParams->size() == 1);
		assert(a_map.isValid((*a_pvstrParams)[0]));
		
		a_map.getSize((*a_pvstrParams)[0], i);
		os << i;
		
		break;

	case CTT_GET_INDEX:
		
		assert(a_pvstrParams->size() == 1);
		assert(a_map.isValid((*a_pvstrParams)[0]));

		a_map.getIndex((*a_pvstrParams)[0], i);
		os << i;
		
		break;
		
	case CTT_SKIP:
		
		for(i = 0; i < (int)a_pvstrParams->size(); ++i) {
		
			a_map.skip((*a_pvstrParams)[i]);
		}
		
		break;
		
	case CTT_GOFIRST:

		for(i = 0; i < (int)a_pvstrParams->size(); ++i) {
		
			a_map.goFirst((*a_pvstrParams)[i]);
		}
			
		break;
		
	case CTT_GET_TEXT:
		
		assert(a_pvstrParams->size() == 1);
		os << (*a_pvstrParams)[0];
		
		{
			for(i = 0; i < (int)(*a_pvstrParams)[0].size(); ++i) {
				if(((*a_pvstrParams)[0])[i] == '\n') {
					a_map.incSrcLineNo();
				}
			}
			
		}
		break;
		
	case CTT_TEXT:
		
		os << m_strText;
		a_map.addSrcLineNo(m_vnParams[0]);
		
		break;
		
	case CTT_GET_ACTLINE: {
		
		const action_t *pact = nullptr;
		
		if(a_map.getAction((*a_pvstrParams)[0], pact)) {

			assert(pact);
			
			os << pact->getLineno();
		}
	}
		return true;
	case CTT_GET_SRCLINE: {
		os << a_map.getSrcLineNo();
	}
		break;
		
	case CTT_GET_ACTION:
		
		if(a_map.isAction((*a_pvstrParams)[0])) {
			
			const action_t *pact = a_map.nextAction((*a_pvstrParams)[0]);
			assert(pact);
			a_map.addSrcLineNo(pact->getLineCount());
			
			string strText;
			int nType;
			
			i = 0;
			while(i < pact->size()) {
				
				if( ! pact->get(i, strText, nType)) {
					return false;
				}
				
				assert(action_t::BLT_PARAM != nType);
					
				if( action_t::BLT_TEXT == nType) {
					
					os << strText;
				}
				else {	// action_t::BLT_MACRO == nType
					
					cmacro_base_t *pmac = m_cmMgr.getMacro(strText);
					assert(pmac);
						
					vstr.clear();
						
					while(pact->get(i + 1, strText, nType)
							&& nType == action_t::BLT_PARAM) {
						
						vstr.push_back(strText);
						++i;
					}
					
					if(vstr.size() > 0) {
						if( ! pmac->expand(a_map, &vstr, os)) {
							return false;
						}
					}
					else {
						if( ! pmac->expand(a_map, &m_cmMgr.getEmptyParam(), os)) {
							return false;
						}
					}
				}
				
				++i;
			}
		}
		
		break;
		
	case CTT_MACRO:
		{
			cmacro_base_t *pmac = m_cmMgr.getMacro(m_strText);
			assert(pmac);
			
			if(m_vnParams.size() > 0 && m_vnParams.size() < a_pvstrParams->size()) {
				
				vstr_t *pvs = m_cmMgr.newTempParams();
				
				for(i = 0; i < (int)m_vnParams.size(); ++i) {
					
					assert(m_vnParams[i] >= 0 && m_vnParams[i] < (int)a_pvstrParams->size());
					pvs->push_back((*a_pvstrParams)[m_vnParams[i]]);
				}

				if( ! pmac->expandWithStack(a_map, pvs, os)) {
					return false;
				}
			}
			else {
				
				if( ! pmac->expandWithStack(a_map, a_pvstrParams, os)) {
					return false;
				}
			}
		}

		break;
	}
	
	return true;

}

cmacro_t::~cmacro_t(void) {

	int i;
	// delete only nodes, deleting macro terms is cmacro_mgr_t's job
	for(i = 0; i < (int)m_vpnBody.size(); ++i) {
		
		node_t *p = m_vpnBody[i];
		node_t *q = nullptr;
		while(p) {
			
			q = p->plink;
			delete p;
			p = q;
		}
	}
}

/*
 expand macro recursively and output code
 @a_map: data map, get value via its key
 @a_pvstrParams: macro in-coming parameters
 @os: output stream
 */
bool cmacro_t::expand(dmmap_t &a_map, const vstr_t *a_pvstrParams, ostream &os) {
	
	assert(a_pvstrParams->size() > 0);
	int idx = m_conds.test(a_map, (*a_pvstrParams)[0]);
	if(idx < (int)m_vpnBody.size()) {
		
		node_t *p = m_vpnBody[idx];
		vector<node_t*> vnode;
		while(p) {
		
			vnode.push_back(p);
			p = p->plink;
		}
		
		for(int i = (int)vnode.size() - 1; i >= 0; --i) {
		
			if( ! vnode[i]->pterm->expand(a_map, a_pvstrParams, os)) {

				return false;
			}
			
		}
	}

	return true;
}

/*
 expand macro (using stack to avoid recursion) and output code
 @a_map: data map, get value via its key
 @a_pvstrParams: macro in-coming parameters
 @os: output stream
 */
bool cmacro_t::expandWithStack(dmmap_t &a_map, const vstr_t *a_pvstrParams, ostream &os) {
	
	assert(a_pvstrParams->size() > 0);
	
	int idx = m_conds.test(a_map, (*a_pvstrParams)[0]);
	
	if(idx < (int)m_vpnBody.size()) {
		
		node_t *p = m_vpnBody[idx];
		while(p) {
		
			m_cmMgr.addExpandStack(p->pterm, a_pvstrParams);
			p = p->plink;
		}
	}
	
	return true;
}

/*
 * add macro terms to macro
 * @a_pvTerms: macro terms
 * @a_pCond: macro condition
 */ 
void cmacro_t::addTerms(vterm_t *a_pvTerms, cond_item_t *a_pCond) {
	
	assert(a_pvTerms && a_pvTerms->size() >  0);
	
	m_conds.addCondItem(a_pCond);

	// insert reversely
	node_t *p = nullptr;
	for(int i = 0; i < (int)a_pvTerms->size(); ++i) {
		p = new node_t((*a_pvTerms)[i], p);
	}
	
	if(p) {
		m_vpnBody.push_back(p);
	}
}

}
