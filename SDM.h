
#include <iostream>
#include <random>
#include <cmath>

#include "Constants.h"

class Coordinates {
    /**
    manipulating coordinates in 1 and 2 d 
    **/
    
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

double lin(double x) {
    /**
    linear test function 
    **/
    return 4+5*x;
}

class Regression : public Constants {
    /** 
    regress 2 vectors x and y
    **/
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
        
        void linear_model(int start, int end) {
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

class SDM : public Coordinates, Constants {
    /**
    x and y series run 0-N
    W and fcst_x filled lag-N
    **/
    private:
        int N;
        
    public:
        int lag;
        
        // constructor 
        SDM(int n, int lag_) {
            N = n;
            lag = lag_;
        };
        
        // destructor 
        ~SDM() {
            delete x; delete y; delete W; delete MAP_lag; delete fcst_x; 
            delete x_adj; delete y_adj; 
        };
        
        // global class attributes 
        double* x = new double[MAX_LEN];
        double* y_unlagged = new double[MAX_LEN];
        double* y = new double[MAX_LEN];
        double* x_adj = new double[MAX_LEN];
        double* y_adj = new double[MAX_LEN]; 
        int* MAP_lag = new int[MAX_LEN];
        double* fcst_x = new double[MAX_LEN];
        double* W = new double[MAX_LEN*MAX_WIDTH];

        
        /** SETUP **/
        
        void generate_series(double mu, double sig, double err) {
            /** 
            generate test series 
            **/
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
        
        /** HELPER FUNCTIONS **/
        
        double Pr(double d) {
            /**
            probability function for the distribution over the distances 
            between x ad y
            **/ 
            double z = exp(-pow(d, 2)/2)/sqrt(2.*PI);
            return z;
        }
        
        double simp_structure(int i, int j, int lag) {
            return W[idx(i-1, j, lag)]+0.01;
        }
        
        double tcp_structure(int i, int j, int lag) {
            double noise = 0.001;
            if (j == 0) {
                return ((W[idx(i-1, j, lag)]+W[idx(i-1, j+1, lag)])/2)+noise;
            } else if (j == lag) {
                return ((W[idx(i-1, j, lag)]+W[idx(i-1, j-1, lag)])/2)+noise;
            } else {
                return ((W[idx(i-1, j, lag)]+W[idx(i-1, j-1, lag)]+
                            W[idx(i-1, j+1, lag)])/3)+noise;
            }
        }
            
        void run() {
            
            Regression R;
            R.y = y;
            
            zscores();
            
            // reset W to ones 
            for (int i = 0; i <= idx(N, lag, lag); i++) {
                W[i] = 1;
            }
            
            for (int i = lag; i <= N; i++) {
                // convert the row into probabilities and sum total prob 
                double total_prob = 0;
                int curr_idx = 0;
                for (int j = 0; j <= lag; j++) {
                    curr_idx = idx(i, j, lag);
                    W[curr_idx] = Pr(y_adj[i]-x_adj[i-j])
                                    *tcp_structure(i, j, lag);

                    total_prob += W[curr_idx];
                }

                // divide by total prob collect fcst values  
                int map_lag_pos = 0;
                double map_lag_p = 0;
                double map_lag_x = 0;
                for (int j = 0; j <= lag; j++) {
                    W[idx(i, j, lag)] = W[idx(i, j, lag)]/total_prob;
                    if (W[idx(i, j, lag)] > map_lag_p) {
                        map_lag_pos = j;
                        map_lag_p = W[idx(i, j, lag)];
                    }
                }
                MAP_lag[i] = map_lag_pos;
                fcst_x[i] = x[i-MAP_lag[i-1]]; // use estimate from t-1
                
                
                /** this is all forcasting stuff outside of the typical workflow
                **/
                
                R.x = fcst_x;
                
                R.linear_model(lag, i);
                
                std::cout << "i=" << i << " map pos=" << map_lag_pos << " p=" << map_lag_p 
                    << " x=" << R.x[i] << " y=" << y[i] << " " << R.alpha << " " 
                    << R.beta << " " << R.R << std::endl; 
                
            }   
        }     
};

