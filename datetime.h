
#ifndef DATETIME_H
#define DATETIME_H

#ifdef __cplusplus
extern "C" {
#endif

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
    DOW_SUN = 0,
    DOW_MON,
    DOW_TUE,
    DOW_WED,
    DOW_THU,
    DOW_FRI,
    DOW_SAT,
};

DenseTime   date_time_to_dense      (DateTime dt);
DateTime    dense_time_to_date      (DenseTime dense);

DateTime    date_time_now_utc       (void);
DateTime    date_time_now_local     (void);

DateTime    date_time_add_millis    (DateTime dt, int millis);
DateTime    date_time_add_secs      (DateTime dt, int secs);
DateTime    date_time_add_mins      (DateTime dt, int mins);
DateTime    date_time_add_days      (DateTime dt, int days);

DayOfWeek   date_time_day_of_week   (DateTime dt);

bool        date_time_equal         (DateTime a, DateTime b);
int64_t     date_time_diff_ms       (DateTime a, DateTime b);

#define DATETIME_FMT "%d-%02d-%02d %02d:%02d:%02d.%d"
#define DATETIME_VARG(dt) dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec, dt.milli

#ifdef __cplusplus
}
#endif

#endif // DATETIME_H


#ifdef DATETIME_IMPLEMENTATION

#ifdef _WIN32
    #include <windows.h>
#else
    #include <time.h>
    #include <sys/time.h>
#endif

#define DT_DAY_TO_MS 86400000LL
#define DT_HOUR_TO_MS 3600000LL
#define DT_MIN_TO_MS 60000LL
#define DT_SEC_TO_MS 1000LL
#define DT_DAYS_PER_400_YEARS 146097LL

static const DateTime dt_unix_epoch = {
    .year = 1970,
    .month = 1,
    .day = 1
};
static const DenseTime dense_unix_epoch = 62135596800000;

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

int64_t date_time_diff_ms(DateTime a, DateTime b)
{
    return (int64_t)(date_time_to_dense(a) - date_time_to_dense(a));
}

int date_time_compare(DateTime a, DateTime b)
{
    return (int)(date_time_to_dense(a) - date_time_to_dense(a));
}

bool date_time_equal(DateTime a, DateTime b)
{
    return date_time_compare(a, b) == 0;
}

DateTime date_time_now_utc(void)
{
    #ifdef _WIN32
        SYSTEMTIME st;
        GetSystemTime(&st);

        return (DateTime) {
            .year  = st.wYear,
            .month = st.wMonth,
            .day   = st.wDay,
            .hour  = st.wHour,
            .min   = st.wMinute,
            .sec   = st.wSecond,
            .milli = st.wMilliseconds
        };
    #else
        struct timeval tv;
        gettimeofday(&tv, 0);
        DenseTime now = dense_unix_epoch + (tv.tv_sec * DT_SEC_TO_MS) + (tv.tv_usec / 1000);
        return datetime_from_dense_time(now);
    #endif
}

DateTime date_time_now_local(void)
{
    #ifdef _WIN32
        SYSTEMTIME st;
        GetLocalTime(&st);
        return (DateTime) {
            .year  = st.wYear,
            .month = st.wMonth,
            .day   = st.wDay,
            .hour  = st.wHour,
            .min   = st.wMinute,
            .sec   = st.wSecond,
            .milli = st.wMilliseconds
        };
    #else
        struct timeval tv;
        gettimeofday(&tv, 0);
        time_t t = tv.tv_sec;
        struct tm tm_local;
        localtime_r(&t, &tm_local);
        return (DateTime) {
            .year  = tm_local.tm_year + 1900,
            .month = tm_local.tm_mon + 1,
            .day   = tm_local.tm_mday,
            .hour  = tm_local.tm_hour,
            .min   = tm_local.tm_min,
            .sec   = tm_local.tm_sec,
            .milli = tv.tv_usec / 1000
        };
    #endif
}

#endif // DATETIME_IMPLEMENTATION
