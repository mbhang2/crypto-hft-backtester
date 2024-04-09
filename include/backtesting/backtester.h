#pragma once

#include "./order.h"
#include "./strategy.h"
#include "./trade.h"
#include "./user.h"
#include "../data/exchange.h"
#include "../data/security.h"

#include <string>

using namespace std;


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