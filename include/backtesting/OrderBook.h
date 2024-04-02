#include "../data/exchange.h"
#include "order.h"

#include <iostream>
#include <map>
#include <queue>
#include <utility>

using namespace std;

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

    void AddOrder(double price_level, double order_size, bool is_our_order) {
    // Ensure the price level exists in the book.
    AddLevel(price_level);

    if (!is_our_order) {
        // Logic for adding a non-our order.
        if (!book[price_level].empty() && !book[price_level].back().second) {
            // If the last order in the queue is not our order, combine it.
            book[price_level].back().first += order_size;
        } else {
            // Otherwise, add it as a new order.
            book[price_level].emplace(order_size, is_our_order);
        }
    } else {
        // Logic for adding our order.
        if (!book[price_level].empty() && book[price_level].back().second) {
            // If the last order is our order, combine it.
            book[price_level].back().first += order_size;
        } else {
            // Otherwise, add it as a new order.
            book[price_level].emplace(order_size, is_our_order);
        }
    }
}

    
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
    // TODO Priority Queue: maxHeap, minHeap
    // TODO PQ in a big Queue

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
    map<double, queue<pair<double, bool>>> book; // The book stores orders at each price level
};