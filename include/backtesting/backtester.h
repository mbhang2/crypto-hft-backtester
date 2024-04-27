#pragma once

#include "./order.h"
#include "./strategy.h"
#include "./trade.h"
#include "./user.h"
#include "../data/exchange.h"
#include "../data/security.h"
#include <boost/accumulators/accumulators.hpp>
#include <boost/algorithm/string.hpp>

#include <algorithm>
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
        vector<std::shared_ptr<Order>> current_orders;
        vector<TimeType*> timetype_vector;
        ifstream file(data_path);

        if (!file.is_open()) {
            cerr << "Error opening the file." << endl;
            return;
        }

        int n = 2;
        string line;
        getline(file, line);    // Skip first line
        while (getline(file, line)) {
            vector<string> tokens;
            boost::split(tokens, line, boost::is_any_of(","));

            // Finding Exchange
            std::shared_ptr<Exchange> exchange_ptr = user.findExchange(tokens[4]);

            if (exchange_ptr == nullptr) {
                throw runtime_error("Exchange " + tokens[4] + " is not found");
            }

            // Finding Security
            std::shared_ptr<Security> security_ptr = exchange_ptr->findSecurity(MarketType::Spot, tokens[3]);

            if (security_ptr == nullptr) {
                throw runtime_error("Security " + tokens[3] + " is not found");
            }

            // Calling strategy functions
            vector<std::shared_ptr<Order>> order_vector;
            MarketType s = MarketType::Spot;
            TimeType* tt = new TimeType(tokens[0]);
            timetype_vector.push_back(tt);

            if (tokens[2] == "T") {
                // TradeEventMsg msg(tokens[0], *exchange_ptr, tokens[5] == "S" ? MarketType::Spot : MarketType::Futures, *security_ptr, stod(tokens[6]), stod(tokens[7]));
                TradeEventMsg msg(tt, exchange_ptr, s, security_ptr, stod(tokens[6])*180, stod(tokens[7]));
                order_vector = strategy->onTrade(msg);
            }

            else if (tokens[2] == "Q") {
                // QuoteEventMsg msg(tokens[0], *exchange_ptr, tokens[5] == "S" ? MarketType::Spot : MarketType::Futures, *security_ptr, stod(tokens[8]), stod(tokens[9]), stod(tokens[10]), stod(tokens[11]));
                QuoteEventMsg msg(tt, exchange_ptr, s, security_ptr, stod(tokens[8]), stod(tokens[9]), stod(tokens[10]), stod(tokens[11]));
                // order_vector = strategy->onTopQuote(msg);
                // @todo: update orderbook
                cout << order_vector.size() << endl;
            }

            else if (tokens[2] == "D") {
    
            }

            // Append order to the back of the current order
            if (!order_vector.empty()) {

                for (auto&& it : order_vector) {
                    // Security s = it->getSecurity();
                    // cout << "Address while going over for loop: " << &s << endl;
                    // string s = it->getTimestamp();
                    current_orders.push_back(it);
                }
            }

            // Work with orders
            for (auto&& it : current_orders) {
                if (it->isLiveOrder()) {
                    double size_to_fill = it->getLeverageAdjustedBaseCurrencySize();
                    it->fillOrder(size_to_fill, stod(tokens[6])*180);
                    
                    if (it->getSide() == 1) {
                        strategy->updatePosition(it->getMarketType(), *exchange_ptr, *security_ptr, size_to_fill);
                    } else {
                        strategy->updatePosition(it->getMarketType(), *exchange_ptr, *security_ptr, -size_to_fill);
                    }
                    
                }
                 if (it->getOrderState() == OrderState::SentToExchange) {
                    it->checkOrderReceived(*tt);
                    if (it->getOrderState() != OrderState::Working) {
                        break;
                    }
                }
            }

            // Remove not-live orders
            for (auto it = current_orders.begin(); it != current_orders.end();) {
                if (!(*it)->isLiveOrder() && !((*it)->getOrderState() == OrderState::SentToExchange)) {
                    it = current_orders.erase(it);
                    cout << "Remove not live orders" << endl;
                    cout << "Length of current orders after removing not live orders is " << current_orders.size() << endl;
                } else {
                    ++it;
                }
            }

            // cout << "Done" << endl;

            // for (auto&& it : current_orders) {
            //     string s = (it->getSide() == 1) ? "Buy" : "Sell";
            //     cout << "========== Order Information ==========" << endl;
            //     cout << "Security: " << *(it->getSecurity()) << endl;
            //     cout << "Exchange: " << it->getExchange()->getName() << endl;
            //     cout << "Timestamp: " << it->getTimestamp().toString() << endl;
            //     cout << "Order Type: " << it->getOrderType() << endl;
            //     cout << "Side: " << s << endl;
            //     cout << "Size: " << it->getBaseCurrencySize() << it->getSecurity()->getBase() << endl;
            //     cout << "Margin Type: " << it->getMarginType() << endl;
            //     cout << "Leverage: " << it->getLeverage() << "x" << endl;
            //     cout << "Leverage Adjusted Size: " << it->getLeverageAdjustedBaseCurrencySize() << it->getSecurity()->getBase() << endl;
            //     cout << "Order State: " << it->getOrderState() << endl;
            //     cout << "=======================================" << endl;
            // }
        }
    }

    private:
    User user;
    Strategy* strategy;
};