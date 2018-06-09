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

#include <fstream>
#include <iostream>
#include <string>

// [0abfnrtv\'\"\?\\]
static std::string convert(const std::string& line) {
    std::string s;
    for(auto c: line) {
        if(c == '\\' || c == '\'' || c == '\"' || c == '?') {
            s += '\\';
        }
        s += c;
    }
    s += "\\n\\";

    return s;
}

int main(int argc, char **argv) {
    auto usage = []() {
        std::cerr << "Usage:" << std::endl;
        std::cerr << "\tcsconv string-file" << std::endl;
    };

    if(argc < 2) {
        usage();
        return - 1;
    }

    std::ifstream ifs(argv[1]);
    if(!ifs) {
        std::cerr << "Error: failed to access `" << argv[1] << "\'" << std::endl;
        return -1;
    }

    std::string strout {argv[1]};
    strout += ".string";

    std::ofstream ofs(strout);
    if(!ofs) {
        std::cerr << "Error: failed to create `" << strout << "\'" << std::endl;
        return -1;
    }

    int totalSize = 2;

    ofs << "\"";

    std::string inputLine;
    while(std::getline(ifs, inputLine)) {
        auto s = convert(inputLine);
        totalSize += s.size();
        ofs << s << std::endl;
    }

    ofs << "\\0\\0\";\n" << std::endl;
    totalSize += 8;

    ofs.flush();

    std::cout << "Total size: " << totalSize << std::endl;


    return 0;
}
