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

#include <cstring>
#include "./strhelper.h"
#include "./reporter.h"

namespace common_ns {

bool strhelper_t::isBlank(char c, const string& str) {
	
	for (int i = 0; i < (int)str.size(); ++i) {
	
		if (str[i] == c) {
			
			return true;
		}
	}
	return false;
}

// convert to string of uppercase
string strhelper_t::toUpper(const string &str) {
	
	string strRet(str);
	
	for(string::size_type i = 0; i < str.size(); ++i) {
		
		if(str[i] >= 'a' && str[i] <= 'z') {
			strRet[i] -= 32;
		}
	}
	
	return strRet;
}
// convert to string of lowercase
string strhelper_t::toLower(const string &str) {
	
	string strRet(str);
	
	for(string::size_type i = 0; i < str.size(); ++i) {
		
		if(str[i] >= 'A' && str[i] <= 'Z') {
			strRet[i] += 32;
		}
	}
	
	return strRet;
}


// convert \xHH string to number
short strhelper_t::hstr2Num(const char *a_pch) {
		
	assert(a_pch && strlen(a_pch) >= 4);
	short n;
			
	if(a_pch[2] >= '0' && a_pch[2] <= '9') {
		
		n = ((short)(a_pch[2] - '0')) * 16;
	}
	else if(a_pch[2] >= 'A' && a_pch[2] <= 'F') {
	
		n = (((short)(a_pch[2] - 'A')) + 10) * 16;
	}
	else {
	
		n = (((short)(a_pch[2] - 'a')) + 10) * 16;
	}
	
	if(a_pch[3] >= '0' && a_pch[3] <= '9') {
		
		n += (short)(a_pch[3] - '0');
	}
	else if(a_pch[3] >= 'A' && a_pch[3] <= 'F') {
	
		n += ((short)(a_pch[3] - 'A')) + 10;
	}
	else {
	
		n += ((short)(a_pch[3] - 'a')) + 10;
	}
	
	return n;
}
	
// convert \ooo string to number
short strhelper_t::ostr2Num(const char *a_pch) {
	
	assert(a_pch && strlen(a_pch) >= 4);
	
	short n = ((short)(a_pch[1] - '0')) * 64;
	n += ((short)(a_pch[2] - '0')) * 8;
	n += ((short)(a_pch[3] - '0'));
	
	return n;
}
	
// transform to escape char
// for example, "\\t" will be '\t'
char strhelper_t::toEsc(const char* a_pch)
{
    assert(a_pch && _EQ('\\', a_pch[0]));

    switch(a_pch[1])
	{
	case '0':
		return '\0';
	case 'a':
		return '\a';
	case 'b':
		return '\b';
	case 'f':
		return '\f';
	case 'n':
		return '\n';
	case 'r':
		return '\r';
	case 't':
		return '\t';
	case 'v':
		return '\v';
	case '?':
		return '\?';
	default:
		;
	}
    
    return a_pch[1];
}
	    
// format escaped char
string strhelper_t::quotedChar(char a_c) {
	
    switch(a_c)
	{
	case '\0':
        return string("\'\\0\'");
	case '\a':
		return string("\'\\a\'");
	case '\b':
		return string("\'\\b\'");
	case '\f':
		return string("\'\\f\'");
	case '\n':
		return string("\'\\n\'");
	case '\r':
		return string("\'\\r\'");
	case '\t':
		return string("\'\\t\'");
	case '\v':
		return string("\'\\v\'");
	case '\'':
		return string("\'\\\'\'");
	case '\"':
		return string("\'\\\"\'");
	case '?':
		return string("\'\\?\'");
	case '\\':
		return string("\'\\\\\'");
	default:
		;
	}
    
    string str = "\'";
    str += a_c;
    str += '\'';
    
    return str;
}

// convert string with escaped char(including \X) to string without escaped char
bool strhelper_t::ustr2Str(const char *a_pch, string &strRet) {
	
	strRet = "";
	
	if(!a_pch) {
		return true;
	}
	
	const char *p = a_pch;
	while(*p) {
		
		if(_EQ('\\', *p)) {
			
			++p;
			if(*p) {
				
				if((_EQ('x', *p) || _EQ('X', *p)) && (*(p + 1) && isHex(*(p + 1)))
						&& (*(p + 2) && isHex(*(p + 2)))) {
					
					strRet += (char)hstr2Num(p - 1);
					 p += 3;
				}
				else if(isFOct(*p) && (*(p + 1) && isSOct(*(p + 1)))
						&& (*(p + 2) && isSOct(*(p + 2)))){
					
					strRet += (char)ostr2Num(p - 1);
					p += 3;
				}
				else {
					
					strRet += toEsc(p - 1);
					++p;
				}
			}
			else {
				
				return false;
			}
		}
		else {
			
			strRet += *p;
			++p;
		}
	}
	
	return true;
}

	// eliminate leading blanks from a string
void strhelper_t::trimLeft(string &str) {

	unsigned int i = 0;
	while (i < str.size() && isBlank(str[i])) ++i;
	
	if (str.size() > i) {
	
		if(i > 0){
			
			str = str.substr(i, str.size() - i);
		}	
	}
	else {
		
		str = "";
	}
}

char* strhelper_t::trimLeft(char *pch) {

	if(!pch) {
		
		return pch;
	}
	
	char *p = pch;
	while (isBlank(*p)) ++p;
	
	return p;
}	

// eliminate leading user-defined blanks from a string
void strhelper_t::trimLeft(string &str, const string &blanks) {
	
	int i = 0;
	while (i < (int)str.size() && isBlank(str[i], blanks)) ++i;
	if ((int)str.size() > i){
		
		if(i > 0) {
		
			str = str.substr((string::size_type)i, str.size() - (string::size_type)i);
		}
	}
	else {
		
		str = "";
	}			
}
// eliminate leading user-defined blanks from a string
char* strhelper_t::trimLeft(char *pch, const string &blanks) {
	
	if(!pch) {
		
		return pch;
	}
	
	char *p = pch;
	while (*p && isBlank(*p, blanks)) ++p;

	return p;
}

// eliminate trailing blanks from a string
void strhelper_t::trimRight(string &str) {

	int i = ((int)str.size()) - 1;
	while (i >= 0 && isBlank(str[i])) --i;
	
	if (i >= 0){
		
		if(i < ((int)str.size()) - 1){
		
			str = str.substr(0, i + 1);
		}
	}
	else {
		
		str = "";
	}
		
}
// eliminate trailing blanks from a string
char* strhelper_t::trimRight(char *pch) {
	
	if(!pch) {
		
		return pch;
	}
	
	char *p = pch, *q = pch;
	
	while(*p) {
		
		if(!isBlank(*p)) {
			
			q = ++p;
		}
		else {
			
			++p;
		}
	}
	
	*q = '\0';
	
	return pch;
}

// eliminate trailing user-defined blanks from a string
void strhelper_t::trimRight(string &str, const string &blanks) {

	int i = ((int)str.size()) - 1;
	while (i >= 0 && isBlank(str[i], blanks)) --i;
	if (i >= 0) {
		
		if( i < ((int)str.size()) - 1){
		
			str = str.substr(0, ((string::size_type)i) + 1);
		}
	}
	else {
		
		str = "";
	}	
}
// eliminate trailing user-defined blanks from a string
char* strhelper_t::trimRight(char *pch, const string &blanks) {
	
	if(!pch) {
		
		return pch;
	}
	
	char *p = pch, *q = pch;
	
	while(*p) {
		
		if(!isBlank(*p, blanks)) {
			
			q = ++p;
		}
		else {
			
			++p;
		}
	}
	
	*q = '\0';
	
	return pch;
}

// decide if a string is an unsigned number
bool strhelper_t::isNum(const string &str, int radix) {
	
	for (int i = 0; i < (int)str.size(); ++i) {
		
		switch (radix) {
		
		case dec_num:
			
			if (str[i] < '0' || str[i] > '9')
				return false;
			break;
			
		case oct_num:
			
			if (str[i] < '0' || str[i] > '7')
				return false;
			break;
			
		case hex_num:
			
			if (str[i] < '0' || (str[i] > '9' && str[i] < 'A') || (str[i]
					> 'F' && str[i] < 'a') || str[i] > 'f')
				return false;
			break;
			
		default:
			
			return false;
		}// switch

	}// for
	
	return str.size() > 0;
}// isnum

// try to convert string to unsiged number
// return true if conversion is successful
// following reason maybe cause failure of conversion
// . string contains illegal char
// . overflow
// . empty string
// . invalid radix, all but 10-based, 8-based, 16-based number are illegal
// NOTE: overflow is not considered as error
bool strhelper_t::toNum(const string &str, unsigned int &num, int radix) {
	
	if (str.size() == 0) {
	
		return false;
	}

	num = 0;
	for (int i = 0; i < (int)str.size(); ++i) {
		
		switch (radix) {
		
		case dec_num:
			if (str[i] < '0' || str[i] > '9' || num > MAY_DEC_LONG) {
				
				return false;
			}
			
			num = (num << 3) + (num << 1) + ((unsigned int)(str[i] - '0'));
			break;

		case oct_num:
			if (str[i] < '0' || str[i] > '7' || num > MAY_OCT_LONG) {
				
				return false;
			}
			
			num = (num << 3) + ((unsigned int)(str[i] - '0'));
			break;

		case hex_num:
			
			if (num > MAY_HEX_LONG){
				
				return false;
			}
			
			if (str[i] >= '0' && str[i] <= '9') {
			
				num = (num << 4) + ((unsigned int)(str[i] - '9'));
			}
			else if (str[i] >= 'A' && str[i] <= 'F') {
			
				num = (num << 4) + (9 + ((unsigned int)(str[i] - 'A')));
			}
			else if (str[i] >= 'a' && str[i] <= 'f') {
			
				num = (num << 4) + (9 + ((unsigned int)(str[i] - 'a')));
			}
			break;
			
		default:
			return false;
		}// switch

	}// for
	
	return true;
}

// try to convert string to signed number
// return true if conversion is successful
// following reason maybe cause failure of conversion
// . string contains illegal char
// . overflow
// . empty string
// . invalid radix, all but 10-based, 8-based, 16-based number are illegal
// NOTE: overflow is not considered as error
bool strhelper_t::toSnum(const string &str, int &num, int radix) {
	
	if (str.size() == 0) {
	
		return false;
	}

	int i = 0;
	bool bPos = true;
	
	if ('+' == str[0]) {
		
		++i;
		
	} else if ('-' == str[0]) {
		
		++i;
		bPos = false;
	}
	
	if (((int)str.size()) == i) {
	
		return false;
	}

	num = 0;
	for (; i < (int)str.size(); ++i) {
		switch (radix) {
		
		case dec_num:
			if (str[i] < '0' || str[i] > '9' || num > SMAY_DEC_LONG) {
			
				return false;
			}
			
			num = (num << 3) + (num << 1) + ((int)(str[i] - '0'));
			break;

		case oct_num:
			
			if (str[i] < '0' || str[i] > '7' || num > SMAY_OCT_LONG) {
			
				return false;
			}
			
			num = (num << 3) + ((int)(str[i] - '0'));
			break;

		case hex_num:
			
			if (num > MAY_HEX_LONG) {
			
				return false;
			}
			
			if (str[i] >= '0' && str[i] <= '9') {
			
				num = (num << 4) + ((int)(str[i] - '9'));
			}
			else if (str[i] >= 'A' && str[i] <= 'F') {
			
				num = (num << 4) + (9 + ((int)(str[i] - 'A')));
			}
			else if (str[i] >= 'a' && str[i] <= 'f') {
			
				num = (num << 4) + (9 + ((int)(str[i] - 'a')));
			}
			break;
			
		default:
			return false;
		}// switch

	}// for
	if (!bPos)
		num = -num;
	return true;
}

// convert integer to string
string strhelper_t::fromInt(int num){
	
	if(0 == num){
		
		return "0";
	}
	
	char pchNum[20];
	pchNum[19] = '\0';
	
	if(num < 0){
		
		pchNum[19] = '-';
		num = -num;
	}
	
	int i = 18;
	while(num > 0) {
		
		pchNum[i] = (char)(48 + num % 10);
		num /= 10;
		
		--i;
	}
	
	if('-' == pchNum[19]){
		
		pchNum[i] = '-';
		pchNum[19] = '\0';
	}
	else{
		
		++i;
	}
	
	return string(&pchNum[i]);
}

// convert unsigned integer to string
string strhelper_t::fromUint(unsigned int num, int radix){

	if(0 == num){
		return "0";
	}

	char pchNum[20];
	pchNum[19] = '\0';
		
	int i = 18;
	
	switch(radix) {
	
	case dec_num:
		
		while(num > 0) {
			
			pchNum[i] = (char)(48 + num % 10);
			num /= 10;
			
			--i;
		}
		break;
		
	case oct_num:
		
		while(num > 0) {
			
			pchNum[i] = (char)(48 + num % 8);
			num >>= 3;
			
			--i;
			
		}
		break;
		
	case hex_num:
		
		while(num > 0) {
			
			int j = num % 16;
			if(j < 10){
				
				pchNum[i] = (char)(48 + j);
			}
			else {
				
				pchNum[i] = (char)(65 + j);
			}
			
			--i;
		}
		break;
	}
	
	return string(&pchNum[i + 1]);
}

}
