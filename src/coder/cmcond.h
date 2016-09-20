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


#ifndef CODER_CMCOND_H__
#define CODER_CMCOND_H__

#include <string>
using std::string;
#include <vector>
using std::vector;

#include "../common/common.h"
#include "./dmmap.h"

namespace coder_ns {

/*
 * cond_item_t: ancestor of all macro condition-item classes 
 */
class cond_item_t {
	
public:
	// check if macro condition is satified
	// @a_map: data map
	// @a_strKey: name of a key
	virtual bool satisfied(const dmmap_t &a_map, const string &a_strKey) const = 0;
	virtual ~cond_item_t(void){}
};

typedef vector < cond_item_t* > vcond_item_t;

/*
 * cond_empty_t: check if corresponding value of a certain key is empty or not;
 * 				 when data map does not contain the key, it is also considered as empty
 */
class cond_empty_t: public cond_item_t {
	
private:
	
	// if @m_bTestEmpty is true, then function @satisfied return true
	//		if corresponding value of a specified key is empty
	// otherwise, function @satisfied return true
	//		if corresponding value of a specified key is not empty
	bool m_bTestEmpty;
	
public:
	
	inline cond_empty_t(bool a_bTestEmpty = true)
	: m_bTestEmpty(a_bTestEmpty) {
	}
	
	virtual bool satisfied(const dmmap_t &a_map, const string &a_strKey) const {
		
		if(m_bTestEmpty) {
			return a_map.isEmpty(a_strKey);
		}
		else {
			return (!a_map.isEmpty(a_strKey));
		}
	}
};

/*
 * cond_end_t: check if end of list is encountered. end of list is next to the last element of list
 */
class cond_end_t: public cond_item_t {
	
public:
	
	virtual bool satisfied(const dmmap_t &a_map, const string &a_strKey) const {
		
		return a_map.isEnd(a_strKey);
	}
};

/*
 * 
 * cond_last_t: check if current element is the last element of list
 */
class cond_last_t: public cond_item_t {
	
public:
	
	virtual bool satisfied(const dmmap_t &a_map, const string &a_strKey) const {
		
		return a_map.isLast(a_strKey);
	}
};

/*
 * 
 * cond_more_t: check if there are one or more elements in list at or after current element
 */
class cond_more_t: public cond_item_t {
	
public:
	
	virtual bool satisfied(const dmmap_t &a_map, const string &a_strKey) const {
		
		return a_map.isMore(a_strKey);
	}
};

/*
 * 
 * cond_mod_t: check if subscript of current element is divisible by a given integer
 */
class cond_mod_t: public cond_item_t {

private:
	// given divisor
	int m_nElemsPerLine;

public:
	
	inline cond_mod_t(int a_nElemsPerLine)
	: m_nElemsPerLine(a_nElemsPerLine) {
	}
	
	virtual bool satisfied(const dmmap_t &a_map, const string &a_strKey) const {
		return a_map.isMod(a_strKey, m_nElemsPerLine);
	}
};

/*
 * cond_group_t: check if current element is group delimiter, whose value is -1
 */
class cond_group_t: public cond_item_t {
	
public:
	
	virtual bool satisfied(const dmmap_t &a_map, const string &a_strKey) const {
		
		return a_map.isGroupFlag(a_strKey);
	}
};

/*
 * cond_groupfirst_t: check if current element is first element of a group delimiter
 */
class cond_groupfirst_t: public cond_item_t {
	
public:
	virtual bool satisfied(const dmmap_t &a_map, const string &a_strKey) const {
		
		return a_map.isGroupFirst(a_strKey);
	}
};

/*
 * cond_elemrel_t: check if current element is [not] greater than(less than or equal to) a given value  
 */
class cond_elemrel_t: public cond_item_t {

public:
	// relation operation type
	enum eop_t {
		
		EOP_EQ = 0,
		EOP_NE,
		EOP_LT,
		EOP_GT,
		EOP_LE,
		EOP_GE
	};
	
private:
	
	// value type
	enum elemrel_type_t {
		
		EVT_BOOL,
		EVT_INT,
		EVT_PSTR,
	};
	
private:
	
	// type of relation type
	elemrel_type_t m_nType;
	// type of value
	eop_t m_nOp;
	
	union {
		
		int m_nVal;
		string *m_pstrVal;
	};
	
public:
	
	inline cond_elemrel_t(eop_t a_rop, int a_nVal)
	: m_nType(EVT_INT)
	, m_nOp(a_rop)
	, m_nVal(a_nVal) {
		
	}
	inline cond_elemrel_t(eop_t a_rop, bool a_bVal)
	: m_nType(EVT_BOOL)
	, m_nOp(a_rop)
	, m_nVal(a_bVal ? TRUE : FALSE) {
		
	} 
	inline cond_elemrel_t(eop_t a_rop, const string &a_strVal)
	: m_nType(EVT_PSTR)
	, m_nOp(a_rop)
	, m_pstrVal(new string(a_strVal)) {
		
	}
	
	virtual  ~cond_elemrel_t(void) {
		
		if(EVT_PSTR == m_nType && m_pstrVal) {
			delete m_pstrVal;
		}
	}
	
public:
	
	virtual bool satisfied(const dmmap_t &a_map, const string &a_strKey) const {
		
		bool bret = false;
		
		switch(m_nType) {
		
		case EVT_INT:
		case EVT_BOOL:
		{
			int nVal;
			if(a_map.getInt(a_strKey, nVal)) {
				
				switch(m_nOp) {
				case EOP_EQ:
					
					bret = (nVal == m_nVal);
					break;
				
				case EOP_NE:
					
					bret = (nVal != m_nVal);
					break;
					
				case EOP_LT:
					
					bret = (nVal < m_nVal);
					break;
					
				case EOP_GT:
					
					bret = (nVal > m_nVal);
					break;
					
				case EOP_LE:
					
					bret = (nVal <= m_nVal);
					break;
					
				default:	// case EOP_GE:
					
					bret = (nVal >= m_nVal);
					break;
				}
				
			}
		}
		
			break;
			
		case EVT_PSTR:
		{
			
			string strVal;
			if(a_map.getString(a_strKey, strVal)) {
				
				switch(m_nOp) {
				case EOP_EQ:
					
					bret = (strVal == (*m_pstrVal));
					break;
					
				case EOP_NE:
				
					bret = !(strVal == (*m_pstrVal));
					break;
					
				case EOP_LT:
					
					bret = (strVal.compare(*m_pstrVal) < 0);
					break;
					
				case EOP_GT:
					
					bret = (strVal.compare(*m_pstrVal) > 0);
					break;
					
				case EOP_LE:
					
					bret = (strVal.compare(*m_pstrVal) <= 0);
					break;
					
				default:		//case EOP_GE
					
					bret = (strVal.compare(*m_pstrVal) >= 0);
					break;
				}
				
			}
		}
			break;
			
		default:
			assert(false);
			break;
		}
		
		return bret;
	}

};

/*
 * cond_opteq_t: check if current element is the value of option
 */
class cond_opteq_t: public cond_item_t {
	
	// option value type
	enum optval_type_t {
		OVT_BOOL,
		OVT_INT,
		OVT_PSTR,
	};
	
private:
	
	string m_strOptionName;
	
	optval_type_t m_nType;
	
	union {
		
		int m_nVal;
		string *m_pstrVal;
	};

public:
	
	inline cond_opteq_t(const string &a_strOptionName, int a_nVal)
	: m_strOptionName(a_strOptionName)
	, m_nType(OVT_INT)
	, m_nVal(a_nVal) {
		
	}
	inline cond_opteq_t(const string &a_strOptionName, bool a_bVal)
	: m_strOptionName(a_strOptionName)
	, m_nType(OVT_BOOL)
	, m_nVal(a_bVal ? TRUE : FALSE) {
		
	} 
	inline cond_opteq_t(const string &a_strOptionName, const string &a_strVal)
	: m_strOptionName(a_strOptionName)
	, m_nType(OVT_PSTR)
	, m_pstrVal(new string(a_strVal)) {
		
	}
	
	virtual ~cond_opteq_t(void) {
		
		if(OVT_PSTR == m_nType) {
			
			assert(m_pstrVal);
			delete m_pstrVal;
		}
	}
	
public:
	
	virtual bool satisfied(const dmmap_t &a_map, const string &a_strKey) const {
		
		bool bret = false;
		
		switch(m_nType) {
		
		case OVT_INT:
		case OVT_BOOL:
		{
			int nVal;
			if(a_map.getInt(m_strOptionName, nVal)) {
				
				bret = (nVal == m_nVal);
			}
		}
		
			break;
			
		case OVT_PSTR:
		{
			
			string strVal;
			if(a_map.getString(m_strOptionName, strVal)) {
				
				bret = (strVal == (*m_pstrVal));
			}
		}
			break;
			
		default:
			assert(false);
			break;
		}
		
		return bret;
	}
	
	inline bool operator==(const cond_opteq_t& src) {
		
		if(m_strOptionName == src.m_strOptionName
				&& m_nType == src.m_nType) {
			
			switch(m_nType) {
			
			case OVT_INT:
			case OVT_BOOL:
				
				return m_nVal == src.m_nVal;
				break;
				
			case OVT_PSTR:
				
				assert(m_pstrVal && src.m_pstrVal);
				return *m_pstrVal == *src.m_pstrVal;
				break;
				
			default:
				assert(false);
				break;
			}			
		}
		return false;
	}
};

/*
 * cond_compound_t: a collection of macro condition-item
 */
class cond_compound_t: public cond_item_t {
	
private:
	
	vcond_item_t m_vciConds;
	
public:
	
	cond_compound_t(void) {}
	virtual ~cond_compound_t(void) {// do nothing about freeing memory, for objec of cmacro_mgr_t does it
	}
	
public:
	
	inline void addCondItem(cond_item_t* a_pciItem) {
		
		if(a_pciItem) {
		
			m_vciConds.push_back(a_pciItem);
		}
	}
	
	virtual bool satisfied(const dmmap_t &a_map, const string &a_strKey) const {
		
		for(int i = 0; i < (int)m_vciConds.size(); ++i) {
			
			if(!m_vciConds[i]->satisfied(a_map, a_strKey)) {
				
				return false;
			}
		}
		return true;
	}
};

/*
 * cond_t: a collection of macro condition-item
 */

class cond_t {
	
public:
	
	inline cond_t(void) {}
	~cond_t(void) {// do nothing about freeing memory, because it is cmacro_mgr_t's job
	}
	
public:
	
	inline void addCondItem(cond_item_t* a_pciItem) {
		m_vciConds.push_back(a_pciItem);
	}
	
	inline int test(const dmmap_t &a_map, const string &a_strKey) const {
		
		int i = 0;
		for(; i < (int)m_vciConds.size(); ++i) {
			
			if( ! m_vciConds[i] || m_vciConds[i]->satisfied(a_map, a_strKey)) {
				
				return i;
			}
		}
		return i;
	}
	
	inline int size(void) const {
		
		return (int)m_vciConds.size();
	}
	
private:
	
	vcond_item_t m_vciConds;
};

typedef vector< cond_t* > vcond_t;

}
#endif // CODER_CMCOND_H__

