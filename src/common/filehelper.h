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

#ifndef COMMON_FILEHELPER_H__
#define COMMON_FILEHELPER_H__

#include <string>
using std::string;

namespace common_ns {

// helper class for file or directory manipulations
class filehelper_t {

public:
	
	// test if file exists
	// @a_pcchFile: file name to be tested
	// @return-value: true if the file exists, false otherwise
	static bool isAccessible(const char *a_pcchFile);
	// @a_strFile: file name to be tested
	static bool isAccessible(const string &a_strFile);
	
	// extract name and extended name from whole name of file
	// @a_pcchFile: whole name of file to be extracted
	// @a_strName: basic name extracting from @a_pcchFile
	// @a_strExt: extended name extracting from @a_pcchFile
	// @return-value: flag indicating the operation is successful
	// @return-value: true if the file exists, false otherwise
	static bool extractExt(const char *a_pcchFile, string &a_strName, string &a_strExt);
	// @a_strFile: whole name of file to be extracted
	// @a_strName: basic name extracting from @a_strFile
	// @a_strExt: extended name extracting from @a_strFile
	// @return-value: flag indicating the operation is successful
	static bool extractExt(const string &a_strFile, string &a_strName, string &a_strExt);
	
	// extract directory and file name from path of file
	// @a_pcchPath: (relative or absolute) path of file to be extracted
	// @a_strDir: file name(may include extended name) extracting from @a_pcchPath
	// @a_strName: directory name extracting from @a_pcchPath
	// @return-value: flag indicating the operation is successful
	static bool extractDir(const char *a_pcchPath, string &a_strDir, string &a_strName);
	// @a_strPath: (relative or absolute) path of file to be extracted
	// @a_strDir: file name(may include extended name) extracting from @a_strPath
	// @a_strName: directory name extracting from @a_strPath
	// @return-value: flag indicating the operation is successful
	static bool extractDir(const string &a_strPath, string &a_strDir, string &a_strName);
	
	// new extended name substitue for the old to make new file name 
	// @a_pcchFile: original file name
	// @a_strExt: new extended name
	// @a_strNewFile: file name to be made
	// @return-value: flag indicating the operation is successful
	static bool subtExt(const char *a_pcchFile, const string &a_strExt, string &a_strNewFile);
	// @a_strFile: original file name
	// @a_strExt: new extended name
	// @a_strNewFile: file name to be made
	// @return-value: flag indicating the operation is successful
	static bool subtExt(const string &a_strFile, const string &a_strExt, string &a_strNewFile);
};

}

#endif // COMMON_FILEHELPER_H_
