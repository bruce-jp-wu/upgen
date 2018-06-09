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


#include <cstring>
#include "./spec.h"

namespace spec_ns {

// array of language SPEC names
const char* sn_ppchSPECName[SPEC_NUM + 1] = {
		"C++",
		"PAS",
		nullptr
};

// array of contents of language SPECs
static char* sn_ppcchSPECs[SPEC_NUM + 1] = {
		sn_cppSpec,
		sn_pasSpec,
		nullptr
};

// array of contents' size of language SPECs
static int sn_nSPECSize[SPEC_NUM + 1] = {
		CPP_SPEC_SIZE,
		PAS_SPEC_SIZE,
		0
};

// get contents of built-in language SPEC
// @a_pcchSPECName: name of language SPEC
// @a_pcchContents: contents of language SPEC
// @a_nSize: contents' size of language SPEC
int getLangSPEC(const char* a_pcchSPECName, char* &a_pcchContents, int &a_nSize) {
	
	if( ! a_pcchSPECName) {
		a_pcchContents = sn_cppSpec;
		a_nSize = CPP_SPEC_SIZE;
		return 0;
	}
	
	int i = 0;
	for(; i < SPEC_NUM; ++i) {
		
		if( ! strcmp(a_pcchSPECName, sn_ppchSPECName[i])) {
			
			a_pcchContents = sn_ppcchSPECs[i];
			a_nSize = sn_nSPECSize[i];
			return i;
		}
	}
	
	return i;
}

}
