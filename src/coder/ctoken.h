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

#ifndef CODER_CTOKEN_H__
#define CODER_CTOKEN_H__

#include "./cmacro.h"
#include "./cmcond.h"

typedef pair<string*, vint_t* > pstr2pvn_t;
typedef pair<int, string*> int2pstr_t;

namespace coder_ns {

// token type used by language SPEC parser
union ctoken_t {
	
	int m_nVal;
	bool m_bVal;
	
	string *m_pstrVal;
	
	cmacro_term_t* m_pmTerm;
	vterm_t* m_pvmTerms;
	cmacro_t* m_pMac;
	cond_compound_t* m_pCpdcond;
	cond_item_t* m_pcondItem;
	
	mac2cond_pair_t *m_pmac2Cond;
	vmac2cond_t *m_pvmac2Cond;
	
	pstr2pvn_t *m_pmacRef;
	vint_t *m_pvnVal;
	vstr_t *m_pvstrVal;
	
	int2pstr_t *m_textVal;
};

}


#endif // CODER_CTOKEN_H__
