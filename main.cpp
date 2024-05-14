#include <iostream>
#include <string>
#include <limits>
#include <cstdlib> // For std::strtol and std::strtof
#include <iomanip>
#include <vector>
#include <thread>
#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <map>
#include <unistd.h>
#include <future>
#include <chrono>  // for time counter
#include "Bootstrap.h"
#include "Ticker.h"
#include "GetPrice.h"
#include "Matrix.h"
#include "plotGnuplot.h"

using namespace std;
using namespace fre;

void DisplayMenu()
{   
    cout << "Menu Display: \n";
    cout << "1 - Enter N to retrieve 2N+1 days of historical price data for all stocks.\n";
    cout << "2 - Pull information for one stock from one group.\n";
    cout << "3 - Show AAR, AAR-STD, CAAR and CAAR-STD for one group.\n";
    cout << "4 - Plot AVG CAAR for 3 groups.\n";
    cout << "5 - Exit.\n";
    cout << "-------------------------------------------------------------------------------\n";
}

int main() {
    
    map<string, double> historical_benchmark_prices;
    map<string, Ticker> miss, meet, beat;
    
    const char* IWV3000SymbolFile = "Russell3000EarningsAnnouncements.csv";
    groupSeperation(IWV3000SymbolFile, miss, meet, beat);
    
    vector<Ticker> missTickers, meetTickers, beatTickers;
    
    for (const auto& item : miss) {
        missTickers.push_back(item.second);
    }
    for (const auto& item : meet) {
        meetTickers.push_back(item.second);
    }
    for (const auto& item : beat) {
        beatTickers.push_back(item.second);
    }
    
    // Declare variables outside the switch statement
    vector<Vector> AAR_avg(3), CAAR_avg(3), AAR_std(3), CAAR_std(3);
    GNUPLOT gnuplot;
    
    char choice;
    bool getData = false;
    int N;
    string input;
    std::thread missThread, meetThread, beatThread;
    vector<vector<vector<double>>> matrix(3, vector<vector<double>>(4));

    do {
        DisplayMenu();
        cout << "User interface: \n";
        cout << "Enter your choice and press return: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << endl;

        switch(choice) {
            case '1': {
                while(true) {
                    cout << "Enter N (where 60 <= N <= 90): ";
                    getline(cin, input);  // Read the entire line as string
                    char* end;  // Pointer to the next character after the integer
                    N = strtol(input.c_str(), &end, 10);  // Try to convert string to integer
                    if (*end != '\0') {
                        cout << "[Choice 1]: invalid user input. Please try again." << endl;
                    }
                    else if (N < 60 || N > 90) {
                        cout << "[Choice 1]: date out of range. Please try again." << endl;
                    }
                    else {
                        break;  // Valid input
                    }
                }
                auto start = chrono::high_resolution_clock::now(); // Start timing
                FetchBenchmarkPrices("IWV", "2023-04-01", "2024-05-31", historical_benchmark_prices);
                missThread = std::thread(FetchPrices, std::ref(missTickers), std::ref(historical_benchmark_prices), N);
                meetThread = std::thread(FetchPrices, std::ref(meetTickers), std::ref(historical_benchmark_prices), N);
                beatThread = std::thread(FetchPrices, std::ref(beatTickers), std::ref(historical_benchmark_prices), N);
        
                missThread.join();
                meetThread.join();
                beatThread.join();
                
                cout << "[Choice 1]: Get all data."<<endl;
                auto end = chrono::high_resolution_clock::now(); // End timing
                chrono::duration<double> elapsed = end - start; // Calculate elapsed time
                cout << "[Choice 1]: Elapsed time for getting data: " << elapsed.count() << " seconds." << endl;
                
                getData = true;
                Bootstrap(missTickers, meetTickers, beatTickers, AAR_avg, CAAR_avg, AAR_std, CAAR_std);
                
                
                for(int n = 0; n < 3; n++) {
                    matrix[n][0] = AAR_avg[n];
                    matrix[n][1] = CAAR_avg[n];
                    matrix[n][2] = AAR_std[n];
                    matrix[n][3] = CAAR_std[n];
                }
                
                cout << "[Choice 1]: finish choice 1. Returning to menu..." << endl << endl;
                break;
            }

            case '2': {
                if (!getData) {
                    cout << "[Choice 2]: data have not been fetched or processed. Please try option 1 first. Returning to menu..." << endl<< endl;
                    break;
                }
                
                string inputSymbol;
                bool found = false;
                Ticker found_ticker;
                
                while(!found) {
                    cout << "Enter ticker symbol: ";
                    cin >> inputSymbol;
                    transform(inputSymbol.begin(), inputSymbol.end(), inputSymbol.begin(), ::toupper);
                    cout << endl;
                    
                    std::array<const std::vector<Ticker>*, 3> tickerLists = {&missTickers, &meetTickers, &beatTickers};
                    
                    for (const auto& list : tickerLists) {
                        for (const auto& ticker : *list) {
                            if (ticker.GetSymbol() == inputSymbol) {
                                found = true;
                                found_ticker = ticker;
                                break;
                            }
                        }
                        if (found) {
                            break; // Break the outer loop if ticker is found
                        }
                    }
                
                    if (found) {
                        cout << "ticker: " << found_ticker.GetSymbol() << endl;
                        cout << "date: " << found_ticker.GetDate() << endl;
                        cout << "period_ending: " << found_ticker.GetEndPeriod() << endl;
                        cout << "estimate: " << found_ticker.GetEstimate() << endl;
                        cout << "reported: " << found_ticker.GetReported() << endl;
                        cout << "surprise: " << found_ticker.GetSurprise() << endl;
                        cout << "surprise%: " << found_ticker.GetSurprisePercent() << endl;
                        cout << "in group: " << found_ticker.GetGroup() << endl;
                        cout << endl;
                        cout << "ajusted close: " << endl;
                        Vector prices = found_ticker.GetAdjClosePrices();
                        cout << prices << endl;
                        cout << endl;
                        cout << "cumulative returns: " << endl;
                        Vector cumReturns = found_ticker.GetCumReturns();
                        cout << cumReturns << endl;
                        cout << endl;
                        cout << "abnormal returns: " << endl;
                        Vector abnormalReturns = found_ticker.GetabnormalReturns();
                        cout << abnormalReturns << endl;
                        cout << endl;
                    } else {
                        cout << "[Choice 2]: invalid user input. Please try again." << endl;
                    }
                }
                
                cout <<"[Choice 2]: finish choice 2. Returning to menu..."  << endl << endl;
                break;
            }   
            
            case '3':
            {
                if (!getData) 
                {
                    cout << "[Choice 3]: data have not been fetched or processed. Please try option 1 first. Returning to menu..." << endl<< endl;
                    break;
                }
                string group;
                do {
                    cout << "Enter group(Beat/Meet/Miss): ";
                    cin >> group;
                    if (group == "Beat")
                    {
                        cout << "AAR_avg: " << endl;
                        cout << matrix[0][0] << endl;
                        cout << "AAR_std: " << endl;
                        cout <<  matrix[0][2] << endl;
                        
                        cout << "CAAR_avg: " << endl;
                        cout <<  matrix[0][1] << endl;
                        cout << "CAAR_std: " << endl;
                        cout <<  matrix[0][3] << endl;
                    }
                    else if (group == "Meet")
                    {
                        cout << "AAR_avg: " << endl;
                        cout << matrix[1][0] << endl;
                        cout << "AAR_std: " << endl;
                        cout << matrix[1][2] << endl;
                        
                        cout << "CAAR_avg: " << endl;
                        cout << matrix[1][1] << endl;
                        cout << "CAAR_std: " << endl;
                        cout << matrix[1][3] << endl;
                    }
                    else if (group == "Miss")
                    {
                        cout << "AAR_avg: " << endl;
                        cout << matrix[2][0] << endl;
                        cout << "AAR_std: " << endl;
                        cout << matrix[2][2] << endl;
                        
                        cout << "CAAR_avg: " << endl;
                        cout << matrix[2][1] << endl;
                        cout << "CAAR_std: " << endl;
                        cout << matrix[2][3] << endl;
                    }
                    else { cout << "[Choice 3]: invalid user input. Please try again." << endl; }
                } while((group != "Beat") & (group != "Meet") & (group != "Miss"));
                cout << "[Choice 3]: finish choice 3. Returning to menu..." << endl << endl;
                break;
            }   
            case '4':
                if (!getData) 
                {
                    cout << "[Choice 4]: data have not been fetched or processed. Please try option 1 first. Returning to menu..."<< endl<< endl;
                    break;
                }
                // cout <<CAAR_avg[0];
                gnuplot.SetBeat(matrix[0][1]);
                gnuplot.SetMeet(matrix[1][1]);
                gnuplot.SetMiss(matrix[2][1]);
                gnuplot.plot();
                cout << "[Choice 4]: finish choice 4. Returning to menu..." << endl << endl;
                break;
            case '5':
            {
                cout << "[Choice 5]: Exiting..." << endl;
                break;
            }
            default:
                std::cout << "[Menu]: invalid user input. Please try again.\n";
        }
    } while(choice != '5');

    return 0;
}