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
    if (argc != 5) {
        cerr << "Usage: " << argv[0] << "<Initial Spot Balance> <Initial Futures Balance> <Configuration Path> <Market Data Path>\n";
    }

    User user(stod(argv[1]), stod(argv[2]), argv[3]);

    MovingAverageCross ma_cross(user, 1, 2, 5);

    Backtester backtester(user, &ma_cross);
    backtester.runBacktest(argv[4]);
    backtester.getTradeLog().exportToCSV("./sample_data/sample_result.csv");

    cout << "Strategy Total Realized P&L: " << backtester.getTradeLog().computeTotalRealizedPNL() << endl;
}