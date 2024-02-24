#pragma once

#include <string>

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

    protected:
        std::string name;           /*< Exchange name */
        int receiving_latency;      /*< Latency from an exchange in nanoseconds */
        int sending_latency;        /*< Laency to an exchange in nanoseconds */
};