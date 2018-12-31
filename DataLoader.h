
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>

#include "Constants.h"

tm string_to_tm(std::string str) {
    struct tm date;
    int pos = 0;
    int start = 0;
    int val;
    
    for (int i = 0; i <= str.size(); i++) {
        if (str[i] == '-' || i == str.size()) {
            // atoi is a standard func for converting string to int 
            val = atoi(str.substr(start, i-start).c_str());
            
            if (pos == 0) {
                date.tm_year = val-1900;
            } else if (pos == 1) {
                date.tm_mon = val-1;
            } else if (pos == 2) {
                date.tm_mday = val;
            }
            
            start = i+1;
            pos++;
        }
    }
    return date;
}


class Tseries : public Constants {
    
    public:
        struct tm* dates = new tm[MAX_LEN];
        double* vals = new double[MAX_LEN];
        int length;
        
        //struct Constants C;
        
        Tseries(int colnum, std::string filename) {
            struct tm date;
            
            std::string line;
            std::ifstream infile;
            infile.open(filename);
            std::string val;
            int filled = 0; // how many slots have we filled
            
            while(getline(infile, line) && filled <= MAX_LEN) {
                int start = 0;
                int pos = 0;
                bool marker = 0;
                for(int i = 0; i <= line.size(); i++) {            
                    if (line[i] == ',' || i == line.size()) { 
                        val = line.substr(start, i-start);
                        
                        if (pos == 0) {
                            date = string_to_tm(val);
                            if (date.tm_year > 100) {
                                dates[filled] = date;
                                marker = 1;
                            }
                        }
                        
                        if (pos == colnum && marker == 1) {
                            // stod string to double function from std::string 
                            vals[filled] = stod(val);
                            filled++; // increment filled here when op is complete 
                        }
                        
                        start = i+1;
                        pos++;
                    }
                }   
            }
            
            length = filled;
            
            std::cout << length << std::endl;
            infile.close();                    
        }
        
        
        ~Tseries() { 
            delete[] dates;
            delete[] vals;
            std::cout << "tseries destructor" << std::endl;
        }  
        
};

