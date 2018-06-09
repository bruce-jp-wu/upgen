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

#ifndef COMMON_CHARMAP_H__
#define COMMON_CHARMAP_H__

#include <vector>
using std::vector;

#include "./common.h"
#include "./types.h"
#include "./uset.h"

namespace common_ns {

// class charmap_t: a compressed char table.
// in charmap_t object, char set will be divided into equivalent groups,
// and characters in the same group are not distinguishable;
// initially, all the chars in charset belong the same group.
// the group may be divided into smaller groups by a serial of group division

// by introducing an array @m_cParent, we can identify group: any two char have
// same parent, are in the same group.
// the one whose parent is itself, we call representive to its group,
// or representive for short in later comments

class charmap_t {
	
	struct _node_t {
		
		unsigned char c;
		_node_t *p;
	};
	
private:
	
	// array of used flag
	charset_t m_csUsed;
	// used to distinguish different group
	// if two char have same parent, then they are in the same group
	unsigned char m_cParent[256];
	
private:
	
	inline bool isUsed(unsigned char a_cElem) const {
		
		return m_csUsed.test(a_cElem);
	}
	
public:
	
	inline charmap_t(void)
	: m_csUsed() {
		
		clear();
	}
	
	void clear(void);
	// identify a single-char group, this may cause group division
	void addGroup(unsigned char a_cGrp);
	// identify groups. it doesn't meam that chars in @a_ustrGrp are in the same group;
	// it means that, if chars in @a_ustrGrp are in same group before operation, then
	// they are in the same group after operation
	void addGroup(const ustring &a_ustrGrp);
	// get equivalent all groups in charmap
	void getGroups(vustr_t &a_vustr);
};
}

#endif // COMMON_CHARMAP_H__
