#pragma once


#include "./exchange.h"
#include "./security.h"

#include <string>

using namespace std;


/**
 * Parent class for all event messages
 */
class EventMsg {
public:
    EventMsg(const string& timestamp_, const Exchange& exchange_, const MarketType market_type_, const Security& security_)
        : timestamp(timestamp_), exchange(exchange_), market_type(market_type_), security(security_) {}

    const string timestamp;         /*< timestamp of an event */
    const Exchange exchange;        /*< exchange an event occurred */
    const MarketType market_type;  /*< Market type (Futures or spot) */
    const Security security;        /*< instrument an event occurred */
};

/**
 * Derived class for trade messages
 */
class TradeEventMsg : public EventMsg {
public:
    TradeEventMsg(const string& timestamp_, const Exchange& exchange_, const MarketType market_type_, const Security& security_, double price_, double size_): 
            EventMsg(timestamp_, exchange_, market_type_, security_), price(price_), size(size_) {}

    const double price;  /*< price at which the trade occurred */
    const double size;   /*< size of the trade in base currency */ 

    /**
     * Override << operator for Market class.
     */
    friend ostream& operator<<(ostream& os, const TradeEventMsg& msg) {
        os << "========== Trade Event ==========" << endl;
        os << "Timestamp: " << msg.timestamp << endl;
        os << "Market: " << msg.market_type << endl;
        os << "Security: " << msg.security << endl;
        os << "Exchange: " << msg.exchange.getName() << endl;
        os << "Price: " << msg.price << endl;
        os << "Size: " << msg.size << msg.security.getBase() << endl;
        os << "=======================================" << endl;
        return os;
    }
};


/**
 * Derived class for quote update messages
 */ 
class QuoteEventMsg : public EventMsg {
public:
    QuoteEventMsg(const string& timestamp_, const Exchange& exchange_, const MarketType market_type_, const Security& security_, double bid_price_, double bid_size_, double ask_price_, double ask_size_): 
        EventMsg(timestamp_, exchange_, market_type_, security_), bid_price(bid_price_), bid_size(bid_size_), ask_price(ask_price_), ask_size(ask_size_) {}

    const double bid_price;  /*< best bid price */
    const double bid_size;   /*< best bid size in base currency */
    const double ask_price;  /*< best ask price */
    const double ask_size;   /*< best ask size in base currency */
};


/**
 * Derived class for depth messages
 */ 
class DepthEventMsg : public EventMsg {
public:
    DepthEventMsg(const string& timestamp_, const Exchange& exchange_, const MarketType market_type_, const Security& security_, int side_, double price_, double size_): 
        EventMsg(timestamp_, exchange_, market_type_, security_), side(side_), price(price_), size(size_) {}

    const int side;      /*< indicates bid (1) or ask (-1) side of the order book */
    const double price;  /*< price for the entry in the order book */
    const double size;   /*< base currency size associated with the book entry */
};
