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

#ifndef COMMON_STRHELPER_H__
#define COMMON_STRHELPER_H__

#include <string>
using std::string;

#define MAX_UNSIGNED_LONG 0xFFFFFFFF
#define MAX_SIGNED_LONG   0x7FFFFFFF

#define MAY_DEC_LONG      429496729
#define MAY_OCT_LONG      0x20000000
#define MAY_HEX_LONG      0x10000000

#define SMAY_DEC_LONG     214748364
#define SMAY_OCT_LONG     0x10000000
#define SMAY_HEX_LONG     0x08000000

namespace common_ns {

enum {
	oct_num = 8,
	dec_num = 10,
	hex_num = 16,
};

// helper class for string
// utility functions for string which are widely used in applications
class strhelper_t {
	
public:

	inline static bool isDec(char a_c) {
		
		if(a_c >= '0' && a_c <= '9') {
			
			return true;
		}
		
		return false;
	}
	
	inline static bool isHex(char a_c) {
		
		if((a_c >= '0' && a_c <= '9')
				|| (a_c >= 'A' && a_c <= 'F')
				|| (a_c >= 'a' && a_c <= 'a')) {
			return true;
		}
		
		return false;
	}
	
	inline static bool isFOct(char a_c) {
		
		if(a_c >= '0' && a_c <= '3') {
			
			return true;
		}
		
		return false;
	}
	
	inline static bool isSOct(char a_c) {
		
		if(a_c >= '0' && a_c <= '7') {
			
			return true;
		}
		
		return false;		
	}
	
	// convert string(in hexidecimal form: \XHH) to short number
    static int hstr2Num(const char *a_pch, int *pcount = nullptr);
	// convert string(in octal form: \OOO) to short number
    static int ostr2Num(const char *a_pch, int *pcount = nullptr);
	// transform to escape char
	// for example, "\\t" will be '\t'
	static char toEsc(const char* a_pch);
	// format escaped char
	static string quotedChar(char a_c);
	static bool ustr2Str(const char *a_pch, string &strRet);
	
public:

	// decide if a char is a blank
	inline static bool isBlank(char c) {
		
		return ' ' == c || '\n' == c || '\r' == c || '\t' == c;
	}
	
	// decide if a char is a user-defined blank
	static bool isBlank(char c, const string& str);

public:
	
	// eliminate leading blanks from a string
	static void trimLeft(string &str);
	static char* trimLeft(char *pch);	
	
	// eliminate leading user-defined blanks from a string
	static void trimLeft(string &str, const string &blanks);
	static char* trimLeft(char *pch, const string &blanks);

	// eliminate trailing blanks from a string
	static void trimRight(string &str);
	static char* trimRight(char *pch);

	// eliminate trailing user-defined blanks from a string
	static void trimRight(string &str, const string &blanks);
	static char* trimRight(char *pch, const string &blanks);
	
	
	// eliminate leading and trailing blanks from a string
	inline static void trim(string &str) {
		
		trimLeft(str);
		trimRight(str);
	}
	// eliminate leading and trailing blanks from a string
	inline static char* trim(char *pch) {
		
		if(!pch || !pch[0]) {
			
			return pch;
		}
		
		return trimRight(trimLeft(pch));
	}

	// eliminate leading and trailing user-defined blanks from a string
	inline static void trim(string &str, const string &blanks) {
		
		trimLeft(str, blanks);
		trimRight(str, blanks);
	}

	inline static char* trim(char *pch, const string &blanks) {
		
		if(!pch || !pch[0]) {
			
			return pch;
		}
		
		return trimRight(trimLeft(pch, blanks), blanks);
	}
	
	// convert to string of upper case
	static string toUpper(const string &str);
	// convert to string of lower case
	static string toLower(const string &str);

	// decide if a string is an unsigned number
	static bool isNum(const string &str, int radix = dec_num);

	// decide if a string is a signed number
	inline static bool isSnum(const string &str, int radix) {
		
		if (str.empty()) {
			
			return false;
		}
		
		if (str[0] == '+' || str[0] == '-') {
			
			return isNum(str.substr(1, str.size() - 1), radix);
		}
		
		return isNum(str, radix);
	}

	// try to convert string to unsigned number
	// return true if conversion is successful
	// following reason maybe cause failure of conversion
	// . string contains illegal char
	// . overflow
	// . empty string
	// . invalid radix, all but 10-based, 8-based, 16-based number are illegal
	// NOTE: overflow is not considered as error
	static bool toNum(const string &str, unsigned int &num, int radix = dec_num);

	// try to convert string to signed number
	// return true if conversion is successful
	// following reason maybe cause failure of conversion
	// . string contains illegal char
	// . overflow
	// . empty string
	// . invalid radix, all but 10-based, 8-based, 16-based number are illegal
	// NOTE: overflow is not considered as error
	static bool toSnum(const string &str, int &num, int radix = dec_num);
	
	// convert from integer to string
	static string fromInt(int num);
	// convert from unsigned integer to string
	static string fromUint(unsigned int num, int radix = dec_num);
};

}

#endif // COMMON_STRHELPER_H__
