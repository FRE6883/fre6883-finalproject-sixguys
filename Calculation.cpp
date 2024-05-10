#include <iostream>
#include <map>
#include <vector>
#include <string>

#include <cmath>
#include "Calculation.h"
using namespace std;

namespace fre {
    void Calculation::CalculateAAR(vector<Ticker>& tickers) {
        int numTickers = tickers.size();
        int numReturns = tickers[0].GetabnormalReturns().size();
    
        AAR.resize(numReturns, 0.0); // Vector 2N
    
        // cout << "RETURNS SIZE: " << numReturns << endl;
        // cout << "NUMBER OF STOCKS: " << numTickers << endl;
    
        for (const auto& ticker : tickers) {
            const auto& returns = ticker.GetabnormalReturns(); // return is a Vector(2N)
            
            if (returns.size() != (unsigned int)numReturns) {
                cout << "Drop" << ticker.GetSymbol() << endl;
                continue;
            }
    
            for (size_t j = 0; j < returns.size(); ++j) {
                AAR[j] += returns[j];
            }
        }
    
        for (double& aar : AAR) {
            aar /= numTickers;
        }
    }

    void Calculation::CalculateCAAR() {
        CAAR.resize(AAR.size(), 0.0);
        if (!CAAR.empty()) {
            CAAR[0] = AAR[0];  
            for (size_t i = 1; i < CAAR.size(); ++i) {
                CAAR[i] = CAAR[i - 1] + AAR[i];
            }
        }
    }
}