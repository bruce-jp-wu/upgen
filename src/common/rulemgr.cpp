/*
    Upgen -- a scanner and parser generator.
    CCopyright (C) 2009-2018 Bruce Wu
    
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

#include <iomanip>
#include <ostream>
using std::ostream;
using std::endl;

#include "./rulemgr.h"

namespace common_ns {

// rule_mgr_t: rule/action manager class

void rule_mgr_t::clear(void) {
	
	m_rule2Action.clear();
	m_rule2LineNo.clear();
	
	for(int i = 0; i < (int)m_vaActions.size(); ++i) {
		
		if(m_vaActions[i]) {
			
			delete m_vaActions[i];
		}
	}
	m_vaActions.clear();
}

// update rule/action information, by remove useless rule
// @a_vnMap: array of index of useful rule, otherwise are useless
void rule_mgr_t::updateRActions(const vint_t &a_vnMap) {
	
	if(a_vnMap.size() < m_rule2Action.size()) {
	
		int i, j, k;
		int s = (int)m_rule2Action.size();
		int s2 = (int)m_vaActions.size();
	
		// array of counter of referrenced rule
		vint_t vact(m_vaActions.size(), 0);		
		
		for(i = 0; i < (int)a_vnMap.size(); ++i) {
		
			m_rule2Action[i] = m_rule2Action[a_vnMap[i]];
			m_rule2LineNo[i] = m_rule2LineNo[a_vnMap[i]];
			
			++vact[m_rule2Action[i]];
		}
		
		for(; i < s; ++i) {
			
			m_rule2Action.pop_back();
			m_rule2LineNo.pop_back();
		}
		
		j = 0;
		for(i = 0; i < (int)vact.size(); ++i) {
			
			if((0 == vact[i])) {
				// not referrenced, useless
				if(m_vaActions[i]) {
					delete m_vaActions[i];
					m_vaActions[i] = nullptr;
				}
			}
			else {
				
				++j;
			}
		}
		
		if(j < i) {
			// rearrange arrays
			k = 0;
			for(i = 0; i < (int)vact.size(); ++i) {
				
				if( ! m_vaActions[i]) {
					
					vact[i] = j;
					++j;
				}
				else {
					
					vact[i] = k;
					m_vaActions[k] = m_vaActions[i];
					
					++k;
				}
			}
			
			for(; k < s2; ++k) {
				
				m_vaActions.pop_back();
			}
			
			for(i = 0; i < (int)m_rule2Action.size(); ++i) {
				
				m_rule2Action[i] = vact[m_rule2Action[i]];
			}
		}
	}
}

ostream& operator<<(ostream &os, const rule_mgr_t& src) {

	int i, j, nLine;
	
	if(src.m_rule2Action.size() > 0) {
		os << "int rule2action[" << src.m_rule2Action.size() << "] = {" << endl;
		nLine = (((int)src.m_rule2Action.size()) - 1) >> 3;
		for(i = 0; i < nLine; ++i) {
			
			for(j = 0; j < 8; ++j) {
				
				os << std::setw(8)
					<< src.m_rule2Action[ (i << 3) + j ] << ",";
			}
			os << endl;
		}
		
		for(i = (nLine << 3); i < ((int)src.m_rule2Action.size()) - 1; ++i) {
			
			os << std::setw(8)
				<< src.m_rule2Action[i] << ",";
		}
		os << std::setw(8) << src.m_rule2Action[i] << endl << "};" << endl;
		os << endl;
	}
	
	if(src.m_rule2LineNo.size() > 0) {
		os << "int lineno_refs[" << src.m_rule2LineNo.size() << "] = {" << endl;
		nLine = (((int)src.m_rule2LineNo.size()) - 1) >> 3;
		for(i = 0; i < nLine; ++i) {
			
			for(j = 0; j < 8; ++j) {
				
				os << std::setw(8)
					<< src.m_rule2LineNo[ (i << 3) + j ] << ",";
			}
			os << endl;
		}
		
		for(i = (nLine << 3); i < ((int)src.m_rule2LineNo.size()) - 1; ++i) {
			
			os << std::setw(8)
				<< src.m_rule2LineNo[i] << ",";
		}
		os << std::setw(8) << src.m_rule2LineNo[i] << endl << "};" << endl;
		os << endl;
	}

	// user action
	for(i = 0; i < (int)src.m_vaActions.size(); ++i) {
		
		os << "case " << i << ":\t";
		if(src.m_vaActions[i]) {
			string str;
			int nType;
			for(int j = 0; j < src.m_vaActions[i]->size(); ++j) {
				
				bool bret = src.m_vaActions[i]->get(j, str, nType);
				assert(bret);
				os << str;
			}
		}
	}

	return os;
}

}
