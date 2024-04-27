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

    User user(1000000.0, 1000000.0, "./configuration/exchange.json");

    MovingAverageCross ma_cross(1, 5, 20, 1000000.0, 1000000.0);

    Backtester backtester(user, &ma_cross);
    backtester.runBacktest("./sample_data/20230103_trades.csv");

    // Market market(btcusdt, MarketType::Spot, "2024-04-19 20:30:23.123456789", -1, 2, 0, 5, MarginType::Isolated, 69321.23, binance, 1000000);
    // cout << market.getSide() << endl;
    // market.fillOrder(3, 60000);
    // cout << market << endl;
}