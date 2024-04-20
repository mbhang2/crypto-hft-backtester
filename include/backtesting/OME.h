#include "OrderBook.h"
#include <iostream>
#include <queue>
#include <map>

using namespace std;

/**
 * Order Matching Engine class which can prevent self-match order
 */
class OME {
public:
    /**
     * Constructor for the Order Matching Engine class.
     */
    OME(Orderbook &orderbook_)
        : orderbook(orderbook_) {}
    
    /**
     * Destructor
     */
    virtual ~OME() {}

    /**
     * Matches orders while preventing self-matching.
     * @param price_level The price level at which the order is to be matched.
     * @param order_size The size of the order.
     * @param is_our_order Indicates whether the order is from us, to prevent self-matching.
     */
    void matchOrder(double price_level, double order_size, bool is_our_order) {
        if (orderbook.isEmpty()) return;
        
        while (order_size > 0) {
            auto& book = orderbook.getBook();
            // Check if the current price level exists in the book
            if (book.find(price_level) == book.end() || book[price_level].empty()) {
                price_level++; // move to the next price level if current is empty or doesn't exist
                continue;
            }

            auto& queue = book[price_level];
            std::queue<std::pair<double, bool>> resultQueue;

            while (order_size > 0 && !queue.empty()) {
                auto cur_pair = queue.front();
                queue.pop();
                if (cur_pair.second && is_our_order) {
                    // Skip matching our own orders
                    resultQueue.push(cur_pair);
                    continue;
                }
                
                double match_size = min(cur_pair.first, order_size);
                cur_pair.first -= match_size;
                order_size -= match_size;

                if (cur_pair.first > 0) {
                    resultQueue.push(cur_pair); // Put back the remaining part if any
                }
            }

            while (!queue.empty()) {
                resultQueue.push(queue.front());
                queue.pop();
            }

            if (resultQueue.empty()) {
                book.erase(price_level); // Remove empty price level
            } else {
                book[price_level] = resultQueue;
            }
            if (order_size > 0) {
                price_level++; // Move to next price level if there's still order size left
            }
        }
    }

    void printQueue(std::queue<std::pair<double, bool>> q) {
        std::cout << "Queue contents:\n";
        while (!q.empty()) {
            std::pair<double, bool> front = q.front();
            q.pop();
            std::cout << "[" << front.first << ", " << (front.second ? "true" : "false") << "]" << std::endl;
        }
    }

private:
    Orderbook &orderbook;
};
