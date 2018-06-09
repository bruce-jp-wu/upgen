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

#ifndef COMMON_TYPES_H__
#define COMMON_TYPES_H__

#include <string>
using std::string;
using std::basic_string;
#include <vector>
using std::vector;
#include <set>
using std::set;
#include <map>
using std::map;
using std::pair;

#define BYTE_SIZE			(sizeof(byte_t) * 8)
typedef unsigned char byte_t;

typedef vector<byte_t> bit_vect_t;

// g++-3 only implements part member functions of basic_string<unsigned char>
// use basic_string<char> instead
//typedef basic_string<unsigned char> ustring;
typedef char uchar;
typedef basic_string<uchar> ustring;
typedef vector< string > vstr_t;
typedef vector< ustring > vustr_t;


typedef vector< char* > vpchar_t;

typedef vector< bool > vbool_t;
typedef vector < int > vint_t;
typedef vector < int > table_t;

typedef pair<int, bool> i2b_pair_t;

typedef set< unsigned char > suchar_t;
typedef suchar_t::iterator suchar_it_t;
typedef suchar_t::const_iterator suchar_cit_t;

typedef set<int> sint_t;
typedef sint_t::iterator sint_it_t;
typedef sint_t::const_iterator sint_cit_t;

typedef map< string, int > s2i_map_t;
typedef pair< string, int > s2i_pair_t;
typedef s2i_map_t::iterator s2i_it_t;
typedef s2i_map_t::const_iterator s2i_cit_t;

typedef map< string, string > s2s_map_t;
typedef pair< string, string > s2s_pair_t;
typedef s2s_map_t::iterator s2s_it_t;
typedef s2s_map_t::const_iterator s2s_cit_t;

typedef map< int, int > i2i_map_t;
typedef pair< int, int > i2i_pair_t;
typedef i2i_map_t::iterator i2i_it_t;
typedef i2i_map_t::const_iterator i2i_cit_t;

typedef map<int, vint_t* > i2vi_map_t;
typedef pair<int, vint_t* > i2vi_pair_t; 
typedef i2vi_map_t::iterator i2vi_it_t;
typedef i2vi_map_t::const_iterator i2vi_cit_t;

typedef map<int, sint_t* > i2si_map_t;
typedef pair<int, sint_t* > i2si_pair_t;
typedef i2si_map_t::iterator i2si_it_t;
typedef i2si_map_t::const_iterator i2si_cit_t;

typedef vector < vint_t* > vpvint_t;

typedef vector< sint_t* > vpsint_t;

#endif // COMMON_TYPES_H__
