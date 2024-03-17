#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "security.h"
#include "../rapidjson/document.h"
#include "../rapidjson/filereadstream.h"

using namespace rapidjson;
using namespace std;

/**
 * Abstract class for exchagnes.
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

                    // Extract trading rules for Spot/Margin
                    const Value& spot_margin_rules = exchange.value["tradeingRules"]["Spot/Margin"];
                    for (auto& security : spot_margin_rules.GetObject()) {
                        string security_str = security.name.GetString();
                        size_t slash_pos = security_str.find('/');
                        Security sec(security_str.substr(0, slash_pos), security_str.substr(slash_pos + 1));

                        // Save rule values
                        const Value& rule_values = security.value;
                        vector<double> rule_vector;
                        for (auto& v : rule_values.GetArray()) {
                            rule_vector.push_back(v.GetDouble());
                        }
                        spot_trading_rules[sec] = rule_vector;
                    }
                    return true; // Found the exchange
                }
            }

            cerr << "Exchange not found in JSON data." << endl;
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
            cerr << "Trading Rule not found for given security." << endl;
            return vector<double>();
        }

        /**
         * Getter for the exchange name.
         */
        string getName() {return name;}

        /**
         * Getter for the receiving latency.
         */
        int getReceivingLatency() {return receiving_latency;}

        /**
         * Getter for the sending latency.
         */
        int getSendingLatency() {return sending_latency;}

    protected:
        string name;           /*< Exchange name */
        int receiving_latency;      /*< Latency from an exchange in nanoseconds */
        int sending_latency;        /*< Laency to an exchange in nanoseconds */ 
        unordered_map<Security, vector<double>, Security::Hash> spot_trading_rules; /*< Map storing trade rules per security for spot/margin market (Hash was used to ensure the same objects to store the same tick size) */
};