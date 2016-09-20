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

#include "./clhandler.h"
#include "./reporter.h"

namespace common_ns {

// cl_handler_t: commandline processor

// process commandline
// @argc: number of in-coming parameters
// @argv: in-coming parameters; to confirm to C-convention, @argv[0] is not considered as an argument
// @strLegalOpt: array of legal option chars; any other char will be considered illegal
// @strHasArg: array of option type; indicating corresponding option char in @strLegalOpt has
// 	an argument or not.
// 	0: no argument attached
//	1: at most an argument attached
//	2: extact one argument attached
bool cl_handler_t::process(int argc, char **argv, 
		const string &strLegalOpt, const string &strHasArg) {
	
    assert(argc >= 1 && argv && argv[0]);
    clear();
    
    string strErr;
    
    int i = 1;
    for(; i < argc; ++i) {
        assert(argv[i]);
        
        // option leading char: '-'
        if('-' == argv[i][0]) {
        	
            if(!argv[i][1]) {
            	
                _ERROR("lack of option.", ECMD(13));
                return false;
            }
            
            // get option letter, and its argument if any
            bool bDup = false;
            char *p = &argv[i][1];
            while(*p) {
            	
            	string::size_type idx = strLegalOpt.find(*p);
            	if(idx == string::npos) {// illegal option found
                    strErr = "invalid option `";
                    strErr += *p;
                    strErr += "\'.";
                    _ERROR(strErr, ECMD(14));
                    return false;
            	}
            	
            	if(strHasArg[idx] != '0') {
            		// try to get argument attached
    	            if('\0' != *(p + 1)) {
    	            	bDup = ! addOption(*p, (p + 1)) || bDup;
    	            	break;
    	            }
    	            else if(strHasArg[idx] == '2') {
    	            	// argument is a must
    	            	if(i < argc - 1) {
    	            		bDup = ! addOption(*p, argv[i + 1]) || bDup;
    		                ++i;
    		                break;
    	            	}
    	            	else {
    	                    strErr = "invalid option `";
    	                    strErr += *p;
    	                    strErr += "\': an argument needed.";
    	                    _ERROR(strErr, ECMD(15));
    	                    return false;
    	            	}
    	            }
            	}

            	bDup = ! addOption(*p) || bDup;
            	
            	if(bDup) break;
            	
            	++p;
            }
            
            if(bDup) {// duplicated options found
            	
                strErr = "duplicated option `";
                strErr += *p;
                strErr += "\'.";
                _WARNING(strErr);
                
                //return false;
            }
        }
        else {
        	// non-option argument found
        	addNonOpt(argv[i]);
        }
    }
    
    return true;
}

}
