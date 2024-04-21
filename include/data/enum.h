#pragma once

#include <iostream>
#include <string>

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
 * << operator overload for MarketType class.
 */
ostream& operator<<(ostream& os, const MarketType& market_type) {
    switch (market_type) {
        case MarketType::Spot:
            os << "Spot";
            break;
        case MarketType::Futures:
            os << "Futures";
            break;
        default:
            os << "Unknown"; // Handle any other values gracefully
    }
    return os;
}

/**
 * Hash overload for MarketType
 */
namespace std {
    template <>
    struct hash<MarketType> {
        size_t operator()(const MarketType& type) const {
            // Return a constant hash value for each enum value
            switch (type) {
                case MarketType::Spot:
                    return 42; // Replace with your desired hash value
                case MarketType::Futures:
                    return 24; // Replace with another desired hash value
            }
        }
    };
}

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