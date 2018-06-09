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

#ifndef COMMON_REPORTER_H__
#define COMMON_REPORTER_H__

// common library: includes classes which can be shared among different applications
// error report classes

#include "common.h"
#include <string>
using std::string;

namespace common_ns{

// base class for information reporting
class reporter_base_t{
	
public:
	
	// bits indicate if certain type of information is suppressed;
	// fatal error can never be suppressed
	enum{
		
		FATAL_ERROR_BIT = 0,
		ERROR_BIT = 1,
		WARNING_BIT = 2,
		VERBOSE_BIT = 4,
	};
	
public:
	// set which kinds of information are reported
	virtual void setOptions(int a_nBits) = 0;
	
	// get which kinds of information are reported
	virtual int getOptions(void) const = 0;
	
	// get error code
	virtual int getLastError(void) const = 0;
	
	// get number of errors emitted
	virtual int getErrNum(void) const = 0;
	
	// emit detailed information which might be helpful for debugging or testing
	virtual void emitVerbose(const string &a_strMsg) = 0;
	
	// report warnings
	virtual void emitWarning(const string &a_strMsg) = 0;
	
	// report errors
	virtual void emitError(const string &a_strMsg, int a_nCode) = 0;
	
	// report fatal errors
	virtual void emitFatalError(const string &a_strMsg, int a_nCode) = 0;
	
	// reset error information
	virtual void clearError(void) = 0;
	
	virtual ~reporter_base_t(void){}
};

// factory class for information reporting
class reporter_factory_t{

private:
	
	static reporter_base_t *sm_pReporter;

public:
	
	static reporter_base_t *setReporter(reporter_base_t *a_pReporter){
		
		reporter_base_t *pFormer = sm_pReporter;
		sm_pReporter = a_pReporter;
		
		return pFormer;
	}
	
	static reporter_base_t *getReporter(void){
		
		assert(sm_pReporter);
		return sm_pReporter;
	}
};

}

#define _VERBOSE(msg)	{\
	common_ns::reporter_factory_t::getReporter()->emitVerbose(msg); \
	}

#define _WARNING(msg)	{\
	common_ns::reporter_factory_t::getReporter()->emitWarning(msg); \
	}

#define _ERROR(msg, x)	{\
	common_ns::reporter_factory_t::getReporter()->emitError(msg, x); \
	}

#define _FATAL_ERROR(msg, x)	{\
	common_ns::reporter_factory_t::getReporter()->emitError(msg, x); \
	}

#define _GETERR() \
	common_ns::reporter_factory_t::getReporter()->getLastError()

#define _ERRNUM() \
	common_ns::reporter_factory_t::getReporter()->getErrNum()

#define _CLRERR() {\
	common_ns::reporter_factory_t::getReporter()->clearError(); \
	}

#endif //COMMON_REPORTER_H__
