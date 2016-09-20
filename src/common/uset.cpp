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

#include "./uset.h"

namespace common_ns{

const unsigned char sn_cFullBits[8] = {
		1,		3,		7,		15,
		31,		63,		127,	255
};

const unsigned char sn_cSingleBit[8] = {
		1,		2,		4,		8,
		16,		32,		64,		128
};

const unsigned char sn_cMissBit[8] = {
		254,	253,	251,	247,
		239,	223,	191,	127,
};

const unsigned char sn_cOnes[256] = {
		0,		1,		1,		2,		1,		2,		2,		3,
		1,		2,		2,		3,		2,		3,		3,		4,
		1,		2,		2,		3,		2,		3,		3,		4,
		2,		3,		3,		4,		3,		4,		4,		5,
		1,		2,		2,		3,		2,		3,		3,		4,
		2,		3,		3,		4,		3,		4,		4,		5,
		2,		3,		3,		4,		3,		4,		4,		5,
		3,		4,		4,		5,		4,		5,		5,		6,
		1,		2,		2,		3,		2,		3,		3,		4,
		2,		3,		3,		4,		3,		4,		4,		5,
		2,		3,		3,		4,		3,		4,		4,		5,
		3,		4,		4,		5,		4,		5,		5,		6,
		2,		3,		3,		4,		3,		4,		4,		5,
		3,		4,		4,		5,		4,		5,		5,		6,
		3,		4,		4,		5,		4,		5,		5,		6,
		4,		5,		5,		6,		5,		6,		6,		7,
		1,		2,		2,		3,		2,		3,		3,		4,
		2,		3,		3,		4,		3,		4,		4,		5,
		2,		3,		3,		4,		3,		4,		4,		5,
		3,		4,		4,		5,		4,		5,		5,		6,
		2,		3,		3,		4,		3,		4,		4,		5,
		3,		4,		4,		5,		4,		5,		5,		6,
		3,		4,		4,		5,		4,		5,		5,		6,
		4,		5,		5,		6,		5,		6,		6,		7,
		2,		3,		3,		4,		3,		4,		4,		5,
		3,		4,		4,		5,		4,		5,		5,		6,
		3,		4,		4,		5,		4,		5,		5,		6,
		4,		5,		5,		6,		5,		6,		6,		7,
		3,		4,		4,		5,		4,		5,		5,		6,
		4,		5,		5,		6,		5,		6,		6,		7,
		4,		5,		5,		6,		5,		6,		6,		7,
		5,		6,		6,		7,		6,		7,		7,		8
};


bool sbitset_t::any(void) const{
	
	for(unsigned int i = 0; i < m_nBytes; ++i){
		
		if(0 != m_pchBits[i]){
			
			return true;
		}
	}
	
	return false;
}

unsigned int sbitset_t::count(void) const {
	
	unsigned int nBits = 0;
	for(unsigned int i = 0; i < m_nBytes; ++i) {
		
		nBits += (unsigned int)sn_cOnes[(unsigned int)m_pchBits[i]];
	}
	
	return nBits;
}

sbitset_t& sbitset_t::flip(void) {
	
	for(unsigned int i = 0; i < m_nBytes; ++i) {
		
		m_pchBits[i] ^= 0xff;
	}
	
	m_pchBits[m_nBytes - 1] ^= sn_cFullBits[m_nBits - (m_nBytes - 1) * BYTE_SIZE - 1];
	
	return *this;
}


string sbitset_t::to_string(void) const {
	
	string str;
	
	for(unsigned int i = 0; i < m_nBytes - 1; ++i) {
		
		for(unsigned int j = 0; j < BYTE_SIZE; ++j) {
			
			if(m_pchBits[i] & sn_cSingleBit[j]) {
				
				str += '1';
			}
			else {
				
				str += '0';
			}
		}
	}
	
	for(unsigned int i = 0; i < m_nBits - (m_nBytes - 1) * BYTE_SIZE; ++i) {
		
		if(m_pchBits[m_nBytes - 1] & sn_cSingleBit[i]) {
			
			str += '1';
		}
		else {
			
			str += '0';
		}
	}
	
	return str;
}

bool sbitset_t::operator==(const sbitset_t &src) const {
	
	if(m_nBits != src.m_nBits) {
		
		return false;
	}
	
	for(unsigned int i = 0; i < m_nBytes; ++i) {
		
		if(m_pchBits[i] != src.m_pchBits[i]) {
			
			return false;
		}
	}
	
	return true;
}

sbitset_t& sbitset_t::operator&=(const sbitset_t &src) {

	if(m_nBits <= src.m_nBits){
		
		for(unsigned int i = 0; i < m_nBytes; ++i) {
			
			m_pchBits[i] &= src.m_pchBits[i];
		}
	}
	else {
		
		for(unsigned int i = 0; i < src.m_nBytes - 1; ++i) {
			
			m_pchBits[i] &= src.m_pchBits[i];
		}
		
		if(src.m_nBits % BYTE_SIZE > 0) {
			
			m_pchBits[src.m_nBytes - 1] &=
				((0xff << (src.m_nBits % BYTE_SIZE)) | src.m_pchBits[src.m_nBytes - 1]);
		}
		else {
			
			m_pchBits[src.m_nBytes - 1] &= src.m_pchBits[src.m_nBytes - 1];
		}			
	}
	
	return *this;
}

sbitset_t& sbitset_t::operator|=(const sbitset_t &src) {

	if(m_nBits < src.m_nBits){
		
		for(unsigned int i = 0; i < m_nBytes; ++i) {
			
			m_pchBits[i] |= src.m_pchBits[i];
		}
		
		if(m_nBits % BYTE_SIZE > 0) {
			
			m_pchBits[m_nBytes - 1] &= (0xff >> (BYTE_SIZE - (m_nBits % BYTE_SIZE)));
		}
	}
	else {
		
		for(unsigned int i = 0; i < src.m_nBytes; ++i) {
			
			m_pchBits[i] |= src.m_pchBits[i];
		}
	}
	
	return *this;
}

sbitset_t& sbitset_t::operator^=(const sbitset_t &src) {
	
	if(m_nBits < src.m_nBits){
		
		for(unsigned int i = 0; i < m_nBytes; ++i) {
			
			m_pchBits[i] ^= src.m_pchBits[i];
		}
		
		if(m_nBits % BYTE_SIZE > 0) {
			
			m_pchBits[m_nBytes - 1] &= (0xff >> (BYTE_SIZE - (m_nBits % BYTE_SIZE)));
		}
	}
	else {
		
		for(unsigned int i = 0; i < src.m_nBytes - 1; ++i) {
			
			m_pchBits[i] ^= src.m_pchBits[i];
		}
		
		if(src.m_nBits % BYTE_SIZE > 0) {
			
			m_pchBits[src.m_nBytes - 1] ^=
				((0xff >> (BYTE_SIZE - src.m_nBits % BYTE_SIZE)) & src.m_pchBits[src.m_nBytes - 1]);
		}
		else {
			
			m_pchBits[src.m_nBytes - 1] ^= src.m_pchBits[src.m_nBytes - 1];
		}	
	}
	
	return *this;		
}

bool sbitset_t::operator[](const sbitset_t &src) const {
	
	if(src.m_nBits > m_nBits){
		
		for(unsigned int i = m_nBytes; i < src.m_nBytes; ++i) {
			
			if(src.m_pchBits[i]) {
				
				return false;
			}
		}
	}

		
	for(unsigned int i = 0; i < src.m_nBytes; ++i) {
		
		if(_EQ(0, (m_pchBits[i] & ((unsigned char)~src.m_pchBits[i])))) {
			
			return false;
		}
	}
	
	return true;
}


////////////////////////////////////////////////////////////////////////
// special static bitset: character set
charset_t::charset_t(char a_cLower, char a_cUpper, bool a_bExcluded) {
	
	memset(m_cBytes, 0, BYTES);
	
	for(int c = (int)((unsigned char) a_cLower); c <= (int)((unsigned char) a_cUpper); ++c ) {
		
		set(c);
	}
	
	if(a_bExcluded) {
		
		flip();
	}
}

charset_t::charset_t(const char *a_pch, bool a_bExcluded) {
	
	memset(m_cBytes, 0, BYTES);
	if(a_pch) {
		
		while(*a_pch) {
			
			set((unsigned char) *a_pch);
			++a_pch;
		}
	}
	if(a_bExcluded) {
		
		flip();
	}
}

charset_t::charset_t(const string &a_str, bool a_bExcluded) {
	
	memset(m_cBytes, 0, BYTES);
	for(unsigned int i = 0; i < a_str.size(); ++i) {
		
		set((unsigned char) a_str[i]);
	}
	if(a_bExcluded) {
		
		flip();
	}
}

charset_t::charset_t(posix_char_t a_nType) {
		
	switch(a_nType) {
	case ALNUM:
		setAlnum();
		break;
	case ALPHA:
		setAlpha();
		break;
	case BLANK:
		setBlank();
		break;
	case CNTRL:
		setCntrl();
		break;
	case DIGIT:
		setDigit();
		break;
	case GRAPH:
		setGraph();
		break;
	case LOWER:
		setLower();
		break;
	case PRINT:
		setPrint();
		break;
	case PUNCT:
		setPunct();
		break;
	case SPACE:
		setSpace();
		break;
	case UPPER:
		setUpper();
		break;
	case XDIGIT:
		setXdigit();
		break;
	case CIDF:
		setCIDF();
		break;
	case CIDS:
		setCIDS();
		break;
	default:
		assert(0);
		break;
	}
}

bool charset_t::any(void) const {

	for(unsigned int i = 0; i < BYTES; ++i) {
		
		if(0 != m_cBytes[i]) {
			
			return true;
		}
	}
	return false;
}

unsigned int charset_t::count(void) const {
	
	unsigned int nBits = 0;
	for(unsigned int i = 0; i < BYTES; ++i) {
		
		nBits += (unsigned int)sn_cOnes[m_cBytes[i]];
	}
	return nBits;
}

charset_t& charset_t::flip(void) {
	
	for(unsigned int i = 0; i < BYTES; ++i) {
		
		m_cBytes[i] ^= 0xff;
	}
	return *this;
}

string charset_t::to_string(void) const {
	
	string str;
	
	for(unsigned int i = 0; i < BYTES; ++i) {
		
		for(unsigned int j = 0; j < BYTE_SIZE; ++j) {
			
			if(m_cBytes[i] & sn_cSingleBit[j]) {
				
				str += '1';
			}
			else {
				
				str += '0';
			}
		}
	}
	
	return str;
}

void charset_t::getSetChars(OUT string &vc) const {
	
	vc.resize(count());	
	
	unsigned int i, j = 0;
	for(i = 0; i < 256; ++i) {
		
		if(test(i)) {
			
			vc[j++] = (char)i;
		}
	}
		
	assert(_EQ(j, vc.size()));
}

bool charset_t::operator==(const charset_t &src) const {
	
	for(unsigned int i = 0; i < BYTES; ++i) {
		
		if(m_cBytes[i] != src.m_cBytes[i]) {
			
			return false;
		}
	}
	return true;
}


charset_t& charset_t::operator&=(const charset_t &src) {
	
	for(unsigned int i = 0; i < BYTES; ++i) {
		
		m_cBytes[i] &= src.m_cBytes[i];
	}
	return *this;
}

charset_t& charset_t::operator|=(const charset_t &src) {

	for(unsigned int i = 0; i < BYTES; ++i) {
		
		m_cBytes[i] |= src.m_cBytes[i];
	}
	
	return *this;
}

charset_t& charset_t::operator^=(const charset_t &src) {

	for(unsigned int i = 0; i < BYTES; ++i) {
		
		m_cBytes[i] ^= src.m_cBytes[i];
	}
	return *this;
}

bool charset_t::operator[](const charset_t &src) const {
	
	for(unsigned int i = 0; i < BYTES; ++i) {
		
		if((m_cBytes[i] & src.m_cBytes[i]) != src.m_cBytes[i]) {
			
			return false;
		}
	}
	
	return true;
}

}
