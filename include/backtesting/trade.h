#pragma once

#include <string>
#include "./order.h"
#include "../data/exchange.h"
#include "../data/security.h"

using namespace std;


/**
 * Abstract class for trades.
 */
class Trade {
    public:
        /**
         * Constructor to create a trade.
         */
        Trade(int parent_order_id_, Security security_, const string& timestamp_, int side_, double base_currency_size_, double price_): 
                parent_order_id(parent_order_id_), security(security_), timestamp(timestamp_), side(side_), base_currency_size(base_currency_size_), price(price_) {
                    id = ++next_id; // Assign a unique order id
                }

        /**
         * Destructor
         */
        virtual ~Trade() {}

        /**
         * Getter for trade ID.
         */
        int getID() const { return id; }

        /**
         * Getter for security.
         */
        Security getSecurity() const {return security;}

        /**
         * Getter for timestamp.
         */
        string getTimestamp() const {return timestamp;}

        /**
         * Getter for side.
         */
        int getSide() {return side;}

        /**
         * Getter for size.
         */
        double getBaseCurrencySize() {return base_currency_size;}

        /**
         * Getter for price.
         */
        double getPrice() {return price;}

        /**
         * Getter for exchange.
         */
        Exchange getExchange() const {return exchange;}

        /**
         * Getter for boolean if the trade was making or taking
         */
        bool getIfMaker() const {return is_maker;}

        /**
         * Getter for trade fee
         */
        double getFee() const {return fee;}

    private:
        int id;                     /*< Unique id */
        int parent_order_id;        /*< Parent order's unique id */
        static int next_id;         /*< Static member to track the next available ID */ 
        Security security;          /*< Name of the security */
        const string timestamp;     /*< UTC date time, specified in format 'yyyy-MM-dd HH:mm:ss.fffffffff' */
        int side;                   /*< -1 for SELL, 1 for BUY */
        double base_currency_size;  /*< Quantity of the order in base currency */
        double price;               /*< Price at which trade occurred */
        Exchange exchange;          /*< Exchange the trade occurred */
        bool is_maker;              /*< Whether the trade was making or taking */
        double fee;                 /*< Execution fee associated with the order */
};