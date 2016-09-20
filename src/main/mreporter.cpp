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

#include <iostream>
using std::cerr;
#include <fstream>
using std::ofstream;
using std::endl;

#include "../common/common.h"
#include "../common/filehelper.h"
using common_ns::filehelper_t;

#include "./mreporter.h"

mreporter_t::mreporter_t(const string &a_strOut, int a_nBits)
: m_nLastError(0)
, m_nErrNum(0)
, m_nBits(a_nBits)
, m_pLog(nullptr) {
	
	if( ! filehelper_t::isAccessible(a_strOut)) {
		m_pLog = &cerr;
	}
	else {
		m_pLog = new ofstream(a_strOut.c_str());
	}
}

mreporter_t::~mreporter_t(void) {
	
	if(&cerr != m_pLog) {
		delete m_pLog;
	}
}
// set mask bits
void mreporter_t::setOptions(int a_nBits) {
	
	m_nBits = a_nBits;
}

// get which kinds of information are reported
int mreporter_t::getOptions(void) const {
	
	return m_nBits;
}

// get error code
int mreporter_t::getLastError(void) const {
	
	return m_nLastError;
}

// get number of errors emitted
int mreporter_t::getErrNum(void) const {
	
	return m_nErrNum;
}

// emit detailed information which might be helpful for debugging or testing
void mreporter_t::emitVerbose(const string &a_strMsg) {
	
	if(reporter_base_t::VERBOSE_BIT & m_nBits) {
		(*m_pLog) << "Log: " << a_strMsg << endl;
	}
}

// report warnings
void mreporter_t::emitWarning(const string &a_strMsg) {
	
	m_nLastError = WRNID_BASE + 1;
	if(reporter_base_t::WARNING_BIT & m_nBits) {
		(*m_pLog) << "Warning: " << a_strMsg << endl;
	}
}

// report errors
void mreporter_t::emitError(const string &a_strMsg, int a_nCode) {
	
	m_nLastError = a_nCode;
	++m_nErrNum;
	if(reporter_base_t::ERROR_BIT & m_nBits) {
		(*m_pLog) << "Error: " << a_strMsg << endl;
	}
}

// report fatal errors
void mreporter_t::emitFatalError(const string &a_strMsg, int a_nCode) {

	m_nLastError = a_nCode;
	++m_nErrNum;
	(*m_pLog) << "Fatal error: " << a_strMsg << endl;
}

// reset error information
void mreporter_t::clearError(void) {
	m_nLastError = 0;
	m_nErrNum = 0;
}

reporter_base_t* mreporter_t::getOne(const string &a_strOut) {
	
	static mreporter_t  sf_mReport(a_strOut,
			reporter_base_t::ERROR_BIT | reporter_base_t::WARNING_BIT);
	return &sf_mReport;
}
