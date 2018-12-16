

#include <iostream>
#include <random>
#include <cmath>

// global max dimensions 
const int MAX_LEN = 5000;
const int MAX_WIDTH = 500;
const unsigned int SEED = 1;
const double PI = 3.14159265359;

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
double Pr(double d, double mu, double sig_2) {
    double z = exp(-pow(d-mu, 2)/2*sig_2)/sqrt(2.*PI*sig_2);
    return z;
}

/**
linear test function 
**/
double lin(double x) {
    return 4+5*x;
}

/** 
regress 2 vectors x and y
**/
class Regression {
    public:
        //constructor 
        Regression() {};
        //destructor 
        ~Regression() {};
    
        double* x = new double[MAX_LEN];
        double* y = new double[MAX_LEN];
        double alpha = 0;
        double beta = 0;
        double R = 0;
        
        void run(int start, int end) {
            double mean_x = 0;
            double mean_y = 0;
            double var_x = 0;
            double var_y = 0;
            double cov_x_y = 0; 
            double y_var = 0;
            
            // calculate means 
            for (int i = start; i <= end; i++) {
                mean_x += x[i];
                mean_y += y[i];
            }
            mean_x = mean_x/(end-start);
            mean_y = mean_y/(end-start);
            
            // variance and covariance 
            for (int i = start; i <= end; i++) {
                cov_x_y += (x[i]-mean_x)*(y[i]-mean_y);
                var_x += pow(x[i]-mean_x, 2);
                var_y += pow(y[i]-mean_y, 2);
            }
            
            beta = cov_x_y/var_x;
            alpha = mean_y-(beta*mean_x);
            R = cov_x_y/sqrt(var_x*var_y);
            
        }  
};

/**
class for generating pairs of test time series 
**/
class SDM {
    /**
    x and y series run 0-N
    W and fcst_x filled lag-N
    **/
    private:
        int N;

    public:
        // constructor 
        SDM(int);
        // destructor 
        ~SDM() {
            delete x; delete y; delete W; delete MAP_lag; delete fcst_x; delete x_adj; delete y_adj; 
        };
        // attributes 
        double* x = new double[MAX_LEN];
        double* y_unlagged = new double[MAX_LEN];
        double* y = new double[MAX_LEN];
        double* x_adj = new double[MAX_LEN];
        double* y_adj = new double[MAX_LEN]; 
        int* MAP_lag = new int[MAX_LEN];
        double* fcst_x = new double[MAX_LEN];
        double* W = new double[MAX_LEN*MAX_WIDTH];
        double* D = new double[MAX_LEN*MAX_WIDTH];
        double beta = 0;
        double alpha = 0;
        double mean_D = 0;
        double std_D = 0;

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
                x[i] = k;
                y_unlagged[i] = lin(k)+u;
                
                
                if (i < 150) {
                    y[i+3] = lin(k)+u;
                } else {
                    y[i+5] = lin(k)+u;
                }
                
            }
        }

        void zscores() {
            double mean_x = 0;
            double mean_y = 0;
            double std_x = 0;
            double std_y = 0;
            
            double mean_d = 0;
            double std_d = 0;
            
            // zscore x_adj and y_adj 
            for (int i = 0; i <= N; i++) {
                mean_x += x[i];
                mean_y += y[i];
                std_x += pow(x[i], 2);
                std_y += pow(y[i], 2);
            }
            mean_x = mean_x/N;
            mean_y = mean_y/N;
            std_x = sqrt(std_x/N);
            std_y = sqrt(std_y/N);
            
            for (int i = 0; i<= N; i++) {
                x_adj[i] = (x[i]-mean_x)/std_x;
                y_adj[i] = (y[i]-mean_y)/std_y;
            }            
        }
        
        void run(int lag) {
            Coordinates C;
            Regression R;
            R.y = y;
            double d = 0;
            double mu_d = 0;
            double var_d = 1;
            
            zscores();
            
            // W to ones 
            for (int i = 0; i <= C.idx(N, lag, lag); i++) {
                W[i] = 1;
            }
            
            for (int i = lag; i <= N; i++) {
                // convert the row into probabilities and sum total prob 
                double total_prob = 0;
                int idx = 0;
                for (int j = 0; j <= lag; j++) {
                    d = y_adj[i]-x_adj[i-j];
                    mu_d += d;
                    var_d += pow(d, 2);
                    idx = C.idx(i, j, lag);
                    W[idx] = (Pr(d, mu_d/(idx-lag), var_d/(idx-lag))+0.0001)*W[C.idx(i-1, j, lag)];
                    total_prob += W[idx];
                }

                // divide by total prob collect fcst values  
                int map_lag_pos = 0;
                double map_lag_p = 0;
                double map_lag_x = 0;
                for (int j = 0; j <= lag; j++) {
                    W[C.idx(i, j, lag)] = W[C.idx(i, j, lag)]/total_prob;
                    if (W[C.idx(i, j, lag)] > map_lag_p) {
                        map_lag_pos = j;
                        map_lag_p = W[C.idx(i, j, lag)];
                    }
                }
                MAP_lag[i] = map_lag_pos;
                fcst_x[i] = x[i-MAP_lag[i-1]]; // use estimate from t-1 
                R.x = fcst_x;
                R.run(lag, i);
                
                std::cout << "i=" << i << " map pos=" << map_lag_pos << " p=" << map_lag_p 
                    << " x=" << fcst_x[i] << " y=" << y[i] << " " << R.alpha << " " 
                    << R.beta << " " << R.R << std::endl; 
                    
                

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
    int N = 300;
    int lag = 50;
    double mu = 0;
    double sig = 1;
    double err = 0.5;
    Coordinates C;
    
    SDM sdm(N);
    sdm.generate_series(mu, sig, err);
    sdm.run(lag);
    
    Regression r;
    r.x = sdm.x;
    r.y = sdm.y;
    r.run(0, N);
    
    r.x = sdm.fcst_x;
    r.run(lag, N);
    
    r.x = sdm.x;
    r.y = sdm.y_unlagged;
    r.run(lag, N);
    
    return 0;
    
}