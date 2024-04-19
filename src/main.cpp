#include <iostream>
#include <iomanip>
#include "../include/backtesting/order.h"
#include "../include/backtesting/trade.h"
#include "../include/data/exchange.h"
#include "../include/data/security.h"

int Order::next_id = 0;

int main(int  /*argc*/, char**  /*argv*/) {
    Exchange Binance("Binance");
    Exchange Okx("Okx");
    Exchange Bybit("Bybit");

    Security BTCUSDT = Security("BTC", "USDT");
    Security DOTUSDT = Security("DOT", "USDT");
    Security DOGEUSDT = Security("DOGE", "USDT");

    Binance.loadJson("./tests/exchange_json_test/exchange.json");
    Bybit.loadJson("./tests/exchange_json_test/exchange.json");
    
    // cout << std::fixed << std::setprecision(7) << Binance.getSpotTradeRules(BTCUSDT)[1] << endl;
    Limit limit(BTCUSDT, MarketType::Spot, "2024-03-22 12:30:15.123456789", 1, 1, 0, 5, MarginType::Cross, 67650.37, Binance, 1000000);

    cout << limit << setprecision(9) << endl;
    
    limit.fillOrder(0.5, 67530.37);

    cout << limit << setprecision(9) << endl;
}