#include "../include/backtesting/OrderBook.h"
#include "../include/data/exchange.h"
#include "../include/data/security.h"

#include <iostream>
#include <cassert>

// simulate multiple stategies: different users
// orderbook with exchange

int main() {    
    Exchange exchange("Binance");
    Security security("BTC", "USDT");
    MarketType marketType = MarketType::Spot;
    
    Orderbook orderbook(exchange, marketType, security);
    
    orderbook.AddLevel(100); 
    orderbook.AddOrder(100, 6, false); 
    orderbook.AddOrder(100, 1, true);
    orderbook.AddOrder(100, 8, false);
    orderbook.AddOrder(100, 11, false);
    orderbook.printOrderBook();

    orderbook.TradeHappened(100, 3);
    orderbook.printOrderBook();
    
    orderbook.TradeHappened(100, 3.5);
    cout << "Trade happened at 100, 3.5" << endl;
    orderbook.printOrderBook();

    orderbook.AddLevel(101);
    orderbook.AddLevel(102); 
    orderbook.AddLevel(99); 
    orderbook.printOrderBook();

    // Test case 1: Cancel an existing order
    cout << "Test case 1: Cancel an existing order" << endl;
    orderbook.CancelOrder(100, 0.5, true);
    orderbook.printOrderBook();
    return 0;
}
