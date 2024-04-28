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

    MovingAverageCross ma_cross(user, 1, 5, 20);

    Backtester backtester(user, &ma_cross);
    backtester.runBacktest("./sample_data/sample_input.csv");
}