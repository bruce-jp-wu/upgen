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

#ifndef COMMON_ACTION_H__
#define COMMON_ACTION_H__

#include <vector>
using std::vector;
#include <cassert>
#include "./types.h"

namespace common_ns {

// class action_t: user-action class.
// user-action is a piece of code in target program language(e.g. C/C++, Java, Pacal, etc.).
// an action_t object includes code blocks. a code block can be
//		1) plain text
//		2) macro name
//		3) macro parameter
// sometimes, the contents of a block can not be decided until after certain amount of time,
// we simply hold the place for the block, which is called a placeholder.
class action_t {
public:
	// type of block
	enum {
		// plain text block: will be copyied literally to generated program
		// during code generation
		BLT_TEXT = 0,
		// macro name: macro name together with macro parameters(if has any)
		// will be expanded(replaced) by macro body, and copied to generated program
		// during code generation
		BLT_MACRO = 1,
		// macro parameter
		BLT_PARAM = 2,
	};
	
private:
	
	// code blocks: 
	vstr_t m_vsBlock;
	// 
	vint_t m_vnType;
	// next placehole
	int m_nNextPH;
	int m_nLineno;
	int m_nLines;
	
private:

	// negative-valued type means the block is a place-holder, we have to defer filling its contents
	inline int getType(int idx) const {
		assert(idx >= 0 && idx < (int)m_vnType.size());
		return m_vnType[idx] >= 0 ? 
				m_vnType[idx] :
				(-m_vnType[idx]) % 3;
	}
	
public:
	
	inline action_t(int a_nLine = 0)
	: m_nNextPH(0)
	, m_nLineno(a_nLine)
	, m_nLines(0) {
	}
	action_t(const action_t &src)
	: m_nNextPH(src.m_nNextPH)
	, m_nLineno(src.m_nLineno)
	, m_nLines(src.m_nLines) {
		for(int i = 0; i < (int)src.m_vsBlock.size(); ++i) {
			m_vsBlock.push_back(src.m_vsBlock[i]);
			m_vnType.push_back(src.m_vnType[i]);
		}
	}
	
	inline void clear(void) {
		
		m_vsBlock.clear();
		m_vnType.clear();
		m_nNextPH = 0;
		m_nLineno = 0;
		m_nLines = 0;
	}
	
	// add char as plain text
	inline void addText(char c) {
		
		if(m_vsBlock.size() > 0 && m_vnType[m_vnType.size() - 1] == BLT_TEXT) {
			// append text to last added block
			m_vsBlock[m_vsBlock.size() - 1] += c;
		}
		else {
			// add new block
			m_vsBlock.push_back("");
			m_vsBlock[m_vsBlock.size() - 1] += c;
			m_vnType.push_back(BLT_TEXT);
		}
		
		// keep track of number of lines
		if(c == '\n') {
			++m_nLines;
		}
	}
	
	// add string as plain text
	void addText(const string &a_strText) {
		
		if(m_vsBlock.size() > 0 && m_vnType[m_vnType.size() - 1] == BLT_TEXT) {
			// add text to last added block
			m_vsBlock[m_vsBlock.size() - 1] += a_strText;
		}
		else {
			// add new block
			m_vsBlock.push_back(a_strText);
			m_vnType.push_back(BLT_TEXT);
		}
		
		// keep track of number of lines in action
		for(string::size_type i = 0; i < a_strText.size(); ++i) {
			if(a_strText[i] == '\n') {
				++m_nLines;
			}
		}
	}
	// add macro name without parameter
	inline void addMacro(const string &a_strMacro) {
		
		m_vsBlock.push_back(a_strMacro);
		m_vnType.push_back(BLT_MACRO);
	}
	// add macro name with one parameter
	inline void addMacro(const string &a_strMacro, const string &a_strParam) {
		
		m_vsBlock.push_back(a_strMacro);
		m_vnType.push_back(BLT_MACRO);
		m_vsBlock.push_back(a_strParam);
		m_vnType.push_back(BLT_PARAM);
	}
	// add macro name with two parameter
	inline void addMacro(const string &a_strMacro, const string &a_strParam1, const string &a_strParam2) {
		
		m_vsBlock.push_back(a_strMacro);
		m_vnType.push_back(BLT_MACRO);
		m_vsBlock.push_back(a_strParam1);
		m_vnType.push_back(BLT_PARAM);
		m_vsBlock.push_back(a_strParam2);
		m_vnType.push_back(BLT_PARAM);
	}
	
	// add placeholder macro: macro name is known, but its parameter remain unknown
	inline void addMacroPH(const string &a_strMacro) {
		
		m_vsBlock.push_back(a_strMacro);
		m_vnType.push_back(BLT_MACRO);
		m_vsBlock.push_back("");
		m_vnType.push_back(BLT_PARAM);
		
		m_vnType[m_nNextPH] = 0 - (m_vnType[m_nNextPH] + (int)(m_vnType.size() - 1) * 3);
		++m_nNextPH;
	}
	
	// get contents of a placeholder
	inline bool getMacroPH(int a_nPH, string &a_strParam) {
		
		if(a_nPH < 0 || a_nPH >= m_nNextPH) {
			return false;
		}
		assert(m_vnType[a_nPH] < 0);
		int n = (-m_vnType[a_nPH]) / 3;
		assert(n < (int)m_vsBlock.size() && getType(n) == BLT_PARAM);
		a_strParam = m_vsBlock[n];
		
		return true;
	}
	
	// update contents of a placeholder
	inline bool updateMacroPH(int a_nPH, const string &a_strParam) {
		
		if(a_nPH < 0 || a_nPH >= m_nNextPH) {
			return false;
		}
		assert(m_vnType[a_nPH] < 0);
		int n = (-m_vnType[a_nPH]) / 3;
		assert(n < (int)m_vsBlock.size() && getType(n) == BLT_PARAM);
		m_vsBlock[n] = a_strParam;
		
		return true;
	}
	
	// get a block of action; the block returned in pair of contents:type
	inline bool get(int idx, string &a_strCont, int &a_nType) const {
		
		if(idx >= 0 && idx < (int)m_vsBlock.size()) {
			
			a_strCont = m_vsBlock[idx];
			if(m_vnType[idx] >= 0) {
				a_nType = m_vnType[idx];
			}
			else {
				a_nType = (-m_vnType[idx]) % 3;
			}
			
			return true;
		}
		return false;
	}
	
	inline int size(void) const {
		return (int)m_vsBlock.size();
	}

	inline int getPHCount(void) const {
		return m_nNextPH;
	}
	
	inline int getLineno(void) const {
		return m_nLineno;
	}
	
	inline int getLineCount(void) const {
		return m_nLines;
	}
};

typedef vector < action_t* > vaction_t;

}
#endif //COMMON_ACTION_H__
