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

    void AddOrder(double price_level, double order_size, bool is_our_order)
    {
        // Ensure the price level exists in the book.
        AddLevel(price_level);

        if (!is_our_order)
        {
            // Logic for adding a non-our order.
            if (!book[price_level].empty() && !book[price_level].back().second)
            {
                // If the last order in the queue is not our order, combine it.
                book[price_level].back().first += order_size;
            }
            else
            {
                // Otherwise, add it as a new order.
                book[price_level].emplace(order_size, is_our_order);
            }
        }
        else
        {
            // Logic for adding our order.
            if (!book[price_level].empty() && book[price_level].back().second)
            {
                // If the last order is our order, combine it.
                book[price_level].back().first += order_size;
            }
            else
            {
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

    void printOrderBook() const
    {
        cout << exchange.getName() << ", " << marketTypeToString(market_type)
             << ", " << security.getBase() << endl;

        for (const auto &level : book)
        {
            std::cout << "Price Level " << level.first << ": ";
            auto &queue = level.second;
            std::queue<std::pair<double, bool>>
                tempQueue = queue; // Copy to a temporary queue for iteration
            cout << "{";
            while (!tempQueue.empty())
            {
                auto order = tempQueue.front();
                tempQueue.pop();
                cout << "[" << order.first << ", " << order.second << "]";
            }
            cout << "}" << "\n\n";
        }
    }
    // TODO When someone cancel order, positions of order

    static std::string marketTypeToString(MarketType type)
    {
        switch (type)
        {
        case MarketType::Spot:
            return "Spot";
        case MarketType::Futures:
            return "Futures";
        default:
            return "Unknown";
        }
    }

    void CancelOrder(
        double price_level,
        double order_size,
        bool is_our_order
    )
    {
        if (book.find(price_level) == book.end())
        {
            cout << "Price level does not exist." << endl;
            return;
        }

        auto &ordersQueue = book[price_level];
        queue<pair<double, bool>> tempQueue;
        bool orderFound = false;

        // Iterate through the orders to find a match for cancellation
        while (!ordersQueue.empty())
        {
            auto order = ordersQueue.front();
            ordersQueue.pop();

            if (!orderFound && order.first == order_size
                && order.second == is_our_order)
            {
                // If this is the first matching order, don't push it back to
                // the temporary queue (effectively cancel it)
                orderFound = true;
                continue;
            }
            tempQueue.push(order);
        }

        // Replace the original queue with the temporary queue, minus the
        // cancelled order
        book[price_level] = tempQueue;

        if (!orderFound)
        {
            cout << "Order not found for cancellation." << endl;
        }
    }


private:

    Exchange &exchange;
    MarketType market_type;
    Security &security;
    map<double, queue<pair<double, bool>>>
        book; // The book stores orders at each price level
};