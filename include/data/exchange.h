#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include "security.h"
#include "../rapidjson/document.h"

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
        Exchange(const std::string& name_, int receiving_latency_, int sending_latency_): 
                name(name_), receiving_latency(receiving_latency_), sending_latency(sending_latency_) {}

        /**
         * Destructor
         */
        virtual ~Exchange() {}

        /**
         * Reads tick size from json file and stores it in an unordered_map.
         * @param filename Filename of the Json file to read.
        */
        void loadTickSizesFromJson(const std::string& filename) {
            std::ifstream file(filename);
            if (!file.is_open()) {
                std::cerr << "Error opening file: " << filename << std::endl;
                return;
            }

            std::string json((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            rapidjson::Document doc;
            doc.Parse(json.c_str());

            for (const auto& member : doc.GetObject()) {
                if (member.value.IsNumber()) {
                    std::string securityName = member.name.GetString();
                    size_t slashPos = securityName.find('/');
                    if (slashPos != std::string::npos) {
                        std::string base = securityName.substr(0, slashPos);
                        std::string quote = securityName.substr(slashPos + 1);
                        Security sec(base, quote);
                        tick_size[sec] = member.value.GetDouble();
                    }
                }
            }
        }

        /**
         * Getter for the tick size of the security within the exchange.
         * @param sec Security that we would like the tick size.
        */
        double getTickSize(const Security& sec) const {
            auto it = tick_size.find(sec);
            if (it != tick_size.end()) {
                return it->second;
            }
            return -1.0; // Could not find tick size
        }

        /**
         * Getter for the exchange name.
         */
        string getName() {return name;}

    protected:
        std::string name;           /*< Exchange name */
        int receiving_latency;      /*< Latency from an exchange in nanoseconds */
        int sending_latency;        /*< Laency to an exchange in nanoseconds */
        std::unordered_map<Security, double, Security::Hash> tick_size;      /*< Map storing tick size per security (Hash was used to ensure the same objects to store the same tick size) */
};