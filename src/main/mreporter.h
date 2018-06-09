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


#ifndef UPGEN_MREPORTER_H__
#define UPGEN_MREPORTER_H__
#include <iosfwd>
using std::ostream;

#include "../common/reporter.h"
using common_ns::reporter_base_t;

class mreporter_t: public reporter_base_t {
	
private:
	
	int m_nLastError;
	int m_nErrNum;
	int m_nBits;
	ostream *m_pLog;

public:
	mreporter_t(const string &a_strOut, int a_nBits);
	~mreporter_t(void);
	// set mask bits
	void setOptions(int a_nBits);
	
	// get which kinds of information are reported
	int getOptions(void) const;
	
	// get last error code
	int getLastError(void) const;
	
	// get number of errors emitted
	int getErrNum(void) const;
	
	// emit detailed information which might be helpful for debugging or testing
	void emitVerbose(const string &a_strMsg);
	
	// report warnings
	void emitWarning(const string &a_strMsg);
	
	// report errors
	void emitError(const string &a_strMsg, int a_nCode);
	
	// report fatal errors
	void emitFatalError(const string &a_strMsg, int a_nCode);
	
	// reset error information
	void clearError(void);
	// get an instance of error-reporter class
	static reporter_base_t* getOne(const string &a_strOut);
};

#endif // UPGEN_MREPORTER_H__
