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

#ifndef COMMON_RULEMGR_H__
#define COMMON_RULEMGR_H__

#include <iosfwd>
using std::ostream;

#include "./common.h"
#include "./types.h"
#include "./action.h"
using common_ns::action_t;
using common_ns::vaction_t;

namespace common_ns {

// rule_mgr_t: rule/action manager class

class rule_mgr_t {
	
private:

	// array of action object
	vaction_t m_vaActions;
	// map array: rule-index -> action-index
	vint_t m_rule2Action;
	// map array: rule-index -> line-number-of-rule
	vint_t m_rule2LineNo;
	
public:
	
	void clear(void);
	
	friend ostream& operator<<(ostream &os, const rule_mgr_t& src);
	
public:
	// map: rule-ID -> action-ID
	void mapRule2Action(int a_nRule, int a_nActionIndex) {
		// TODO: remove zero elements when FIRST_RULE = 1
		while((int)m_rule2Action.size() <= a_nRule) {
			
			m_rule2Action.push_back(INVALID_ACTION);
		}
		
		m_rule2Action[a_nRule] = a_nActionIndex;
	}
	
	// map: rule-ID -> line-number-of-rule
	void mapRule2Line(int a_nRule, int a_nLineNo) {
		// TODO: remove zero elements when FIRST_RULE = 1
		while((int)m_rule2LineNo.size() <= a_nRule) {
			
			m_rule2LineNo.push_back(INVALID_LINENO);
		}
		
		m_rule2LineNo[a_nRule] = a_nLineNo;
	}
	
	// prepare to add new action
	inline int addEmptyAction(int a_nLine = 0) {
		
		action_t *pact = new action_t(a_nLine);
		m_vaActions.push_back(pact);
		return ((int)m_vaActions.size()) - 1;
	}
	
	// update rule/action information, by remove useless rule
	// @a_vnMap: array of index of useful rule, otherwise are useless
	void updateRActions(const vint_t &a_vnMap);
	
public:
	// accessor functions	
	inline action_t* getCurAction(void) {
		assert(m_vaActions.size() > 0);
		return m_vaActions[m_vaActions.size() - 1];
	}

	inline vaction_t& getActions(void) {
		return m_vaActions;
	}

	inline const vaction_t& getActions(void) const {
		return m_vaActions;
	}

	inline vint_t& getRule2Actions(void) {
		return m_rule2Action;
	}

	inline const vint_t& getRule2Actions(void) const {
		return m_rule2Action;
	}

	inline vint_t& getRule2LineNos(void) {
		return m_rule2LineNo;
	}

	inline const vint_t& getRule2LineNos(void) const {
		return m_rule2LineNo;
	}
};

}
#endif //COMMON_RULEMGR_H__
