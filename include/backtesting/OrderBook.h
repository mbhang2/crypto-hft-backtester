#include "../data/exchange.h"
#include "order.h"

#include <iostream>
#include <map>
#include <queue>
#include <utility>

using namespace std;

/**
 * Map<double (price level, Ex: 60500.37), queue<pair<double (size), bool (is it
 * our order?)>>
 * Binance Spot BTC/USD
 * price_level:{[6,f],[1,true],[5,f]...}
 */
class Orderbook
{
public:

    Orderbook(Exchange &exchange_, MarketType market_type_, Security &security_)
        : exchange(exchange_)
        , market_type(market_type_)
        , security(security_)
    {}

    void AddLevel(double price_level)
    {
        // If the level does not exist, create it with an empty queue
        if (book.find(price_level) == book.end())
        {
            book[price_level] = queue<pair<double, bool>>();
        }
    }

    void AddLevel(double price_level, double order_size, bool is_our_trade)
    {
        AddLevel(price_level); // Ensure the level exists
        book[price_level].emplace(order_size, is_our_trade);
    }

    void RemoveLevel(double price_level)
    {
        // If the level exists, remove it
        if (book.find(price_level) != book.end())
        {
            book.erase(price_level);
        }
    }
    /**
     * Add order at a certain price level
     *
     * Cases to consider:
     *
     * 1. Our order does not exist at this price level --> Change the order size
     * 2. Our order exist at this price level and is the last one (last index of
     * queue is our order) --> We have to add new "block/pair" at the end of the
     * queue
     * 3. Our order exist at this price level, but is not the last one --> Only
     * modify the order size of the last pair
     *
     * Useful to have a helper function that is only for modifying order size
     * given price level and size to add/subtract
     */
    void AddOrder(double price_level, double order_size, bool is_our_order)
    {
        // Ensure the price level exists
        // TODO Combine rest orders
        AddLevel(price_level);

        if (book[price_level].empty() || !book[price_level].back().second
            || !is_our_order)
        {
            // Case 1 & 3
            book[price_level].emplace(order_size, is_our_order);
        }
        else
        {
            // Case 2
            auto &last_order = book[price_level].back();
            last_order.first += order_size; // Modify the size
        }
    }
    /**
     * Subtract trade size from the front of the queue
     * If trade size is greater than the front pair order size, since it is 0
     * now, pop the front pair and then resume subtracting
     */
    void TradeHappened(double price_level, double trade_size)
    {
        if (book.find(price_level) != book.end())
        {
            auto &queue = book[price_level];

            while (!queue.empty() && trade_size > 0)
            {
                auto &front_order = queue.front();
                if (trade_size >= front_order.first)
                {
                    trade_size -= front_order.first;
                    queue.pop();
                }
                else
                {
                    front_order.first -= trade_size;
                    trade_size = 0;
                }
            }
        }
    }

    void printOrderBook() const {

        cout << exchange.getName() << ", " << marketTypeToString(market_type) << ", " << security.getBase() << endl;

        for (const auto& level : book) {
            std::cout << "Price Level " << level.first << ": ";
            auto& queue = level.second;
            std::queue<std::pair<double, bool>> tempQueue = queue; // Copy to a temporary queue for iteration
            cout << "{";
            while (!tempQueue.empty()) {
                auto order = tempQueue.front();
                tempQueue.pop();
                cout << "[" << order.first << ", " << order.second << "]";
                
            }
            cout << "}" << "\n\n";
        }
    }
    // TODO When someone cancel order, positions of order
    // Input: series of order; Output: profits

    static std::string marketTypeToString(MarketType type) {
    switch (type) {
        case MarketType::Spot: return "Spot";
        case MarketType::Futures: return "Futures";
        default: return "Unknown";
    }
}


private:

    Exchange &exchange;
    MarketType market_type;
    Security &security;
    map<double, queue<pair<double, bool>>>
        book; // The book stores orders at each price level
};