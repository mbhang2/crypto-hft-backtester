#pragma once

#include <cstdint>
#include <iostream>
#include <optional>
#include <string>
#include "exchange.h"

using namespace std;

enum class OrderState {
    Working,
    Filled,
    PartiallyFilled,
    Cancelled,
    Rejected
};


/**
 * Abstract class for orders.
 * @todo Add exchange option
 */
class Order {
    public:
        /**
         * Constructor to create an order.
         * @todo add sanity check for security and unrealistic price
         */
        Order(const string& security_, const string& timestamp_, const string& type_, int side_, double size_, double price_, OrderState state_, Exchange exchange_ = Exchange()): 
                security(security_), timestamp(timestamp_), type(type_), side(side_), size(size_), price(price_), state(state_), exchange(exchange_) {
                    // Sanity checks
                    if (side != 1 && side != -1) {
                        std::cerr << "Invalid order side: " << side << std::endl;
                        state = OrderState::Rejected;
                    }
                    if (size_ <= 0.0) {
                        std::cerr << "Invalid order size: " << size_ << std::endl;
                        state = OrderState::Rejected;
                    }
                    if (type == "LIMIT" && price_ <= 0.0) {
                        std::cerr << "Invalid order price: " << price_ << std::endl;
                        state = OrderState::Rejected;
                    }
                }
        
        /**
         * Destructor
         */
        virtual ~Order() {}

        /**
         * Checks if the order could be filled.
         * @param best_bid The best bid price of the given security
         * @param best_ask The best ask price of the given security
         * @return Boolean whether the order could be filled
         */
        virtual bool checkFillability(double best_bid, double best_ask) const = 0;

        /**
         * Modifies the order.
         */
        virtual void modifyOrder() const = 0;


        /**
         * Checks if the order is live or dead
         */
        bool isLiveOrder() const {return state == OrderState::Working || state == OrderState::PartiallyFilled;} 

        /**
         * Cancels the order if it is live.
         */
        void cancelOrder() {
            if (!isLiveOrder()) {return;}

            state = OrderState::Cancelled;
        }

        /**
         * Getter for security.
         */
        string getSecurity() const {return security;}

        /**
         * Getter for timestamp.
         */
        string getTimestamp() const {return timestamp;}

        /**
         * Getter for order type.
         */
        string getOrderType() const {return type;}

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
         * Getter for orderState.
         */
        OrderState getOrderState() {return state;}

        /**
         * Getter for exchange.
         */
        Exchange getExchange() {return exchange;}

    protected:
        const string security;      /*< Name of the security */
        const string timestamp;     /*< UTC date time, specified in format 'yyyy-MM-dd HH:mm:ss.fffffffff' */
        const string type;           /*< Order type */
        int side;                   /*< -1 for SELL, 1 for BUY */
        double size;                /*< Quantity of the order */
        double price;               /*< Price for LIMIT and STOP LIMIT orders. -1 for MARKET and STOP orders */
        Exchange exchange;          /*< Target exchange for an order (optional) */
        OrderState state;           /*< State of the order */
};


/**
 * Limit order class
 */
class Limit : public Order {
public:
    /**
     * Constructor to create an order.
     */
    Limit(const string& security_, const string& timestamp_, int side_, double size_, double price_, OrderState state_): 
            Order(security_, timestamp_, "LIMIT", side_, size_, price_, state_) {}

    /**
     * Checks if the order could be filled.
     * @param best_bid The best bid price of the given security
     * @param best_ask The best ask price of the given security
     * @return Boolean whether the order could be filled
     */
    bool checkFillability(double best_bid, double best_ask) const override {
        if (!isLiveOrder()) {return false;}
        if (side == 1 && price >= best_ask) {return true;}
        if (side == -1 && price <= best_bid) {return true;}

        return false;
    }

    /**
     * Modifies the order. Could be used not only when user changes the order, but also when the order is partially filled.
     * @param modified_size New size of the order
     * @param modified_price New price of the order
     * @param filled_size Filled quantity if this function is called after being filled
     */
    void modifyOrder(double modified_size, double modified_price, double filled_size) {
        // Sanity checks
        if (!isLiveOrder()) {return;}
        if (modified_size <= 0.0) {
            std::cerr << "Invalid order size: " << modified_size << std::endl;
            return;
        }
        if (modified_price <= 0.0) {
            std::cerr << "Invalid order price: " << modified_price << std::endl;
            return;
        }

        if (filled_size == size) {
            state = OrderState::Filled;
            return;
        } else if (filled_size > 0.0) {
            state = OrderState::PartiallyFilled;
        }

        size = modified_size, price = modified_price;
    }
};


/**
 * Market order class
 */
class Market : public Order {
public:
    /**
     * Constructor to create an order.
     */
    Market(const string& security_, const string& timestamp_, int side_, double size_, OrderState state_): 
            Order(security_, timestamp_, "MARKET", side_, size_, -1.0, state_) {}

    /**
     * Checks if the order could be filled.
     * @param best_bid The best bid price of the given security
     * @param best_ask The best ask price of the given security
     * @return Boolean whether the order could be filled
     */
    bool checkFillability(double best_bid, double best_ask) const override {
        if (!isLiveOrder()) {return false;}

        return true;
    }

    /**
     * Modifies the order. Only used to change order state to filled.
     */
    void modifyOrder() {
        // Sanity checks
        if (!isLiveOrder()) {return;}
        
        state = OrderState::Filled;
    }
};


/**
 * Stop order class
 */
class Stop : public Order {
public:
    /**
     * Constructor to create an order.
     */
    Stop(const string& security_, const string& timestamp_, int side_, double size_, double trigger_price_, OrderState state_): 
            Order(security_, timestamp_, "STOP", side_, size_, -1.0, state_) {
                triggered = false, trigger_price = trigger_price_;
            }

    /**
     * Checks if the order could be filled.
     * @param best_bid The best bid price of the given security
     * @param best_ask The best ask price of the given security
     * @return Boolean whether the order could be filled
     */
    bool checkFillability(double best_bid, double best_ask) const override {
        if (!isLiveOrder() || !triggered) {return false;}

        return true;
    }

    /**
     * Modifies the order. Could be used not only when user changes the order, but also when the order is partially filled.
     * @param modified_size New size of the order
     * @param modified_trigger_price New trigger price of the order
     */
    void modifyOrder(double modified_size, double modified_trigger_price,  double filled_size) {
        // Sanity checks
        if (!isLiveOrder()) {return;}
        if (modified_size <= 0.0) {
            std::cerr << "Invalid order size: " << modified_size << std::endl;
            return;
        }
        if (modified_trigger_price <= 0.0) {
            std::cerr << "Invalid order trigger price: " << modified_trigger_price << std::endl;
            return;
        }

        if (filled_size > 0) {
            state = OrderState::Filled;
            return;
        }
        size = modified_size, trigger_price = modified_trigger_price;
    }

    /**
     * Getter for triggered.
     */
    bool checkTriggered() {return triggered;}

    /**
     * Getter for trigger price.
     */
    double getTriggerPrice() {return trigger_price;}

private:
    bool triggered;         /*< Whether the trigger price has been hit */
    double trigger_price;   /*< Trigger price for the stop order */
};



/**
 * Stop Limit order class
 */
class StopLimit : public Order {
public:
    /**
     * Constructor to create an order.
     */
    StopLimit(const string& security_, const string& timestamp_, int side_, double size_, double price_, double trigger_price_, OrderState state_): 
            Order(security_, timestamp_, "STOPLIMIT", side_, size_, price_, state_) {
                triggered = false, trigger_price = trigger_price_;
            }

    /**
     * Checks if the order could be filled.
     * @param best_bid The best bid price of the given security
     * @param best_ask The best ask price of the given security
     * @return Boolean whether the order could be filled
     */
    bool checkFillability(double best_bid, double best_ask) const override {
        if (!isLiveOrder() || !triggered) {return false;}

        if (side == 1 && price >= best_ask) {return true;}
        if (side == -1 && price <= best_bid) {return true;}

        return false;
    }

    /**
     * Modifies the order. Could be used not only when user changes the order, but also when the order is partially filled.
     * @param modified_size New size of the order
     * @param modified_price New price of the order
     * @param modified_trigger_price New trigger price of the order
     * @param filled_size Filled quantity if this function is called after being filled
     */
    void modifyOrder(double modified_size, double modified_price, double modified_trigger_price, double filled_size) {
        // Sanity checks
        if (!isLiveOrder()) {return;}
        if (modified_size <= 0.0) {
            std::cerr << "Invalid order size: " << modified_size << std::endl;
            return;
        }
        if (modified_price <= 0.0) {
            std::cerr << "Invalid order price: " << modified_price << std::endl;
            return;
        }
        if (modified_trigger_price <= 0.0) {
            std::cerr << "Invalid order trigger price: " << modified_trigger_price << std::endl;
            return;
        }

        if (filled_size == size) {
            state = OrderState::Filled;
            return;
        } else if (filled_size > 0.0) {
            state = OrderState::PartiallyFilled;
        }

        size = modified_size, price = modified_price;

        if (!triggered) {trigger_price=modified_trigger_price;}
    }

    /**
     * Getter for triggered.
     */
    bool checkTriggered() {return triggered;}

    /**
     * Getter for trigger price.
     */
    double getTriggerPrice() {return trigger_price;}

private:
    bool triggered;         /*< Whether the trigger price has been hit */
    double trigger_price;   /*< Trigger price for the stop order */
};