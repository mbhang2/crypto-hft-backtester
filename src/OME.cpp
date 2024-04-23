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
    // Adding sell orders
    orderbook.AddOrder(99, 10, false, OrderSide::Sell);  // Sell order from another trader
    orderbook.AddOrder(98, 6, false, OrderSide::Sell);   // Sell order from another trader

    // Adding buy orders
    orderbook.AddOrder(97, 5, true, OrderSide::Buy);     // Our buy order
    orderbook.AddOrder(96, 6, false, OrderSide::Buy);    // Buy order from another trader

    cout << "Initial Order Book: " << endl;
    orderbook.printOrderBook(); // Expected: Sell orders at 98 and 99, buy orders at 96 and 97

    // Incoming sell order matching with existing buy orders
    ome.matchOrder(97, 3, false, OrderSide::Sell);
    cout << "After matching 3 units of sell order at price level 97 (not our order): " << endl;
    orderbook.printOrderBook(); // Expected: Our buy order at 97 reduced by 3

    // Incoming buy order matching with existing sell orders
    ome.matchOrder(98, 4, true, OrderSide::Buy);
    cout << "After matching 4 units of buy order at price level 98 (our order): " << endl;
    orderbook.printOrderBook(); // Expected: Sell order at 98 reduced by 4

    // Matching another buy order
    ome.matchOrder(99, 2, false, OrderSide::Buy);
    cout << "After matching 2 units of buy order at price level 99 (not our order): " << endl;
    orderbook.printOrderBook(); // Expected: Sell order at 99 reduced by 2

    // Incoming buy order that is larger than any single sell order
    ome.matchOrder(96, 10, false, OrderSide::Buy);
    cout << "After matching 10 units of buy order at price level 96 (not our order): " << endl;
    orderbook.printOrderBook(); // Expected: Buy order at 96 reduced completely and part of 97 if possible

    // Continue with additional test cases as needed...

    return 0;
}
