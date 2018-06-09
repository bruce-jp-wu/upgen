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

#ifndef COMMON_USET_H__
#define COMMON_USET_H__
#include <iosfwd>
using std::ostream;
#include <string>
using std::string;
#include <set>
using std::set;
#include <vector>
using std::vector;

#include <cstring>
#include <cstdlib>
#include "./common.h"
#include "./types.h"
#include "./strhelper.h"
#include "./reporter.h"

namespace common_ns{

extern const unsigned char sn_cOnes[256];
extern const unsigned char sn_cSingleBit[8];
extern const unsigned char sn_cFullBits[8];
extern const unsigned char sn_cMissBit[8];

// static bitset class
class sbitset_t {
	
private:
	unsigned char *m_pchBits;
	unsigned int m_nBytes;
	unsigned int m_nBits;

private:
	// calculate bytes that can contain certain number of bits
	inline unsigned int getBytes(unsigned int a_nSize) const {		
		
		return (a_nSize + BYTE_SIZE - 1) / BYTE_SIZE;
	}
public:
	inline sbitset_t(unsigned int sz)
	: m_pchBits(nullptr)
	, m_nBits(sz) {
		
		m_nBytes = getBytes(m_nBits);
		if(_EQ(0, m_nBytes)){
			
			m_nBytes = 1;
		}

		m_pchBits = new unsigned char[m_nBytes + 1];

		memset(m_pchBits, 0, m_nBytes * sizeof(unsigned char));
		m_pchBits[m_nBytes] = 0;
	}
	
	inline ~sbitset_t(void) {
		
		if(m_pchBits) {
			
			delete[] m_pchBits;
		}
	}
	
public:
	// 
	bool any(void) const;
	
	unsigned int count(void) const;
	
	sbitset_t& flip(void);
	
	inline sbitset_t& flip(unsigned int idx) {
		
		if(idx < m_nBits){
			
			m_pchBits[idx / BYTE_SIZE] ^= sn_cSingleBit[idx % BYTE_SIZE];
		}
		
		return *this;
	}
	
	inline bool none(void) const {
		
		return !any();
	}
	
	inline sbitset_t& reset(void) {
		
		memset(m_pchBits, 0, m_nBytes * sizeof(unsigned char));
		
		return *this;
	}
	
	inline sbitset_t& reset(unsigned int idx) {
		
		if(idx < m_nBits){
			
			if( idx < BYTE_SIZE){
				
				m_pchBits[0] &= sn_cMissBit[idx];
			}
			else{
			
				m_pchBits[idx / BYTE_SIZE] &= sn_cMissBit[idx % BYTE_SIZE];
			}
		}
		
		return *this;		
	}
	
	inline sbitset_t& set(void) {
		
		if(m_nBytes > 1) {
		
			memset(m_pchBits, 0xff, sizeof(unsigned char) * (m_nBytes -1));
		}
		
		m_pchBits[m_nBytes - 1] = sn_cFullBits[m_nBits - (m_nBytes - 1) * BYTE_SIZE - 1];
		
		return *this;
	}
	
	inline sbitset_t& set(unsigned int idx) {
		
		if(idx < m_nBits){
			
			if( idx < BYTE_SIZE){
				
				m_pchBits[0] |= sn_cSingleBit[idx];
			}
			else{
			
				m_pchBits[idx / BYTE_SIZE] |= sn_cSingleBit[idx % BYTE_SIZE];
			}
		}
		
		return *this;
	}
	
	inline unsigned int size(void) const {
		
		return m_nBits;
	}
	
	inline bool test(unsigned int idx) const {
		
		if(idx < m_nBits){
			
			if( idx < BYTE_SIZE){
				
				return m_pchBits[0] & sn_cSingleBit[idx];
			}
			else{
			
				return m_pchBits[idx / BYTE_SIZE] & sn_cSingleBit[idx % BYTE_SIZE];
			}
		}
		
		return false;
	}
	
	string to_string(void) const;

	bool operator==(const sbitset_t &src) const;
	
	inline bool operator!=(const sbitset_t &src) const {
		
		return (operator==(src) == false);
	}

	sbitset_t& operator&=(const sbitset_t &src);
	
	sbitset_t& operator|=(const sbitset_t &src);
	
	sbitset_t& operator^=(const sbitset_t &src);
	
	inline sbitset_t& operator~(void){
		
		return flip();
	}
	
	inline bool operator[](unsigned int idx) const {
		
		return test(idx);
	}
	
	bool operator[](const sbitset_t &src) const;
	
	inline friend ostream& operator<<(ostream& os, const sbitset_t& src) {
		
		os << src.to_string();
		
		return os;
	}
};

#ifndef BYTE_SIZE

#define BYTE_SIZE			sizeof(byte_t)

#define BYTES				(256 / BYTE_SIZE)

typedef unsigned char byte_t;

#else

#define BYTES				(256 / BYTE_SIZE)

#endif

////////////////////////////////////////////////////////////////////////
// special static bitset: character set
class charset_t{
	
private:
	
	byte_t m_cBytes[BYTES];

public:
	
	// posix's predefined character set
	// legal letter to be first letter of C identifier
	// legal letter to be letter of C identifier other than the first one
	enum posix_char_t{
		ALNUM,
		ALPHA,
		BLANK,
		CNTRL,
		DIGIT,
		GRAPH,
		LOWER,
		PRINT,
		PUNCT,
		SPACE,
		UPPER,
		XDIGIT,		
		CIDF,
		CIDS
	};
	
private:
	
	inline void setAlnum(void) {

		memset(m_cBytes, 0, 6);
		m_cBytes[6] = 0xff;
		m_cBytes[7] = 0x03;
		
		m_cBytes[8] = 0xfe;
		m_cBytes[9] = 0xff;
		m_cBytes[10] = 0xff;
		m_cBytes[11] = 0x07;
		
		m_cBytes[12] = 0xfe;
		m_cBytes[13] = 0xff;
		m_cBytes[14] = 0xff;
		m_cBytes[15] = 0x07;
		memset(&m_cBytes[16], 0, 16);		
	}
	inline void setAlpha(void) {
	
		memset(m_cBytes, 0, 8);
		m_cBytes[8] = 0xfe;
		m_cBytes[9] = 0xff;
		m_cBytes[10] = 0xff;
		m_cBytes[11] = 0x07;
		
		m_cBytes[12] = 0xfe;
		m_cBytes[13] = 0xff;
		m_cBytes[14] = 0xff;
		m_cBytes[15] = 0x07;
		memset(&m_cBytes[16], 0, 16);	
	}

	inline void setBlank(void) {

		memset(m_cBytes, 0, 32);
		m_cBytes[1] = 0x02;
		m_cBytes[4] = 0x01;
	}
	
	inline void setCntrl(void) {

		memset(m_cBytes, 0xff, 4);
		memset(&m_cBytes[4], 0, 28);
		m_cBytes[15] = 0x80;
	}
	
	inline void setDigit(void) {

		memset(m_cBytes, 0, 32);
		m_cBytes[6] = 0xff;
		m_cBytes[7] = 0x03;
	}
	
	inline void setGraph(void) {

		memset(m_cBytes, 0, 4);
		memset(&m_cBytes[4], 0xff, 12);
		m_cBytes[4] = 0xfe;
		m_cBytes[15] = 0x7f;
		memset(&m_cBytes[16], 0, 16);
	}
	
	inline void setLower(void) {

		memset(m_cBytes, 0, 12);
		m_cBytes[12] = 0xfe;
		m_cBytes[13] = 0xff;
		m_cBytes[14] = 0xff;
		m_cBytes[15] = 0x07;
		memset(&m_cBytes[16], 0, 16);
	}
	
	inline void setPrint(void) {

		memset(m_cBytes, 0, 4);
		memset(&m_cBytes[4], 0xff, 12);
		m_cBytes[15] = 0x7f;
		memset(&m_cBytes[16], 0, 16);
	}
	
	inline void setPunct(void) {

		memset(m_cBytes, 0, 4);
		m_cBytes[4] = 0xfe;
		m_cBytes[5] = 0xff;
		m_cBytes[6] = 0;
		m_cBytes[7] = 0xfc;
		
		m_cBytes[8] = 0x01;
		m_cBytes[9] = 0;
		m_cBytes[10] = 0;
		m_cBytes[11] = 0xf8;
		
		m_cBytes[12] = 0x01;
		m_cBytes[13] = 0;
		m_cBytes[14] = 0;
		m_cBytes[15] = 0x78;
		memset(&m_cBytes[16], 0, 16);
	}
	
	inline void setSpace(void) {

		memset(m_cBytes, 0, 32);
		m_cBytes[1] = 0x3e;
		m_cBytes[4] = 0x01;
	}
	
	inline void setUpper(void) {

		memset(m_cBytes, 0, 8);
		
		m_cBytes[8] = 0xfe;
		m_cBytes[9] = 0xff;
		m_cBytes[10] = 0xff;
		m_cBytes[11] = 0x07;
		
		memset(&m_cBytes[12], 0, 20);
	}
	
	inline void setXdigit(void) {

		memset(m_cBytes, 0, 32);
		m_cBytes[6] = 0xff;
		m_cBytes[7] = 0x03;
		
		m_cBytes[8] = 0x7e;
		m_cBytes[12] = 0x7e;
	}
	
	inline void setCIDF(void) {

		memset(m_cBytes, 0, 8);
		m_cBytes[8] = 0xfe;
		m_cBytes[9] = 0xff;
		m_cBytes[10] = 0xff;
		m_cBytes[11] = 0x87;
		
		m_cBytes[12] = 0xfe;
		m_cBytes[13] = 0xff;
		m_cBytes[14] = 0xff;
		m_cBytes[15] = 0x07;
		memset(&m_cBytes[16], 0, 16);
	}
	
	inline void setCIDS(void) {

		memset(m_cBytes, 0, 6);
		m_cBytes[6] = 0xff;
		m_cBytes[7] = 0x03;
		
		m_cBytes[8] = 0xfe;
		m_cBytes[9] = 0xff;
		m_cBytes[10] = 0xff;
		m_cBytes[11] = 0x87;
		
		m_cBytes[12] = 0xfe;
		m_cBytes[13] = 0xff;
		m_cBytes[14] = 0xff;
		m_cBytes[15] = 0x07;
		memset(&m_cBytes[16], 0, 16);
	}
	
public:
	
	inline charset_t(void) {
		memset(m_cBytes, 0, BYTES);
	}
	inline charset_t(char a_c, bool a_bExcluded = false) {
		
		memset(m_cBytes, 0, BYTES);
		set((unsigned char) a_c);
		
		if(a_bExcluded) {
			
			flip();
		}
	}
	charset_t(char a_cLower, char a_cUpper, bool a_bExcluded = false);	
	charset_t(const char *a_pch, bool a_bExcluded = false);
	charset_t(const string &a_str, bool a_bExcluded = false);	
	charset_t(posix_char_t a_nType);

	inline charset_t(const charset_t &src) {
		memcpy(m_cBytes, src.m_cBytes, BYTES);
	}
	
	inline charset_t& operator=(const charset_t &src) {
		if(_EQ(&src, this)) {
			
			return *this;
		}
		
		memcpy(m_cBytes, src.m_cBytes, BYTES);
		return *this;
	}
	
	inline charset_t* clone(void) {
		
		charset_t *pcs = new charset_t;
		memcpy(pcs->m_cBytes, m_cBytes, BYTES);
		
		return pcs;
	}
	
	bool any(void) const;
	
	unsigned int count(void) const ;
	
	charset_t& flip(void);
	
	inline charset_t& flip(unsigned char idx) {

		m_cBytes[idx / BYTE_SIZE] ^= sn_cSingleBit[idx % BYTE_SIZE];

		return *this;
	}
	
	inline bool none(void) const {
		
		return !any();
	}
	
	inline charset_t& reset(void) {

		memset(m_cBytes, 0, BYTES);
		return *this;
	}
	
	inline charset_t& reset(unsigned char idx) {
		
		m_cBytes[idx / BYTE_SIZE] &= sn_cMissBit[idx % BYTE_SIZE];
		return *this;
	}
	
	inline charset_t& set(void) {

		memset(m_cBytes, 0xff, BYTES);
		return *this;
	}
	
	inline charset_t& set(unsigned char idx) {
		
		m_cBytes[idx / BYTE_SIZE] |= sn_cSingleBit[idx % BYTE_SIZE];
		return *this;
	}
	
	inline bool test(unsigned char idx) const {
		
		return (m_cBytes[idx / BYTE_SIZE] & sn_cSingleBit[idx % BYTE_SIZE]);
	}
	
	string to_string(void) const;
	// get all characters that are set,
	// @vc: out-parameter, a sequence of chars that are set	
	void getSetChars(OUT string &vc) const;

	bool operator==(const charset_t &src) const;
	
	inline bool operator!=(const charset_t &src) const {
		
		return operator==(src) == false;
	}
	
	charset_t& operator&=(const charset_t &src) ;
	
	charset_t& operator|=(const charset_t &src);
	
	charset_t& operator^=(const charset_t &src);
	
	inline charset_t& operator~(void) {
		
		return flip();
	}
	
	inline bool operator[](unsigned char idx) const {
		
		return test(idx);
	}
	
	bool operator[](const charset_t &src) const;
	
	
	inline friend ostream& operator<<(ostream& os, const charset_t& src) {
		
		os << src.to_string();
		
		return os;
	}
};


// index set
class iset_t {

public:
	
	typedef int value_type;
	
	typedef set<value_type> set_t;
	
	
	typedef set_t::iterator iterator;
	typedef set_t::const_iterator const_iterator;
	
public:
	iset_t(void){}
	iset_t(const iset_t &src) {
		
		merge(src);
	}
	
	inline const_iterator begin(void) const {
		
		return m_iSet.begin();
	}
	inline iterator begin(void) {
		
		return m_iSet.begin();
	}
	
	inline const_iterator end(void) const {
		
		return m_iSet.end();
	}
	inline iterator end(void) {
		
		return m_iSet.end();
	}
	
	inline bool insert(value_type idx) {
		
		return (m_iSet.insert(idx)).second;
		
	}
	
	inline bool erase(value_type e) {
		return (m_iSet.erase(e) > 0);
	}
	
	inline iset_t& merge(const iset_t& src) {
		
		m_iSet.insert(src.begin(), src.end());
		
		return *this;
	}
	
	inline bool empty(void) const {
		return m_iSet.empty();
	}
	
	inline unsigned int size(void) const {
		
		return m_iSet.size();
	}
	
	inline void clear(void) {
		
		m_iSet.clear();
	}
	
	inline bool operator==(const iset_t& src) const {

		return this->m_iSet == src.m_iSet; 
	}
	
	inline bool operator<(const iset_t& src) const {

		return this->m_iSet < src.m_iSet;
	}
	
	inline iset_t& operator=(const iset_t& src) {
		
		if(_EQ(&src, this)) {
			
			return *this;
		}
		clear();
		merge(src);
		
		return *this;
	}
	
	inline bool include(value_type idx) const {
		
		return m_iSet.find(idx) != m_iSet.end();
	}
	
private:
	
	set_t m_iSet;
};

typedef vector< iset_t* > viset_t;

}
#endif // COMMON_USET_H__

