#pragma once

#include <stdexcept>
#include <vector>
#include "../backtesting/order.h"

using namespace std;

/**
 * Class for order log
 */
class OrderLog {
public:
    /**
     * Default constructor
     */
    OrderLog() {}

    /**
     * Destructor
     */
    ~OrderLog() {}

    /**
     * Add order to the order list
     * @param order order to add
     */
    void addOrder(std::shared_ptr<Order> order) {
        orders.push_back(order);
    }

    /**
     * Retrieve an order by ID
     * @param order_id unique order id of an order to retrieve
     * @return reference to the order
     */
    // const Order& getOrderByID(int order_id) const {
    //     for (Order* order : orders) {
    //         if (order->getID() == order_id) {
    //             return *order;
    //         }
    //     }
        
    //     // Throw an exception when order not found
    //     throw runtime_error("Order with ID " + to_string(order_id) + " not found.");
    // }

private:
    std::vector<std::shared_ptr<Order>> orders;
};