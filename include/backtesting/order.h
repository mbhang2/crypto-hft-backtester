#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include "../data/exchange.h"
#include "../data/security.h"

using namespace std;

/**
 * Enumeration class for order state
 * @todo: Add a state where packet is on the way to the exchange
 */
enum class OrderState {
    SentToExchange,
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
        case OrderState::SentToExchange:
            os << "Sent to Exchange";
            break;
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
 * @todo Make sure the exchange's market type support certain order type
 * @todo Rate limit per session (later)
 * @todo Throw exception, catch and log the error
 */
class Order {
    public:
        /**
         * Constructor to create an order.
         */
        Order(Security security_, const MarketType market_type_, const string& timestamp_, const string& type_, int side_, double base_currency_size_, double quote_currency_size_, unsigned leverage_, const MarginType margintype_ ,double price_, Exchange exchange_): 
                security(security_), market_type(market_type_), timestamp(timestamp_), type(type_), side(side_), base_currency_size(base_currency_size_), quote_currency_size(quote_currency_size_), leverage(leverage_), margin_type(margintype_), price(price_), exchange(exchange_) {
                    id = ++next_id; // Assign a unique order id
                    
                    // Sanity checking zero and negative inputs
                    if (side != 1 && side != -1) {
                        throw invalid_argument("Order side must be 1 (buy) or -1 (sell)");
                        state = OrderState::Rejected;
                        return;
                    }
                    if (quote_currency_size == 0.0 && base_currency_size == 0.0) {
                        throw invalid_argument("Order size must be non-zero");
                        state = OrderState::Rejected;
                        return;
                    }
                    if (quote_currency_size_ < 0.0) {
                        throw invalid_argument("Quote currency order size must be non-negative");
                        state = OrderState::Rejected;
                        return;
                    }
                    if (base_currency_size_ < 0.0) {
                        throw invalid_argument("Base currency order size must be non-negative");
                        state = OrderState::Rejected;
                        return;
                    }
                    if (price_ <= 0.0) {
                        throw invalid_argument("Order price must be positive");
                        state = OrderState::Rejected;
                        return;
                    }
                    if (base_currency_size != 0 && quote_currency_size != 0) {
                        throw invalid_argument("Order size must only provided in base currency or quote currency, not both");
                        state = OrderState::Rejected;
                        return;
                    }

                    // Sanity checking price
                    if (countDigitsAfterDecimal(price) > countDigitsAfterDecimal(exchange.getSpotTradeRules(security)[0])) {
                        throw invalid_argument("Price must obey the minimum tick size");
                        state = OrderState::Rejected;
                        return;
                    }


                    // Sanity checking leverage
                    if (leverage < 1) {
                        throw invalid_argument("Leverage must be greater than or equal to 1");
                        state = OrderState::Rejected;
                        return;
                    }
                    if (leverage == 1 && margin_type != MarginType::NoMargin) {
                        throw invalid_argument("If leverage is 1x, margin type must be 'NoMargin'");
                        state = OrderState::Rejected;
                        return;
                    }
                    if (leverage != 1 && margin_type == MarginType::NoMargin) {
                        throw invalid_argument("If leverage is not 1x, margin type must not be 'NoMargin'");
                        state = OrderState::Rejected;
                        return;
                    }
                    if (market_type == MarketType::Spot && margin_type == MarginType::Isolated && leverage_ > exchange.getSpotTradeRules(security_)[10]) {
                        throw invalid_argument("Leverage must not exceed the maximum spot isolated leverage limit");
                        state = OrderState::Rejected;
                        return;
                    }
                    if (market_type == MarketType::Spot && margin_type == MarginType::Cross && leverage_ > exchange.getSpotTradeRules(security_)[11]) {
                        throw invalid_argument("Leverage must not exceed the maximum spot cross leverage limit");
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
                        throw invalid_argument("Order value must be greater than the minimum order value");
                        state = OrderState::Rejected;
                        return;
                    }
                    if (market_type == MarketType::Spot && quote_currency_size * leverage < exchange.getSpotTradeRules(security)[2]) {
                        throw invalid_argument("Order value must be greater than the minimum order value");
                        state = OrderState::Rejected;
                        return;
                    }

                    // Sanity checking maximum trade size
                    if (market_type == MarketType::Spot && (type == "LIMIT" || type == "STOPLIMIT") && (exchange.getSpotTradeRules(security)[3] != -1 && leverage_adjusted_base_currency_size > exchange.getSpotTradeRules(security)[3])) {
                        throw invalid_argument("Order value must not exceed the maximum limit order value");
                        state = OrderState::Rejected;
                        return;
                    }
                    if (market_type == MarketType::Spot && (type == "LIMIT" || type == "STOPLIMIT") && (exchange.getSpotTradeRules(security)[4] != -1 && leverage * quote_currency_size > exchange.getSpotTradeRules(security)[4])) {
                        throw invalid_argument("Order value must not exceed the maximum limit order value");
                        state = OrderState::Rejected;
                        return;
                    }
                    if (market_type == MarketType::Spot && (type == "MARKET" || type == "STOP") && (exchange.getSpotTradeRules(security)[5] != -1 && leverage_adjusted_base_currency_size > exchange.getSpotTradeRules(security)[5])) {
                        throw invalid_argument("Order value must not exceed the maximum market order value");
                        state = OrderState::Rejected;
                        return;
                    }
                    if (market_type == MarketType::Spot && (type == "LIMIT" || type == "STOPLIMIT") && (exchange.getSpotTradeRules(security)[6] != -1 && leverage * quote_currency_size > exchange.getSpotTradeRules(security)[6])) {
                        throw invalid_argument("Order value must not exceed the maximum market order value");
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
         * @param qty Base currency quantity to fill.
        */
        void fillOrder(double qty, double filled_price) {
            // Sanity checks
            if (qty <= 0) {
                throw invalid_argument("Filled quantity should be positive");
                return;
            }
            if (qty > leverage_adjusted_base_currency_size) {
                throw invalid_argument("Filled quantity should not be greater than the order size");
            }

            average_fill_price = (average_fill_price * filled_size + filled_price * qty)/(filled_size+qty);
            filled_size += qty;
            base_currency_size -= qty;
            quote_currency_size = base_currency_size * price;
            leverage_adjusted_base_currency_size = base_currency_size * leverage;

            if (base_currency_size > 0) {
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
            if (!isLiveOrder() && state != OrderState::SentToExchange) {return;}

            state = OrderState::Cancelled;
        }

        /**
         * Check if an exchange received the order and if so, change order status to working
         * @param current_timestamp Current timestamp in format 'yyyy-MM-dd HH:mm:ss.fffffffff'
         */
        void checkOrderReceived(const string& current_timestamp, const string& submitted_timestamp = "") {
            // Parse submitted timestamp
            tm tm_submitted;
            istringstream ss_submitted (timestamp);
            ss_submitted >> get_time (&tm_submitted, "%Y-%m-%d %H:%M:%S.%f");
            auto submitted_time_ns = chrono::time_point_cast<chrono::nanoseconds>(chrono::system_clock::from_time_t (mktime (&tm_submitted)));
            auto epoch_submitted = submitted_time_ns.time_since_epoch();
            epoch_submitted += chrono::nanoseconds (stoll (timestamp.substr (20)));

            // Parse current timestamp
            tm tm_current;
            istringstream ss_current (current_timestamp);
            ss_current >> get_time (&tm_current, "%Y-%m-%d %H:%M:%S.%f");
            auto current_time_ns = chrono::time_point_cast<chrono::nanoseconds>(chrono::system_clock::from_time_t (mktime (&tm_current)));
            auto epoch_current = current_time_ns.time_since_epoch();
            epoch_current += chrono::nanoseconds (stoll (current_timestamp.substr (20)));

            // Compute the time difference in seconds
            auto time_difference = epoch_current.count() - epoch_submitted.count();

            // Compare the time difference with the exchange's sending latency
            if (time_difference >= exchange.getSendingLatency()) {
                state = OrderState::Working;
            }
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
         * Getter for market type.
         */
        MarketType getMarketType() const {return market_type;}

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
        int id;                         /*< Unique ID */
        const MarketType market_type;   /*< Market type */
        const Security security;        /*< Security */
        const string timestamp;         /*< UTC date time, specified in format 'yyyy-MM-dd HH:mm:ss.fffffffff' */
        const string type;              /*< Order type */
        const int side;                 /*< -1 for SELL, 1 for BUY */
        double base_currency_size;      /*< Quantity of the order in base currency before leverage (For BTC/USDT, order size in BTC) */
        double quote_currency_size;     /*< Quantity of the order in quote currency before leverage (For BTC/USDT, order size in USDT) */
        unsigned leverage;              /*< Leverage of the order */
        const MarginType margin_type;   /*< Margin type of the order */
        double leverage_adjusted_base_currency_size;    /*< Leverage adjusted base currency size */
        double filled_size = 0.0;       /*< Quantify filled of the order */
        double average_fill_price;      /*< Average filled price of the order */
        double price;                   /*< Price for LIMIT and STOP LIMIT orders. Current price for MARKET and STOP orders */
        const Exchange exchange;        /*< Target exchange for an order (optional) */
        OrderState state = OrderState::SentToExchange;         /*< State of the order */
        vector<int> child_trade_id;     /*< Vector of trade ids executed from this order */

    private:
        static int next_id; /*< Static member to track the next available ID */ 
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
            throw invalid_argument("Modified price must be positive");
            return;
        }
        if (modified_base_currency_size == 0.0 && modified_quote_currency_size == 0.0) {
            throw invalid_argument("Order size must be non-zero");
            return;
        }
        if (modified_base_currency_size < 0.0) {
            throw invalid_argument("Modified base currency size must not be negative");
            return;
        }
        if (modified_quote_currency_size < 0.0) {
            throw invalid_argument("Modified quote currency size must not be negative");
            return;
        }
        if (base_currency_size != 0 && quote_currency_size != 0) {
            throw invalid_argument("Order size must only provided in base currency or quote currency, not both");
            return;
        }
        if (countDigitsAfterDecimal(modified_price) > countDigitsAfterDecimal(exchange.getSpotTradeRules(security)[0])) {
            throw invalid_argument("Modified price must obey the minimum tick size");
            return;
        }

        if (modified_base_currency_size == 0) {
            quote_currency_size = modified_quote_currency_size;
            double base_currency_min_size = exchange.getSpotTradeRules(security)[1];
            base_currency_size = int((modified_quote_currency_size / modified_price)/base_currency_min_size) * base_currency_min_size;
        } else if (modified_quote_currency_size == 0) {
            base_currency_size = modified_base_currency_size;
            quote_currency_size = base_currency_size * modified_price;
        }

        leverage_adjusted_base_currency_size = leverage * base_currency_size;
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
    Stop(Security security_, const MarketType market_type_, const string& timestamp_, int side_, double base_currency_size_, double quote_currency_size_, unsigned leverage_, const MarginType margintype_, double trigger_price_, Exchange exchange_): 
            Order(security_, market_type_, timestamp_, "STOP", side_, base_currency_size_, quote_currency_size_, leverage_, margintype_, trigger_price_, exchange_) {
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
            throw invalid_argument("Current price must be positive");
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
            throw invalid_argument("Order size must be non-zero");
            return;
        }
        if (modified_base_currency_size < 0.0) {
            throw invalid_argument("Modified base currency order size must not be negative");
            return;
        }
        if (modified_quote_currency_size < 0.0) {
            throw invalid_argument("Modified quote currency order size must not be negative");
            return;
        }
        if (base_currency_size != 0 && quote_currency_size != 0) {
            throw invalid_argument("Order size should only provided in base currency or quote currency, not both");
            return;
        }
        if (modified_trigger_price <= 0) {
            throw invalid_argument("Modified trigger price must be positive");
            return;
        }
        if (countDigitsAfterDecimal(modified_trigger_price) > countDigitsAfterDecimal(exchange.getSpotTradeRules(security)[0])) {
            throw invalid_argument("Modified trigger price must obey the minimum tick size");
            return;
        }
        
       if (modified_base_currency_size == 0) {
            quote_currency_size = modified_quote_currency_size;
            double base_currency_min_size = exchange.getSpotTradeRules(security)[1];
            base_currency_size = int((modified_quote_currency_size / modified_trigger_price)/base_currency_min_size) * base_currency_min_size;
        } else if (modified_quote_currency_size == 0) {
            base_currency_size = modified_base_currency_size;
            quote_currency_size = base_currency_size * modified_trigger_price;
        }

        leverage_adjusted_base_currency_size = leverage * base_currency_size;
        trigger_price = modified_trigger_price;
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
            throw invalid_argument("Current price must be positive");
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
     * @param modified_trigger_price New trigger price of the order. If it has been triggered, should input the original trigger price.
     */
    void modifyOrder(double modified_base_currency_size, double modified_quote_currency_size, double modified_price, double modified_trigger_price) {
        // Sanity checks
        if (!isLiveOrder()) {return;}
        if (modified_trigger_price != trigger_price && triggered) {
            throw invalid_argument("Trigger price cannot be modified once triggered");
            return;
        }
        if (modified_trigger_price <= 0.0) {
            throw invalid_argument("Trigger price must be positive");
            return;
        }
        if (modified_base_currency_size == 0.0 && modified_quote_currency_size == 0.0) {
            throw invalid_argument("Order size must be non-zero");
            return;
        }
        if (modified_base_currency_size < 0.0) {
            throw invalid_argument("Modified base currency size must not be negative");
            return;
        }
        if (modified_quote_currency_size < 0.0) {
            throw invalid_argument("Modified quote currency size must not be negative");
            return;
        }
        if (base_currency_size != 0 && quote_currency_size != 0) {
            throw invalid_argument("Order size should only provided in base currency or quote currency, not both");
            return;
        }
        if (countDigitsAfterDecimal(modified_price) > countDigitsAfterDecimal(exchange.getSpotTradeRules(security)[0])) {
            throw invalid_argument("Modified price must obey the minimum tick size");
            return;
        }
        if (countDigitsAfterDecimal(modified_trigger_price) > countDigitsAfterDecimal(exchange.getSpotTradeRules(security)[0])) {
            throw invalid_argument("Modified trigger price must obey the minimum tick size");
            return;
        }

        if (modified_base_currency_size == 0) {
            quote_currency_size = modified_quote_currency_size;
            double base_currency_min_size = exchange.getSpotTradeRules(security)[1];
            base_currency_size = int((modified_quote_currency_size / modified_price)/base_currency_min_size) * base_currency_min_size;
        } else if (modified_quote_currency_size == 0) {
            base_currency_size = modified_base_currency_size;
            quote_currency_size = base_currency_size * modified_price;
        }

        leverage_adjusted_base_currency_size = leverage * base_currency_size;

        double new_base_currency_size = modified_base_currency_size;
        double new_quote_currency_size = modified_quote_currency_size;

        trigger_price = modified_trigger_price;
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