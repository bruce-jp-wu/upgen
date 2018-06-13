/*******************************************************************************
A parser program in C++, generated by Upgen 0.5.4.
******************************************************************************/

#ifndef CLASS_yyPARSE_T_FILE_ULYAC_1528898923_H__
#define CLASS_yyPARSE_T_FILE_ULYAC_1528898923_H__

#line 1 "meta/ulyac.upg"

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

#include "../main/upgmain.h"
#include "../lexer/token.h"
using lexer_ns::token_t;

#include "../lexer/dtable.h"
using lexer_ns::dtable_t;

#include "../lexer/lexer.h"
using lexer_ns::lexer_t;

#include "../parser/defines.h"
#include "../parser/grammar.h"
using parser_ns::grammar_t;
#include "../parser/ptable.h"
using parser_ns::ptable_t;

#include "../coder/dmmap.h"
using coder_ns::dmmap_t;


#line 51 "ulyac.h"
namespace yynsx {

// token ID definition
enum {
ENDP = 258,
SECT_DELIM = 259,
KW_OFF = 260,
KW_ON = 261,
KW_INITIAL = 262,
KW_EOF = 263,
KW_ALLSC = 264,
ISCLABEL = 265,
XSCLABEL = 266,
NCS = 267,
POSIX_SET = 268,
OPT_KEY = 269,
XISC = 270,
CSTR = 271,
CID = 272,
LFTID = 273,
INT = 274,
LEX_ACTION = 275,
PACT_BEGIN = 276,
PACT_END = 277,
RANGE_EQUAL = 278,
RANGE_LESS = 279,
RANGE_MORE = 280,
RANGE_BET = 281,
MCALL = 282,
CCHAR = 283,
QCHAR = 284,
QSTR_TOKEN = 285,
TOKEN_LABEL = 286,
RIGHT_LABEL = 287,
LEFT_LABEL = 288,
NONASSOC_LABEL = 289,
TYPE_LABEL = 290,
DPRE_LABEL = 291,
PREC_LABEL = 292,
START_LABEL = 293,
DESTRUCTOR_LABEL = 294,

__UPGEN_GENERATED_PARSE_TOKEN_VALUE = 0
};

// function type for error-reporting
typedef void(*yyerror_t)(const char* msg, lexer_ns::lexer_t& lexer, lexer_ns::dtable_t &dtbl,
	parser_ns::grammar_t &grammar, parser_ns::ptable_t &ptbl,
	dmmap_t &dmap, gsetting_t &gsetup);

// assign new log stream
void yysetstream(FILE *poutput, FILE *plogger);
// assign new error-reporter and return the old one
yyerror_t yyseterror(yyerror_t);
// reset all (both scanner and parser, if they are available)
void yyclearall(void);

// location defines here
struct yyltype_t {
	int firstLine;
	int firstColumn;
	int lastLine;
	int lastColumn;
	
};

// semantic value type defined by user as token_t
typedef token_t xstype_t;
typedef yyltype_t xltype_t;
// function type for scanning
typedef int (*yylex_t)(lexer_ns::lexer_t& lexer, lexer_ns::dtable_t &dtbl,
	parser_ns::grammar_t &grammar, parser_ns::ptable_t &ptbl,
	dmmap_t &dmap, gsetting_t &gsetup);

// assign new scanner and return the old one
yylex_t yysetlex(yylex_t);

// initializing file buffer before parsing or patter-matching
int yylexfile(const char* pchFile, bool iMod = false);
// initializing string buffer before parsing or patter-matching
int yylexstr(char *strbuffer, int size);
// initializing const string buffer before parsing or patter-matching
int yylexcstr(const char *strbuffer, int size);

// generated scanner, can be replaced
int yylex(lexer_ns::lexer_t& lexer, lexer_ns::dtable_t &dtbl,
	parser_ns::grammar_t &grammar, parser_ns::ptable_t &ptbl,
	dmmap_t &dmap, gsetting_t &gsetup);


// generated parser, it should not be replaced
int yyparse(lexer_ns::lexer_t& lexer, lexer_ns::dtable_t &dtbl,
	parser_ns::grammar_t &grammar, parser_ns::ptable_t &ptbl,
	dmmap_t &dmap, gsetting_t &gsetup);

extern xstype_t yylval;
extern xltype_t yylloc;

} // namespace
#endif
