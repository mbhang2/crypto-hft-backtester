#include <iostream>
#include <iomanip>
#include "../include/backtesting/order.h"
#include "../include/backtesting/trade.h"
#include "../include/data/exchange.h"
#include "../include/data/security.h"


int main(int  /*argc*/, char**  /*argv*/) {
    Exchange Binance("Binance");
    Exchange Okx("Okx");

    Security BTCUSDT = Security("BTC", "USDT");
    Security DOTUSDT = Security("DOT", "USDT");
    Security DOGEUSDT = Security("DOGE", "USDT");

    Binance.loadJson("./tests/exchange_json_test/exchange.json");

    cout << Binance.getSpotMakerFee() << endl;

    Binance.setSpotTradingFee(5);

    cout << Binance.getSpotMakerFee() << endl;

    cout << Binance.getFuturesMakerFee() << endl;

    Binance.setFuturesTradingFee(5);

    cout << Binance.getFuturesMakerFee() << endl;
    
    // cout << std::fixed << std::setprecision(7) << Binance.getSpotTradeRules(BTCUSDT)[1] << endl;
    Limit limit(BTCUSDT, MarketType::Spot, "2024-03-22 12:30:15.123456789", 1, 0, 0.1, 5, MarginType::Cross, 67650.37, Binance);

    cout << limit << setprecision(9) << endl;
}