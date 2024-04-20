#include <iostream>
#include <iomanip>

#include "../include/backtesting/order.h"
#include "../include/backtesting/trade.h"
#include "../include/backtesting/backtester.h"
#include "../include/data/exchange.h"
#include "../include/data/security.h"
#include "./sample_strategy.h"

using namespace std;

int main(int argc, char* argv[]) {
    // if (argc != 3) {
    //     cerr << "Usage: " << argv[0] << " <arg1> <arg2>\n";
    // }

    Security btcusdt("BTC", "USDT");

    Exchange binance("Binance");
    binance.loadJson("./configuration/exchange.json");

    Exchange bybit("Bybit");
    bybit.loadJson("./configuration/exchange.json");

    User user(1000000.0, 1000000.0);
    user.addExchange(binance);
    user.addExchange(bybit);

    MarketType spot = MarketType::Spot;

    cout << user.findExchange("binance")->findSecurity(spot, "BTC/USDT")->getBase() << endl;

    MovingAverageCross ma_cross(1, 5, 20, 1000000.0, 1000000.0);

    Backtester backtester(user, &ma_cross);
    backtester.runBacktest("./sample_data/sample_input_data.csv");
}