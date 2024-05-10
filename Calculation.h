#include <iostream>
#include <map>
#include <vector>
#include <string>
#include<cmath>
#include "Ticker.h"
#include "Matrix.h"

using namespace std;

namespace fre {
    class Calculation {
        private:
            Vector AAR;
            Vector CAAR;
        
        public:
            void CalculateAAR(vector<Ticker>& tickers);
            void CalculateCAAR();
            Vector GetAAR() const {return AAR;}
            Vector GetCAAR() const {return CAAR;}
    };
}