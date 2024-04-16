#include "OrderBook.h"

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
        auto& book = orderbook.getBook();
        auto queue = book[price_level];
        std::queue<std::pair<double, bool>> resultQueue;

        if (!is_our_order) {
            while (order_size > 0 && !queue.empty()) {
                auto cur_pair = queue.front();
                if (cur_pair.first > order_size) {
                    cur_pair.first -= order_size;
                    resultQueue.push(cur_pair);
                    order_size = 0;
                } else {
                    order_size -= cur_pair.first;
                    cur_pair.first = 0;
                }
                queue.pop();
            }
            while (!queue.empty()) {
                resultQueue.push(queue.front());
                queue.pop();
            }
            book[price_level] = resultQueue;
        } else {
            while (order_size > 0 && !queue.empty()) {
                auto cur_pair = queue.front();
                if (cur_pair.second) {
                    resultQueue.push(cur_pair);
                } else {
                    if (cur_pair.first > order_size) {
                        cur_pair.first -= order_size;
                        resultQueue.push(cur_pair);
                        order_size = 0;
                    } else {
                        order_size -= cur_pair.first;
                        cur_pair.first = 0;
                    }
                }
                queue.pop();
            }
            while (!queue.empty()) {
                resultQueue.push(queue.front());
                queue.pop();
            }
            book[price_level] = resultQueue;
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