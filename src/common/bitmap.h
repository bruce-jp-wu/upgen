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

#ifndef COMMON_BITMAP_H__
#define COMMON_BITMAP_H__

#include "./types.h"
#include "./common.h"

namespace common_ns {

//class bitmap_t: encapsulated class of 0-1 matrix

class bitmap_t {

private:
	
	// number of rows in matrix
	int m_nRows;
	// number of bytes in a row, hence we have
	// 	columns = @m_nBytesPerRow * 8
	int m_nBytesPerRow;
	// bit matrix
	bit_vect_t m_bitTbl;

public:
	
	inline bitmap_t(int a_nRows = 0, int a_nCols = 0)
	: m_nRows((a_nRows > 0)? a_nRows : 1)
	, m_nBytesPerRow ((a_nCols > 0)? ((a_nCols + BYTE_SIZE - 1) / BYTE_SIZE): 1)
	, m_bitTbl(m_nRows * m_nBytesPerRow, 0) {
	}
	
	inline void resize(int a_nRows, int a_nCols) {
		
		m_nRows = (a_nRows > 0)? a_nRows : 1;
		m_nBytesPerRow = (a_nCols > 0)? ((a_nCols + BYTE_SIZE - 1) / BYTE_SIZE): 1;
		m_bitTbl.resize(m_nRows * m_nBytesPerRow, 0);
	}
	// get value of a cell in matrix
	inline bool get(int a_nRow, int a_nCol) const {
		assert(a_nRow >= 0 && a_nRow < m_nRows);
		assert(a_nCol >= 0 && a_nCol < m_nBytesPerRow * (int)BYTE_SIZE);
		
		int byt = a_nRow * m_nBytesPerRow + a_nCol / BYTE_SIZE;
		int bit = a_nCol % BYTE_SIZE;
		
		return m_bitTbl[byt] & ((byte_t)(1 << bit));
	}
	// set value of a cell to 1
	inline void set(int a_nRow, int a_nCol) {
		assert(a_nRow >= 0 && a_nRow < m_nRows);
		assert(a_nCol >= 0 && a_nCol < m_nBytesPerRow * (int)BYTE_SIZE);
		
		int byt = a_nRow * m_nBytesPerRow + a_nCol / BYTE_SIZE;
		int bit = a_nCol % BYTE_SIZE;
		
		m_bitTbl[byt] |= ((byte_t)(1 << bit));
	}
	// set value of a cell in matrix
	inline void set(int a_nRow, int a_nCol, bool a_bVal) {
		assert(a_nRow >= 0 && a_nRow < m_nRows);
		assert(a_nCol >= 0 && a_nCol < m_nBytesPerRow * (int)BYTE_SIZE);
		
		int byt = a_nRow * m_nBytesPerRow + a_nCol / BYTE_SIZE;
		int bit = a_nCol % BYTE_SIZE;
		
		if(a_bVal) {
			m_bitTbl[byt] |= ((byte_t)(1 << bit));
		}
		else {
			m_bitTbl[byt] &= ((byte_t)-1) - ((byte_t)(1 << bit));
		}
	}
	
	inline int getRowNum(void) const {
		return m_nRows;
	}
	inline int getRowSize(void) const {
		return m_nBytesPerRow;
	}
	inline const bit_vect_t& getBVect(void) const {
		return m_bitTbl;
	}
	
	inline void clear(void) {
		m_nRows = 0;
		m_nBytesPerRow = 0;
		m_bitTbl.clear();
	}
};

}

#endif //COMMON_BITMAP_H__
