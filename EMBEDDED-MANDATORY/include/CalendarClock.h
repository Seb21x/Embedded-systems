#pragma once
#include <cstddef>
#include <cstdint>

// Software calendar: starts from the supplied date/time after each restart.
class CalendarClock
{
public:
    CalendarClock(const char *date, const char *time);
    void format(char *output, size_t capacity, uint64_t elapsedMs) const;

private:
    static int daysInMonth(int year, int month);
    int _year = 2000;
    int _month = 1;
    int _day = 1;
    int _seconds = 0;
};
