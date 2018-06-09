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


#include <iomanip>
using std::setw;

#include <ostream>
using std::ostream;
using std::endl;

#include "./dfafwd.h"
#include "./dgnode.h"
#include "./dgraph.h"
#include "./dtable.h"

namespace lexer_ns {

int dgraph_node_t::getTotalLabelCount(void) const {
	assert(!m_bArc);
	return m_dgRef.getDTable().getLabelCount();
}

ostream& operator<<(ostream &os, const dgraph_node_t& src) {

	os << "State " << src.getID() << " ["
		<< "Default: " << src.getDefault() << "; "
		<< "Cycled: " << src.getCycled() << "; "
		<< "Rule: " << src.getRule() << "; "
		<< "Action: " << src.getAction() << "; "
		<< "Lookahead accept-state:";

	os << "]"	<< endl;
	
	if(src.isArcRep()) {
		
		charset_t cs;
		for(int i = 0; i < src.getOutArcCount(); ++i) {
			
			cs |= (*src.getOutArc(i)).getCharSet();
		}
		
		os << "\t[";
		for(int i = 0; i < 256; ++i) {
			
			os << setw(4) << i;
		}
		os << "]" << endl;
		
		const dtable_t& dtbl = src.getDGraph().getDTable();
		const vint_t& vnMap = dtbl.m_tCharMap;
		vint_t viMap;
		
		if(dtbl.getLabelCount() > 0) {
			
			viMap.resize(dtbl.getLabelCount(), INVALID_STATE);
			os << "\t[";
			for(int i = 0; i < 256; ++i) {
				
				os << setw(4) << (int)vnMap[i];
			}
			os << "]" << endl;
		}
		
		os << "\t[";
		for(int i = 0; i < 256; ++i) {
			
			if(cs[i]) {
			
				for(int j = 0; j < src.getOutArcCount(); ++j) {
					
					if(src.getOutArc(j)->getCharSet()[i]) {
						
						if(dtbl.getLabelCount() > 0) {
							
							viMap[vnMap[i]] = src.getOutArc(j)->getToState();
						}
						os << setw(4) << src.getOutArc(j)->getToState();
						break;
					}
				}
			}
			else {
				
				os << setw(4) << INVALID_STATE;
			}
		}
		os << "]" << endl;
		
		if(dtbl.getLabelCount() > 0) {
		
			os << endl;
			os << "\t[";
			for(int i = 0; i < dtbl.getLabelCount(); ++i) {
				
				os << setw(4) << i;
			}
			os << "]" << endl;
			
			os << "\t[";
			for(int i = 0; i < dtbl.getLabelCount(); ++i) {
				
				os << setw(4) << viMap[i];
			}
			os << "]" << endl;
		}
	}
	else {
		
		if(src.m_nLabels > 0) {
			
			int j = 0;
			for(; j < src.getTotalLabelCount() && _EQ(src.getTargetState(j), INVALID_STATE); ++j) {
				
				;
			}
			int k = src.getTotalLabelCount() - 1;
			for(; k >= j && _EQ(src.getTargetState(k), INVALID_STATE); --k) {
				
				;
			}
			
			assert(k >= j);
			
			os << "\t[";
			for(int i = j; i <= k; ++i) {
		
				os << setw(4) << i;		
			}
			os << endl;
			
			os << "\t[";
			for(int i = j; i <= k; ++i) {
		
				os << setw(4) << src.getTargetState(i);		
			}
			os << endl;
		}
	}
	
	return os;
}

}


