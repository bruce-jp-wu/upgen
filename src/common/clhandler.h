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

#ifndef COMMON_CMDLINE_HANDLER_H__
#define COMMON_CMDLINE_HANDLER_H__

#include <cstring>
#include "types.h"

namespace common_ns {

// cl_handler_t: commandline processor

class cl_handler_t {
	
private:
	
	// suppose only digit or letter to be valid option
	enum {
		A_0 = 0,
		A_1,	A_2,	A_3,	A_4,	A_5,	A_6,	A_7,	A_8,
		A_9,	A_a,	A_b,	A_c,	A_d,	A_e,	A_f,	A_g,
		A_h,	A_i,	A_j,	A_k,	A_l,	A_m,	A_n,	A_o,
		A_p,	A_q,	A_r,	A_s,	A_t,	A_u,	A_v,	A_w,
		A_x,	A_y,	A_z,	A_A,	A_B,	A_C,	A_D,	A_E,
		A_F,	A_G,	A_H,	A_I,	A_J,	A_K,	A_L,	A_M,
		A_N,	A_O,	A_P,	A_Q,	A_R,	A_S,	A_T,	A_U,
		A_V,	A_W,	A_X,	A_Y,	A_Z,	A_QM,
		NUM_OF_OPT
	};
	
private:
	// argument clung to option; that is, an option can be attached at most one argument
	string m_strOptArg[NUM_OF_OPT];
	// array of present flag, indicating if an option appears in commandline or not
	bool m_bSet[NUM_OF_OPT];
	// arguments not clung to option
	vstr_t m_vstrNPArg;

private:
	
	// map: value of char(in ASCII) to entry index to @m_strOptArg(also @m_bSet)
	inline int chr2Idx(char c) const {
		
        int idx = NUM_OF_OPT;
        if(c >= 65 && c <= 90) {
        	
            idx = c - 55;
        }
        else if(c >= 97 && c <= 122) {
        	
            idx = c - 61;
        }
        else if(c == '?') {
        	
        	idx = A_QM;
        }
        else if(c >= 48 && c <= 57) {
        
        	idx = c - 48;
        }

        return idx;
	}
	
	// set option without argument @c is present
	inline bool addOption(char c) {
		
		int idx = chr2Idx(c);
        
        if(idx >= NUM_OF_OPT || m_bSet[idx]) {
        	
        	return false;
        }
        
        m_bSet[idx] = true;
        return true;
	}
	
	// set option with an argument @c is present
	inline bool addOption(char c, const char *pchArg) {
		
		int idx = chr2Idx(c);
        
        if(idx >= NUM_OF_OPT || m_bSet[idx]) {
        	
        	return false;
        }
        
        m_bSet[idx] = true;
        if(pchArg) {
        	
        	m_strOptArg[idx] = string(pchArg);
        }
        return true;
	}
	
	// set option with an argument @c is present
	inline bool addOption(char c, const string& strArg) {
		
		int idx = chr2Idx(c);
		if(idx >= NUM_OF_OPT || m_bSet[idx]) {
		
			return false;
		}
		
		m_bSet[idx] = false;
		m_strOptArg[idx] = strArg;
		return true;
	}
	
	// add non-option argument
	inline void addNonOpt(const char *pchArg) {
		
		m_vstrNPArg.push_back(string(pchArg));
	}
	// add non-option argument
	inline void addNoOpt(const string &strArg) {
		
		m_vstrNPArg.push_back(strArg);
	}
	
public:
	
	// process commandline
	bool process(int argc, char **argv, const string &strLegalOpt, const string &strHasArg);
		
public:
	
	inline cl_handler_t(void) {
	}
	inline ~cl_handler_t(void) {}
	
	inline void clear(void) {
		
		memset(m_bSet, 0, sizeof(m_bSet));
		m_vstrNPArg.clear();
	}
	
	inline bool hasOption(char c) const {
		
		int idx = chr2Idx(c);
        
        return idx < NUM_OF_OPT && m_bSet[idx];
	}
	
	inline bool getOption(char c, string& strOption) const {
		
		int idx = chr2Idx(c);
        
        if(idx >= NUM_OF_OPT || !m_bSet[idx]) {
        	
        	return false;
        }
        strOption = m_strOptArg[idx];
        return true;
	}
	
	inline vstr_t& getNPArgs(void) {
		
		return m_vstrNPArg;
	}
	
	inline const vstr_t& getNPArgs(void) const {
		
		return m_vstrNPArg;
	}
};

}
#endif // COMMON_CMDLINE_HANDLER_H__
