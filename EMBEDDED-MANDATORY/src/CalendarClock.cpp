#include "CalendarClock.h"
#include <cstdio>
#include <cstring>

CalendarClock::CalendarClock(const char *date, const char *time)
{
    char month[4] = {};
    int hour = 0, minute = 0, second = 0;
    std::sscanf(date, "%3s %d %d", month, &_day, &_year);
    const char *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
    for (int index = 0; index < 12; ++index)
    {
        if (std::strncmp(month, months + index * 3, 3) == 0)
        {
            _month = index + 1;
            break;
        }
    }
    std::sscanf(time, "%d:%d:%d", &hour, &minute, &second);
    _seconds = hour * 3600 + minute * 60 + second;
}

int CalendarClock::daysInMonth(int year, int month)
{
    const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    const bool leap = year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
    return days[month - 1] + (month == 2 && leap ? 1 : 0);
}

void CalendarClock::format(char *output, size_t capacity, uint64_t elapsedMs) const
{
    const uint64_t seconds = _seconds + elapsedMs / 1000;
    uint64_t remainingDays = seconds / 86400;
    int year = _year, month = _month, day = _day;
    while (remainingDays > 0)
    {
        const int daysLeft = daysInMonth(year, month) - day;
        if (remainingDays <= static_cast<uint64_t>(daysLeft))
        {
            day += static_cast<int>(remainingDays);
            break;
        }
        remainingDays -= daysLeft + 1;
        day = 1;
        if (++month > 12)
        {
            month = 1;
            ++year;
        }
    }
    const unsigned hour = (seconds % 86400) / 3600;
    const unsigned minute = (seconds % 3600) / 60;
    std::snprintf(output, capacity, "%04d-%02d-%02d %02u:%02u", year, month, day, hour, minute);
}
