#pragma once

#include "../include/backtesting/strategy.h"

#include <string>
#include <vector>

using namespace std;


/**
 * Struct for storing candlestick data
 */
struct Candlestick {
    string timestamp_;
    double open_;
    double high_;
    double low_;
    double close_;
    unsigned volume_;

    /**
     * Constructor for candlestick struct
     */
    Candlestick(const string& timestamp, double open, double high, double low, double close, unsigned volume): 
            timestamp_(timestamp), open_(open), high_(high), low_(low), close_(close), volume_(volume) {}

    /**
     * Override << operator for Candlestick class.
     */
    friend ostream& operator<<(ostream& os, const Candlestick& cs) {
        os << "========== Candlestick ==========" << endl;
        os << "Time: " << cs.timestamp_ << endl;
        os << "Open: " << cs.open_ << endl;
        os << "High: " << cs.high_ << endl;
        os << "Low: " << cs.low_<< endl;
        os << "Close: " << cs.close_ << endl;
        os << "Volume: " << cs.volume_ << endl;
        os << "=======================================" << endl;
        return os;
    }
};


/**
 * Abstract class for strategy
 */
class MovingAverageCross: public Strategy {
    public:
        /**
         * Constructor
         */
        MovingAverageCross(int candlestick_minute_ = 1, int short_length_ = 5, int long_length_ = 20, double spot_capital_ = 1000000.0, double futures_capital_ = 1000000.0): 
                Strategy("Moving Average Cross", spot_capital_, futures_capital_), candlestick_minute(candlestick_minute_), short_length(short_length_), long_length(long_length_) {}

        /**
         * Destructor
         */
        virtual ~MovingAverageCross() {}

        /**
         * Triggers when a trade event message arrives
         * @param event_msg trade event message
         * @return vector of orders to submit. Return empty vector if no orders to submit.
         */
        virtual vector<std::shared_ptr<Order>> onTrade(TradeEventMsg& event_msg) {
            vector<std::shared_ptr<Order>> orders;

            string trade_timestamp = event_msg.timestamp->toString();
            double trade_price = event_msg.price;
            double trade_size = event_msg.size;

            if (candlestick_vector.empty()) {
                string rounded_timestamp = roundDownTimestamp(trade_timestamp, candlestick_minute);
                candlestick_vector.push_back(Candlestick(rounded_timestamp, trade_price, trade_price, trade_price, trade_price, trade_size));
                next_candlestick_open = addMinutesToTimestamp(rounded_timestamp, candlestick_minute);
            } 
            else if (trade_timestamp.compare(next_candlestick_open) < 0) {
                Candlestick cd = candlestick_vector.back();
                cd.close_ = trade_price;

                if (trade_price > cd.high_) {cd.high_ = trade_price;}
                if (trade_price < cd.low_) {cd.low_ = trade_price;}

                cd.volume_ += trade_size;
            } 
            else {
                string rounded_timestamp = roundDownTimestamp(trade_timestamp, candlestick_minute);
                next_candlestick_open = addMinutesToTimestamp(rounded_timestamp, candlestick_minute);
                candlestick_vector.push_back(Candlestick(rounded_timestamp, trade_price, trade_price, trade_price, trade_price, trade_size));

                if (candlestick_vector.size() == long_length + 1) {
                    int index = 0;
                    double short_ma = 0.0;
                    double long_ma = 0.0;
                    double prev_short_ma = 0.0;
                    double prev_long_ma = 0.0;

                    for (int i = candlestick_vector.size() - 1; i >= 0; i--) {
                        double curr_close = candlestick_vector.at(i).close_;

                        if (index >= 0 && index < short_length) {
                            short_ma += curr_close;
                        }
                        if (index > 0 && index <= short_length) {
                            prev_short_ma += curr_close;
                        }
                        if (index >= 0 && index < long_length) {
                            long_ma += curr_close;
                        }
                        if (index > 0 && index <= long_length) {
                            prev_long_ma += curr_close;
                        }

                        index++;
                    }

                    short_ma /= short_length;
                    prev_short_ma /= short_length;
                    long_ma /= long_length;
                    prev_long_ma /= long_length;

                    double curr_pos = getPosition(MarketType::Spot, *event_msg.exchange, *event_msg.security);
                    cout << "Current position is " << curr_pos << endl;

                    MarginType mt = MarginType::NoMargin;
                    // Check long entry condition
                    if (prev_short_ma < prev_long_ma && short_ma > long_ma) {
                        if (curr_pos == 0) {
                            Market* market = new Market(event_msg.security, event_msg.market_type, event_msg.timestamp, 1, int(getSpotCapital() * 0.1 / trade_price), 0, 1, mt, trade_price, event_msg.exchange, getSpotCapital());
                            // cout << market.getTimestamp().toString() << endl;
                            orders.push_back(std::shared_ptr<Order>(market));
                        } else {
                            Market* market = new Market(event_msg.security, event_msg.market_type, event_msg.timestamp, 1, int(getSpotCapital() * 0.1 / trade_price) + abs(curr_pos), 0, 1, mt, trade_price, event_msg.exchange, getSpotCapital());
                            // cout << market.getTimestamp().toString() << endl;
                            orders.push_back(std::shared_ptr<Order>(market));
                        }
                    }
                    // Check short entry condition
                    else if (prev_short_ma > prev_long_ma && short_ma < long_ma) {
                        if (curr_pos == 0) {
                            Market* market = new Market(event_msg.security, event_msg.market_type, event_msg.timestamp, -1, int(getSpotCapital() * 0.1 / trade_price), 0, 1, mt, trade_price, event_msg.exchange, getSpotCapital());
                            // cout << market.getTimestamp().toString() << endl;
                            orders.push_back(std::shared_ptr<Order>(market));
                        } else {
                            Market* market = new Market(event_msg.security, event_msg.market_type, event_msg.timestamp, -1, int(getSpotCapital() * 0.1 / trade_price) + abs(curr_pos), 0, 1, mt, trade_price, event_msg.exchange, getSpotCapital());
                            // cout << market.getTimestamp().toString() << endl;
                            orders.push_back(std::shared_ptr<Order>(market));
                        }
                    }

                    candlestick_vector.erase(candlestick_vector.begin());
                }
            }

            return orders;
        }

        /**
         * Triggers when a change in top quote (BBO) message arrives
         * @param event_msg BBO update event message
         * @return vector of orders to submit. Return empty vector if no orders to submit.
         */
        virtual vector<std::shared_ptr<Order>> onTopQuote(QuoteEventMsg& event_msg) {return vector<std::shared_ptr<Order>>();}

        /**
         * Triggers when a change in orderbook message arrives
         * @param event_msg order book update event message
         * @return vector of orders to submit. Return empty vector if no orders to submit.
         */
        virtual vector<std::shared_ptr<Order>> onDepth(DepthEventMsg& event_msg) {return vector<std::shared_ptr<Order>>();}

    protected:
        string next_candlestick_open;           /*< String timestamp for next candlestick open */
        const int candlestick_minute;           /*< Candlestick timeframe in minutes */
        const int short_length;                 /*< Short moving average length */
        const int long_length;                  /*< Long moving average length */
        vector<Candlestick> candlestick_vector;   /*< Queue of candlesticks */

    private:
    /**
     * Helper function that rounds down a timestamp to the nearest minute increment
     * @param inputTimestamp timestamp specified in format 'yyyy-MM-dd HH:mm:ss.fffffffff'
     * @param minute_increment minute resolution to round down to
     * @return rounded down timestamp in the same format
    */
    string roundDownTimestamp(const string& input_timestamp, int minute_increment) {
        // Parse the input timestamp
        tm tmStruct = {};
        istringstream ss(input_timestamp);
        ss >> get_time(&tmStruct, "%Y-%m-%d %H:%M:%S");

        // Calculate the rounded down timestamp
        int totalMinutes = tmStruct.tm_hour * 60 + tmStruct.tm_min;
        int roundedMinutes = (totalMinutes / minute_increment) * minute_increment;
        tmStruct.tm_hour = roundedMinutes / 60;
        tmStruct.tm_min = roundedMinutes % 60;
        tmStruct.tm_sec = 0;

        // Convert back to string
        ostringstream result;
        result << put_time(&tmStruct, "%Y-%m-%d %H:%M:%S");
        result << ".000000000"; // Append fractional seconds

    return result.str();
    }

    /**
     * Helper function that adds given minutes to the timestamp
     * @param inputTimestamp timestamp specified in format 'yyyy-MM-dd HH:mm:ss.fffffffff'
     * @param minute_increment minute resolution to round down to
     * @return timestamp after adding time in the same format
    */
    string addMinutesToTimestamp(const string& input_timestamp, int minutes_to_add) {
        // Parse the input timestamp
        tm tmStruct = {};
        istringstream ss(input_timestamp);
        ss >> get_time(&tmStruct, "%Y-%m-%d %H:%M:%S");

        // Calculate the new timestamp
        time_t timestampInSeconds = mktime(&tmStruct);
        timestampInSeconds += minutes_to_add * 60; // Convert minutes to seconds

        // Convert back to struct tm
        tm newTmStruct = *localtime(&timestampInSeconds);

        // Convert back to string
        ostringstream result;
        result << put_time(&newTmStruct, "%Y-%m-%d %H:%M:%S");
        result << ".000000000"; // Append fractional seconds

        return result.str();
    }
};