#pragma once

#include "../data/eventmsg.h"
#include "../data/exchange.h"
#include "../data/security.h"

#include <string>

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
         */
        virtual void onTrade(TradeEventMsg& event_msg);

        /**
         * Triggers when a change in top quote (BBO) message arrives
         */
        virtual void onTopQuote(QuoteEventMsg& event_msg);

        /**
         * Triggers when a change in orderbook message arrives
         */
        virtual void onDepth(DepthEventMsg& event_msg);

    protected:
        const string strategy_name;
};