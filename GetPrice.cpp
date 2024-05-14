#include <stdio.h>
#include <string.h>
#include <cstring>
#include <string> 
#include <iostream>
#include <sstream>  
#include <vector>
#include <locale>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <functional>
#include <map>
#include "curl/curl.h"
#include "GetPrice.h"


using namespace std;
namespace fre {
    vector<string> GetDates(map<string, double> priceMap)
    {
        vector<string> dates;
        for (map<string, double>::iterator itr = priceMap.begin(); itr != priceMap.end(); itr++){
            dates.push_back(itr->first);
        }
        return dates;
    }
    
    vector<double> GetPrices(map<string, double> priceMap)
    {
        vector<double> prices;
        for (map<string, double>::iterator itr = priceMap.begin(); itr != priceMap.end(); itr++){
            prices.push_back(itr->second);
        }
        return prices;
    }

    vector<double> BenchmarkRet(string startDate, string endDate, map<string, double>& benchmark)
    {
	    vector<double> prices;
        map<string, double>::iterator start = benchmark.find(startDate);
        map<string, double>::iterator end = benchmark.find(endDate);
        
        if (start != benchmark.end() && end != benchmark.end()) {
            for (map<string, double>::iterator itr = start; itr != benchmark.end(); itr++) {
                prices.push_back(itr->second);
                if (itr == end) break;
            }
        }
	    
	    vector<double> returns;
	    int size = prices.size();
        for (int i = 0; i < size - 1; i++) {
            returns.push_back(log(prices[i+1]/prices[i]));
        }
        
	    return returns;
    }
    
    string startDay(vector<string> tradingDays, string day0, int N)
    {
        auto itr = find(tradingDays.begin(), tradingDays.end(), day0);
        if (itr == tradingDays.end()) {
            cout << "Trading day not found: " << day0 << endl;
            return "ERROR";  
        }
    
        if (itr - tradingDays.begin() < N) {
            cout << "Insufficient data" << endl;
            return "ERROR";
        }
    
        return *(itr - N);  // Directly return the date that is N days before day0
    }
    
    string endDay(vector<string> tradingDays, string day0, int N)
    {
        auto itr = find(tradingDays.begin(), tradingDays.end(), day0);
        if (itr == tradingDays.end()) {
            cout << "Trading day not found: " << day0 << endl;
            return "ERROR";  // Indicating failure by returning an empty string
        }
    
        if (itr + N >= tradingDays.end()) {
            return "ERROR";  // Not enough days after day0 to fulfill the request
        }
    
        return *(itr + N);  // Directly return the N-th trading day after day0
    }
    
    string convert_date(string date) {
        int dash1 = date.find('-');  
        int dash2 = date.rfind('-'); 
    
        string day = date.substr(0, dash1);
        if (day.length() == 1){
            day = "0" + day;
        }
        
        string month = date.substr(dash1 + 1, dash2 - dash1 - 1);
        // Convert month to title case
        month[0] = toupper(month[0]);
        for (size_t i = 1; i < month.size(); i++) {
            month[i] = tolower(month[i]);
        }
    
        int m = 0;
        string months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                             "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    
        for (int i = 0; i < 12; i++){
            if (months[i] == month){
                m = i + 1;
                break;
            }
        }
        month = to_string(m);
        if (month.size() == 1){
            month = "0" + month;
        }
        
        string year = date.substr(dash2 + 1);
        year = "20" + year;
    
        return year + "-" + month + "-" + day;
    }

    
    
    void* myrealloc(void* ptr, size_t size)
    {
    	if (ptr)
    		return realloc(ptr, size);
    	else
    		return malloc(size);
    }
    
    int write_data2(void* ptr, size_t size, size_t nmemb, void* data)
    {
    	size_t realsize = size * nmemb;
    	struct MemoryStruct* mem = (struct MemoryStruct*)data;
    	mem->memory = (char*)myrealloc(mem->memory, mem->size + realsize + 1);
    	if (mem->memory) {
    		memcpy(&(mem->memory[mem->size]), ptr, realsize);
    		mem->size += realsize;
    		mem->memory[mem->size] = 0;
    	}
    	return realsize;
    }
    
    int FetchPrices(vector<Ticker>& tickers, map<string, double>& benchmark, int N) {
        // declare a pointer of CURL
        CURL* handle;
		CURLcode status;
		
		// set up the program environment that libcurl needs 
    	curl_global_init(CURL_GLOBAL_ALL);
    
		// curl_easy_init() returns a CURL easy handle 
 		handle = curl_easy_init();
 		
 		// if everything's all right with the easy handle...
 		if (handle)
 		{	
 			vector<string> tradingDays;
    		tradingDays = GetDates(benchmark);
 			string url_common = "https://eodhistoricaldata.com/api/eod/";
 			// You must replace this API token with yours 
 			string api_token = "6622179d9d7205.32281540";
 	
 			
 			for (vector<Ticker>::iterator itr = tickers.begin(); itr != tickers.end(); itr++) {
 				
 				string day0 = convert_date(itr->GetDate());
 				string startDate = startDay(tradingDays, day0, N);
 				string endDate = endDay(tradingDays, day0, N);
 				
 				if (startDate == "ERROR" || endDate == "ERROR"){
 				    cout << "Insufficient data for " << itr->GetSymbol() << endl;
 				    continue;
 				}
    			else {
        			struct MemoryStruct data;
     				data.memory = NULL;
     				data.size = 0;
     				string url_request = url_common + itr->GetSymbol() + ".US?" + "from="
    					+ startDate + "&to=" + endDate + "&api_token="
    					+ api_token + "&period=d";
    				curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());
    				
    				//adding a user agent
    				curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
    				curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0); 
    				curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
    				curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2); 
    				curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data); 
    				status = curl_easy_perform(handle);
    			
    				// check for errors
    				if (status != CURLE_OK) {
    					cout << "curl_easy_perform() failed: " << curl_easy_strerror(status) << endl;
    					return 1;
    				}
    			
    				stringstream sData;
    				sData.str(data.memory);
    				string sDate, sAdjustedClose, line;
    				double dAdjustedClose;

    				map<string, double> historical_date_prices;
    				
    				while (getline(sData, line)) {
    					size_t found = line.find('-');
    					if (found != std::string::npos) {
    						sDate = line.substr(0, line.find_first_of(','));
    						line.erase(line.find_last_of(','));
    				    	sAdjustedClose = line.substr(line.find_last_of(',') + 1);
    				    	dAdjustedClose = strtod(sAdjustedClose.c_str(), NULL);
    				    	historical_date_prices[sDate] = dAdjustedClose;
    					}
    				}
    				itr->SetAdjClosePrices(GetPrices(historical_date_prices));
    				itr->SetCumReturns(itr->CumReturns()); //calculate return by CumReturns and update by SetCumReturns
    				auto benchmarkReturns = BenchmarkRet(startDate, endDate, benchmark);
    				auto abnormReturns = itr->AbnormalReturns(benchmarkReturns); 
                    itr->SetabnormalReturns(abnormReturns);
    				free(data.memory);
    			    data.size = 0;
    			}
				
 			}
 		} 
 		else {
    		cout << "Curl init failed!" << endl;
    		return -1;
 		}
 		
 		// cleanup what is created by curl_easy_init
 		curl_easy_cleanup(handle);
 		
	    // release resources acquired by curl_global_init()
		curl_global_cleanup();
		return 0;
	}
	
	int FetchBenchmarkPrices(string symbol, string start_date, string end_date, map<string, double>& historical_date_prices){
		// declare a pointer of CURL
		CURL* handle;
		CURLcode status;
		
		// set up the program environment that libcurl needs 
    	curl_global_init(CURL_GLOBAL_ALL);
    
		// curl_easy_init() returns a CURL easy handle 
 		handle = curl_easy_init();
 		
 		// if everything's all right with the easy handle...
 		if (handle)
 		{
 			string url_common = "https://eodhistoricaldata.com/api/eod/";
 			// You must replace this API token with yours 
 			string api_token = "662166cb8e3d13.57537943";
 			
 			struct MemoryStruct data;
 			data.memory = NULL; 
 			data.size = 0;
 			string url_request = url_common + symbol + ".US?" + "from="
				+ start_date + "&to=" + end_date + "&api_token="
				+ api_token + "&period=d";
			
			curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());
			//adding a user agent
			curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0); 
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2); 
			curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data); 
			status = curl_easy_perform(handle);
			
			if (status != CURLE_OK) {
				cout << "curl_easy_perform() failed: " << curl_easy_strerror(status) << endl;
				return 1;
			}
			
			stringstream sData;
			sData.str(data.memory);
			string sDate;
			string sAdjustedClose;
			double dAdjustedClose;
			string line;
			while (getline(sData, line)) {
				size_t found = line.find('-');
				if (found != std::string::npos) {
					sDate = line.substr(0, line.find_first_of(','));
					line.erase(line.find_last_of(','));
				    sAdjustedClose = line.substr(line.find_last_of(',') + 1);
				    dAdjustedClose = strtod(sAdjustedClose.c_str(), NULL);
				    historical_date_prices[sDate] = dAdjustedClose;
				}
			}
			free(data.memory);
			data.size = 0;
 		} else {
 			cout << "Curl not initialized. Cannot fetch prices." << endl;
 		    return -1;
 		}
 		
 		// cleanup since you've used curl_easy_init 
 		curl_easy_cleanup(handle);
		// release resources acquired by curl_global_init() 
		curl_global_cleanup();
		return 0;
	}
    
	
	void groupSeperation(const char* cIWB3000SymbolFile, map<string, Ticker>& miss, map<string, Ticker>& meet, map<string, Ticker>& beat) 
	{
        ifstream fin(cIWB3000SymbolFile);
        string line;
        vector<Ticker> tickers;
    
        // Read and parse each line into Ticker objects
        getline(fin, line); // Skip header if there is one
        while (getline(fin, line)) {
            stringstream ss(line);
            string symbol, date, period_ending, estimate_string, reported_string, surprise_string, surprise_percent_string;
            getline(ss, symbol, ',');
            getline(ss, date, ',');
            getline(ss, period_ending, ',');
            getline(ss, estimate_string, ',');
            double estimate = stod(estimate_string);
            getline(ss, reported_string, ',');
            double reported = stod(reported_string);
            getline(ss, surprise_string, ',');
            double surprise = stod(surprise_string);
            getline(ss, surprise_percent_string);
            double surprise_percent = stod(surprise_percent_string);
    
            Ticker ticker(symbol, date, period_ending, estimate, reported, surprise, surprise_percent, "");
            tickers.push_back(ticker);
        }
    
        // Sort tickers by surprise percent
        sort(tickers.begin(), tickers.end(), [](const Ticker& a, const Ticker& b) {
            return a.GetSurprisePercent() > b.GetSurprisePercent();
        });
    
        // Distribute tickers into three maps
        size_t size = tickers.size();
        size_t third = size / 3;
        
        for (size_t i = 0; i < size; ++i) {
            if (i < third) {
                tickers[i].SetGroup("beat");
                beat[tickers[i].GetSymbol()] = tickers[i];
            } else if (i < 2 * third) {
                tickers[i].SetGroup("meet");
                meet[tickers[i].GetSymbol()] = tickers[i];
            } else {
                tickers[i].SetGroup("miss");
                miss[tickers[i].GetSymbol()] = tickers[i];
            }
        }
    }
}