#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "../data/exchange.h"
#include "../record/orderlog.h"

using namespace std;

/**
 * @todo (Reach goal) Add restristictions per subaccount
 * @todo (Reach goal) Manage risks/positions per subaccounts/users for a firm (tree structure)
 */
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
    void addExchange(Exchange& exchange) {
        exchange_list.push_back(exchange);
    }

    private:
    int id; /*< Unique user id */
    double spot_balance; /*< Spot balance */
    double futures_balance; /*< Futures balance */
    static int next_id; /*< Static member to track the next available ID */ 
    vector<Exchange> exchange_list; /*< List of exchanges for the user */
    OrderLog orderlog;  /*< Order log */
};

int User::next_id = 0;