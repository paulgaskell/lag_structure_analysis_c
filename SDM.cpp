
/**

N.B.
We are currently using the MAP_lag fcst from t-1 so it is a true model 

TODO:
    - should regression be inherited by SDM?
    - put in SDM path structure 
    - mean forcast as well as MAP forecast 
    - do not forecast on bad days
    - load data from file
    
**/


#include "SDM.h"

int main() {
    
    /** 
    define all variables here
    **/

    int N = 300;
    int lag = 50;
    double mu = 0;
    double sig = 1;
    double err = 0.5;
    const int max_len = 5000;
    const int max_width = 500;
    const unsigned int seed = 1;
    const double pi = 3.14159265359;
    
    std::cout << "start" << std::endl;
    SDM sdm(max_len, max_width, pi, N, lag);
    
    sdm.generate_series(seed, mu, sig, err);
    sdm.run();

    sdm.load_x_series_from_file(1, "IBM.csv");
    
    return 0;
    
}