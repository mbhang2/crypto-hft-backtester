#include <iostream>
#include "../include/data/security.h"
#include "../include/data/exchange.h"
#include "../include/backtesting/order.h"
#include "../include/backtesting/trade.h"

using namespace std;


int main() {
    Security BTCUSDT("BTC", "USDT");
    Security ETHUSDT("ETH", "USDT");
    Security XRPUSDT("XRP", "USDT");
    Security BCHUSDT("BCH", "USDT");

    Exchange Binance("Binance", 1000, 1000);

    Binance.loadTickSizesFromJson("tests/exchange_json_test/binance.json");
    cout << "Testing tick size configuration" << endl;
    cout << "Tick size for " << BTCUSDT << " is " << Binance.getTickSize(BTCUSDT) << endl;
    cout << "Tick size for " << ETHUSDT << " is " << Binance.getTickSize(ETHUSDT) << endl;
    cout << "Tick size for " << XRPUSDT << " is " << Binance.getTickSize(XRPUSDT) << endl;

    cout << "**Ticksize for BCH/USDT is not provided in json file for testing purposes" << endl;
    cout << "Tick size for " << BCHUSDT << " is " << Binance.getTickSize(BCHUSDT) << endl;

    Limit l(BTCUSDT, "2024-02-24 12:30:05.123456789", 1, 1.5, 50000.00, Binance);

    cout << l << endl;

    return 0;
}