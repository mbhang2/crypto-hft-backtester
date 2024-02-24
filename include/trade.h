#pragma once

#include <string>
#include "exchange.h"

using namespace std;

class Trade {
    public:
        /**
         * Constructor to create a trade.
         */
        Trade(const string& security_, const string& timestamp_, int side_, double size_, double price_, Exchange exchange_): 
                security(security_), timestamp(timestamp_), side(side_), size(size_), price(price_), exchange(exchange_) {}

        /**
         * Destructor
         */
        virtual ~Trade() {}

        /**
         * Getter for security.
         */
        string getSecurity() const {return security;}

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
        const string security;      /*< Name of the security */
        const string timestamp;     /*< UTC date time, specified in format 'yyyy-MM-dd HH:mm:ss.fffffffff' */
        int side;                   /*< -1 for SELL, 1 for BUY */
        double size;                /*< Quantity of the order */
        double price;               /*< Price at which trade occurred */
        Exchange exchange;          /*< Exchange the trade occurred */
};