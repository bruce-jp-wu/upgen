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

#include "./reporter.h"
namespace common_ns{

// a deaf-and-blind reporter, let all error messages vanish into the void
class report_null_t: public reporter_base_t{

private:
	
	report_null_t(void){}
	report_null_t(const report_null_t &src);
	void operator=(const report_null_t &src);
	
public:
	
	void setOptions(int a_nBits){
		// do nothing
	}
	
	// get which kinds of information are reported
	int getOptions(void) const {
		return 0;
	}
	
	// get error code
	int getLastError(void) const {
		return 0;
	}
	
	// get number of errors emitted
	virtual int getErrNum(void) const {
		return 0;
	}
	
	// emit detailed information which might be helpful for debugging or testing
	void emitVerbose(const string &a_strMsg) {
		// do nothing
	}
	
	// report warnings
	void emitWarning(const string &a_strMsg) {
		// do nothing
	}
	
	// report errors
	void emitError(const string &a_strMsg, int a_nCode) {
		// do nothing
	}
	
	// report fatal errors
	void emitFatalError(const string &a_strMsg, int a_nCode) {
		// do nothing
	}
	
	void clearError(void) {}
	
	// get an instance of error-reporter class
	static reporter_base_t* getOne(void){
		static report_null_t  sf_pNullReport;
		return &sf_pNullReport;
	}
};

// just avoid setting it to nullptr
reporter_base_t *reporter_factory_t::sm_pReporter = report_null_t::getOne();

}
