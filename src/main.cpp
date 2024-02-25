#include <iostream>
#include "../include/data/security.h"
#include "../include/data/exchange.h"
#include "../include/backtesting/order.h"

using namespace std;


int main() {
    Security BTCUSDT("BTC", "USDT");

    StopLimit sb(BTCUSDT, "2024-02-24 12:30:05.123456789", 1, 1.5, 50000.50, 50001);

    cout << sb << endl;

    double current_price = 50005;
    double bb1 = 50000;
    double bo1 = 50000.5;

    cout << "Testing Stop Limit Buy Order @50000.5 with trigger price @ 50001 where current price = 50005" << endl;
    sb.checkTriggered(current_price);
    cout << sb.checkFillability(bb1,bo1) << endl;
    cout << sb.isTriggered() << endl;

    cout << "Testing Stop Limit Buy Order @50000.5 where best offer = 50000.25" << endl;
    bo1 = 50000.25;
    cout << sb.checkFillability(bb1, bo1) << endl;

    sb.fillOrder(0.5);
    cout << "Filled 0.5" << endl;
    cout << sb.getFilledSize() << endl;
    cout << sb << endl;

    cout << "Filled 1.0" << endl;
    sb.fillOrder(1.0);
    cout << sb << endl;

    // cout << stop_buy << endl;

    return 0;
}