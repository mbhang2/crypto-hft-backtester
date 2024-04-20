#pragma once

#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "security.h"
#include "enum.h"
#include "../rapidjson/document.h"
#include "../rapidjson/filereadstream.h"

enum class MarketType;

using namespace rapidjson;
using namespace std;


/**
 * Abstract class for exchagnes.
 * @todo: Have a method to configure latency per each day
 */
class Exchange {
    public:
        /**
         * Default constructor, used to indicate "ALL EXCHANGES"
         */
        Exchange(): name("") {}

        /**
         * Constructor to create an exchange
         */
        Exchange(const string& name_): name(name_) {}

        /**
         * Destructor
         */
        virtual ~Exchange() {}

        /**
         * Reads json file and stores latencies, trade rules, and fee structures.
         * @param filename Filename of the Json file to read.
        */
        bool loadJson(const string& json_file_path) {
            FILE* json_file = fopen(json_file_path.c_str(), "r");
            if (!json_file) {
                cerr << "Error opening JSON file: " << json_file_path << endl;
                return false;
            }

            char buffer[65536];
            FileReadStream fs(json_file, buffer, sizeof(buffer));
            Document data;
            data.ParseStream(fs);
            fclose(json_file);

            // Case-insensitive search for the exchange name
            for (auto& exchange : data.GetObject()) {
                if (exchange.name.GetString() == name) {
                    // Extract latency values
                    sending_latency = exchange.value["nanosecondLatencyTo"].GetInt();
                    receiving_latency = exchange.value["nanosecondLatencyFrom"].GetInt();

                    /**
                     * Extract Trading Rules
                     */
                    // Spot/Margin
                    const Value& spot_margin_rules = exchange.value["tradeingRules"]["Spot/Margin"];
                    for (auto& security : spot_margin_rules.GetObject()) {
                        string security_str = security.name.GetString();
                        size_t slash_pos = security_str.find('/');
                        Security sec(security_str.substr(0, slash_pos), security_str.substr(slash_pos + 1));
                        listed_securities[MarketType::Spot].push_back(sec);

                        // Save rule values
                        const Value& rule_values = security.value;
                        vector<double> rule_vector;
                        for (auto& v : rule_values.GetArray()) {
                            rule_vector.push_back(v.GetDouble());
                        }
                        spot_trading_rules[sec] = rule_vector;
                    }

                    // Futures
                    const Value& futures_rules_rules = exchange.value["tradeingRules"]["Futures"];
                    for (auto& security : futures_rules_rules.GetObject()) {
                        string security_str = security.name.GetString();
                        size_t slash_pos = security_str.find('/');
                        Security sec(security_str.substr(0, slash_pos), security_str.substr(slash_pos + 1));
                        listed_securities[MarketType::Futures].push_back(sec);

                        // Save rule values
                        const Value& rule_values = security.value;
                        vector<double> rule_vector;
                        for (auto& v : rule_values.GetArray()) {
                            rule_vector.push_back(v.GetDouble());
                        }
                        futures_trading_rules[sec] = rule_vector;
                    }

                    /**
                     * Extract Fee Structures
                     */

                    // Spot/Margin
                    const Value& spotMakerFees = exchange.value["feeStructure"]["Spot/Margin"]["Maker"];
                    const Value& spotTakerFees = exchange.value["feeStructure"]["Spot/Margin"]["Taker"];

                    for (SizeType i = 0; i < spotTakerFees.Size(); ++i) {
                        spot_trading_fees.emplace_back(spotMakerFees[i].GetDouble(), spotTakerFees[i].GetDouble());
                    }

                    spot_maker_fee = spot_trading_fees[0].first; // Set maker fee by default
                    spot_taker_fee = spot_trading_fees[0].second; // Set taker fee by default

                    // Futures
                    const Value& futuresMakerFees = exchange.value["feeStructure"]["Futures"]["Maker"];
                    const Value& futuresTakerFees = exchange.value["feeStructure"]["Futures"]["Taker"];

                    for (SizeType i = 0; i < futuresTakerFees.Size(); ++i) {
                        futures_trading_fees.emplace_back(futuresMakerFees[i].GetDouble(), futuresTakerFees[i].GetDouble());
                    }

                    futures_maker_fee = futures_trading_fees[0].first; // Set maker fee by default
                    futures_taker_fee = futures_trading_fees[0].second; // Set taker fee by default

                    return true; // Found the exchange
                }
            }

            throw invalid_argument("Exchange configuration not found");
            return false;
        }


        /**
         * Getter for the trade rules of the security within the exchange.
         * @param sec Security that we would like the trade rules.
        */
        vector<double> getSpotTradeRules(const Security& sec) const {
            auto it = spot_trading_rules.find(sec);
            if (it != spot_trading_rules.end()) {
                return it->second;
            }

            // Could not find tick size
            throw invalid_argument("Trading Rule not found for given security.");
            return vector<double>();
        }

        /**
         * Getter for the exchange name.
         */
        string getName() const {return name;}

        /**
         * Getter for the receiving latency.
         */
        int getReceivingLatency() const {return receiving_latency;}

        /**
         * Getter for the sending latency.
         */
        int getSendingLatency()const {return sending_latency;}

        /**
         * Getter for spot maker fee.
         */
        double getSpotMakerFee() const {return spot_maker_fee;}

        /**
         * Getter for spot taker fee.
         */
        double getSpotTakerFee() const {return spot_taker_fee;}

        /**
         * Getter for futures maker fee.
         */
        double getFuturesMakerFee() const {return futures_maker_fee;}

        /**
         * Getter for futures taker fee.
         */
        double getFuturesTakerFee() const {return futures_taker_fee;}

        /**
         * Getter for listed securities
         * @param market_type type of the market (Futures, Spot)
         */
        const vector<Security>& getListedSecurities(MarketType& market_type) const {
            auto it = listed_securities.find(market_type);
            
            if (it != listed_securities.end()) {
                return it->second; // Return a const reference to the vector
            } else {
                static vector<Security> emptyVector;
                return emptyVector;
            }
        }

        /**
         * Search for the secuity with provided name
         */
        Security* findSecurity(MarketType& market_type, const std::string& name) const {
            auto it = listed_securities.find(market_type);
            if (it != listed_securities.end()) {
                // Search within the vector for the specified security name
                for (const Security& security : it->second) {
                    if (security.getBase() + "/" + security.getQuote() == name) {
                        return const_cast<Security*>(&security); // Return a pointer to the found security
                    }
                }
            }
            return nullptr; // Security not found
        }

        /**
         * Setter for spot trading fee.
         * @param level customer (VIP) level fee structure to use
         */
        void setSpotTradingFee(int level) {
            if (level >= spot_trading_fees.size()) {
                cerr << "Invalid customer level of " << level << ". Highest customer level is " << spot_trading_fees.size() - 1 << endl;
                return;
            }

            spot_maker_fee = spot_trading_fees[level].first;
            spot_taker_fee = spot_trading_fees[level].second;
        }

        /**
         * Setter for spot maker fee.
         * Manually set the fee outside the provided fee structure.
         * @param fee spot maker fee in percent.
         */
        void setSpotMakerFee(double fee) {spot_maker_fee = fee;}

        /**
         * Setter for spot taker fee.
         * Manually set the fee outside the provided fee structure.
         * @param fee spot taker fee in percent.
         */
        void setSpotTakerFee(double fee) {spot_taker_fee = fee;}

        /**
         * Setter for futures trading fee.
         * @param level customer (VIP) level fee structure to use
         */
        void setFuturesTradingFee(int level) {
            if (level >= futures_trading_fees.size()) {
                cerr << "Invalid customer level of " << level << ". Highest customer level is " << futures_trading_fees.size() - 1 << endl;
                return;
            }

            futures_maker_fee = futures_trading_fees[level].first;
            futures_taker_fee = futures_trading_fees[level].second;
        }

        /**
         * Setter for futures maker fee.
         * Manually set the fee outside the provided fee structure.
         * @param fee futures maker fee in percent.
         */
        void setFuturesMakerFee(double fee) {futures_maker_fee = fee;}

        /**
         * Setter for futures taker fee.
         * Manually set the fee outside the provided fee structure.
         * @param fee futures taker fee in percent.
         */
        void setFuturesTakerFee(double fee) {futures_taker_fee = fee;}


        bool operator==(const Exchange& other) const {
            return name == other.name; //TODO: Check if this is the correct way to compare
        }

    protected:
        const string name;           /*< Exchange name */
        int receiving_latency;      /*< Latency from an exchange in nanoseconds */
        int sending_latency;        /*< Laency to an exchange in nanoseconds */ 
        double spot_maker_fee;      /*< Maker fee in a spot market in percent */
        double spot_taker_fee;      /*< Taker fee in a spot market in percent */
        double futures_maker_fee;      /*< Maker fee in a futures market in percent */
        double futures_taker_fee;      /*< Taker fee in a futures market in percent */
        unordered_map<Security, vector<double>, Security::Hash> spot_trading_rules; /*< Map storing trade rules per security for spot/margin market (Hash was used to ensure the same objects to store the same tick size) */
        unordered_map<Security, vector<double>, Security::Hash> futures_trading_rules; /*< Map storing trade rules per security for futures market (Hash was used to ensure the same objects to store the same tick size) */
        vector<pair<double, double>> spot_trading_fees;     /*< Trading fees in spot market */
        vector<pair<double, double>> futures_trading_fees;     /*< Trading fees in futurs market */
        map<MarketType, vector<Security>> listed_securities;      /*< List of securities listed per market*/
};