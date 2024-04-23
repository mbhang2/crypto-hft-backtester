#include "../data/exchange.h"
#include "order.h"

#include <iostream>
#include <map>
#include <queue>
#include <utility>

using namespace std;

enum class OrderSide { Buy, Sell };

class Orderbook {
public:
    Orderbook(Exchange &exchange_, MarketType market_type_, Security &security_)
        : exchange(exchange_), market_type(market_type_), security(security_) {}

    virtual ~Orderbook() {}

    void AddLevel(double price_level, OrderSide side) {
        pair<double, OrderSide> levelKey = {price_level, side};
        if (book.find(levelKey) == book.end()) {
            book[levelKey] = queue<pair<double, bool>>();
        }
    }

    void AddOrder(double price_level, double order_size, bool is_our_order, OrderSide side) {
        AddLevel(price_level, side); // Ensure the level exists
        pair<double, OrderSide> levelKey = {price_level, side};
        book[levelKey].emplace(order_size, is_our_order);
    }

    void RemoveLevel(double price_level, OrderSide side) {
        pair<double, OrderSide> levelKey = {price_level, side};
        if (book.find(levelKey) != book.end()) {
            book.erase(levelKey);
        }
    }

    void TradeHappened(double price_level, double trade_size, OrderSide side) {
        pair<double, OrderSide> levelKey = {price_level, side};
        if (book.find(levelKey) != book.end()) {
            auto &queue = book[levelKey];

            while (!queue.empty() && trade_size > 0) {
                auto &front_order = queue.front();
                if (trade_size >= front_order.first) {
                    trade_size -= front_order.first;
                    queue.pop();
                } else {
                    front_order.first -= trade_size;
                    trade_size = 0;
                }
            }
        }
    }

    bool isEmpty() const {
        return book.empty();
    }

    map<pair<double, OrderSide>, queue<pair<double, bool>>>& getBook() {
        return book;
    }

    void printOrderBook() const {
        cout << exchange.getName() << ", " << marketTypeToString(market_type)
             << ", " << security.getBase() << endl;

        for (const auto &level : book) {
            auto side = level.first.second == OrderSide::Buy ? "Buy" : "Sell";
            cout << "Price Level " << level.first.first << " (" << side << "): ";
            auto &ordersQueue = level.second;
            queue<pair<double, bool>> tempQueue = ordersQueue; // Copy for iteration
            cout << "{";
            while (!tempQueue.empty()) {
                auto order = tempQueue.front();
                tempQueue.pop();
                cout << "[" << order.first << ", " << (order.second ? "Our Order" : "Other Order") << "]";
                if (!tempQueue.empty()) cout << ", ";
            }
            cout << "}" << "\n\n";
        }
    }

    void CancelOrder(double price_level, double order_size, bool is_our_order, OrderSide side) {
        pair<double, OrderSide> levelKey = {price_level, side};
        if (book.find(levelKey) == book.end()) {
            cout << "Price level does not exist." << endl;
            return;
        }

        auto &ordersQueue = book[levelKey];
        queue<pair<double, bool>> tempQueue;
        bool orderFound = false;

        while (!ordersQueue.empty()) {
            auto order = ordersQueue.front();
            ordersQueue.pop();

            if (!orderFound && order.first == order_size && order.second == is_our_order) {
                orderFound = true;
                continue;
            }
            tempQueue.push(order);
        }

        book[levelKey] = tempQueue;

        if (!orderFound) {
            cout << "Order not found for cancellation." << endl;
        }
    }

    static std::string marketTypeToString(MarketType type) {
        switch (type) {
            case MarketType::Spot:
                return "Spot";
            case MarketType::Futures:
                return "Futures";
            default:
                return "Unknown";
        }
    }

private:
    Exchange &exchange;
    MarketType market_type;
    Security &security;
    map<pair<double, OrderSide>, queue<pair<double, bool>>> book;
};
