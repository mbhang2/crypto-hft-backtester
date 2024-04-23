#include "OrderBook.h"
#include <iostream>
#include <queue>
#include <map>

using namespace std;

class OME {
public:
    OME(Orderbook &orderbook_)
        : orderbook(orderbook_) {}

    virtual ~OME() {}

    void matchOrder(double price_level, double order_size, bool is_our_order, OrderSide side) {
    if (orderbook.isEmpty()) return;

    // Determine the opposite side to match with
    OrderSide opposite_side = (side == OrderSide::Buy) ? OrderSide::Sell : OrderSide::Buy;

    // Try to find matches until the order is completely filled or there are no more matches
    while (order_size > 0) {
        auto& book = orderbook.getBook();

        // Find the next best price level for the opposite side
        auto it = (opposite_side == OrderSide::Sell) ? 
                  book.upper_bound({price_level, opposite_side}) : 
                  book.lower_bound({price_level, opposite_side});
        
        // If we're buying, we want the lowest sell price (upper bound gives us the first item greater than the key).
        // If we're selling, we want the highest buy price (lower bound gives us the first item not less than the key).

        // No more orders to match with on the opposite side
        if (it == book.end() || it->first.second != opposite_side) {
            break;
        }

        // Match with the current price level
        auto& queue = it->second;
        std::queue<std::pair<double, bool>> resultQueue;

        while (order_size > 0 && !queue.empty()) {
            auto cur_pair = queue.front();
            queue.pop();

            // Skip matching with our own orders
            if (cur_pair.second == is_our_order) {
                resultQueue.push(cur_pair);
                continue;
            }

            // Perform the match
            double match_size = min(cur_pair.first, order_size);
            cur_pair.first -= match_size;
            order_size -= match_size;

            // Put back the remaining part if any
            if (cur_pair.first > 0) {
                resultQueue.push(cur_pair);
            }
        }

        // Update the order book
        if (resultQueue.empty()) {
            // Remove the level if all orders are matched
            book.erase(it);
        } else {
            // Update the level with the remaining orders
            it->second = resultQueue;
        }

        // If order size is still not zero, adjust the price level for next iteration
        if (order_size > 0) {
            if (opposite_side == OrderSide::Sell) {
                // If we're buying and there are no more sell orders at this price level, increase the price level
                price_level = it->first.first + 1;
            } else {
                // If we're selling and there are no more buy orders at this price level, decrease the price level
                if (it->first.first != price_level) {
                    price_level = it->first.first;
                } else {
                    --price_level;
                }
            }
        }
    }
}



    void printQueue(std::queue<std::pair<double, bool>> q) {
        std::cout << "Queue contents:\n";
        while (!q.empty()) {
            std::pair<double, bool> front = q.front();
            q.pop();
            std::cout << "[" << front.first << ", " << (front.second ? "Our Order" : "Other Order") << "]" << std::endl;
        }
    }

private:
    Orderbook &orderbook;
};
