#pragma once

#include <stdexcept>
#include <vector>
#include "../backtesting/trade.h"

using namespace std;

/**
 * Class for trade log
 */
class TradeLog {
public:
    /**
     * Default constructor
     */
    TradeLog() {}

    /**
     * Destructor
     */
    ~TradeLog() {}

    /**
     * Add trade to the trade list
     * @param trade trade to add
     */
    void addTrade(const Trade& trade) {
        trades.push_back(trade);
    }

    /**
     * Retrieve an trade by ID
     * @param trade_id unique trade id of an trade to retrieve
     * @return reference to the trade
     */
    const Trade& getTradeByID(int trade_id) const {
        for (const Trade& trade : trades) {
            if (trade.getID() == trade_id) {
                return trade;
            }
        }
        
        // Throw an exception when trade not found
        throw runtime_error("trade with ID " + to_string(trade_id) + " not found.");
    }

private:
    std::vector<Trade> trades;
};