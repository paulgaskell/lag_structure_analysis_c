
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>

tm string_to_tm(std::string str) 
{
    struct tm date;
    int pos = 0;
    int start = 0;
    int val;
    
    for (int i = 0; i <= str.size(); i++) 
    {
        if (str[i] == '-' || i == str.size()) 
        {
            // atoi is a standard func for converting string to int 
            val = atoi(str.substr(start, i-start).c_str());
            
            if (pos == 0) 
            {
                date.tm_year = val-1900;
            } else if (pos == 1) 
            {
                date.tm_mon = val-1;
            } else if (pos == 2) 
            {
                date.tm_mday = val;
            }
            
            start = i+1;
            pos++;
        }
    }
    return date;
}

class Tseries 
{    
    public:        
            
        //constructor 
        Tseries(const int max_len_, int colnum_, std::string filename_)
        {
            max_len = max_len_;
            colnum = colnum_;
            filename = filename_;
        }
 
        // destructor 
        ~Tseries() 
        { 
            delete[] dates;
            delete[] vals;
            std::cout << "tseries destructor" << std::endl;
        }  
        
        // attrs
        struct tm* dates = new struct tm[max_len];
        double* vals = new double[max_len];
        int length;
        int max_len;
        int colnum;
        std::string filename;
        
        void get_data()
        {   
            struct tm date;
            
            std::string line;
            std::ifstream infile;
            infile.open(filename);
            std::string val;
            int filled = 0; // how many slots have we filled
            
            while(getline(infile, line) && filled <= max_len) 
            {
                int start = 0;
                int pos = 0;
                bool marker = 0;
                for(int i = 0; i <= line.size(); i++) 
                {            
                    if (line[i] == ',' || i == line.size()) 
                    { 
                        val = line.substr(start, i-start);
                        
                        if (pos == 0) 
                        {
                            date = string_to_tm(val);
                            if (date.tm_year > 100) 
                            {
                                dates[filled] = date;
                                marker = 1;
                            }
                        }
                        
                        if (pos == colnum && marker == 1) 
                        {
                            // stod string to double function from std::string 
                            vals[filled] = stod(val);
                            filled++; // increment filled here when op is complete 
                        }
                        
                        start = i+1;
                        pos++;
                    }
                }   
            }
            
            //length = filled;
            std::cout << "here" << std::endl;
            std::cout << filled << std::endl;
            infile.close();      
            length = filled;
        }
};

