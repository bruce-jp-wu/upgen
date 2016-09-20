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
#include <sys/stat.h>

#include "./filehelper.h"

#define MAX_EXT_SIZE		32
#define MAX_DIR_SIZE		64
#define DIR_SEPERATOR		'/'

namespace common_ns {

// test if file exists
// @a_pcchFile: file name to be tested
// @return-value: true if the file exists, false otherwise
bool filehelper_t::isAccessible(const char *a_pcchFile) {
	
	struct stat st;
	return  0== stat(a_pcchFile, &st);
}
// test if file exists
// @a_strFile: file name to be tested
// @return-value: true if the file exists, false otherwise
bool filehelper_t::isAccessible(const string &a_strFile) {
	
	struct stat st;
	return  0== stat(a_strFile.c_str(), &st);
}

// extract name and extended name from whole name of file
// @a_pcchFile: whole name of file to be extracted
// @a_strName: basic name extracting from @a_pcchFile
// @a_strExt: extended name extracting from @a_pcchFile
// @return-value: flag indicating the operation is successful
bool filehelper_t::extractExt(const char *a_pcchFile, string &a_strName, string &a_strExt) {
	
	if( ! a_pcchFile) {
		return false;
	}

////	a_strName = "";
////	a_strExt.resize(MAX_EXT_SIZE);
////	
////	int i = 0, j = -1, k = 0;
////	for(; a_pcchFile[i]; ++i) {
////	
////		if('.' == a_pcchFile[i]) {
////			
////			a_strName += a_strExt;
////			a_strExt.resize(MAX_EXT_SIZE);
////			j = i;
////			k = 0;
////		}
////		
////		if(k >= MAX_EXT_SIZE) {
////			a_strExt += a_pcchFile[i];
////		}
////		else {
////			a_strExt[k] = a_pcchFile[i];
////		}
////		++k;
////	}
////	
////	if(-1 == j) {
////		a_strName = a_strExt;
////		a_strExt = "";
////	}
//	
//	int i = 0, j = -1;
//	for(; a_pcchFile[i]; ++i) {
//		if('.' == a_ppchFile[i]) {
//			j = i;
//		}
//	}
//	if(-1 == j) {
//		a_strName = string(a_pcchFile, i);
//		a_strExt = "";
//	}
//	else {
//		
//		if( j > 0 ) {
//			a_strName.resize(j);
//			a_strExt.resize(i - j);
//			
//			for(i = 0; i < j; ++i) {
//				a_strName[i] = a_pcchFile[i];
//			}
//			
//			j = 0;
//			for(; a_pcchFile[i]; ++i, ++j) {
//				a_strExt[j] = a_strFile[i];
//			}
//		}
//		else {
//			
//			a_strName = "";
//			a_strExt = string(a_pcchFile, i);
//		}
//	}
	
	a_strName = "";
	a_strExt = "";
	a_strExt.resize(MAX_EXT_SIZE);
	
	int i = 0, j = -1;
	for(; a_pcchFile[i]; ++i) {
	
		if('.' == a_pcchFile[i]) {
			
			a_strName += a_strExt;
			a_strExt = "";
			a_strExt.resize(MAX_EXT_SIZE);
			j = i;
		}
		
		a_strExt += a_pcchFile[i];
	}
	
	if(-1 == j) {
		a_strName = a_strExt;
		a_strExt = "";
	}
	
	return true;
}

// extract name and extended name from whole name of file
// @a_strFile: whole name of file to be extracted
// @a_strName: basic name extracting from @a_strFile
// @a_strExt: extended name extracting from @a_strFile
// @return-value: flag indicating the operation is successful
bool filehelper_t::extractExt(const string &a_strFile, string &a_strName, string &a_strExt) {
	
	string::size_type pos = a_strFile.rfind('.');
	if(string::npos == pos) {
		
		a_strName = a_strFile;
		a_strExt = "";
	}
	else {
		
		if(pos > 0) {
			a_strName = a_strFile.substr(0, pos);
			a_strExt = a_strFile.substr(pos, a_strFile.size() - pos);
		}
		else {
			a_strName = "";
			a_strExt = a_strFile;
		}
	}
	
	return true;
}

// extract directory and file name from path of file
// @a_pcchPath: (relative or absolute) path of file to be extracted
// @a_strDir: file name(may include extended name) extracting from @a_pcchPath
// @a_strName: directory name extracating from @a_pcchPath
// @return-value: flag indicating the operation is successful
bool filehelper_t::extractDir(const char *a_pcchPath, string &a_strDir, string &a_strName) {
	
	if( ! a_pcchPath) {
		return false;
	}
	a_strDir = "";
	a_strName = "";
	a_strName.resize(MAX_DIR_SIZE);
	
	int i = 0, j = -1;
	for(; a_pcchPath[i]; ++i) {
	
		if(DIR_SEPERATOR == a_pcchPath[i]) {
			
			a_strDir += a_strName;
			a_strName = "";
			a_strName.resize(MAX_DIR_SIZE);
			j = i;
			
			a_strDir += a_pcchPath[i];
		}
		else {	
		
			a_strName += a_pcchPath[i];
		}
	}
	
	return true;
}

// extract directory and file name from path of file
// @a_strPath: (relative or absolute) path of file to be extracted
// @a_strDir: file name(may include extended name) extracting from @a_strPath
// @a_strName: directory name extracting from @a_strPath
// @return-value: flag indicating the operation is successful
bool filehelper_t::extractDir(const string &a_strPath, string &a_strDir, string &a_strName) {
	
	string::size_type pos = a_strPath.rfind(DIR_SEPERATOR);
	if(string::npos == pos) {
		
		a_strName = a_strPath;
		a_strDir = "";
	}
	else {
		
		if(pos < a_strPath.size() - 1) {
			a_strDir = a_strPath.substr(0, pos + 1);
			a_strName = a_strPath.substr(pos + 1, a_strPath.size() - pos - 1);
		}
		else {
			a_strName = "";
			a_strDir = a_strPath;
		}
	}
	
	return true;
}

// new extended name substitue for the old to make new file name 
// @a_pcchFile: original file name
// @a_strExt: new extended name
// @a_strNewFile: file name to be made
// @return-value: flag indicating the operation is successful
bool filehelper_t::subtExt(const char *a_pcchFile, const string &a_strExt, string &a_strNewFile) {
	
	if( ! a_pcchFile) {
		return false;
	}
	
	string strExt;
	
	a_strNewFile = "";
	strExt.resize(MAX_EXT_SIZE);
	
	int i = 0, j = -1;
	for(; a_pcchFile[i]; ++i) {
	
		if('.' == a_pcchFile[i]) {
			
			a_strNewFile += strExt;
			strExt = "";
			strExt.resize(MAX_EXT_SIZE);
			j = i;
		}
		
		strExt += a_pcchFile[i];
	}
	
	if(-1 == j) {
		a_strNewFile = strExt;
		strExt = "";
	}
	
	a_strNewFile += a_strExt;
	
	return true;
}

// new extended name substitue for the old to make new file name 
// @a_strFile: original file name
// @a_strExt: new extended name
// @a_strNewFile: file name to be made
// @return-value: flag indicating the operation is successful
bool filehelper_t::subtExt(const string &a_strFile, const string &a_strExt, string &a_strNewFile) {
	
	string::size_type pos = a_strFile.rfind('.');
	if(string::npos == pos) {
		
		a_strNewFile = a_strFile;
		a_strNewFile += a_strExt;
	}
	else {
		
		if(pos > 0) {
			a_strNewFile = a_strFile.substr(0, pos);
			a_strNewFile += a_strExt;
		}
		else {
			a_strNewFile = a_strExt;
		}
	}
	
	return true;
}

}
