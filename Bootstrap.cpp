#include "Bootstrap.h"

#include <vector>
#include <string>
#include <iostream>
#include <algorithm> // for std::shuffle
#include <random> 
#include <cmath>

namespace fre {

    vector<Ticker> RandomlySelectTickers(const vector<Ticker>& tickers, size_t count) {
        vector<Ticker> selected_tickers = tickers; // Copy the original list
        random_device rd;
        mt19937 g(rd());
        
        // Shuffle the vector using the random generator
        shuffle(selected_tickers.begin(), selected_tickers.end(), g);
        
        // Ensure we don't exceed the available tickers
        if (count > selected_tickers.size()) {
            count = selected_tickers.size();
        }
    
        return vector<Ticker>(selected_tickers.begin(), selected_tickers.begin() + count);
    }


 void Bootstrap(vector<Ticker>& miss_tickers, vector<Ticker>& meet_tickers, vector<Ticker>& beat_tickers,
                   vector<Vector>& AAR_avg, vector<Vector>& CAAR_avg,
                   vector<Vector>& AAR_std, vector<Vector>& CAAR_std)
    {
        // 40 times sampling
        vector<vector<Vector>> AAR(40, vector<Vector>(3));
        vector<vector<Vector>> CAAR(40, vector<Vector>(3));
        
        // Calculation objects
        vector<Calculation> calc_beat(40), calc_meet(40), calc_miss(40); // 3 group each have 40 round
        vector<vector<Calculation>> calc_groups = {calc_beat, calc_meet, calc_miss};
        vector<vector<Ticker>> group_tickers = {beat_tickers, meet_tickers, miss_tickers};
        vector<string> group_names = {"beat", "meet", "miss"};
        
        for (unsigned int n = 0; n < 40; n++) {
        cout << n+1 << "th loop start" << endl; // n = round, i = group

        // Loop over each group (beat, meet, miss)
            for (int i = 0; i < 3; i++) {
                vector<Ticker> chosen_group = RandomlySelectTickers(group_tickers[i], 30);
                calc_groups[i][n].CalculateAAR(chosen_group); // each group, each round
                calc_groups[i][n].CalculateCAAR();
                AAR[n][i] = calc_groups[i][n].GetAAR();
                CAAR[n][i] = calc_groups[i][n].GetCAAR();
                cout << "finish " << group_names[i] << endl;
            }
        }

    
        // Calculate average and standard deviation of AAR and CAAR
        cout << "start calculation" << endl;

        for (int i = 0; i < 40; i++) {
            for (int j = 0; j < 3; j++) {
                if (i == 0) {
                    AAR_avg[j] = AAR[i][j];
                    CAAR_avg[j] = CAAR[i][j];
                    AAR_std[j] = AAR[i][j] * AAR[i][j];
                    CAAR_std[j] = CAAR[i][j] * CAAR[i][j];
                } else {
                    AAR_avg[j] = AAR_avg[j] + AAR[i][j];
                    CAAR_avg[j] = CAAR_avg[j] + CAAR[i][j];
                    AAR_std[j] = AAR_std[j] + AAR[i][j] * AAR[i][j];
                    CAAR_std[j] = CAAR_std[j] + CAAR[i][j] * CAAR[i][j];
                }
            }
        }
        
        for (int j = 0; j < 3; j++) {
            AAR_avg[j] = AAR_avg[j] / 40.0;
            CAAR_avg[j] = CAAR_avg[j] / 40.0;
            for (size_t k = 0; k < AAR[0][j].size(); k++) {
     
                AAR_std[j][k] = sqrt(AAR_std[j][k] / 40.0 - AAR_avg[j][k] * AAR_avg[j][k]);
                CAAR_std[j][k] = sqrt(CAAR_std[j][k] / 40.0 - CAAR_avg[j][k] * CAAR_avg[j][k]);
            }
        }
    }
}