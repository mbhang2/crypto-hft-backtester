#pragma once

#include "./order.h"
#include "./strategy.h"
#include "./trade.h"
#include "./user.h"
#include "../data/exchange.h"
#include "../data/security.h"
#include <boost/accumulators/accumulators.hpp>
#include <boost/algorithm/string.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


using namespace std;
using namespace boost;


/**
 * Class for backtesting
 */
class Backtester {
    public:
    /**
     * Constructor
     */
    Backtester(User& user_, Strategy* strategy_): user(user_), strategy(strategy_) {}

    /**
     * Destructor
     */
    ~Backtester() {}

    /**
     * Run backtest
     * 
     * Backtest steps:
     *  1. Open the input data path and read/parse line-by-line
     *  2. While reading line-by-line call strategy functions accordingly
     *  3. Also, update orderbooks and check trade fillabilities
     *  4. Record all trades
     * 
     * @param data_path file path for market data input
     */
    void runBacktest(const string& data_path) {
        ifstream file(data_path);

        if (!file.is_open()) {
            cerr << "Error opening the file." << endl;
            return;
        }

        string line;
        while (getline(file, line)) {
            vector<string> tokens;
            boost::split(tokens, line, boost::is_any_of(","));

            for (auto& it : tokens) {
                cout << it << endl;
            }

            // if (tokens.size() == 5 /*< Change this number accordingly */) {
                // string timestamp = tokens[0];

                /*
                trade_event_message;
                vector<Order> order_vector;

                if (trade_event) {
                    order_vector = strategy.onTrade(trade_event_message);
                } else if (quote_event) {
                    order_vector = strategy.onQuote(trade_event_message);
                } else if (depth_event) {
                    order_vector = strategy.onDepth(trade_event_message);
                } else {
                    continue;
                }
                
                if (order_vector.size() == 0) {
                    continue;
                } else {
                    for (auto& it: order_vector) {
                        // Check if order exceeds buying power
                        
                        // push back to orders waiting to be filled
                    }
                }    
                */
            // }
        }

    }

    private:
    User user;
    Strategy* strategy;
};