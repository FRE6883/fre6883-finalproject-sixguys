#include <stdio.h>
#include <cstring>
#include <string> 
#include <iostream>
#include <sstream>  
#include <vector>
#include <cmath>
#include <locale>
#include <iomanip>
#include <fstream>
#include "curl/curl.h"
#include "Matrix.h"
#include "Ticker.h"


using namespace std;

namespace fre {
    
    struct MemoryStruct {
    	char* memory;
    	size_t size;
    };
    vector<string> GetDates(map<string, double> priceMap);
    vector<double> GetPrices(map<string, double> priceMap);
    vector<double> CalcBenchmarkReturn(string startDate, string endDate, map<string, double>& benchmark);
    string startDay(vector<string> tradingDays, string day0, int N);
    string endDay(vector<string> tradingDays, string day0, int N);
    string convert_date(string date); 
    void* myrealloc(void* ptr, size_t size);
    int write_data2(void* ptr, size_t size, size_t nmemb, void* data);
    int FetchPrices(vector<Ticker>& tickers, map<string, double>& benchmark, int N);
    int FetchBenchmarkPrices(string symbol, string start_date, string end_date, map<string, double>& historical_date_prices);
    void groupSeperation(const char* cIWB3000SymbolFile, map<string, Ticker>& miss, map<string, Ticker>& meet, map<string, Ticker>& beat);
}