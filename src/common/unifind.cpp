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

#include "./unifind.h"

namespace common_ns {
/////////////////////////////////////////////////////////////////
// union-find set class
// use it for equivalent set division

unifind_t::unifind_t(int sz) :
	m_pElem(nullptr) {
	
	init(sz);
}

unifind_t::~unifind_t(void) {
	
	destroy();
}

void unifind_t::destroy(void) {
	
	if (m_pElem) {
		
		delete[] m_pElem;
	}
	m_pElem = nullptr;
	m_nSize = 0;
}

// initialize union-find set
void unifind_t::init(int sz) {
	
	if (m_pElem) {
		
		delete[] m_pElem;
		m_pElem = nullptr;
	}

	if (sz < DEFAULT_SIZE) {
		
		m_nSize = DEFAULT_SIZE;
	}
	else {
		
		m_nSize = sz;
	}

	m_pElem = new int[m_nSize];
	for (int i = 0; i < m_nSize; ++i) {
		
		m_pElem [i] = INVALID_ELEM;
	}
}

int unifind_t::find(int e) {
	
	if (e < 0 || e >= m_nSize) {
		
		return INVALID_ELEM;
	}

	int p = e;
	while (m_pElem[p] >= 0) {
		
		p = m_pElem[p];
	}
	
	return p;
}

bool unifind_t::merge(int e1, int e2) {
	
	if ((e1 < 0 || e1 >= m_nSize) || (e2 < 0 || e2 >= m_nSize)) {
		
		return false;
	}
	
	int p1 = find(e1);
	int p2 = find(e2);

	if (p1 == p2) {
		
		return true;
	}
	
	if (m_pElem[p1] > m_pElem[p2]) {
		
		m_pElem[p2] += m_pElem[p1];
		m_pElem[p1] = p2;
		collapse(e1, p2);
	}
	else {
		
		m_pElem[p1] += m_pElem[p2];
		m_pElem[p2] = p1;
		collapse(e2, p1);
	}
	
	return true;
}

void unifind_t::collapse(int e, int r) {
	
	int i = e, j;
	while (i != r) {
		
		j = m_pElem[i];
		m_pElem[i] = r;
		i = j;
	}
}

void unifind_t::collapseAll(void) {
	
	for (int i = 0; i < m_nSize; ++i) {
		
		if (m_pElem[i] >= 0) {
			
			collapse(i, find(i));
		}
	}
}

void unifind_t::getClasses(vpvint_t &vvi) {
	
	int i;
	for(i = 0; i < (int)vvi.size(); ++i) {
		
		if(vvi[i]) {
			
			delete vvi[i];
		}
	}
	vvi.clear();
	
	if (m_nSize <= 0) {
	
		return;
	}
	
	vint_t vindex;
	vint_t *pv;

	vindex.resize(m_nSize);
	collapseAll();

	for (i = 0; i < m_nSize; ++i) {
		
		if (m_pElem[i] < 0) {
			
			vindex[i] = (int)vvi.size();
			pv = new vint_t;
			pv->push_back(i);
			vvi.push_back(pv);
		}
	}

	for (i = 0; i < m_nSize; ++i) {
		
		if (m_pElem[i] >= 0) {
			
			vvi[vindex[m_pElem[i]]]->push_back(i);
		}
	}
}
//unifind_t
/////////////////////////////////////////////////////////////////
}
