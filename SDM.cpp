

#include <iostream>
#include <random>
#include <cmath>

//#include <iomanip>
//#include <fstream>

// global max dimensions 
const int MAX_LEN = 5000;
const int MAX_WIDTH = 500;
const unsigned int SEED = 4;

/**
manipulating coordinates in 1 and 2 d 
**/
class Coordinates {
    public:
        //calculate 1d array coordinates from 2d ref  
        int idx(int r, int c, int lag) {
            return (r*lag)+c;
        }
        //calculate row from 1d ref
        int row(int i, int lag) {
            return i/lag;
        }
        //calculate col from 1d ref
        int col(int i, int lag) {
            return i-(lag*(i/lag));
        }
};

/**
probability function for the distribution over the distances 
between x ad y
**/
double Pr(double x, double y) {
    double z = pow(x-y, 2);
    z = exp(-z);
    return z;
}

/**
class for generating pairs of test time series 
**/
class SDM {
    private:
        int N;

    public:
        // constructor 
        SDM(int);
        // destructor 
        ~SDM() { delete x; delete y; delete W; };
        double* x = new double[MAX_LEN];
        double* y = new double[MAX_LEN];
        double* W = new double[MAX_LEN*MAX_WIDTH];

        // generate test series 
        void generate_series(double mu, double sig, double err) {
            std::default_random_engine generator(SEED);
            std::normal_distribution<double> distribution(mu, sig);
            std::normal_distribution<double> error(0, err);
            double u;
            double k;  
                    
            for (int i = 0; i < N; i++) {
                k = distribution(generator);
                u = error(generator);
                y[i] = k;
                if (i < 5) {
                    x[i] = k+u;
                } else {
                    x[i-5] = k+u;
                }
            }
        }
        
        void run(int lag) {
            Coordinates C;

            // W to ones 
            for (int i = 0; i <= C.idx(N, lag, lag); i++) {
                W[i] = 1;
            }

            for (int i = lag; i <= N; i++) {
                // convert the row into probabilities and sum total prob 
                double total_prob = 0;
                for (int j = 0; j <= lag; j++) {
                    W[C.idx(i, j, lag)] = (Pr(y[i], x[i-j])+0.0001)*W[C.idx(i-1, j, lag)];
                    total_prob += W[C.idx(i, j, lag)];
                }

                // divide by total prob
                for (int j = 0; j <= lag; j++) {
                    W[C.idx(i, j, lag)] = W[C.idx(i, j, lag)]/total_prob;
                }
            }
        }
          

        void argmax(int startN, int endN, int lag) {
            Coordinates C;
            double agmx = 0;
            int agmx_i = 0;
            for (int i = startN; i < endN; i++) {
                if (C.col(i, lag) == 0) {
                    std::cout <<
                        i << ' ' << C.row(i, lag) << ' ' << C.col(i, lag) << ' ' <<
                        agmx << ' ' << agmx_i << ' ' << 
                        C.row(agmx_i, lag) << ' ' << C.col(agmx_i, lag) << 
                    std::endl;

                    agmx = W[i];
                    agmx_i = i;
                };
                
                if (agmx < W[i]) {
                    agmx = W[i];
                    agmx_i = i;
                };
                
            }

        }
        
};

/**
constructor for init SDM
**/
SDM::SDM(int n) {
    N = n;
}

int main() {
    int N = 30;
    int lag = 20;
    double mu = 0;
    double sig = 1;
    double err = 0.01;
    Coordinates C;
    
    SDM sdm(N);
    sdm.generate_series(mu, sig, err);
    sdm.run(lag);
    sdm.argmax(C.idx(19, 0, lag), N*lag, lag);
    
    
    return 0;
    
}