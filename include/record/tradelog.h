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
    void addTrade(std::shared_ptr<Trade> trade) {
        trades.push_back(trade);
    }

    /**
     * Getter for trades
     */
    vector<std::shared_ptr<Trade>> getTrades() const {return trades;}

    /**
     * Compute P&L.
     */
    double computePNL() const {
        double pnl = 0;

        for (auto& it : trades) {
            pnl -= it->getSide() * it->getPrice() * it->getBaseCurrencySize() + it->getFee();
        }

        return pnl;
    }

private:
    vector<std::shared_ptr<Trade>> trades;
};