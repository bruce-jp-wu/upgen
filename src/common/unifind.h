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

#ifndef COMMON_UNIFIND_H__
#define COMMON_UNIFIND_H__

#include "./common.h"
#include "./types.h"
namespace common_ns {

//////////////////////////////////////////////////////////////////
// union-find set class
// use it for equivalent set division
class unifind_t {

private:

	enum {

		DEFAULT_SIZE = 1
	};

public:
	enum {

		INVALID_ELEM = -1
	};

private:
	
	int *m_pElem;
	int m_nSize;

public:

	unifind_t(int sz = DEFAULT_SIZE);
	~unifind_t(void);
	void destroy(void);
	void init(int sz);
	int find(int e);
	bool merge(int e1, int e2);

	//get equivalent sets
	void getClasses(vpvint_t &vvi);
	
private:

	void collapse(int e, int r);
	void collapseAll(void);
};

}

#endif // COMMON_UNIFIND_H__
