#pragma once

#include "Orderbook.h"
#include <memory>
#include <vector>

class OrderMatchingEngine {
public:
    OrderMatchingEngine() = default;

    // Adds a new order to the system
    void OrderMatchingEngine::addOrder(double price, double size, bool isBuyOrder, bool isOurOrder, MarketType marketType, Security security) {
    Orderbook& selectedOrderBook = selectOrderBook(isBuyOrder, marketType);
    selectedOrderBook.AddOrder(price, size, isOurOrder);
    // Optionally, after adding an order, you could immediately attempt to match orders.
    matchOrders();
}

    // Matches orders in the orderbook and executes trades
    void OrderMatchingEngine::matchOrders() {
    // Simplified matching logic: Match the best buy with the best sell if their prices overlap.
    // This simplistic approach does not account for volumes or partial fills.

    // Example for spot market, similar logic would be applied to futures market if needed.
    double bestBuyPrice = buyOrderBookSpot.getBestPrice(true);
    double bestSellPrice = sellOrderBookSpot.getBestPrice(false);

    while (bestBuyPrice >= bestSellPrice && !buyOrderBookSpot.isEmpty() && !sellOrderBookSpot.isEmpty()) {
        // Here, we should determine the size to trade and execute the trade, but for simplicity:
        std::cout << "Trade executed at price: " << bestSellPrice << std::endl;

        // After a trade, update or remove the orders from the order books. This step is oversimplified here:
        // Assume each trade completely fills one order from each book.
        buyOrderBookSpot.TradeHappened(bestBuyPrice, 1); // Assuming a trade size of 1 for simplicity
        sellOrderBookSpot.TradeHappened(bestSellPrice, 1); // Assuming a trade size of 1 for simplicity

        // Recalculate best prices for next iteration
        bestBuyPrice = buyOrderBookSpot.getBestPrice(true);
        bestSellPrice = sellOrderBookSpot.getBestPrice(false);
    }
}

    // Cancels an order in the system
    void OrderMatchingEngine::cancelOrder(double price, double size, bool isOurOrder, MarketType marketType, Security security) {
    Orderbook& selectedOrderBook = selectOrderBook(true, marketType); // 'true' is arbitrary here since we're just looking up by price
    // The cancellation process might need to be more sophisticated in a real system to accurately identify the order.
    selectedOrderBook.CancelOrder(price, size, isOurOrder);
}

private:
    // One possible approach is to have separate order books for buy and sell orders for both spot and futures markets.
    // This assumes you have a way to distinguish between spot and futures markets within the Security or MarketType classes.
    Orderbook buyOrderBookSpot;
    Orderbook sellOrderBookSpot;
    Orderbook buyOrderBookFutures;
    Orderbook sellOrderBookFutures;

    // A method to select the appropriate order book based on the order details
    Orderbook& OrderMatchingEngine::selectOrderBook(bool isBuyOrder, MarketType marketType) {
    if (marketType == MarketType::Spot) {
        return isBuyOrder ? buyOrderBookSpot : sellOrderBookSpot;
    } else {
        return isBuyOrder ? buyOrderBookFutures : sellOrderBookFutures;
    }
}

    // Utility functions to process orders
    void OrderMatchingEngine::processOrder(Orderbook& orderBook, double price, double size, bool isOurOrder) {
    // This function could encapsulate logic for adding an order to the book and any immediate processing needed.
    orderBook.AddOrder(price, size, isOurOrder);
}
    void OrderMatchingEngine::executeTrade(Orderbook& buyBook, Orderbook& sellBook) {
    // This function would handle the mechanics of executing a trade, including updating the books and possibly notifying parties.
    // Placeholder for trade execution logic.
}
};
