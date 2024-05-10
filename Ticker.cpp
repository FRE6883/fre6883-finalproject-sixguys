#include <algorithm>  // Add this header for std::sort
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <cstring>
#include <cmath>
#include <string> 
#include <locale>
#include <iomanip>
#include "curl/curl.h"
#include "Ticker.h"

using namespace std;
namespace fre{

    Vector Ticker::Returns(){
        Vector log_returns;
        for (long unsigned int i = 0; i < adjClosePrices.size() - 1; i++){
            double value = log(adjClosePrices[i+1]/adjClosePrices[i]);
            log_returns.push_back(value);
        }
        return log_returns;
    }
    
    Vector Ticker::CumReturns(){
        Vector returns = Returns();
        double cumReturn = 0.0;
        int n = returns.size();
        Vector CumReturns(n);
        for (int i = 0; i < n; i++){
            cumReturn += returns[i];
            CumReturns[i] +=  cumReturn;
        }
        return CumReturns;
    }
    
    Vector Ticker::AbnormalReturns(Vector& benchmarkReturns){
        abnormalReturns = Returns() - benchmarkReturns;
        return abnormalReturns;
    }
}