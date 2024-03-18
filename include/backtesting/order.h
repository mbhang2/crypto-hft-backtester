#pragma once

#include <cstdint>
#include <iomanip>
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
ostream& operator<<(ostream& os, const OrderState& state) {
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
 * Enumeration class for market type
 */
enum class MarketType {
    Spot,
    Futures
};

/**
 * Enumeration class for market type
 */
enum class MarginType {
    NoMargin,
    Cross,
    Isolated
};

/**
 * << operator overload for OrderState class.
 */
ostream& operator<<(ostream& os, const MarginType& margin_type) {
    switch (margin_type) {
        case MarginType::NoMargin:
            os << "No Margin";
            break;
        case MarginType::Isolated:
            os << "Isolated";
            break;
        case MarginType::Cross:
            os << "Cross";
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
         */
        Order(Security security_, const MarketType market_type_, const string& timestamp_, const string& type_, int side_, double base_currency_size_, double quote_currency_size_, unsigned leverage_, const MarginType margintype_ ,double price_, Exchange exchange_): 
                security(security_), market_type(market_type_), timestamp(timestamp_), type(type_), side(side_), base_currency_size(base_currency_size_), quote_currency_size(quote_currency_size_), leverage(leverage_), margin_type(margintype_), price(price_), exchange(exchange_) {
                    // Sanity checking zero and negative inputs
                    if (side != 1 && side != -1) {
                        cerr << "Invalid order side: " << side << endl;
                        state = OrderState::Rejected;
                        return;
                    }
                    if (quote_currency_size == 0.0 && base_currency_size == 0.0) {
                        cerr << "Please provide non-zero order size" << endl;
                        state = OrderState::Rejected;
                        return;
                    }
                    if (quote_currency_size_ < 0.0) {
                        cerr << "Invalid order size: " << quote_currency_size_ << security.getQuote() << endl;
                        state = OrderState::Rejected;
                        return;
                    }
                    if (base_currency_size_ < 0.0) {
                        cerr << "Invalid order size: " << base_currency_size_ << security.getBase() << endl;
                        state = OrderState::Rejected;
                        return;
                    }
                    if ((type == "LIMIT" || type == "STOPLIMIT") && price_ <= 0.0) {
                        cerr << "Invalid order price: " << price_ << security.getQuote() << endl;
                        state = OrderState::Rejected;
                        return;
                    }
                    if (base_currency_size != 0 && quote_currency_size != 0) {
                        cerr << "Order size should only provided in base currency or quote currency, not both" << endl;
                        state = OrderState::Rejected;
                        return;
                    }

                    // Sanity checking price
                    if (countDigitsAfterDecimal(price) > countDigitsAfterDecimal(exchange.getSpotTradeRules(security)[0])) {
                        cerr << "Invalid price of " << price << security.getQuote() << ". Minimum tick size is " << exchange.getSpotTradeRules(security)[0] << security.getQuote() << endl;
                        state = OrderState::Rejected;
                        return;
                    }


                    // Sanity checking leverage
                    if (leverage < 1) {
                        cerr << "Invalid leverage: " << leverage_ << "x" << endl;
                        state = OrderState::Rejected;
                        return;
                    }
                    if (leverage == 1 && margin_type != MarginType::NoMargin) {
                        cerr << "If leverage is 1x, margin type should be 'NoMargin'" << endl;
                        state = OrderState::Rejected;
                        return;
                    }
                    if (market_type == MarketType::Spot && margin_type == MarginType::Isolated && leverage_ > exchange.getSpotTradeRules(security_)[10]) {
                        cerr << "Leverage " << leverage_ << "x is greater than the spot isolated margin limit of " << exchange.getSpotTradeRules(security_)[10] << "x" << endl;
                        state = OrderState::Rejected;
                        return;
                    }
                    if (market_type == MarketType::Spot && margin_type == MarginType::Cross && leverage_ > exchange.getSpotTradeRules(security_)[11]) {
                        cerr << "Leverage " << leverage_ << "x is greater than the spot cross margin limit of " << exchange.getSpotTradeRules(security_)[11] << "x" << endl;
                        state = OrderState::Rejected;
                        return;
                    }

                    // Sanity checking minimum order size
                    if (base_currency_size == 0) {
                        double base_currency_min_size = exchange.getSpotTradeRules(security)[1];
                        base_currency_size = int((quote_currency_size / price)/base_currency_min_size) * base_currency_min_size;
                    } else if (quote_currency_size == 0) {
                        quote_currency_size = base_currency_size * price;
                    }

                    leverage_adjusted_base_currency_size = leverage * base_currency_size;


                    if (market_type == MarketType::Spot && leverage_adjusted_base_currency_size < exchange.getSpotTradeRules(security)[1]) {
                        cerr << "Order value " << leverage_adjusted_base_currency_size << security.getBase() << " is lesser than the minimum trade size " << exchange.getSpotTradeRules(security)[1] << security.getBase() << endl;
                        state = OrderState::Rejected;
                        return;
                    }
                    if (market_type == MarketType::Spot && quote_currency_size * leverage < exchange.getSpotTradeRules(security)[2]) {
                        cerr << "Order value " << quote_currency_size * leverage << security.getQuote() << " is lesser than the minimum trade size " << exchange.getSpotTradeRules(security)[2] << security.getQuote() << endl;
                        state = OrderState::Rejected;
                        return;
                    }

                    // Sanity checking maximum trade size
                    if (market_type == MarketType::Spot && (type == "LIMIT" || type == "STOPLIMIT") && (exchange.getSpotTradeRules(security)[3] != -1 && leverage_adjusted_base_currency_size > exchange.getSpotTradeRules(security)[3])) {
                        cerr << "Order value " << leverage_adjusted_base_currency_size << security.getBase() << " is greater than the maximum trade size " << exchange.getSpotTradeRules(security)[3] << security.getBase() << endl;
                        state = OrderState::Rejected;
                        return;
                    }
                    if (market_type == MarketType::Spot && (type == "LIMIT" || type == "STOPLIMIT") && (exchange.getSpotTradeRules(security)[4] != -1 && leverage * quote_currency_size > exchange.getSpotTradeRules(security)[4])) {
                        cerr << "Order value " << leverage * quote_currency_size << security.getQuote() << " is greater than the maximum trade size " << exchange.getSpotTradeRules(security)[4] << security.getQuote() << endl;
                        state = OrderState::Rejected;
                        return;
                    }
                    if (market_type == MarketType::Spot && (type == "MARKET" || type == "STOP") && (exchange.getSpotTradeRules(security)[5] != -1 && leverage_adjusted_base_currency_size > exchange.getSpotTradeRules(security)[5])) {
                        cerr << "Order value " << leverage_adjusted_base_currency_size << security.getBase() << " is greater than the maximum trade size " << exchange.getSpotTradeRules(security)[5] << security.getBase() << endl;
                        state = OrderState::Rejected;
                        return;
                    }
                    if (market_type == MarketType::Spot && (type == "LIMIT" || type == "STOPLIMIT") && (exchange.getSpotTradeRules(security)[6] != -1 && leverage * quote_currency_size > exchange.getSpotTradeRules(security)[6])) {
                        cerr << "Order value " << leverage * quote_currency_size << security.getQuote() << " is greater than the maximum trade size " << exchange.getSpotTradeRules(security)[6] << security.getQuote() << endl;
                        state = OrderState::Rejected;
                        return;
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
            if (qty <= 0 || qty > leverage_adjusted_base_currency_size - filled_size) {
                cerr << "Invalid fill quantity: " << qty << endl;
                return;
            }

            filled_size += qty;

            if (filled_size == leverage_adjusted_base_currency_size) {
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
         * Getter for unique id.
         */
        uint32_t getID() const {return id;}

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
         * Getter for base currency size.
         */
        double getBaseCurrencySize() const {return base_currency_size;}

        /**
         * Getter for quote currency size.
         */
        double getQuoteCurrencySize() const {return quote_currency_size;}

        /**
         * Getter for leverage
         */
        unsigned getLeverage() const {return leverage;}

        /**
         * Getter for leverage adjusted base currency size.
         */
        double getLeverageAdjustedBaseCurrencySize() const {return leverage_adjusted_base_currency_size;}

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

        /**
         * Getter for margin type.
         */
        MarginType getMarginType() const {return margin_type;}

        /**
         * Finds the number of digits after the decimal point
         * @param value number to find the number of decimal digits
         * @return number of decimal digits
         */
        int countDigitsAfterDecimal(double value) {
            string valueStr = to_string(value);
            size_t decimalPos = valueStr.find('.');
            if (decimalPos != string::npos) {
                return valueStr.length() - decimalPos - 1;
            }
            return 0; // No decimal point found
        }

    protected:
        uint32_t id;                /*< Unique ID */
        const MarketType market_type;     /*< Market type */
        const Security security;          /*< Security */
        const string timestamp;     /*< UTC date time, specified in format 'yyyy-MM-dd HH:mm:ss.fffffffff' */
        const string type;          /*< Order type */
        const int side;                   /*< -1 for SELL, 1 for BUY */
        double base_currency_size;  /*< Quantity of the order in base currency before leverage (For BTC/USDT, order size in BTC) */
        double quote_currency_size; /*< Quantity of the order in quote currency before leverage (For BTC/USDT, order size in USDT) */
        unsigned leverage;          /*< Leverage of the order */
        const MarginType margin_type;      /*< Margin type of the order */
        double leverage_adjusted_base_currency_size;      /*< Leverage adjusted base currency size */
        double filled_size = 0.0;   /*< Quantify filled of the order */
        double price;               /*< Price for LIMIT and STOP LIMIT orders. -1 for MARKET and STOP orders */
        const Exchange exchange;          /*< Target exchange for an order (optional) */
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
    Limit(Security security_, const MarketType market_type_ ,const string& timestamp_, int side_, double base_currency_size_, double quote_currency_size_, unsigned leverage_, const MarginType margintype_, double price_, Exchange exchange_): 
            Order(security_, market_type_, timestamp_, "LIMIT", side_, base_currency_size_, quote_currency_size_, leverage_, margintype_, price_, exchange_) {}

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
     * @param modified_base_currency_size New size of the order in base currency
     * @param modified_quote_currency_size New size of the order in quote currency
     * @param modified_price New price of the order
     */
    void modifyOrder(double modified_base_currency_size, double modified_quote_currency_size, double modified_price) {
        // Sanity checks
        if (!isLiveOrder()) {return;}
        if (modified_price <= 0.0) {
            cerr << "Invalid order price: " << modified_price << endl;
            return;
        }
        if (modified_base_currency_size == 0.0 && modified_quote_currency_size == 0.0) {
            cerr << "Invalid order size: 0" << security.getQuote() << endl;
            return;
        }
        if (modified_base_currency_size < 0.0) {
            cerr << "Invalid order size: " << modified_base_currency_size << security.getBase() << endl;
            return;
        }
        if (modified_quote_currency_size < 0.0) {
            cerr << "Invalid order size: " << modified_quote_currency_size << security.getQuote() << endl;
            return;
        }
        if (base_currency_size != 0 && quote_currency_size != 0) {
            cerr << "Order size should only provided in base currency or quote currency, not both" << endl;
            return;
        }
        if (countDigitsAfterDecimal(modified_price) > countDigitsAfterDecimal(exchange.getSpotTradeRules(security)[0])) {
            cerr << "Invalid price of " << modified_price << security.getQuote() << ". Minimum tick size is " << exchange.getSpotTradeRules(security)[0] << security.getQuote() << endl;
            return;
        }

        double new_base_currency_size = modified_base_currency_size;
        double new_quote_currency_size = modified_quote_currency_size;

        if (new_base_currency_size == 0) {
            double base_currency_min_size = exchange.getSpotTradeRules(security)[1];
            new_base_currency_size = int((new_quote_currency_size / modified_price)/base_currency_min_size) * base_currency_min_size;
        } else if (new_quote_currency_size == 0) {
            new_quote_currency_size = new_base_currency_size * price;
        }

        double new_leverage_adjusted_base_currency_size = leverage * new_base_currency_size;

        if (new_leverage_adjusted_base_currency_size <= filled_size) {
            cerr << "Modified order size of " << new_leverage_adjusted_base_currency_size << " is lesser than the already filled size of " << filled_size << security.getBase() << endl;
            return;
        }

        base_currency_size = new_base_currency_size, quote_currency_size = new_quote_currency_size, leverage_adjusted_base_currency_size = new_leverage_adjusted_base_currency_size, price = modified_price;
    }

    /**
     * Override << operator for Limit class.
     */
    friend ostream& operator<<(ostream& os, const Limit& limit) {
        string s = (limit.getSide() == 1) ? "Buy" : "Sell";
        os << "========== Order Information ==========" << endl;
        os << "Security: " << limit.getSecurity() << endl;
        os << "Exchange: " << limit.getExchange().getName() << endl;
        os << "Timestamp: " << limit.getTimestamp() << endl;
        os << "Order Type: " << limit.getOrderType() << endl;
        os << "Side: " << s << endl;
        os << "Size: " << limit.getBaseCurrencySize() << limit.getSecurity().getBase() << endl;
        os << "Margin Type: " << limit.getMarginType() << endl;
        os << "Leverage: " << limit.getLeverage() << "x" << endl;
        os << "Leverage Adjusted Size: " << limit.getLeverageAdjustedBaseCurrencySize() << limit.getSecurity().getBase() << endl;
        os << "Price: " << limit.getPrice() << limit.getSecurity().getQuote() << endl;
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
    Market(Security security_, const MarketType market_type_, const string& timestamp_, int side_, double base_currency_size_, double quote_currency_size_, unsigned leverage_, const MarginType margintype_, double current_price_, Exchange exchange_): 
            Order(security_, market_type_, timestamp_, "MARKET", side_, base_currency_size_, quote_currency_size_, leverage_, margintype_, current_price_, exchange_) {}

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
    friend ostream& operator<<(ostream& os, const Market& market) {
        string s = (market.getSide() == 1) ? "Buy" : "Sell";
        os << "========== Order Information ==========" << endl;
        os << "Security: " << market.getSecurity() << endl;
        os << "Exchange: " << market.getExchange().getName() << endl;
        os << "Timestamp: " << market.getTimestamp() << endl;
        os << "Order Type: " << market.getOrderType() << endl;
        os << "Side: " << s << endl;
        os << "Size: " << market.getBaseCurrencySize() << market.getSecurity().getBase() << endl;
        os << "Margin Type: " << market.getMarginType() << endl;
        os << "Leverage: " << market.getLeverage() << "x" << endl;
        os << "Leverage Adjusted Size: " << market.getLeverageAdjustedBaseCurrencySize() << market.getSecurity().getBase() << endl;
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
    Stop(Security security_, const MarketType market_type_, const string& timestamp_, int side_, double base_currency_size_, double quote_currency_size_, unsigned leverage_, const MarginType margintype_, double current_price_, double trigger_price_, Exchange exchange_): 
            Order(security_, market_type_, timestamp_, "STOP", side_, base_currency_size_, quote_currency_size_, leverage_, margintype_, current_price_, exchange_) {
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
            cerr << "Invalid current price: " << current_price << endl;
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
     * @param modified_base_currency_size New size of the order in base currency
     * @param modified_quote_currency_size New size of the order in quote currency
     * @param modified_trigger_price New trigger price of the order
     */
    void modifyOrder(double modified_base_currency_size, double modified_quote_currency_size, double modified_trigger_price) {
        // Sanity checks
        if (!isLiveOrder() || triggered) {return;}
        if (modified_base_currency_size == 0.0 && modified_quote_currency_size == 0.0) {
            cerr << "Invalid order size: 0" << security.getQuote() << endl;
            return;
        }
        if (modified_base_currency_size < 0.0) {
            cerr << "Invalid order size: " << modified_base_currency_size << security.getBase() << endl;
            return;
        }
        if (modified_quote_currency_size < 0.0) {
            cerr << "Invalid order size: " << modified_quote_currency_size << security.getQuote() << endl;
            return;
        }
        if (base_currency_size != 0 && quote_currency_size != 0) {
            cerr << "Order size should only provided in base currency or quote currency, not both" << endl;
            return;
        }
        if (countDigitsAfterDecimal(modified_trigger_price) > countDigitsAfterDecimal(exchange.getSpotTradeRules(security)[0])) {
            cerr << "Invalid trigger price of " << modified_trigger_price << security.getQuote() << ". Minimum tick size is " << exchange.getSpotTradeRules(security)[0] << security.getQuote() << endl;
            return;
        }
        
        double new_base_currency_size = modified_base_currency_size;

        // if (new_base_currency_size == 0) {
        //     double base_currency_min_size = exchange.getSpotTradeRules(security)[1];
        //     new_base_currency_size = int((new_quote_currency_size / modified_price)/base_currency_min_size) * base_currency_min_size;
        // } else if (new_quote_currency_size == 0) {
        //     new_quote_currency_size = new_base_currency_size * price;
        // }

        double new_leverage_adjusted_base_currency_size = leverage * new_base_currency_size;

        if (new_leverage_adjusted_base_currency_size <= filled_size) {
            cerr << "Modified order size of " << new_leverage_adjusted_base_currency_size << " is lesser than the already filled size of " << filled_size << security.getBase() << endl;
            return;
        }
        if (modified_trigger_price <= 0.0) {
            cerr << "Invalid order trigger price: " << modified_trigger_price << endl;
            return;
        }

        base_currency_size = new_base_currency_size, leverage_adjusted_base_currency_size = new_leverage_adjusted_base_currency_size, trigger_price = modified_trigger_price;
        // quote_currency_size = new_quote_currency_size;
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
    friend ostream& operator<<(ostream& os, const Stop& stop) {
        string s = (stop.getSide() == 1) ? "Buy" : "Sell";
        string trigger_status = (stop.isTriggered() == 1) ? "Triggered" : "Not Triggered";
        os << "========== Order Information ==========" << endl;
        os << "Security: " << stop.getSecurity() << endl;
        os << "Exchange: " << stop.getExchange().getName() << endl;
        os << "Timestamp: " << stop.getTimestamp() << endl;
        os << "Order Type: " << stop.getOrderType() << endl;
        os << "Side: " << s << endl;
        os << "Size: " << stop.getBaseCurrencySize() << stop.getSecurity().getBase() << endl;
        os << "Margin Type: " << stop.getMarginType() << endl;
        os << "Leverage: " << stop.getLeverage() << "x" << endl;
        os << "Leverage Adjusted Size: " << stop.getLeverageAdjustedBaseCurrencySize() << stop.getSecurity().getBase() << endl;
        os << "Trigger Price: " << stop.getTriggerPrice() << stop.getSecurity().getQuote() << endl;
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
    StopLimit(Security security_, const MarketType market_type_, const string& timestamp_, int side_, double base_currency_size_, double quote_currency_size_, double price_, unsigned leverage_, const MarginType margintype_, double trigger_price_, Exchange exchange_): 
            Order(security_, market_type_, timestamp_, "STOPLIMIT", side_, base_currency_size_, quote_currency_size_, leverage_, margintype_, price_, exchange_) {
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
            cerr << "Invalid current price: " << current_price << endl;
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
     * @param modified_base_currency_size New size of the order in base currency
     * @param modified_quote_currency_size New size of the order in quote currency
     * @param modified_price New price of the order
     * @param modified_trigger_price New trigger price of the order
     */
    void modifyOrder(double modified_base_currency_size, double modified_quote_currency_size, double modified_price, double modified_trigger_price) {
        // Sanity checks
        if (!isLiveOrder()) {return;}
        if (modified_trigger_price != trigger_price && triggered) {
            cerr << "Cannot modify trigger price once triggered" << endl;
            return;
        }
        if (modified_trigger_price <= 0.0) {
            cerr << "Invalid order trigger price: " << modified_trigger_price << endl;
            return;
        }
        if (modified_base_currency_size == 0.0 && modified_quote_currency_size == 0.0) {
            cerr << "Invalid order size: 0" << security.getQuote() << endl;
            return;
        }
        if (modified_base_currency_size < 0.0) {
            cerr << "Invalid order size: " << modified_base_currency_size << security.getBase() << endl;
            return;
        }
        if (modified_quote_currency_size < 0.0) {
            cerr << "Invalid order size: " << modified_quote_currency_size << security.getQuote() << endl;
            return;
        }
        if (base_currency_size != 0 && quote_currency_size != 0) {
            cerr << "Order size should only provided in base currency or quote currency, not both" << endl;
            return;
        }
        if (countDigitsAfterDecimal(modified_price) > countDigitsAfterDecimal(exchange.getSpotTradeRules(security)[0])) {
            cerr << "Invalid price of " << modified_price << security.getQuote() << ". Minimum tick size is " << exchange.getSpotTradeRules(security)[0] << security.getQuote() << endl;
            return;
        }
        if (countDigitsAfterDecimal(modified_trigger_price) > countDigitsAfterDecimal(exchange.getSpotTradeRules(security)[0])) {
            cerr << "Invalid trigger price of " << modified_trigger_price << security.getQuote() << ". Minimum tick size is " << exchange.getSpotTradeRules(security)[0] << security.getQuote() << endl;
            return;
        }

        double new_base_currency_size = modified_base_currency_size;
        double new_quote_currency_size = modified_quote_currency_size;

        if (new_base_currency_size == 0) {
            double base_currency_min_size = exchange.getSpotTradeRules(security)[1];
            new_base_currency_size = int((new_quote_currency_size / modified_price)/base_currency_min_size) * base_currency_min_size;
        } else if (new_quote_currency_size == 0) {
            new_quote_currency_size = new_base_currency_size * price;
        }

        double new_leverage_adjusted_base_currency_size = leverage * new_base_currency_size;

        if (new_leverage_adjusted_base_currency_size <= filled_size) {
            cerr << "Modified order size of " << new_leverage_adjusted_base_currency_size << " is lesser than the already filled size of " << filled_size << security.getBase() << endl;
            return;
        }

        base_currency_size = new_base_currency_size, quote_currency_size = new_quote_currency_size, leverage_adjusted_base_currency_size = new_leverage_adjusted_base_currency_size, price = modified_price, trigger_price = modified_trigger_price;
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
    friend ostream& operator<<(ostream& os, const StopLimit& stoplimit) {
        string s = (stoplimit.getSide() == 1) ? "Buy" : "Sell";
        string trigger_status = (stoplimit.isTriggered() == 1) ? "Triggered" : "Not Triggered";
        os << "========== Order Information ==========" << endl;
        os << "Security: " << stoplimit.getSecurity() << endl;
        os << "Exchange: " << stoplimit.getExchange().getName() << endl;
        os << "Timestamp: " << stoplimit.getTimestamp() << endl;
        os << "Order Type: " << stoplimit.getOrderType() << endl;
        os << "Side: " << s << endl;
        os << "Size: " << stoplimit.getBaseCurrencySize() << stoplimit.getSecurity().getBase() << endl;
        os << "Margin Type: " << stoplimit.getMarginType() << endl;
        os << "Leverage: " << stoplimit.getLeverage() << "x" << endl;
        os << "Leverage Adjusted Size: " << stoplimit.getLeverageAdjustedBaseCurrencySize() << stoplimit.getSecurity().getBase() << endl;
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