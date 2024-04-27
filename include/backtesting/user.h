#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../data/exchange.h"
#include "../record/orderlog.h"
#include "../rapidjson/document.h"
#include "../rapidjson/filereadstream.h"

using namespace std;

/**
 * @todo (Reach goal) Add restristictions per subaccount
 * @todo (Reach goal) Manage risks/positions per subaccounts/users for a firm (tree structure)
 */
class User {
public:
    /**
     * Constructor for user
     */
    User(double spot_balance_, double futures_balance_, const string& json_file_path): spot_balance(spot_balance_), futures_balance(futures_balance_) {
        id = ++next_id; // Assign a unique ID

        FILE* json_file = fopen(json_file_path.c_str(), "r");
        if (!json_file) {
            cerr << "Error opening JSON file: " << json_file_path << endl;
            return;
        }

        char buffer[65536];
        FileReadStream fs(json_file, buffer, sizeof(buffer));
        Document data;
        data.ParseStream(fs);
        fclose(json_file);

        for (auto& exchange : data.GetObject()) {
            Exchange* exchange_ = new Exchange(exchange.name.GetString());
            exchange_->loadJson(json_file_path);
            exchange_list.push_back(std::shared_ptr<Exchange>(exchange_));
        }
    }

    /**
     * Destructor for user
     */
    ~User() {}

    /**
     * Getter for user id
     */
    int getId() const { return id; }

    /**
     * Getter for user's spot balance
     */
    double getSpotBalance() const { return spot_balance; }

    /**
     * Getter for user's futures balance
     */
    double getFuturesBalance() const { return futures_balance; }

    /**
     * Getter for user's list of exchanges
     */
    vector<std::shared_ptr<Exchange>> getExchanges() const { return exchange_list; }

    /**
     * Getter for user's order log
     */
    OrderLog getOrderLog() const { return orderlog; }

    /**
     * Add exchange to the list
     * @param exchange exchange to add
     */
    void addExchange(Exchange& exchange) {
        exchange_list.push_back(std::shared_ptr<Exchange>(&exchange));
    }

    /**
     * Find exchange by name, case insensitive
     * @param name name of the exchange to find
     * @return pointer to the exchange
     */
    std::shared_ptr<Exchange> findExchange(const string& name) {
    // Convert the target name to lowercase for case-insensitive comparison
    string lowercase_name = name;
    transform(lowercase_name.begin(), lowercase_name.end(), lowercase_name.begin(), ::tolower);

        for (auto&& exchange : exchange_list) {
            string exchangeName = exchange->getName();
            transform(exchangeName.begin(), exchangeName.end(), exchangeName.begin(), ::tolower);
            if (exchangeName == lowercase_name) {
                return exchange;
            }
        }

        return nullptr; // Exchange not found
    }

    private:
    int id; /*< Unique user id */
    double spot_balance; /*< Spot balance */
    double futures_balance; /*< Futures balance */
    static int next_id; /*< Static member to track the next available ID */ 
    vector<std::shared_ptr<Exchange>> exchange_list; /*< List of exchanges for the user */
    OrderLog orderlog;  /*< Order log */
};

int User::next_id = 0;