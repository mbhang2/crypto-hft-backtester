
#include "../include/data/security.h"
#include "../include/backtesting/OME.h"
#include <iostream>
#include <cassert>

int main() {
    Exchange exchange("Binance");
    Security security("BTC", "USDT");
    MarketType marketType = MarketType::Spot;
    
    Orderbook orderbook(exchange, marketType, security);
    OME ome(orderbook);

    // Populate the order book with initial orders
    orderbook.AddOrder(100, 10, false);  // Order from another trader
    orderbook.AddOrder(100, 5, true);    // Our order
    orderbook.AddOrder(100, 6, false);    // Our order
    orderbook.printOrderBook();

    ome.matchOrder(100, 3, false);
    orderbook.printOrderBook();// Price Level 100: {[10, 0][5, 1][6, 0]}

    ome.matchOrder(100, 5, true);
    orderbook.printOrderBook();// Price Level 100: {[7, 0][5, 1][6, 0]}

    ome.matchOrder(100, 5, false);
    orderbook.printOrderBook(); // Price Level 100: {[2, 0][5, 1][6, 0]}

    ome.matchOrder(100, 2, true);
    orderbook.printOrderBook(); // Price Level 100: {[2, 1][6, 0]}

    ome.matchOrder(100, 4, false);
    orderbook.printOrderBook(); // Price Level 100: {[2, 1][4, 0]}

    ome.matchOrder(100, 5, false);
    orderbook.printOrderBook(); // Price Level 100: {}


    return 0;
}
