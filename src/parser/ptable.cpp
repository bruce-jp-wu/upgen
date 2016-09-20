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

#include "../main/upgmain.h"
#include <ostream>
using std::ostream;
#include <iomanip>
using std::setw;
using std::endl;
#include "./ptable.h"

namespace parser_ns {

void ptable_t::clear(void) {
	
	m_nSState = FIRST_STATE;
	m_nAState = INVALID_STATE;
	m_nEState = FIRST_STATE;
	m_nERule = INVALID_RULE;
	m_nTypeBase = 0;
	m_nSymbNum = 0;
	
	m_vstrSName.clear();

	m_strSType = "";
	m_strLType = "";
	m_vstrTokenDef.clear();
	
	m_tTokenDef.clear();
	
	m_tTokenID.clear();
	m_tTokenMap.clear();
	m_tRPartNum.clear();
	
	m_tLPartIdx.clear();
	m_tRPBase.clear();
	m_tRPartIdx.clear();
	
	m_tParse.clear();
	m_tBase.clear();

	m_tGoto.clear();
	m_tGBase.clear();
	
	m_ruleMgr.clear();
	m_pCurAction = nullptr;
	
	m_tDstrctSymb.clear();
	m_tDstrctIdx.clear();
	
	for(int i = 0; i < (int)m_vaDstrct.size(); ++i) {
		if(m_vaDstrct[i]) {
			delete m_vaDstrct[i];
		}
	}
}

ostream& operator<<(ostream &os, const ptable_t &src) {
	
	int i, j, nLine;
	
	if(src.m_vstrSName.size() == 0 || src.m_tBase.size() == 0 || src.m_tGBase.size() == 0) {
		
		// output empty arrays here
		return os;
	}

	// symbol names
	os << "int symName[" << src.m_vstrSName.size() << "] = {\n";
	nLine = (((int)src.m_vstrSName.size()) - 1) / 4;
	for(i = 0; i < nLine; ++i) {
		
		for(j = 0; j < 4; ++j) {
			
			os << "\t" << src.m_vstrSName[i * 4 + j] << ",";
		}
		
		os << "\n";
	}
	for(i = nLine * 4; i < ((int)src.m_vstrSName.size()) - 1; ++i) {
		
		os << "\t" << src.m_vstrSName[i] << ",";
	}
	os << "\t" << src.m_vstrSName[i] << "\n};\n";
	os << "\n";
	
	os << "#define TYPE_BASE " << src.m_nTypeBase << "\n";
	os << "#define SYMB_COUNT " << src.m_nSymbNum << "\n";
	
	// token ID
	os << "int tokenID[" << src.m_tTokenID.size() << "] = {\n";
	nLine = (((int)src.m_tTokenID.size()) - 1) / 8;
	for(i = 0; i < nLine; ++i) {
		
		for(j = 0; j < 8; ++j) {
			
			os << std::setw(8) 
				<< (int)src.m_tTokenID[i * 8 + j] << ",";
		}
		
		os << "\n";
	}
	for(i = nLine * 8; i < ((int)src.m_tTokenID.size()) - 1; ++i) {
		
		os << std::setw(8)
			<< (int)src.m_tTokenID[i] << ",";
	}
	os << std::setw(8) << (int)src.m_tTokenID[i] << "\n};\n";
	os << "\n";
	
	// output token map
	os << "int tokenMap[" << src.m_tTokenMap.size() << "] = {\n";
	nLine = (((int)src.m_tTokenMap.size()) - 1) / 8;
	for(i = 0; i < nLine; ++i) {
		
		for(j = 0; j < 8; ++j) {
			
			os << std::setw(8) 
				<< (int)src.m_tTokenMap[i * 8 + j] << ",";
		}
		
		os << "\n";
	}
	for(i = nLine * 8; i < ((int)src.m_tTokenMap.size()) - 1; ++i) {
		
		os << std::setw(8)
			<< (int)src.m_tTokenMap[i] << ",";
	}
	os << std::setw(8) << (int)src.m_tTokenMap[i] << "\n};\n";
	os << "\n";
	
	// number of symbols in rhs of rule
	os << "int rhsNum[" << src.m_tRPartNum.size() << "] = {\n";
	nLine = (((int)src.m_tRPartNum.size()) - 1) / 8;
	for(i = 0; i < nLine; ++i) {
		
		for(j = 0; j < 8; ++j) {
			
			os << std::setw(8)
				<< src.m_tRPartNum[ i * 8 + j ] << ",";
		}
		
		os << "\n";
	}
	
	for(i = nLine * 8; i < ((int)src.m_tRPartNum.size()) - 1; ++i) {
		
		os << std::setw(8)
			<< src.m_tRPartNum[i] << ",";
	}
	os << std::setw(8) << src.m_tRPartNum[i] << "\n};\n";
	os << "\n";
	
	// lhs symbols of rules
	os << "int lhsSym[" << src.m_tLPartIdx.size() << "] = {\n";
	nLine = (((int)src.m_tLPartIdx.size()) - 1) / 8;
	for(i = 0; i < nLine; ++i) {
		
		for(j = 0; j < 8; ++j) {
			
			os << std::setw(8)
				<< src.m_tLPartIdx[ i * 8 + j ] << ",";
		}
		
		os << "\n";
	}
	
	for(i = nLine * 8; i < ((int)src.m_tLPartIdx.size()) - 1; ++i) {
		
		os << std::setw(8)
			<< src.m_tLPartIdx[i] << ",";
	}
	os << std::setw(8) << src.m_tLPartIdx[i] << "\n};\n";
	os << "\n";

	// base table for rules
	// it is used to determine the base location of the entries
	// for each rule in the table of right symbol indexes 
	os << "int rhsSymBase[" << src.m_tRPBase.size() << "] = {\n";
	nLine = (((int)src.m_tRPBase.size()) - 1) / 8;
	for(i = 0; i < nLine; ++i) {
		
		for(j = 0; j < 8; ++j) {
			
			os << std::setw(8)
				<< src.m_tRPBase[ i * 8 + j ] << ",";
		}
		
		os << "\n";
	}
	
	for(i = nLine * 8; i < ((int)src.m_tRPBase.size()) - 1; ++i) {
		
		os << std::setw(8)
			<< src.m_tRPBase[i] << ",";
	}
	os << std::setw(8) << src.m_tRPBase[i] << "\n};\n";
	os << "\n";
	
	// rhs symbols of rules
	os << "int rhsSymIdx[" << src.m_tRPartIdx.size() << "] = {\n";
	nLine = (((int)src.m_tRPartIdx.size()) - 1) / 8;
	for(i = 0; i < nLine; ++i) {
		
		for(j = 0; j < 8; ++j) {
			
			os << std::setw(8)
				<< src.m_tRPartIdx[ i * 8 + j ] << ",";
		}
		
		os << "\n";
	}
	
	for(i = nLine * 8; i < ((int)src.m_tRPartIdx.size()) - 1; ++i) {
		
		os << std::setw(8)
			<< src.m_tRPartIdx[i] << ",";
	}
	os << std::setw(8) << src.m_tRPartIdx[i] << "\n};\n";
	os << "\n";

	
	// action base table
	os << "int actBase[" << src.m_tBase.size() << "] = {\n";
	nLine = (((int)src.m_tBase.size()) - 1) / 8;
	for(i = 0; i < nLine; ++i) {
		
		for(j = 0; j < 8; ++j) {
			
			os << std::setw(8)
				<< src.m_tBase[ i * 8 + j ] << ",";
		}
		os << "\n";
	}
	
	for(i = (nLine * 8); i < ((int)src.m_tBase.size()) - 1; ++i) {
		
		os << std::setw(8)
			<< src.m_tBase[i] << ",";
	}
	os << std::setw(8) << src.m_tBase[i] << "\n};\n";
	os << "\n";
	
	// action table
	if(src.m_tParse.size() > 0) {

		os << "int actTbl[" << src.m_tParse.size() << "] = {\n";
		nLine = (((int)src.m_tParse.size()) - 1) >> 3;
		for(i = 0; i < nLine; ++i) {
			
			for(j = 0; j < 8; ++j) {
				
				os << std::setw(8)
					<< src.m_tParse[ i * 8 + j ] << ",";
			}
			os << "\n";
		}
		
		for(i = nLine * 8; i < ((int)src.m_tParse.size()) - 1; ++i) {
			
			os << std::setw(8)
				<< src.m_tParse[i] << ",";
		}
		os << std::setw(8) << src.m_tParse[i] << "\n};\n";
		os << "\n";
	}
	
	// goto base table
	os << "int gotoBase[" << src.m_tGBase.size() << "] = {\n";
	nLine = (((int)src.m_tGBase.size()) - 1) / 8;
	for(i = 0; i < nLine; ++i) {
		
		for(j = 0; j < 8; ++j) {
			
			os << std::setw(8)
				<< src.m_tGBase[ i * 8 + j ] << ",";
		}
		os << "\n";
	}
	
	for(i = (nLine * 8); i < ((int)src.m_tGBase.size()) - 1; ++i) {
		
		os << std::setw(8)
			<< src.m_tGBase[i] << ",";
	}
	os << std::setw(8) << src.m_tGBase[i] << "\n};\n";
	os << "\n";
	
	// goto table
	if(src.m_tGoto.size() > 0) {

		os << "int gotoTbl[" << src.m_tGoto.size() << "] = {\n";
		nLine = (((int)src.m_tGoto.size()) - 1) >> 3;
		for(i = 0; i < nLine; ++i) {
			
			for(j = 0; j < 8; ++j) {
				
				os << std::setw(8)
					<< src.m_tGoto[ i * 8 + j ] << ",";
			}
			os << "\n";
		}
		
		for(i = nLine * 8; i < ((int)src.m_tGoto.size()) - 1; ++i) {
			
			os << std::setw(8)
				<< src.m_tGoto[i] << ",";
		}
		os << std::setw(8) << src.m_tGoto[i] << "\n};\n";
		os << "\n";
	}
	
	// user action
	os << src.m_ruleMgr << '\n';

	if(src.m_vaDstrct.size() > 0) {
		// user destructors
		os << "// Destructors (actions):\n";
		for(i = 0; i < (int)src.m_vaDstrct.size(); ++i) {
			os << "// at line: " << src.m_vaDstrct[i]->getLineno() << "\n";
			os << "case " << i << ":\t";
			if(src.m_vaDstrct[i]) {
				string str;
				int nType;
				for(int j = 0; j < src.m_vaDstrct[i]->size(); ++j) {
					
					bool bret = src.m_vaDstrct[i]->get(j, str, nType);
					assert(bret);
					os << str;
				}
			}
		}
		os << "\n";
		
		os << "// Destructors (symbols)\n";
		os << "int dstrctSymb[" << src.m_tDstrctSymb.size() << "] = {\n";
		nLine = (((int)src.m_tDstrctSymb.size()) - 1) >> 3;
		for(i = 0; i < nLine; ++i) {
			
			for(j = 0; j < 8; ++j) {
				
				os << std::setw(8)
					<< src.m_tDstrctSymb[ i * 8 + j ] << ",";
			}
			os << "\n";
		}
		
		for(i = nLine * 8; i < ((int)src.m_tDstrctSymb.size()) - 1; ++i) {
			
			os << std::setw(8)
				<< src.m_tDstrctSymb[i] << ",";
		}
		os << std::setw(8) << src.m_tDstrctSymb[i] << "\n};\n";
		os << "\n";
		
		os << "// Destructors (indices)\n";
		os << "int dstrctIndex[" << src.m_tDstrctIdx.size() << "] = {\n";
		nLine = (((int)src.m_tDstrctIdx.size()) - 1) >> 3;
		for(i = 0; i < nLine; ++i) {
			
			for(j = 0; j < 8; ++j) {
				
				os << std::setw(8)
					<< src.m_tDstrctIdx[ i * 8 + j ] << ",";
			}
			os << "\n";
		}
		
		for(i = nLine * 8; i < ((int)src.m_tDstrctIdx.size()) - 1; ++i) {
			
			os << std::setw(8)
				<< src.m_tDstrctIdx[i] << ",";
		}
		os << std::setw(8) << src.m_tDstrctIdx[i] << "\n};\n";
		os << "\n";
	}
	
	os.flush();

	return os;
}

}
