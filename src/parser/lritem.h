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

#ifndef PARSER_LRITEM_H__
#define PARSER_LRITEM_H__

#include "./defines.h"
#include "../common/common.h"
#include "../common/types.h"

#include <functional>
using std::binary_function;
#include <set>
using std::set;
#include <map>
using std::map;
using std::pair;
#include <vector>
using std::vector;
#include <iterator>
using std::iterator;

namespace parser_ns {

// LR0 item pair
struct lri_pair_t {
	
public:
	
	union {
		int m_nRule;
		int m_nState;
	};
	union {
		int m_nDot;
		int m_nLookahead;
		int m_nSymbol;
	};
	
public:
	
	inline lri_pair_t(int a_nRule = INVALID_INDEX, int a_nDot = INVALID_INDEX)
	: m_nRule(a_nRule)
	, m_nDot(a_nDot){
	}
	inline lri_pair_t(const lri_pair_t &src)
	: m_nRule(src.m_nRule)
	, m_nDot(src.m_nDot){
	}
	
	inline const lri_pair_t& operator=(const lri_pair_t& src) {
		m_nRule = src.m_nRule;
		m_nDot = src.m_nDot;
		return *this;
	}
	inline bool operator==(const lri_pair_t& src) const {
		return _EQ(m_nRule, src.m_nRule) && _EQ(m_nDot, src.m_nDot);
	}
	inline bool operator!=(const lri_pair_t& src) const {
		return m_nRule != src.m_nRule || m_nDot != src.m_nDot;
	}
	inline bool operator<(const lri_pair_t &src) const {
		return m_nRule < src.m_nRule
			|| (_EQ(m_nRule, src.m_nRule)	&& m_nDot < src.m_nDot);
	}

};

typedef lri_pair_t lalr_item_t;
typedef vector<lalr_item_t> vlaitem_t;

typedef set< lri_pair_t > lrpair_set_t;
typedef lrpair_set_t::iterator lrps_it_t;
typedef lrpair_set_t::const_iterator lrps_cit_t;

class lrps_less_t: public binary_function < lrpair_set_t*, lrpair_set_t*, bool> {
public:
	bool operator() (const lrpair_set_t* a_pLeft, const lrpair_set_t* a_pRight) const {
		assert(a_pLeft && a_pRight);
		return *a_pLeft < *a_pRight;
	}
};

typedef map< lri_pair_t, sint_t* > lr2si_map_t;
typedef pair< lri_pair_t, sint_t* > lr2si_pair_t;
typedef lr2si_map_t::iterator lr2si_it_t;
typedef lr2si_map_t::const_iterator lr2si_cit_t;

typedef vector< lrpair_set_t* > vlrps_t;

typedef map< lrpair_set_t*, int, lrps_less_t > lrps2i_map_t;
typedef pair< lrpair_set_t*, int > lrps2i_pair_t;
typedef lrps2i_map_t::iterator lrps2i_it_t;
typedef lrps2i_map_t::const_iterator lrps2i_cit_t;

typedef map< int, lrpair_set_t* > i2lrps_map_t;
typedef pair<int, lrpair_set_t* > i2lrps_pair_t;
typedef i2lrps_map_t::iterator i2lrps_it_t;
typedef i2lrps_map_t::const_iterator i2lrps_cit_t;

// LR(K) item
struct lrk_item_t {
public:
	
	struct lri_pair_t m_lriPair;
	int m_nLookahead;
	
public:
	
	inline lrk_item_t(int a_nRule = INVALID_INDEX,
			int a_nDot = INVALID_INDEX,
			int a_nLookahead = INVALID_INDEX)
	: m_lriPair(a_nRule, a_nDot)
	, m_nLookahead(a_nLookahead) {
	}
	
	inline lrk_item_t(const lrk_item_t &src) 
	: m_lriPair(src.m_lriPair)
	, m_nLookahead(src.m_nLookahead) {
	}
	
	inline lrk_item_t& operator=(const lrk_item_t &src) {
		if(&src == this) {
			return *this;
		}
		m_lriPair = src.m_lriPair;
		m_nLookahead = src.m_nLookahead;
		return *this;
	}
	
	inline bool operator==(const lrk_item_t &src) const {
		return m_lriPair == src.m_lriPair;
	}
	
	inline bool operator<(const lrk_item_t &src) const {
		return m_lriPair < src.m_lriPair;
	}
};

typedef vector<lrk_item_t> vkitem_t;

enum {
	FT_CELL_ERROR = 0,
	FT_CELL_SHIFT,
	FT_CELL_REDUCE,
	FT_CELL_GOTO,
};

// cell of full parse table
struct fcell_t {
	
	struct {
		int m_nCellType: 4;
			
		union {
			int m_nState:	28;
			int m_nProd:	28;
			int m_nErrNo:	28;
			int m_nGoto:	28;
		};
	};
	
	inline fcell_t(void)
	: m_nCellType(FT_CELL_ERROR) {
	}
	inline fcell_t(int a_nState)
	: m_nCellType(FT_CELL_ERROR)
	, m_nState(a_nState) {
	}
	inline fcell_t(int a_nType, int a_nState)
	: m_nCellType(a_nType)
	, m_nState(a_nState) {
	}
	inline fcell_t(const fcell_t &src)
	: m_nCellType(src.m_nCellType)
	, m_nState(src.m_nState) {
		
	}
	inline fcell_t& operator=(const fcell_t &src) {
		m_nCellType = src.m_nCellType;
		m_nState = src.m_nState;
		return *this;
	}
	
	inline bool operator<(const fcell_t &src) const {
		return m_nCellType < src.m_nCellType ||
			(_EQ(m_nCellType, src.m_nCellType) && m_nState < src.m_nState);
	}
	inline bool operator==(const fcell_t &src) const {
		return _EQ(m_nCellType, src.m_nCellType) && _EQ(m_nState, src.m_nState);
	}
	inline bool operator!=(const fcell_t &src) const {
		return m_nCellType != src.m_nCellType || m_nState != src.m_nState;
	}
};

typedef vector< fcell_t > vcell_t;

// row of full parse table
struct frow_t {

public:
	int m_nID;
	int m_nValidCells;
	vcell_t m_vnCells;

public:

	inline frow_t(int a_nID)
	: m_nID(a_nID)
	, m_nValidCells(0) {
	}
	inline frow_t(int a_nID, int a_nSize)
	: m_nID(a_nID)
	, m_nValidCells(0) {
		assert(a_nSize > 0);
		m_vnCells.resize(a_nSize);
	}
	inline frow_t(int a_nID, int a_nSize, int a_nInitType, int a_nInitVal)
	: m_nID(a_nID)
	, m_nValidCells(0) {
		assert(a_nSize > 0);
		m_vnCells.resize(a_nSize, fcell_t(a_nInitType, a_nInitVal));
	}
	
	inline fcell_t& operator[](int a_nIdx) {
		assert(a_nIdx >= 0 && a_nIdx < (int)m_vnCells.size());
		return m_vnCells[a_nIdx];
	}
	inline const fcell_t& operator[](int a_nIdx) const {
		assert(a_nIdx >= 0 && a_nIdx < (int)m_vnCells.size());
		return m_vnCells[a_nIdx];
	}
	inline int getID(void) const {
		return m_nID;
	}
	inline void setID(int a_nID) {
		m_nID = a_nID;
	}
	inline int incValid(int a_nNum) {
		m_nValidCells += a_nNum;
		return m_nValidCells;
	}
	inline int size(void) const {
		return (int)m_vnCells.size();
	}
	inline int validSize(void) const {
		return m_nValidCells;
	}
private:
	frow_t(const frow_t&);
	frow_t& operator=(const frow_t&);
};

// full parse table
typedef vector< frow_t* > vftbl_t;

typedef binary_function < frow_t*, frow_t*, bool > frow_compare_t;

class frow_less_t: public frow_compare_t {
	
public:
	inline bool operator()(const frow_t* a_pLeft, const frow_t* a_pRight) const {
		if(! a_pLeft) {
			return true;
		}
		else if( ! a_pRight) {
			return false;
		}
		return a_pLeft->m_nValidCells < a_pRight->m_nValidCells;
	}
};

// conflict item class
struct cnft_item_t {

public:
	int m_nShift;
	int m_nDefRule;
	sint_t m_siRules;
	
public:
	inline cnft_item_t(const sint_t &a_siRule)
	: m_nShift(INVALID_STATE) {
		assert(a_siRule.size() > 1);
		sint_cit_t cit = a_siRule.begin();
		m_nDefRule = *cit;
		++cit;
		m_siRules.insert(cit, a_siRule.end());
	}
	inline cnft_item_t(int a_nAct, const sint_t &a_siRule)
	: m_nShift(a_nAct) {
		
		assert(a_siRule.size() > 0);
		sint_cit_t cit = a_siRule.begin();
		m_nDefRule = *cit;
		++cit;
		m_siRules.insert(cit, a_siRule.end());
		
	}
	inline void addRule(int a_nRule, bool a_bIsDef = false) {
		if(a_bIsDef) {
			m_nDefRule = a_nRule;
		}
		m_siRules.insert(a_nRule);
	}
	inline void addRules(const sint_t &a_siRules) {
		m_siRules.insert(a_siRules.begin(), a_siRules.end());
	}
	inline int getShift(void) const {
		return m_nShift;
	}
	void setShift(int a_nShift) {
		m_nShift = a_nShift;
	}
	inline int getDefRule(void) const {
		return m_nDefRule;
	}
	inline void setDefRule(int a_nRule) {
		m_nDefRule = a_nRule;
		m_siRules.insert(a_nRule);
	}
	
	inline sint_it_t ruleBegin(void) {
		return m_siRules.begin();
	}
	inline sint_cit_t ruleBegin(void) const {
		return m_siRules.begin();
	}
	inline sint_it_t ruleEnd(void) {
		return m_siRules.end();
	}
	inline sint_cit_t ruleEnd(void) const {
		return m_siRules.end();
	}
	inline int getCount(void) const {
		
		return (int)m_siRules.size() + 1 + (m_nShift != INVALID_STATE ? 1 : 0);
	}
};

typedef map<int, cnft_item_t* > i2cnft_map_t;
typedef pair<int, cnft_item_t* > i2cnft_pair_t;
typedef i2cnft_map_t::iterator i2cnft_it_t;
typedef i2cnft_map_t::const_iterator i2cnft_cit_t;

typedef vector< i2cnft_map_t* > conflicts_t;

}

#endif // PARSER_LRITEM_H__
