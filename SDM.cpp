

#include <iostream>
#include <random>

//#include <iomanip>
//#include <cmath>
//#include <fstream>


/**
class for generating pairs of test time series 
**/
class TestData {
    private:
        unsigned int seed;
        int N;

    public:
        TestData(unsigned int, int);
        std::vector<double> x;
        std::vector<double> y;

        // get the base random series 
        std::vector<double> generate_base_series(double mu, double sig) {
            std::default_random_engine generator(seed);
            std::normal_distribution<double> distribution(mu, sig);
            std::vector<double> time_series(N);
            double k;
            
            for (int i = 0; i < N; i++) {
                k = distribution(generator);
                time_series[i] = k;
            }
            return time_series;
        }

        // get the lagged/leading series 
        std::vector<double> generate_comp_series(double mu, double sig) {
            std::vector<double> error = generate_base_series(mu, sig);
            std::vector<double> time_series(N);
            double k;
            
            for (int i = 0; i< N; i++) {
                time_series[i] = k+x[i];
            }
            return time_series; 
        }
    
};

/**
constructor for init TestData
**/
TestData::TestData(unsigned int s, int n) {
    seed = s;
    N = n;
}


/**
SDM
**/
class SDM {
    private:
        int N;
        std::vector<double> x;
        std::vector<double> y;  
    public:
        SDM(std::vector<double>, std::vector<double>, int);
            
        std::vector<double> run(int lag) {
            int t = lag;
            for (t; t < N; t++) {
                std::cout << t << std::endl;
            }
            return x;
        }            
        
    
};

/**
constructor for init SDM
**/
SDM::SDM(std::vector<double> inp_x, std::vector<double> inp_y, int n) {
    N = n;
    x = inp_x;
    y = inp_y;
}

int main() {
    int N = 30;
    unsigned int seed = 2;
    TestData data(seed, N);
    data.x = data.generate_base_series(0, 1);
    data.y = data.generate_comp_series(0, 1);
    
    SDM sdm(data.x, data.y, N);
    
    std::cout << "hello" << std::endl;
    for (int i=0; i<N; i++) {
        std::cout << data.x[i] << std::endl;
    }
    
    return 0;
    
}