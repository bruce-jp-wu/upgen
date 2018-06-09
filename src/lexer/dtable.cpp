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

#include <ostream>
using std::ostream;
using std::endl;
#include <iomanip>
using std::setw;
#include "./dtable.h"

namespace lexer_ns {

// update char map
void dtable_t::updateCharmap(const vustr_t &a_vustrMap) {
	
	m_tMeta.clear();
	int i;
	for(i = 0; i < 256; ++i) {
		
		m_tCharMap[i] = (int)a_vustrMap.size();
	}
	
	for(i = 0; i < (int)a_vustrMap.size(); ++i) {
		
		for(int j = 0; j < (int)a_vustrMap[i].size(); ++j) {
			
			m_tCharMap[(unsigned char)(a_vustrMap[i][j])] = (unsigned char)i;
		}
		m_tMeta.push_back(0);
	}
	m_nLabelCount = i;
}

void dtable_t::checkLookahead(const vint_t& a_vnRules, const vint_t& a_vnLaNodes) {
	
	assert(a_vnRules.size() > 0);
	
	int i = 0;
	for(; i < (int)a_vnRules.size(); ++i) {
		if(m_tRuleRef[a_vnRules[i]] > 0) {
			break;
		}
	}
	
	if((int)a_vnRules.size() > i) {
		
		m_tLookahead.resize(m_tRuleRef.size(), 0);
		for(i = 0; i < (int)a_vnRules.size(); ++i) {
			
			if(m_tRuleRef[a_vnRules[i]] > 0) {
				
				m_tLookahead[a_vnRules[i]] = a_vnLaNodes[i];
			}
		}
	}
}

void dtable_t::updateRules(vint_t &a_sc2Rules, ostream &os) {
	
	int i, j, k;
	vint_t vnMap;
	vint_t& vnAct = m_ruleMgr.getRule2Actions();
	vint_t& vnLine = m_ruleMgr.getRule2LineNos();
	
	// check if there are some rules that can never be referred
	for(i = 0; i < (int)m_tRuleRef.size(); ++i) {
		
		if(_EQ(0, m_tRuleRef[i])) {
			os << "Warning: lexical rule at line " << vnLine[i] << " is never referred." << endl;
		}
		else {
			vnMap.push_back(i);
		}
	}
	
	if(vnMap.size() < m_tRuleRef.size()) {
		
		// if there ARE some useless rules, remove them
		// and update DFA tables
		vint_t vnrMap(m_tRuleRef.size());
		j = (int)vnMap.size();
		k = 0;
		
		if(m_tLookahead.size() > 0) {	// DFAs have lookahead states
			
			for(i = 0; i < (int)m_tRuleRef.size(); ++i) {
				
				if(_EQ(0, m_tRuleRef[i])) {
					
					vnrMap[i] = j;
					++j;
				}
				else {
					vnrMap[i] = k;
					m_tLookahead[k] = m_tLookahead[i];
					++k;
				}
			}
			
			for(; k < (int)m_tRuleRef.size(); ++k) {
				
				m_tLookahead.pop_back();
			}
		}
		else {
			
			for(i = 0; i < (int)m_tRuleRef.size(); ++i) {
				
				if(_EQ(0, m_tRuleRef[i])) {
					
					vnrMap[i] = j++;
				}
				else {
					vnrMap[i] = k++;
				}
			}
		}
		
		m_ruleMgr.updateRActions(vnMap);
		
		// update accept table
		for(i = 0; i < (int)m_tAccept.size(); ++i) {
			
			if(m_tAccept[i] != INVALID_RULE) {
				
				m_tAccept[i] = vnrMap[m_tAccept[i]];
			}
		}
		
		for(i = 0; i < (int)a_sc2Rules.size(); ++i) {
			
			if(a_sc2Rules[i] != INVALID_RULE) {
				
				a_sc2Rules[i] = vnrMap[a_sc2Rules[i]];
			}
		}
	}
	
	sint_t si;
	int nRuleIndex = (int)vnLine.size();
	// if a start-condition have no EOF-rule, add a rule and attach to it,
	for(i = 0; i < (int)a_sc2Rules.size(); ++i) {
		
		if(_EQ(a_sc2Rules[i], INVALID_RULE)) {
			
			mapRule2Action(nRuleIndex, addEmptyAction(0));
			mapRule2Line(nRuleIndex, 0);
			si.insert(nRuleIndex);
			a_sc2Rules[i] = nRuleIndex;
			
			++nRuleIndex;
			
			if(m_tLookahead.size() > 0) {
				
				m_tLookahead.push_back(0);
			}
		}
		else if( ! si.insert(a_sc2Rules[i]).second) {
			
			mapRule2Action(nRuleIndex, vnAct[a_sc2Rules[i]]);
			mapRule2Line(nRuleIndex, vnLine[a_sc2Rules[i]]);
			si.insert(nRuleIndex);
			a_sc2Rules[i] = nRuleIndex;
			
			++nRuleIndex;
			
			if(m_tLookahead.size() > 0) {
				
				m_tLookahead.push_back(0);
			}
		}
	}
	
	vint_t 	vn2(a_sc2Rules.size()),
			vn3(a_sc2Rules.size()),
			vn4(a_sc2Rules.size());
	
	vnMap.resize(vnLine.size());
	
	for(i = 0; i < (int)a_sc2Rules.size(); ++i) {
		
		vn2[i] = vnAct[a_sc2Rules[i]];
		vn3[i] = vnLine[a_sc2Rules[i]];
		
		if(m_tLookahead.size() > 0) {
			vn4[i] = m_tLookahead[a_sc2Rules[i]];
		}
	}
	
	j = 0;
	k = 0;
	for(sint_cit_t cit = si.begin(); cit != si.end(); ++cit) {
		
		for(; k < *cit; ++j, ++k) {
			
			vnAct[j] = vnAct[k];
			vnLine[j] = vnLine[k];
			vnMap[k] = j;
			
			if(m_tLookahead.size() > 0) {
				
				m_tLookahead[j] = m_tLookahead[k];
			}
		}
		++k;
	}
	
	for(; k < (int)vnLine.size(); ++j, ++k) {
		
		vnAct[j] = vnAct[k];
		vnLine[j] = vnLine[k];
		vnMap[k] = j;
		
		if(m_tLookahead.size() > 0) {
			
			m_tLookahead[j] = m_tLookahead[k];
		}
	}
	
	m_nSCEOFRuleBase = j;
	
	for(k = 0; j < (int)vnLine.size(); ++j, ++k) {
		
		vnAct[j] = vn2[k];
		vnLine[j] = vn3[k];
		vnMap[a_sc2Rules[k]] = j;
		
		if(m_tLookahead.size() > 0) {
		
			m_tLookahead[j] = vn4[k];
		}
	}
	
	// update accept table
	for(i = 0; i < (int)m_tAccept.size(); ++i) {
		
		if(m_tAccept[i] != INVALID_RULE) {
			
			m_tAccept[i] = vnMap[m_tAccept[i]];
		}
	}
}

void dtable_t::addAdditionChars(void) {
	
	assert(m_tCharMap.size() == 256);
	m_tCharMap.push_back(m_nLabelCount);
	m_tCharMap.push_back(m_nLabelCount);
	m_tMeta.push_back(m_nMetaCount);
}

ostream& operator<<(ostream& os, const dtable_t &src) {

	int i, j, nLine;
	// output char map
	os << "int charMap[" << src.m_tCharMap.size() << "] = {" << endl;
	nLine = (((int)src.m_tCharMap.size()) - 1) / 8;
	for(i = 0; i < nLine; ++i) {
		
		if(i >= 4 && i < 16) {
			
			os << std::setfill('-') << std::setw(8) << (char)(i * 8);
			os << std::setfill(' ');
			for(j = 1; j < 8; ++j) {
				
				os << std::setw(8) 
					<< (char)(i * 8 + j);
			}
		}
		else {
			
			os << endl;
		}
		
		os << endl;
		
		for(j = 0; j < 8; ++j) {
			
			os << std::setw(8) 
				<< (int)src.m_tCharMap[i * 8 + j];
		}
		
		os << endl;
	}
	for(i = nLine * 8; i < ((int)src.m_tCharMap.size()); ++i) {
		
		os << std::setw(8)
			<< (int)src.m_tCharMap[i];
	}
	os <<  endl << "};" << endl;
	
	// output meta-char map
	os << "unsigned char charMeta[" << src.m_tMeta.size() << "] = {" << endl;
	nLine = (((int)src.m_tMeta.size()) - 1) / 8;
	for(i = 0; i < nLine; ++i) {
		
		for(j = 0; j < 8; ++j) {
			
			os << std::setw(8) 
				<< (int)src.m_tMeta[i * 8 + j] << ',';
		}
		
		os << endl;
	}
	for(i = nLine * 8; i < ((int)src.m_tMeta.size()) - 1; ++i) {
		
		os << std::setw(8)
			<< (int)src.m_tMeta[i] << ',';
	}
	os << std::setw(8) << (int)src.m_tMeta[i] <<  endl << "};" << endl;
	os << endl;
	
	if(src.m_vsSCName.size() == 0 || src.m_tBase.size() == 0) {
		
		// output empty arrays here
		return os;
	}
	
	assert(src.m_vsSCName.size() > 0);
	// start-condition name
	for(i = 0; i < (int)src.m_vsSCName.size(); ++i) {
		
		os << "#define " << src.m_vsSCName[i]
			<< '\t' << i * 2 << endl;
	}		
	os << endl << endl;
	
	os << "#define ERROR_STATE " << src.m_nDefaultState << endl;
	
	// start-condition start-states
	os << "int startStates[" << src.m_tStarts.size() << "] = {" << endl;
	nLine = (((int)src.m_tStarts.size()) - 1) / 8;
	for(i = 0; i < nLine; ++i) {
		
		for(j = 0; j < 8; ++j) {
			
			os << std::setw(8)
				<< src.m_tStarts[ i * 8 + j ] << ',';
		}
		
		os << endl;
	}
	
	for(i = nLine * 8; i < ((int)src.m_tStarts.size()) - 1; ++i) {
		
		os << std::setw(8)
			<< src.m_tStarts[i] << ',';
	}
	os << std::setw(8) << src.m_tStarts[i] <<  endl << "};" << endl;
	os << endl;

	// base
	os << "int stateBase[" << src.m_tBase.size() << "] = {" << endl;
	nLine = (((int)src.m_tBase.size()) - 1) / 8;
	for(i = 0; i < nLine; ++i) {
		
		for(j = 0; j < 8; ++j) {
			
			os << std::setw(8)
				<< src.m_tBase[ i * 8 + j ] << ',';
		}
		os << endl;
	}
	
	for(i = (nLine * 8); i < ((int)src.m_tBase.size()) - 1; ++i) {
		
		os << std::setw(8)
			<< src.m_tBase[i] << ',';
	}
	os << std::setw(8) << src.m_tBase[i] <<  endl << "};" << endl;
	os << endl;
	
	assert(src.m_tDefault.size() > 0);
	// default
	os << "int defaultState[" << src.m_tDefault.size() << "] = {" << endl;
	nLine = (((int)src.m_tDefault.size()) - 1) / 8;
	for(i = 0; i < nLine; ++i) {
		
		for(j = 0; j < 8; ++j) {
			
			os << std::setw(8)
				<< src.m_tDefault[ i * 8 + j ] << ',';
		}
		os << endl;
	}
	
	for(i = nLine * 8; i < ((int)src.m_tDefault.size()) - 1; ++i) {
		
		os << std::setw(8)
			<< src.m_tDefault[i] << ',';
	}
	os << std::setw(8) << src.m_tDefault[i] <<  endl << "};" << endl;
	os << endl;
	
	if(src.m_tAccept.size() > 0) {
		// Accepted State
		os << "int acceptState[" << src.m_tAccept.size() << "] = {" << endl;
		nLine = (((int)src.m_tAccept.size()) - 1) / 8;
		for(i = 0; i < nLine; ++i) {
			
			for(j = 0; j < 8; ++j) {
				
				os << std::setw(8)
					<< src.m_tAccept[ i * 8 + j ] << ',';
			}
			os << endl;
		}
		
		for(i = nLine * 8; i < ((int)src.m_tAccept.size()) - 1; ++i) {
			
			os << std::setw(8)
				<< src.m_tAccept[i] << ',';
		}
		os << std::setw(8) << src.m_tAccept[i] <<  endl << "};" << endl;
		os << endl;	
	}
	
	if(src.m_tNext.size() > 0) {
		// next
		os << "int nextState[" << src.m_tNext.size() << "] = {" << endl;
		nLine = (((int)src.m_tNext.size()) - 1) >> 3;
		for(i = 0; i < nLine; ++i) {
			
			for(j = 0; j < 8; ++j) {
				
				os << std::setw(8)
					<< src.m_tNext[ i * 8 + j ] << ',';
			}
			os << endl;
		}
		
		for(i = nLine * 8; i < ((int)src.m_tNext.size()) - 1; ++i) {
			
			os << std::setw(8)
				<< src.m_tNext[i] << ',';
		}
		os << std::setw(8) << src.m_tNext[i] <<  endl << "};" << endl;
		os << endl;
	
		// check
		os << "int checkState[" << src.m_tCheck.size() << "] = {" << endl;
		nLine = (((int)src.m_tCheck.size()) - 1) >> 3;
		for(i = 0; i < nLine; ++i) {
			
			for(j = 0; j < 8; ++j) {
				
				os << std::setw(8)
					<< src.m_tCheck[ i * 8 + j ] << ',';
			}
			os << endl;
		}
		
		for(i = nLine * 8; i < ((int)src.m_tCheck.size()) - 1; ++i) {
			
			os << std::setw(8)
				<< src.m_tCheck[i] << ',';
		}
		os << std::setw(8) << src.m_tCheck[i] <<  endl << "};" << endl;
		os << endl;
	}
	
	if(src.m_tLookahead.size() > 0) {		
		
		os << "int lookahead[" << src.m_tLookahead.size() << "] = {" << endl;
		nLine = (((int)src.m_tLookahead.size()) - 1) >> 3;
		for(i = 0; i < nLine; ++i) {
			
			for(j = 0; j < 8; ++j) {
				
				os << std::setw(8)
					<< src.m_tLookahead[ i * 8 + j ] << ',';
			}
			os << endl;
		}
		
		for(i = nLine * 8; i < ((int)src.m_tLookahead.size()) - 1; ++i) {
			
			os << std::setw(8)
				<< src.m_tLookahead[i] << ',';
		}
		os << std::setw(8) << src.m_tLookahead[i] <<  endl << "};" << endl;
		os << endl;
	}
	
	// user action
	os << src.m_ruleMgr << '\n';
	if(src.m_tRuleRef.size() > 0) {
		os << "int rule_refs[" << src.m_tRuleRef.size() << "] = {" << endl;
		nLine = (((int)src.m_tRuleRef.size()) - 1) >> 3;
		for(i = 0; i < nLine; ++i) {
			
			for(j = 0; j < 8; ++j) {
				
				os << std::setw(8)
					<< src.m_tRuleRef[ (i << 3) + j ] << ',';
			}
			os << endl;
		}
		
		
		for(i = (nLine << 3); i < ((int)src.m_tRuleRef.size()) - 1; ++i) {
			
			os << std::setw(8)
				<< src.m_tRuleRef[i] << ',';
		}
		os << std::setw(8) << src.m_tRuleRef[i] <<  endl << "};" << endl;
		os << endl;
	}
	
	os.flush();

	return os;
}
};
