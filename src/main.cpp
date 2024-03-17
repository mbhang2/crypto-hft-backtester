#include <iostream>
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

    if (Binance.loadJson("./tests/exchange_json_test/exchange.json")) {
        std::cout << "Load successful" << endl;
        std::cout << "DOT/USDT tick size at Binance is " << Binance.getSpotTradeRules(DOTUSDT)[0] << DOTUSDT.getQuote() << endl;
        std::cout << "DOGE/USDT tick size at Binance is " << Binance.getSpotTradeRules(DOGEUSDT)[0] << DOGEUSDT.getQuote() << endl;
        std::cout << "Receiving latency for Binance is " << Binance.getReceivingLatency() << "nanoseconds" << endl;
        std::cout << "Max market order quantity for BTC/USDT in Binance is " << Binance.getSpotTradeRules(BTCUSDT)[5] << BTCUSDT.getQuote() << endl;
    }

    if (Okx.loadJson("./tests/exchange_json_test/exchange.json")) {
        std::cout << "Load successful" << endl;
        std::cout << "DOT/USDT tick size at Okx is " << Okx.getSpotTradeRules(DOTUSDT)[0] << DOTUSDT.getQuote() << endl;
        std::cout << "DOGE/USDT tick size at Okx is " << Okx.getSpotTradeRules(DOGEUSDT)[0] << DOGEUSDT.getQuote() << endl;
        std::cout << "Receiving latency for Okx is " << Okx.getReceivingLatency() << "nanoseconds" << endl;
        std::cout << "Max market order value for BTC/USDT in Okx is " << Okx.getSpotTradeRules(BTCUSDT)[6] << BTCUSDT.getQuote() << endl;
    }
}