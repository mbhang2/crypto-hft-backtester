
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
    orderbook.AddOrder(100, 6, false);    // Not Our order
    orderbook.AddOrder(101, 6, false);    // Not Our order

    cout << "Initial Order Book: " << endl;
    orderbook.printOrderBook();// Price Level 100: {[10, 0][5, 1][6, 0]}

    ome.matchOrder(100, 3, false);
    cout << "After matching 3 units of order at price level 100 (not our order): " << endl;
    orderbook.printOrderBook();// Price Level 100: {[7, 0][5, 1][6, 0]}

    ome.matchOrder(100, 5, true);
    cout << "After matching 5 units of order at price level 100 (our order): " << endl;
    orderbook.printOrderBook();// Price Level 100: {[2, 0][5, 1][6, 0]}

    ome.matchOrder(100, 5, false);
    cout << "After matching 5 units of order at price level 100 (not our order): " << endl;
    orderbook.printOrderBook(); // Price Level 100: {[2, 1][6, 0]}

    ome.matchOrder(100, 2, true);
    cout << "After matching 2 units of order at price level 100 (our order): " << endl;
    orderbook.printOrderBook(); // Price Level 100: {[2, 1][4, 0]}

    ome.matchOrder(100, 4, false);
    cout << "After matching 4 units of order at price level 100 (not our order): " << endl;
    orderbook.printOrderBook(); 

    ome.matchOrder(100, 5, false);
    cout << "After matching 5 units of order at price level 100 (not our order) but move to next price level: " << endl;
    orderbook.printOrderBook(); // Price Level 100: {}


    return 0;
}
