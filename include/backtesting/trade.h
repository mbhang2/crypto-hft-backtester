#pragma once

#include <string>
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
        Trade(Security security_, const string& timestamp_, int side_, double size_, double price_): 
                security(security_), timestamp(timestamp_), side(side_), size(size_), price(price_) {}

        /**
         * Destructor
         */
        virtual ~Trade() {}

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
        double getSize() {return size;}

        /**
         * Getter for price.
         */
        double getPrice() {return price;}

        /**
         * Getter for exchange.
         */
        Exchange getExchange() {return exchange;}

    protected:
        Security security;          /*< Name of the security */
        const string timestamp;     /*< UTC date time, specified in format 'yyyy-MM-dd HH:mm:ss.fffffffff' */
        int side;                   /*< -1 for SELL, 1 for BUY */
        double size;                /*< Quantity of the order */
        double price;               /*< Price at which trade occurred */
        Exchange exchange;          /*< Exchange the trade occurred */
};