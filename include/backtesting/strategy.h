#pragma once


#include "../data/eventmsg.h"
#include "../data/exchange.h"
#include "order.h"
#include "../data/security.h"

#include <string>
#include <vector>

using namespace std;


/**
 * Abstract class for strategy
 */
class Strategy {
    public:
        /**
         * Constructor
         */
        Strategy(const string& strategy_name_): strategy_name(strategy_name_) {}

        /**
         * Destructor
         */
        virtual ~Strategy();

        /**
         * Triggers when a trade event message arrives
         * @param event_msg trade event message
         * @return vector of orders to submit. Return empty vector if no orders to submit.
         */
        virtual vector<Order> onTrade(TradeEventMsg& event_msg);

        /**
         * Triggers when a change in top quote (BBO) message arrives
         * @param event_msg BBO update event message
         * @return vector of orders to submit. Return empty vector if no orders to submit.
         */
        virtual vector<Order> onTopQuote(QuoteEventMsg& event_msg);

        /**
         * Triggers when a change in orderbook message arrives
         * @param event_msg order book update event message
         * @return vector of orders to submit. Return empty vector if no orders to submit.
         */
        virtual vector<Order> onDepth(DepthEventMsg& event_msg);

    protected:
        const string strategy_name;
};