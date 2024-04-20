#pragma once


#include "../data/eventmsg.h"
#include "../data/exchange.h"
#include "order.h"
#include "../data/security.h"

#include <functional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

using namespace std;


/**
 * Abstract class for strategy
 */
class Strategy {
    public:
        /**
         * Default constructor
         */
        Strategy() {}

        /**
         * Constructor
         */
        Strategy(const string& strategy_name_, double spot_capital_, double futures_capital_): strategy_name(strategy_name_), spot_capital(spot_capital_), futures_capital(futures_capital_) {}

        /**
         * Destructor
         */
        virtual ~Strategy() = default;

        /**
         * Triggers when a trade event message arrives
         * @param event_msg trade event message
         * @return vector of orders to submit. Return empty vector if no orders to submit.
         */
        virtual vector<Order> onTrade(TradeEventMsg& event_msg) = 0;

        /**
         * Triggers when a change in top quote (BBO) message arrives
         * @param event_msg BBO update event message
         * @return vector of orders to submit. Return empty vector if no orders to submit.
         */
        virtual vector<Order> onTopQuote(QuoteEventMsg& event_msg) = 0;

        /**
         * Triggers when a change in orderbook message arrives
         * @param event_msg order book update event message
         * @return vector of orders to submit. Return empty vector if no orders to submit.
         */
        virtual vector<Order> onDepth(DepthEventMsg& event_msg) = 0;

        /**
         * Getter for spot capital
         */
        double getSpotCapital() const {return spot_capital;}

        /**
         * Getter for futures capital
         */
        double getFuturesCapital() const {return futures_capital;}

        /**
         * Getter for position
         */
        // double getPosition(MarketType market_type_, const Security& security_, const Exchange& exchange_) {return position[make_tuple(market_type_, security_, exchange_)];}

        /**
         * update spot capital
         */
        void updateSpotCapital(double capital_change) {
            spot_capital += capital_change;
        } 

        /**
         * update futures capital
         */
        void updateFuturesCapital(double capital_change) {
            futures_capital += capital_change;
        } 

        /**
         * Update position
         */
        // void updatePosition(MarketType market_type_, Security& security_, Exchange& exchange_, double position_change) {
        //     position[make_tuple(market_type_, security_, exchange_)] += position_change;
        // }

    protected:
        const string strategy_name;
        double spot_capital;
        double futures_capital;
        // unordered_map<tuple<MarketType, Security, Exchange>, double> position;
};