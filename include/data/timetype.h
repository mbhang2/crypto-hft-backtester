#pragma once

#include <iomanip>
#include <sstream>

/**
 * Struct for recording timestamps
 */
struct TimeType {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int subsecond;  /*< in nanoseconds */

    /**
     * Constructor
     */
    TimeType(const std::string& timestampStr) {
        std::istringstream iss(timestampStr);
        char delimiter;
        iss >> year >> delimiter >> month >> delimiter >> day >> hour >> delimiter >>
            minute >> delimiter >> second >> delimiter >> subsecond;
    }

    /**
     * Return string format
     */
    std::string toString() const {
        std::ostringstream oss;
        oss << std::setfill('0');
        oss << std::setw(4) << year << '-';
        oss << std::setw(2) << month << '-';
        oss << std::setw(2) << day << ' ';
        oss << std::setw(2) << hour << ':';
        oss << std::setw(2) << minute << ':';
        oss << std::setw(2) << second << '.';
        oss << std::setw(9) << std::setprecision(9) << subsecond; // Display nanoseconds
        return oss.str();
    }
};