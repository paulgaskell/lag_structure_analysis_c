
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


#include "Constants.h"
#include "SDM.h"
//#include "DataLoader.h"

int main() {
    
    //Tseries ibm_p(1, "IBM.csv");
    //Tseries ibm_v(6, "IBM.csv");
    
    int N = 300;
    int lag = 50;
    double mu = 0;
    double sig = 1;
    double err = 0.5;
    Coordinates C;
    Constants Const;
    
    SDM sdm(N, lag);
    //sdm.x = ibm_p;
    //sdm.y = ibm_v;
    sdm.generate_series(mu, sig, err);
    sdm.run();
    
    Regression r;
    r.x = sdm.x;
    r.y = sdm.y;
    r.linear_model(0, N);
    std::cout << r.R << std::endl;
    
    r.x = sdm.fcst_x;
    r.linear_model(lag, N);
    std::cout << r.R << std::endl;
    
    r.x = sdm.x;
    r.y = sdm.y_unlagged;
    r.linear_model(lag, N);
    
    std::cout << r.R << std::endl;
    
    return 0;
    
}