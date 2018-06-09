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

#ifndef CODER_CMACRO_H__
#define CODER_CMACRO_H__

#include <iosfwd>
using std::ostream;
#include <string>
using std::string;
#include <vector>
using std::vector;
#include <map>
using std::map;
using std::pair;
#include <iterator>
using std::iterator;

#include "./coderfwd.h"
#include "./cmcond.h"
#include "./dmmap.h"

// macro in code generator (make sure not confuse it with macro in regular expression)
// includes macro condition and macro body, which may consist of several macro terms.
// in fact, macros and macro terms are actions on list, such as get an integer-valued element,
// skip an element etc.
// when a macro is expanded with some parameters, first check macro conditions one by one, and do
// its corresponding macro condition whenever a macro condition is satisfied.

namespace coder_ns {

// type of macro term,
// a macro term can be: 1) atomic action; or 2) a macro with its parameters
enum cmacro_term_type {
	
	// atomic action: get and output current integer-valued element, and move forward
	CTT_GET_INT,
	// atomic action: get and output current string-valued element, and move forward
	CTT_GET_PSTR,
	// atomic action: get and output size of list
	CTT_GET_SIZE,
	// atomic action: get current subscript of element, and move forward
	CTT_GET_INDEX,
	// atomic action: skip current element
	CTT_SKIP,
	// atomic action: goto first element
	CTT_GOFIRST,
	// atomic action: output parameters literally
	CTT_GET_TEXT,
	// atomic action: output text literally
	CTT_TEXT,
	// atomic action: get and output current script-action-valued element, and move forward
	CTT_GET_ACTION,
	// atomic action: get and output current script-action-valued element with its line number in script, and move forward
	CTT_GET_ACTLINE,
	// atomic action: get and output current source-line
	CTT_GET_SRCLINE,
	
	// macro with its parameters
	CTT_MACRO
};

class cmacro_term_t;
typedef vector< cmacro_term_t* > vterm_t;

/*
 cmacro_base_t: ancestor of all macro classes
 */
class cmacro_base_t {
public:
	// expand macro recursively
	// @a_map: data map
	// @a_vstrParams: macro parameters
	// @os: output stream
	virtual bool expand(dmmap_t &a_map, const vstr_t *a_vstrParams, ostream &os) = 0;
	// expand macro by stack (to avoid recursive)
	virtual bool expandWithStack(dmmap_t &a_map, const vstr_t *a_pvstrParams, ostream &os) = 0;
	virtual ~cmacro_base_t(void){}
};

/*
 macro term class
 */
class cmacro_term_t{

private:
	
	// type of macro term
	cmacro_term_type m_nType;
	// text or macro name
	// if @m_nType is @CTT_MACRO, then @m_strText is macro name
	// if @m_nType is @CTT_TEXT, then @m_strText is literal string
	string m_strText;
	// referrence to global macro manager object
	cmacro_mgr_t& m_cmMgr;
	// array of parameter subscripts
	vint_t m_vnParams;

public:
	
	inline cmacro_term_t(cmacro_term_type a_atomicType, cmacro_mgr_t &a_mgr)
	: m_nType(a_atomicType)
	, m_cmMgr(a_mgr) {
		
		assert(CTT_TEXT != m_nType && CTT_MACRO != m_nType);
	}
	
	inline cmacro_term_t(const string &a_strText, int a_nLines, cmacro_mgr_t &a_mgr)
	: m_nType(CTT_TEXT)
	, m_strText(a_strText)
	, m_cmMgr(a_mgr) {
		m_vnParams.push_back(a_nLines);
	}
	
	inline cmacro_term_t(const string &a_strText, cmacro_mgr_t &a_mgr)
	: m_nType(CTT_MACRO)
	, m_strText(a_strText)
	, m_cmMgr(a_mgr) {
		
	}
	
	virtual ~cmacro_term_t(void) {}

public:
	
	inline string getText(void) const {
		
		return m_strText;
	}
	
	inline vint_t& getParamIndexes(void) {
		
		return m_vnParams;
	}
	
	inline const vint_t& getParamIndexes(void) const {
		
		return m_vnParams;
	}
	/*
	 expand macro term recursively and output code
	 @a_map: data map, get value via its key
	 @a_pvstrParams: macro in-coming parameters
	 @os: output stream
	 */
	virtual bool expand(dmmap_t &a_map, const vstr_t *a_pvstrParams, ostream &os);
	/*
	 expand macro term (using stack to avoid recursion) and output code
	 @a_map: data map, get value via its key
	 @a_pvstrParams: macro in-coming parameters
	 @os: output stream
	 */
	virtual bool expandWithStack(dmmap_t &a_map, const vstr_t *a_pvstrParams, ostream &os);
};

/*
 * macro class
 */
class cmacro_t: public cmacro_base_t {
	
	typedef struct _node_t {
		
		cmacro_term_t *pterm;
		_node_t *plink;
		
		_node_t(cmacro_term_t *pt, _node_t *pl = nullptr)
		: pterm(pt)
		, plink(pl)
		{}
	} node_t, *pnode_t;

private:
	
	cond_t m_conds;
	vector<pnode_t> m_vpnBody;
	cmacro_mgr_t& m_cmMgr;

public:
	
	inline cmacro_t(cmacro_mgr_t &a_mgr)
	: m_cmMgr(a_mgr) {
	}
	
	virtual ~cmacro_t(void);
	
public:
	/*
	 expand macro recursively and output code
	 @a_map: data map, get value via its key
	 @a_pvstrParams: macro in-coming parameters
	 @os: output stream
	 */
	virtual bool expand(dmmap_t &a_map, const vstr_t *a_vstrParams, ostream &os);
	
	/*
	 expand macro (using stack to avoid recursion) and output code
	 @a_map: data map, get value via its key
	 @a_pvstrParams: macro in-coming parameters
	 @os: output stream
	 */
	virtual bool expandWithStack(dmmap_t &a_map, const vstr_t *a_pvstrParams, ostream &os);
	
	/*
	 * add macro terms to macro
	 * @a_pvTerms: macro terms
	 * @a_pCond: macro condition
	 */ 
	void addTerms(vterm_t *a_pvTerms, cond_item_t *a_pCond = nullptr);
	
public:
	
	/*
	 * add macro a term to macro
	 * @a_pvTerms: macro term
	 * @a_pCond: macro condition
	 */ 
	inline void addTerm(cmacro_term_t *a_pTerm, cond_item_t *a_pCond = nullptr) {
		
		assert(a_pTerm);
		
		m_conds.addCondItem(a_pCond);
		
		node_t *pn = new node_t(a_pTerm, nullptr);
		m_vpnBody.push_back(pn);
	}
};

typedef pair<vterm_t *, cond_item_t *> mac2cond_pair_t;
typedef vector<mac2cond_pair_t* > vmac2cond_t;

typedef map < string, cmacro_term_t* > str2cmt_map_t;
typedef str2cmt_map_t::iterator str2cmt_it_t;
typedef str2cmt_map_t::const_iterator str2cmt_cit_t;
typedef pair < string, cmacro_term_t* > str2cmt_pair_t;

typedef map< string, cmacro_base_t* > str2mac_map_t;
typedef str2mac_map_t::iterator str2mac_it_t;
typedef str2mac_map_t::const_iterator str2mac_cit_t;
typedef pair< string, cmacro_base_t* > str2mac_pair_t;

}
#endif // CODER_CMACRO_H__
