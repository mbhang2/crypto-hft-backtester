#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include "../data/exchange.h"
#include "../data/security.h"

using namespace std;

/**
 * Enumeration class for order state
 */
enum class OrderState {
    Working,
    Filled,
    PartiallyFilled,
    Cancelled,
    Rejected
};

/**
 * << operator overload for OrderState class.
 */
std::ostream& operator<<(std::ostream& os, const OrderState& state) {
    switch (state) {
        case OrderState::Working:
            os << "Working";
            break;
        case OrderState::Filled:
            os << "Filled";
            break;
        case OrderState::PartiallyFilled:
            os << "Partially Filled";
            break;
        case OrderState::Cancelled:
            os << "Cancelled";
            break;
        case OrderState::Rejected:
            os << "Rejected";
            break;
        default:
            os << "Unknown"; // Handle any other values gracefully
    }
    return os;
}


/**
 * Abstract class for orders.
 */
class Order {
    public:
        /**
         * Constructor to create an order.
         * @todo add sanity check for security and unrealistic price
         */
        Order(Security security_, const string& timestamp_, const string& type_, int side_, double size_, double price_, Exchange exchange_): 
                security(security_), timestamp(timestamp_), type(type_), side(side_), size(size_), price(price_), exchange(exchange_) {
                    // Sanity checks
                    if (side != 1 && side != -1) {
                        std::cerr << "Invalid order side: " << side << std::endl;
                        state = OrderState::Rejected;
                    }
                    if (size_ <= 0.0) {
                        std::cerr << "Invalid order size: " << size_ << std::endl;
                        state = OrderState::Rejected;
                    }
                    if ((type == "LIMIT" || type == "STOPLIMIT") && price_ <= 0.0) {
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
        bool checkFillability() const {return false;}

        /**
         * Fills the order and change the order status accordingly
         * @param qty Quantity to fill.
        */
        void fillOrder(double qty) {
            // Sanity checks
            if (qty <= 0 || qty > size - filled_size) {
                std::cerr << "Invalid fill quantity: " << qty << std::endl;
                return;
            }

            filled_size += qty;

            if (filled_size == size) {
                state = OrderState::Filled;
            } else {
                state = OrderState::PartiallyFilled;
            }
        }


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
        Security getSecurity() const {return security;}

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
        int getSide() const {return side;}

        /**
         * Getter for size.
         */
        double getSize() const {return size;}

        /**
         * Getter for filled size.
         */
        double getFilledSize() const {return filled_size;}

        /**
         * Getter for price.
         */
        double getPrice() const {return price;}

        /**
         * Getter for orderState.
         */
        OrderState getOrderState() const {return state;}

        /**
         * Getter for exchange.
         */
        Exchange getExchange() const {return exchange;}

    protected:
        Security security;          /*< Security */
        const string timestamp;     /*< UTC date time, specified in format 'yyyy-MM-dd HH:mm:ss.fffffffff' */
        const string type;          /*< Order type */
        int side;                   /*< -1 for SELL, 1 for BUY */
        double size;                /*< Quantity of the order */
        double filled_size = 0.0;         /*< Quantify filled of the order */
        double price;               /*< Price for LIMIT and STOP LIMIT orders. -1 for MARKET and STOP orders */
        Exchange exchange;          /*< Target exchange for an order (optional) */
        OrderState state = OrderState::Working;     /*< State of the order */
};


/**
 * Limit order class
 */
class Limit : public Order {
public:
    /**
     * Constructor to create an order.
     */
    Limit(Security security_, const string& timestamp_, int side_, double size_, double price_, Exchange exchange_): 
            Order(security_, timestamp_, "LIMIT", side_, size_, price_, exchange_) {}

    /**
     * Checks if the order could be filled.
     * @param best_bid The best bid price of the given security
     * @param best_ask The best ask price of the given security
     * @return Boolean whether the order could be filled
     */
    bool checkFillability(double best_bid, double best_ask) const {
        if (!isLiveOrder()) {return false;}
        if (side == 1 && price >= best_ask) {return true;}
        if (side == -1 && price <= best_bid) {return true;}

        return false;
    }

    /**
     * Modifies the order. 
     * @param modified_size New size of the order
     * @param modified_price New price of the order
     */
    void modifyOrder(double modified_size, double modified_price) {
        // Sanity checks
        if (!isLiveOrder()) {return;}
        if (modified_size <= 0.0 || modified_size <= filled_size) {
            std::cerr << "Invalid order size: " << modified_size << std::endl;
            return;
        }
        if (modified_price <= 0.0) {
            std::cerr << "Invalid order price: " << modified_price << std::endl;
            return;
        }

        size = modified_size, price = modified_price;
    }

    /**
     * Override << operator for Limit class.
     */
    friend std::ostream& operator<<(std::ostream& os, const Limit& limit) {
        string s = (limit.getSide() == 1) ? "Buy" : "Sell";
        os << "========== Order Information ==========" << endl;
        os << "Security: " << limit.getSecurity() << endl;
        os << "Exchange: " << limit.getExchange().getName() << endl;
        os << "Timestamp: " << limit.getTimestamp() << endl;
        os << "Order Type: " << limit.getOrderType() << endl;
        os << "Side: " << s << endl;
        os << "Size: " << limit.getSize() << endl;
        os << "Price: " << limit.getPrice() << endl;
        os << "Order State: " << limit.getOrderState() << endl;
        os << "=======================================" << endl;
        return os;
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
    Market(Security security_, const string& timestamp_, int side_, double size_, Exchange exchange_): 
            Order(security_, timestamp_, "MARKET", side_, size_, -1.0, exchange_) {}

    /**
     * Checks if the order could be filled.
     * @param best_bid The best bid price of the given security
     * @param best_ask The best ask price of the given security
     * @return Boolean whether the order could be filled
     */
    bool checkFillability() const {
        if (!isLiveOrder()) {return false;}

        return true;
    }

    /**
     * Override << operator for Market class.
     */
    friend std::ostream& operator<<(std::ostream& os, const Market& market) {
        string s = (market.getSide() == 1) ? "Buy" : "Sell";
        os << "========== Order Information ==========" << endl;
        os << "Security: " << market.getSecurity() << endl;
        os << "Exchange: " << market.getExchange().getName() << endl;
        os << "Timestamp: " << market.getTimestamp() << endl;
        os << "Order Type: " << market.getOrderType() << endl;
        os << "Side: " << s << endl;
        os << "Size: " << market.getSize() << endl;
        os << "Order State: " << market.getOrderState() << endl;
        os << "=======================================" << endl;
        return os;
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
    Stop(Security security_, const string& timestamp_, int side_, double size_, double trigger_price_, Exchange exchange_): 
            Order(security_, timestamp_, "STOP", side_, size_, -1.0, exchange_) {
                triggered = false, trigger_price = trigger_price_;
            }

    /**
     * Checks if the order could be filled.
     * @param best_bid The best bid price of the given security
     * @param best_ask The best ask price of the given security
     * @return Boolean whether the order could be filled
     */
    bool checkFillability() const {
        if (!isLiveOrder() || !triggered) {return false;}

        return true;
    }

    /**
     * Checks if the order could be triggered based on current price.
     * @param current_price Current trading price of a security.
     * @return Boolean whether the order is triggered
     */
    bool checkTriggered(double current_price) {
        if (current_price <= 0.0) {
            std::cerr << "Invalid current price: " << current_price << std::endl;
            return false;
        }

        if ((side == 1 && current_price >= trigger_price) || (side == -1 && current_price <= trigger_price)) {
            triggered = true;
            return true;
        }
        return false;
    }

    /**
     * Modifies the order. Could be used not only when user changes the order, but also when the order is partially filled.
     * @param modified_size New size of the order
     * @param modified_trigger_price New trigger price of the order
     */
    void modifyOrder(double modified_size, double modified_trigger_price, double current_price) {
        // Sanity checks
        if (!isLiveOrder() || checkTriggered(current_price)) {return;}
        if (modified_size <= 0.0 || modified_size <= filled_size) {
            std::cerr << "Invalid order size: " << modified_size << std::endl;
            return;
        }
        if (modified_trigger_price <= 0.0) {
            std::cerr << "Invalid order trigger price: " << modified_trigger_price << std::endl;
            return;
        }

        size = modified_size, trigger_price = modified_trigger_price;
    }

    /**
     * Getter for triggered.
     */
    bool isTriggered() const {return triggered;}

    /**
     * Getter for trigger price.
     */
    double getTriggerPrice() const {return trigger_price;}

    /**
     * Override << operator for Limit class.
     */
    friend std::ostream& operator<<(std::ostream& os, const Stop& stop) {
        string s = (stop.getSide() == 1) ? "Buy" : "Sell";
        string trigger_status = (stop.isTriggered() == 1) ? "Triggered" : "Not Triggered";
        os << "========== Order Information ==========" << endl;
        os << "Security: " << stop.getSecurity() << endl;
        os << "Exchange: " << stop.getExchange().getName() << endl;
        os << "Timestamp: " << stop.getTimestamp() << endl;
        os << "Order Type: " << stop.getOrderType() << endl;
        os << "Side: " << s << endl;
        os << "Size: " << stop.getSize() << endl;
        os << "Trigger Price: " << stop.getTriggerPrice() << endl;
        os << "Trigger Status: " << trigger_status << endl;
        os << "Order State: " << stop.getOrderState() << endl;
        os << "=======================================" << endl;
        return os;
    }

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
    StopLimit(Security security_, const string& timestamp_, int side_, double size_, double price_, double trigger_price_, Exchange exchange_): 
            Order(security_, timestamp_, "STOPLIMIT", side_, size_, price_, exchange_) {
                triggered = false, trigger_price = trigger_price_;
            }

    /**
     * Checks if the order could be filled.
     * @param best_bid The best bid price of the given security
     * @param best_ask The best ask price of the given security
     * @return Boolean whether the order could be filled
     */
    bool checkFillability(double best_bid, double best_ask) const {
        if (!isLiveOrder() || !triggered) {return false;}

        if (side == 1 && price >= best_ask) {return true;}
        if (side == -1 && price <= best_bid) {return true;}

        return false;
    }

    /**
     * Checks if the order could be triggered based on current price.
     * @param current_price Current trading price of a security.
     * @return Boolean whether the order is triggered
     */
    bool checkTriggered(double current_price) {
        if (current_price <= 0.0) {
            std::cerr << "Invalid current price: " << current_price << std::endl;
            return false;
        }

        if ((side == 1 && current_price >= trigger_price) || (side == -1 && current_price <= trigger_price)) {
            triggered = true;
            return true;
        }
        return false;
    }

    /**
     * Modifies the order. Could be used not only when user changes the order, but also when the order is partially filled.
     * @param modified_size New size of the order
     * @param modified_price New price of the order
     * @param modified_trigger_price New trigger price of the order
     * @param filled_size Filled quantity if this function is called after being filled
     */
    void modifyOrder(double modified_size, double modified_price, double modified_trigger_price, double current_price) {
        // Sanity checks
        if (!isLiveOrder() || checkTriggered(current_price)) {return;}
        if (modified_size <= 0.0 || modified_size <= filled_size) {
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

        size = modified_size, price = modified_price, trigger_price = modified_trigger_price;
    }

    /**
     * Getter for triggered.
     */
    bool isTriggered() const {return triggered;}

    /**
     * Getter for trigger price.
     */
    double getTriggerPrice() const {return trigger_price;}

    /**
     * Override << operator for Limit class.
     */
    friend std::ostream& operator<<(std::ostream& os, const StopLimit& stoplimit) {
        string s = (stoplimit.getSide() == 1) ? "Buy" : "Sell";
        string trigger_status = (stoplimit.isTriggered() == 1) ? "Triggered" : "Not Triggered";
        os << "========== Order Information ==========" << endl;
        os << "Security: " << stoplimit.getSecurity() << endl;
        os << "Exchange: " << stoplimit.getExchange().getName() << endl;
        os << "Timestamp: " << stoplimit.getTimestamp() << endl;
        os << "Order Type: " << stoplimit.getOrderType() << endl;
        os << "Side: " << s << endl;
        os << "Size: " << stoplimit.getSize() << endl;
        os << "Price: " << stoplimit.getPrice() << endl;
        os << "Trigger Price: " << stoplimit.getTriggerPrice() << endl;
        os << "Trigger Status: " << trigger_status << endl;
        os << "Order State: " << stoplimit.getOrderState() << endl;
        os << "=======================================" << endl;
        return os;
    }

private:
    bool triggered;         /*< Whether the trigger price has been hit */
    double trigger_price;   /*< Trigger price for the stop order */
};