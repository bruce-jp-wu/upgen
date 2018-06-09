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

#ifndef LEXER_TOKEN_H__
#define LEXER_TOKEN_H__
#include "../common/types.h"
#include "../common/uset.h"
using common_ns::charset_t;

#include "../common/action.h"
using common_ns::action_t;

#include "./retree.h"

#include "../parser/prod.h"
using parser_ns::prod_item_t;
using parser_ns::prod_t;

namespace lexer_ns{

// token type used by script parser
	union token_t {
		int m_nVal;
		
		char m_cVal;
		char *m_pchVal;
		
		struct {
			
			short m_nLower;
			short m_nUpper;
		}m_nRange;
		
		charset_t *m_pCS;
		vint_t *m_pIndices;
		
		charset_t::posix_char_t m_nPosix;
		
		renode_t *m_pRENode;
		retree_t *m_pRETree;
		rtpool_t *m_prTrees;
		
		prod_item_t *m_pPItem;
		prod_t *m_pProd;
		
		action_t *m_pDstrct;
		vstr_t *m_pvstrSymbs;
	};

}
#endif // LEXER_TOKEN_H__
