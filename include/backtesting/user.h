#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "../data/exchange.h"
#include "../record/orderlog.h"

using namespace std;

class User {
public:
    /**
     * Constructor for user
     */
    User(double spot_balance_, double futures_balance_)
        : spot_balance(spot_balance_), futures_balance(futures_balance_) {
        id = ++next_id; // Assign a unique ID
    }

    /**
     * Destructor for user
     */
    ~User() {}

    /**
     * Getter for user id
     */
    int getId() const { return id; }

    /**
     * Getter for user's spot balance
     */
    double getSpotBalance() const { return spot_balance; }

    /**
     * Getter for user's futures balance
     */
    double getFuturesBalance() const { return futures_balance; }

    /**
     * Getter for user's list of exchanges
     */
    vector<Exchange> getExchanges() const { return exchange_list; }

    /**
     * Getter for user's order log
     */
    OrderLog getOrderLog() const { return orderlog; }

    /**
     * Add exchange to the list
     * @param exchange exchange to add
     */
    void addExchange(const Exchange& exchange) {
        exchange_list.push_back(exchange);
    }

    /**
     * Remove exchange from the list
     * @param exchange exchange to remove
     */
    void removeExchange(const Exchange& exchange) {
        // Find the exchange in the vector and remove it
        auto it = std::find(exchange_list.begin(), exchange_list.end(), exchange);
        if (it != exchange_list.end()) {
            exchange_list.erase(it);
        }
    }

    private:
    int id; /*< Unique user id */
    double spot_balance; /*< Spot balance */
    double futures_balance; /*< Futures balance */
    static int next_id; /*< Static member to track the next available ID */ 
    vector<Exchange> exchange_list; /*< List of exchanges for the user */
    OrderLog orderlog;  /*< Order log */
};