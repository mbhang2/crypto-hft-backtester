#pragma once

#include <iostream>
#include <string>

using namespace std;


/**
 * Abstract class for securities.
 */
class Security {
    public:
        /**
         * Constructor to create security.
         */
        Security(const string& base_, const string& quote_):
                base(base_), quote(quote_) {}

        /**
         * Destructor.
         */
        virtual ~Security() {}

        /**
         * Override print function
         */
        void print() const {
            std::cout << base << "/" << quote << std::endl;
        }   

        /**
         * Equal operator. 
         * Compares if the base and quote are the same.
         */
        friend bool operator==(const Security& lhs, const Security& rhs) {
            return (lhs.base == rhs.base) && (lhs.quote == rhs.quote);
        }

        /**
         * Not equal operator.
         */
        friend bool operator!=(const Security& lhs, const Security& rhs) {
            return !(lhs == rhs);
        }

        /**
         * Hash used for unordered map in exchange class.
         */
        struct Hash {
            size_t operator()(const Security& sec) const {
                // Combine the hash values of base and quote
                size_t baseHash = std::hash<std::string>{}(sec.base);
                size_t quoteHash = std::hash<std::string>{}(sec.quote);
                return baseHash ^ (quoteHash << 1); // XOR and shift
            }
        };

    protected:
        const string base;  /*< Name of the base security (one before the slash) */
        const string quote; /*< Name of the quote security (one after the slash) */
};