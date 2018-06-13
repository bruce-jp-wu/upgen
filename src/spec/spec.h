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

#ifndef SPEC_SPEC_H__
#define SPEC_SPEC_H__

namespace spec_ns {

#define SPEC_NUM			2
#define	CPP_SPEC_SIZE		64000
#define PAS_SPEC_SIZE		71680

// array of language SPEC names
extern const char* sn_ppchSPECName[SPEC_NUM + 1];
// contents of C++ SPEC
extern char sn_cppSpec[CPP_SPEC_SIZE];
// contents of object pascal SPEC
extern char sn_pasSpec[PAS_SPEC_SIZE];

enum {
	LS_CPP = 0,
	LS_PASCAL = 1,
//	LS_JAVA,
//	LS_VBNET,
//	LS_CSHARP,
};

// get contents of built-in language SPEC
// @a_pcchSPECName: name of language SPEC
// @a_pcchContents: contents of language SPEC
// @a_nSize: contents' size of language SPEC
int getLangSPEC(const char* a_pcchSPECName, char* &a_pchContents, int &a_nSize);

}

#endif // SPEC_SPEC_H__
