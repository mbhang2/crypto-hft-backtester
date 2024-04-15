#pragma once

#include "./order.h"
#include "./strategy.h"
#include "./trade.h"
#include "./user.h"
#include "../data/exchange.h"
#include "../data/security.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/algorithm/string.hpp>

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
    Backtester(User& user_, Strategy& strategy_): user(user_), strategy(strategy_) {}

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


    }

    private:
    User user;
    Strategy strategy;
};