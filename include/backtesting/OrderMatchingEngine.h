#pragma once

#include "Orderbook.h"

#include <memory>
#include <vector>

class OrderMatchingEngine
{
public:

    OrderMatchingEngine() = default;

    void OrderMatchingEngine::addOrder(
        double price,
        double size,
        bool isBuyOrder,
        bool isOurOrder,
        MarketType marketType,
        Security security
    )
    {
        Orderbook &selectedOrderBook = selectOrderBook(isBuyOrder, marketType);
        selectedOrderBook.AddOrder(price, size, isOurOrder);

        matchOrders();
    }

    void OrderMatchingEngine::matchOrders()
    {
        double bestBuyPrice = buyOrderBookSpot.getBestPrice(true);
        double bestSellPrice = sellOrderBookSpot.getBestPrice(false);

        while (bestBuyPrice >= bestSellPrice && !buyOrderBookSpot.isEmpty()
               && !sellOrderBookSpot.isEmpty())
        {
            std::cout << "Trade executed at price: " << bestSellPrice
                      << std::endl;

            buyOrderBookSpot.TradeHappened(bestBuyPrice, 1);
            sellOrderBookSpot.TradeHappened(bestSellPrice, 1);

            // Recalculate best prices for next iteration
            bestBuyPrice = buyOrderBookSpot.getBestPrice(true);
            bestSellPrice = sellOrderBookSpot.getBestPrice(false);
        }
    }

    void OrderMatchingEngine::cancelOrder(
        double price,
        double size,
        bool isOurOrder,
        MarketType marketType,
        Security security
    )
    {
        Orderbook &selectedOrderBook = selectOrderBook(true, marketType);
        selectedOrderBook.CancelOrder(price, size, isOurOrder);
    }

private:

    Orderbook buyOrderBookSpot;
    Orderbook sellOrderBookSpot;
    Orderbook buyOrderBookFutures;
    Orderbook sellOrderBookFutures;

    Orderbook &OrderMatchingEngine::selectOrderBook(
        bool isBuyOrder,
        MarketType marketType
    )
    {
        if (marketType == MarketType::Spot)
        {
            return isBuyOrder ? buyOrderBookSpot : sellOrderBookSpot;
        }
        else
        {
            return isBuyOrder ? buyOrderBookFutures : sellOrderBookFutures;
        }
    }

    void OrderMatchingEngine::processOrder(
        Orderbook &orderBook,
        double price,
        double size,
        bool isOurOrder
    )
    {
        orderBook.AddOrder(price, size, isOurOrder);
    }
    void OrderMatchingEngine::executeTrade(Orderbook &buyBook, Orderbook &sellBook) {}
};
