
#ifndef DATETIME_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct
{
    uint16_t year;    // [1-65535] 0 = undefined, 1 = 1 AD
    uint16_t month;   // [1-12]
    uint16_t day;     // [1-31]
    uint16_t hour;    // [0-23]
    uint16_t min;     // [0-59]
    uint16_t sec;     // [0-59]
    uint16_t milli;   // [0-999]

} DateTime;

typedef int64_t DenseTime; // milli seconds since 0001-01-01 00:00:00.000

typedef int DayOfWeek;
enum 
{
    DT_DOW_SUN = 0,
    DT_DOW_MON,
    DT_DOW_TUE,
    DT_DOW_WED,
    DT_DOW_THU,
    DT_DOW_FRI,
    DT_DOW_SAT,
};

DenseTime date_time_to_dense      (DateTime dt);
DateTime  date_time_add_millis    (DateTime dt, int millis);
DateTime  date_time_add_secs      (DateTime dt, int secs);
DateTime  date_time_add_mins      (DateTime dt, int mins);
DateTime  date_time_add_days      (DateTime dt, int days);
DayOfWeek date_time_day_of_week   (DateTime dt);

int64_t   date_time_diff_ms       (DateTime a, DateTime b);
DateDiff  date_time_diff          (DateTime a, DateTime b);

DateTime  dense_time_to_date      (DenseTime dense);

#endif // DATETIME_H


#ifdef DATETIME_IMPLEMENTATION

#define DT_DAY_TO_MS 86400000LL
#define DT_HOUR_TO_MS 3600000LL
#define DT_MIN_TO_MS 60000LL
#define DT_SEC_TO_MS 1000LL
#define DT_DAYS_PER_400_YEARS 146097LL

static const int64_t dt_days_in_year[2] = { 365, 366 };        // { non-leap, leap }
                                                              
static const int16_t dt_days_before_month[2][12] = {
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 }, // non-leap
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 }  // leap
};

static bool dt_is_leap_year(int year) 
{
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

static int dt_days_in_month_year(int month, int year)
{
    static const int days[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (month == 2 && dt_is_leap_year(year))
        return 29;
    return days[month - 1];
}

static int dt_days_before_year(int year)
{
    int result = 0;
    year = year - 1;
    result += 365 * year;
    result += year / 4;
    result -= year / 100;
    result += year / 400;
    return result;
}

DenseTime date_time_to_dense(DateTime dt)
{
    DenseTime result = 0;
    int64_t days = dt_days_before_year(dt.year);
    days += dt_days_before_month[dt_is_leap_year(dt.year)][dt.month - 1];
    days += dt.day - 1;
    result += days * DT_DAY_TO_MS;
    result += (int64_t)dt.hour * DT_HOUR_TO_MS;
    result += (int64_t)dt.min * DT_MIN_TO_MS;
    result += (int64_t)dt.sec * DT_SEC_TO_MS;
    result += dt.milli;
    return result;
}


DateTime dense_time_to_date(DenseTime dense)
{
    DateTime result;
    int64_t days = dense / DT_DAY_TO_MS; // days since 0001-01-01 00:00:00.000
    int64_t ms_into_day = dense % DT_DAY_TO_MS;

    uint16_t num_cycles = days / DT_DAYS_PER_400_YEARS;
    uint16_t days_into_cycle = days % DT_DAYS_PER_400_YEARS;

    uint16_t year = (uint16_t)(num_cycles * 400 + 1);

    for (;;)
    {
        int64_t days_in_year = dt_days_in_year[dt_is_leap_year(year)];
        if (days_into_cycle < days_in_year)
            break;
        days_into_cycle -= days_in_year;
        year++;
    }

    uint16_t month = 1;
    bool is_leap_year = dt_is_leap_year(year);
    uint16_t days_into_year = (uint16_t)days_into_cycle;
    for (int i = 1; i < 12; i++)
    {
        if (days_into_year < dt_days_before_month[is_leap_year][i])
            break;
        month++;
    }

    uint16_t day = days_into_year - dt_days_before_month[is_leap_year][month-1] + 1;

    uint16_t hour = (uint16_t)(ms_into_day / DT_HOUR_TO_MS);
    int64_t ms_into_hour = ms_into_day % DT_HOUR_TO_MS;

    uint16_t min = (uint16_t)(ms_into_hour / DT_MIN_TO_MS);
    int64_t ms_into_min = ms_into_hour % DT_MIN_TO_MS;

    uint16_t sec = (uint16_t)(ms_into_min / DT_SEC_TO_MS);
    int64_t ms = (uint16_t)(ms_into_min % DT_SEC_TO_MS);

    result.year = year;
    result.month = month;
    result.day = day;
    result.hour = hour;
    result.min = min;
    result.sec = sec;
    result.milli = ms;
    return result;
}

DateTime date_time_add_millis(DateTime dt, int millis)
{
    DenseTime dense = date_time_to_dense(dt);
    return dense_time_to_date(dense);
}

DateTime date_time_add_secs(DateTime dt, int secs)
{
    return date_time_add_millis(dt, secs * DT_SEC_TO_MS);
}

DateTime date_time_add_mins(DateTime dt, int mins)
{
    return date_time_add_millis(dt, mins * DT_MIN_TO_MS);
}

DateTime date_time_add_hours(DateTime dt, int hours)
{
    return date_time_add_millis(dt, hours * DT_HOUR_TO_MS);
}

DateTime date_time_add_days(DateTime dt, int days)
{
    return date_time_add_millis(dt, days * DT_DAY_TO_MS);
}


DayOfWeek date_time_day_of_week(DateTime dt)
{
    DenseTime dense = date_time_to_dense(dt);
    int64_t days = dense / DT_DAY_TO_MS;
    return ((days + 1) % 7);
}

#endif // DATETIME_IMPLEMENTATION
