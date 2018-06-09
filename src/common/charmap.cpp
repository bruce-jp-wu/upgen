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

#include "./charmap.h"

namespace common_ns {

/*
const unsigned char sn_cAnsiMap[256] = {
	0,	130,	131,	132,	133,	134,	135,	136,
	137,	138,	139,	140,	141,	142,	143,	144,
	145,	146,	147,	148,	149,	150,	151,	152,
	153,	154,	155,	156,	157,	158,	159,	160,
	161,	162,	163,	164,	165,	166,	167,	168,
	169,	170,	171,	172,	173,	174,	175,	176,
	177,	178,	179,	180,	181,	182,	183,	184,
	185,	186,	187,	188,	189,	190,	191,	192,
	193,	194,	195,	196,	197,	198,	199,	200,
	201,	202,	203,	204,	205,	206,	207,	208,
	209,	210,	211,	212,	213,	214,	215,	216,
	217,	218,	219,	220,	221,	222,	223,	224,
	225,	226,	227,	228,	229,	230,	231,	232,
	233,	234,	235,	236,	237,	238,	239,	240,
	241,	242,	243,	244,	245,	246,	247,	248,	
	249,	250,	251,	252,	253,	254,	255,	129,
	1,	2,	3,	4,	5,	6,	7,	8,
	9,	10,	11,	12,	13,	14,	15,	16,
	17,	18,	19,	20,	21,	22,	23,	24,
	25,	26,	27,	28,	29,	30,	31,	32,
	33,	34,	35,	36,	37,	38,	39,	40,
	41,	42,	43,	44,	45,	46,	47,	48,
	49,	50,	51,	52,	53,	54,	55,	56,
	57,	58,	59,	60,	61,	62,	63,	64,
	65,	66,	67,	68,	69,	70,	71,	72,
	73,	74,	75,	76,	77,	78,	79,	80,
	81,	82,	83,	84,	85,	86,	87,	88,
	89,	90,	91,	92,	93,	94,	95,	96,
	97,	98,	99,	100,	101,	102,	103,	104,
	105,	106,	107,	108,	109,	110,	111,	112,
	113,	114,	115,	116,	117,	118,	119,	120,
	121,	122,	123,	124,	125,	126,	127,	128
};

const unsigned char sn_cAnsiInverseMap[256] = {

	0,	128,	129,	130,	131,	132,	133,	134,
	135,	136,	137,	138,	139,	140,	141,	142,
	143,	144,	145,	146,	147,	148,	149,	150,
	151,	152,	153,	154,	155,	156,	157,	158,
	159,	160,	161,	162,	163,	164,	165,	166,
	167,	168,	169,	170,	171,	172,	173,	174,
	175,	176,	177,	178,	179,	180,	181,	182,
	183,	184,	185,	186,	187,	188,	189,	190,
	191,	192,	193,	194,	195,	196,	197,	198,
	199,	200,	201,	202,	203,	204,	205,	206,
	207,	208,	209,	210,	211,	212,	213,	214,
	215,	216,	217,	218,	219,	220,	221,	222,
	223,	224,	225,	226,	227,	228,	229,	230,
	231,	232,	233,	234,	235,	236,	237,	238,
	239,	240,	241,	242,	243,	244,	245,	246,
	247,	248,	249,	250,	251,	252,	253,	254,
	255,	127,	1,	2,	3,	4,	5,	6,
	7,	8,	9,	10,	11,	12,	13,	14,
	15,	16,	17,	18,	19,	20,	21,	22,
	23,	24,	25,	26,	27,	28,	29,	30,
	31,	32,	33,	34,	35,	36,	37,	38,
	39,	40,	41,	42,	43,	44,	45,	46,
	47,	48,	49,	50,	51,	52,	53,	54,
	55,	56,	57,	58,	59,	60,	61,	62,
	63,	64,	65,	66,	67,	68,	69,	70,
	71,	72,	73,	74,	75,	76,	77,	78,
	79,	80,	81,	82,	83,	84,	85,	86,
	87,	88,	89,	90,	91,	92,	93,	94,
	95,	96,	97,	98,	99,	100,	101,	102,
	103,	104,	105,	106,	107,	108,	109,	110,
	111,	112,	113,	114,	115,	116,	117,	118,
	119,	120,	121,	122,	123,	124,	125,	126
};
*/

// identify a single-char group, this may cause group division
void charmap_t::addGroup(unsigned char a_cGrp) {
	
	// set used-flag
	m_csUsed.set(a_cGrp);
	if(_EQ(m_cParent[a_cGrp], a_cGrp)) {
		// it is representive
		
		// search new reprensative 
		int i;
		unsigned char newRep = 0;
		for(i = ((int)a_cGrp) + 1; i < 256; ++i) {
			
			if(m_csUsed.test((unsigned char)i) && _EQ(m_cParent[i], a_cGrp)) {
				
				newRep = (unsigned char)i;
				m_cParent[i] = newRep;
				break;
			}
		}
		
		// update others' parent in the group
		for(; i < 256; ++i) {
			
			if(m_csUsed.test((unsigned char)i) && _EQ(m_cParent[i], a_cGrp)) {
				
				assert(newRep != 0);
				m_cParent[i] = newRep;
			}
		}
	}
	else {
		
		m_cParent[a_cGrp] = a_cGrp;
	}
}

// identify groups. it doesn't meam that chars in @a_ustrGrp are in the same group;
// it means that, if chars in @a_ustrGrp are in same group before operation, then
// they are in the same group after operation
void charmap_t::addGroup(const ustring &a_ustrGrp) {
		
	if(_EQ(0, a_ustrGrp.size())) {
		
		return;
	}
	int i = 0;
	
	_node_t *pn = new _node_t[a_ustrGrp.size() + 1];
	
	charset_t csMark;
	for(i = 0; i < (int)a_ustrGrp.size(); ++i) {
		
		pn[i + 1].c = (unsigned char)a_ustrGrp[i];
		m_csUsed.set(pn[i + 1].c);
		csMark.set(pn[i + 1].c);
	}
	csMark.flip();
	
	
	// sort array
	// small array, insertion sorting works well
	for(i = 2; i <= (int)a_ustrGrp.size(); ++i) {
		
		pn[0].c = pn[i].c;
		int j = i - 1;
		
		for(; pn[0].c < pn[j].c ; --j) {
			
			pn[j + 1].c = pn[j].c;
		}
		
		pn[j + 1].c = pn[0].c;
	}
	
	// chain array to be list
	for(i = 1; i < (int)a_ustrGrp.size(); ++i) {
		
		pn[i].p = &pn[i + 1];
	}
	pn[a_ustrGrp.size()].p = nullptr;
	
	// divide groups by adding a new group
	_node_t *p = &pn[1];	
	while(p) {
		
		_node_t *q = p->p;
		_node_t *r = p;
		
		if(_EQ(p->c, m_cParent[p->c])) {
			// it is representative of its group,
			// that is smallest index of the group
			
			// divide group into two, one is represented by p->c,
			// the other need to choose new representative
			
			// leave the first group unchanged
			// filter elements of the same group as p->c
			while(q) {
				
				if(_EQ(p->c, m_cParent[q->c])) {
					
					// remove q from list
					r->p = q->p;
				}
				else {
					
					r = q;
				}
				q = r->p;
			}
			
			// the other group change its representative
			unsigned char newRep = 0;
			for(i = ((int)p->c) + 1; i < 256; ++i) {
				
				if(m_csUsed.test((unsigned char)i)
						&& csMark.test((unsigned char)i)
						&& _EQ(m_cParent[i], p->c)) {
					
					newRep = (unsigned char)i;
					m_cParent[i] = newRep;
					break;
				}
			}
			for(; i < 256; ++i) {
				
				if(m_csUsed.test((unsigned char)i)
						&& csMark.test((unsigned char)i)
						&& _EQ(m_cParent[i], p->c)) {
					
					assert(newRep != 0);
					m_cParent[i] = newRep;
				}
			}
		}
		else {
			
			// divide group of p->c into 2 subgroups
			// p->c will be new representative of its group, 
			// the other group keep unchanged
			unsigned char cp = m_cParent[p->c];
			m_cParent[p->c] = p->c;
			
			while(q) {
				
				if(_EQ(cp, m_cParent[q->c])) {
					
					m_cParent[q->c] = p->c;
					
					// remove q from list
					r->p = q->p;
					q = r->p;
				}
				else {
				
					r = q;
					q = q->p;
				}
			}
		}
		
		p = p->p;
	}
	
	delete[] pn;
}

// get equivalent all groups in charmap
void charmap_t::getGroups(vustr_t &a_vustr) {

	a_vustr.clear();
	vustr_t vus(256);
	ustring ustr;
	
	int i = 0;
	for(; i < 256; ++i) {
		
		if(m_csUsed.test((unsigned char)i)) {
		
			vus[m_cParent[i]].push_back((uchar)i);
		}
		else {
			
			ustr.push_back((uchar)i);
		}
	}
	for(i = 0; i < 256; ++i) {
		
		if(!vus[i].empty()) {
			
			a_vustr.push_back(vus[i]);
		}
	}
	if(!ustr.empty()) {
		
		a_vustr.push_back(ustr);
	}
}

void charmap_t::clear(void) {

	m_csUsed.reset();
	memset(m_cParent, 0, sizeof(unsigned char) * 256);	
}

}
