#pragma once

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

    // Constructor
    TimeType(int y, int m, int d, int h, int min, int s, int subsec)
        : year(y), month(m), day(d), hour(h), minute(min), second(s), subsecond(subsec) {}
};