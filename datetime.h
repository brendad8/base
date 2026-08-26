
/* datetime.h - date and times types

   To use this library, do this in *one* C file:
      #define DATETIME_IMPLEMENTATION
      #include "base/datetime.h"
*/

#ifndef DATETIME_H
#define DATETIME_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 *          INCLUDES
 ***************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

/***************************************************************************
 *          DEFINES
 ***************************************************************************/

#define DATETIME_FMT "%d-%02d-%02d %02d:%02d:%02d.%.6lf"
#define DATETIME_VARG(dt) dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec, ((double)(1000*dt.msec + dt.usec))/1000000.0

#define DATETIME_DEBUG_FMT "DateTime{\n  year = %d,\n  month = %02d,\n  day = %02d,\n  hour = %02d,\n  min = %02d,\n  sec = %02d,\n  msec = %d,\n  usec = %d\n}"
#define DATETIME_DEBUG_VARG(dt) dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec, dt.msec, dt.usec

/***************************************************************************
 *          TYPES
 ***************************************************************************/

typedef struct
{
    uint16_t year;    // [1-65535] 0 = undefined, 1 = 1 AD
    uint16_t month;   // [1-12]
    uint16_t day;     // [1-31]
    uint16_t hour;    // [0-23]
    uint16_t min;     // [0-59]
    uint16_t sec;     // [0-59]
    uint16_t msec;    // [0-999]
    uint16_t usec;    // [0-999]

} DateTime;

typedef int64_t DenseTime; // micro seconds since 0001-01-01 00:00:00.000

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

/***************************************************************************
 *          PROTOTYPES
 ***************************************************************************/

DenseTime   date_time_to_dense         (DateTime);
DateTime    date_time_from_dense       (DenseTime);
DateTime    date_time_now_utc          (void);
DateTime    date_time_now_local        (void);
DayOfWeek   date_time_day_of_week      (DateTime);
DateTime    date_time_add_usec         (DateTime, int);
DateTime    date_time_add_msec         (DateTime, int);
DateTime    date_time_add_secs         (DateTime, int);
DateTime    date_time_add_mins         (DateTime, int);
DateTime    date_time_add_days         (DateTime, int);
int         date_time_compare          (DateTime, DateTime);
bool        date_time_equal            (DateTime, DateTime);
bool        date_time_equal_date       (DateTime, DateTime);
int64_t     date_time_diff_ms          (DateTime, DateTime);

bool        date_time_local_to_utc     (DateTime, DateTime*);
bool        date_time_utc_to_local     (DateTime, DateTime*);

DateTime    date_time_local_from_unix  (int64_t);
int64_t     date_time_local_to_unix    (DateTime);
DateTime    date_time_utc_from_unix    (int64_t);
int64_t     date_time_utc_to_unix      (DateTime);

#ifdef __cplusplus
}
#endif

/***************************************************************************
 *          IMPLEMENTATION
 ***************************************************************************/

#ifdef DATETIME_IMPLEMENTATION

#ifdef _WIN32
    #include <windows.h>
#else
    #include <time.h>
    #include <sys/time.h>
#endif

#define DT_DAY_TO_US 86400000000LL
#define DT_HOUR_TO_US 3600000000LL
#define DT_MIN_TO_US 60000000LL
#define DT_SEC_TO_US 1000000LL
#define DT_MS_TO_US 1000LL

#define DT_DAYS_PER_400_YEARS 146097LL

static const DateTime dt_unix_epoch = {
    .year = 1970,
    .month = 1,
    .day = 1
};

static const DenseTime dense_unix_epoch = 62135596800000000ULL;

static const DateTime dt_win_epoch = {
    .year = 1601,
    .month = 1,
    .day = 1
};

static const DenseTime dense_win_epoch = 50491123200000000ULL;

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
    result += days * DT_DAY_TO_US;
    result += (int64_t)dt.hour * DT_HOUR_TO_US;
    result += (int64_t)dt.min * DT_MIN_TO_US;
    result += (int64_t)dt.sec * DT_SEC_TO_US;
    result += dt.msec * DT_MS_TO_US;
    result += dt.usec;
    return result;
}


DateTime date_time_from_dense(DenseTime dense)
{
    DateTime result;
    int64_t days = dense / DT_DAY_TO_US; // days since 0001-01-01 00:00:00.000
    int64_t usec_into_day = dense % DT_DAY_TO_US;

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

    uint16_t hour = (uint16_t)(usec_into_day / DT_HOUR_TO_US);
    int64_t usec_into_hour = usec_into_day % DT_HOUR_TO_US;

    uint16_t min = (uint16_t)(usec_into_hour / DT_MIN_TO_US);
    int64_t usec_into_min = usec_into_hour % DT_MIN_TO_US;

    uint16_t sec = (uint16_t)(usec_into_min / DT_SEC_TO_US);
    int64_t usec_into_sec = (uint16_t)(usec_into_min % DT_SEC_TO_US);
    
    uint16_t msec = (uint16_t)(usec_into_sec / DT_MS_TO_US);
    int64_t usec = (uint16_t)(usec_into_sec % DT_MS_TO_US);

    result.year  = (uint16_t)year;
    result.month = (uint16_t)month;
    result.day   = (uint16_t)day;
    result.hour  = (uint16_t)hour;
    result.min   = (uint16_t)min;
    result.sec   = (uint16_t)sec;
    result.msec  = (uint16_t)msec;
    result.usec  = (uint16_t)usec;
    return result;
}

DateTime date_time_add_millis(DateTime dt, int millis)
{
    DenseTime dense = date_time_to_dense(dt);
    return date_time_from_dense(dense);
}

DateTime date_time_add_secs(DateTime dt, int secs)
{
    return date_time_add_millis(dt, secs * DT_SEC_TO_US);
}

DateTime date_time_add_mins(DateTime dt, int mins)
{
    return date_time_add_millis(dt, mins * DT_MIN_TO_US);
}

DateTime date_time_add_hours(DateTime dt, int hours)
{
    return date_time_add_millis(dt, hours * DT_HOUR_TO_US);
}

DateTime date_time_add_days(DateTime dt, int days)
{
    return date_time_add_millis(dt, days * DT_DAY_TO_US);
}

DayOfWeek date_time_day_of_week(DateTime dt)
{
    DenseTime dense = date_time_to_dense(dt);
    int64_t days = dense / DT_DAY_TO_US;
    return ((days + 1) % 7);
}

int64_t date_time_diff_usec(DateTime a, DateTime b)
{
    return (int64_t)(date_time_to_dense(a) - date_time_to_dense(b));
}

int date_time_compare(DateTime a, DateTime b)
{
    DenseTime da = date_time_to_dense(a);
    DenseTime db = date_time_to_dense(b);
    return (da < db) - (da > db);
}

bool date_time_equal(DateTime a, DateTime b)
{
    return date_time_compare(a, b) == 0;
}

bool date_time_equal_date(DateTime a, DateTime b)
{
    return a.year == b.year && a.month == b.month && a.day == b.day;
}

DateTime date_time_now_utc(void)
{
    #ifdef _WIN32
        FILETIME ft;
        GetSystemTimeAsFileTime(&ft);
        DenseTime dense = dense_win_epoch + ((((int64_t)ft.dwHighDateTime << 32) | (int64_t)ft.dwLowDateTime) / 10);
        return date_time_from_dense(dense);

    #else
        struct timeval tv;
        gettimeofday(&tv, 0);
        DenseTime now = dense_unix_epoch + (tv.tv_sec * DT_SEC_TO_US) + tv.tv_usec;
        return date_time_from_dense(now);
    #endif
}

DateTime date_time_now_local(void)
{
    #ifdef _WIN32
        FILETIME ft_utc, ft_local;
        GetSystemTimeAsFileTime(&ft_utc);
        FileTimeToLocalFileTime(&ft_utc, &ft_local);
        DenseTime dense = dense_win_epoch + ((((int64_t)ft_local.dwHighDateTime << 32) | (int64_t)ft_local.dwLowDateTime) / 10);
        return date_time_from_dense(dense);
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
            .msec  = tv.tv_usec / 1000,
            .usec  = tv.tv_usec % 1000
        };
    #endif
}

bool date_time_local_to_utc(DateTime local, DateTime* utc)
{
#ifdef _WIN32
    SYSTEMTIME st_utc;
    DYNAMIC_TIME_ZONE_INFORMATION tz;
    SYSTEMTIME st_local = {
        .wYear         = local.year,
        .wMonth        = local.month,
        .wDay          = local.day,
        .wHour         = local.hour,
        .wMinute       = local.min,
        .wSecond       = local.sec,
        .wMilliseconds = local.msec
    };

    DWORD id = GetDynamicTimeZoneInformation(&tz);

    if (id == TIME_ZONE_ID_INVALID)
        return false;

    BOOL res = TzSpecificLocalTimeToSystemTimeEx(&tz, &st_local, &st_utc);
    if (res)
    {
        utc->year  = st_utc.wYear;
        utc->month = st_utc.wMonth;
        utc->day   = st_utc.wDay;
        utc->hour  = st_utc.wHour;
        utc->min   = st_utc.wMinute;
        utc->sec   = st_utc.wSecond;
        utc->msec  = st_utc.wMilliseconds;
        utc->usec  = local.usec;
    }
    return res;
#else
    struct tm tm_local = {
        .tm_sec   = local.sec,
        .tm_min   = local.min,
        .tm_hour  = local.hour,
        .tm_mday  = local.day,
        .tm_mon   = local.month - 1,
        .tm_year  = local.year - 1900,
        .tm_isdst = -1
    };

    time_t t = mktime(&tm_local);

    if (t == (time_t)-1)
        return false;

    struct tm tm_utc;

    if (gmtime_r(&t, &tm_utc) == NULL)
        return false;

    utc->year  = tm_utc.tm_year + 1900;
    utc->month = tm_utc.tm_mon + 1;
    utc->day   = tm_utc.tm_mday;
    utc->hour  = tm_utc.tm_hour;
    utc->min   = tm_utc.tm_min;
    utc->sec   = tm_utc.tm_sec;
    utc->msec  = local.msec;
    utc->usec  = local.usec;

    return true;
#endif
}

bool date_time_utc_to_local(DateTime utc, DateTime* local)
{
#ifdef _WIN32
    SYSTEMTIME st_local;
    DYNAMIC_TIME_ZONE_INFORMATION tz;
    SYSTEMTIME st_utc = {
        .wYear         = utc.year,
        .wMonth        = utc.month,
        .wDay          = utc.day,
        .wHour         = utc.hour,
        .wMinute       = utc.min,
        .wSecond       = utc.sec,
        .wMilliseconds = utc.msec
    };

    DWORD id = GetDynamicTimeZoneInformation(&tz);

    if (id == TIME_ZONE_ID_INVALID)
        return false;

    BOOL res = SystemTimeToTzSpecificLocalTimeEx(&tz, &st_utc, &st_local);
    if (res)
    {
        local->year  = st_local.wYear;
        local->month = st_local.wMonth;
        local->day   = st_local.wDay;
        local->hour  = st_local.wHour;
        local->min   = st_local.wMinute;
        local->sec   = st_local.wSecond;
        local->msec  = st_local.wMilliseconds;
        local->usec  = utc.usec;
    }
    return res;
#else
    struct tm tm_utc = {
        .tm_sec   = utc.sec,
        .tm_min   = utc.min,
        .tm_hour  = utc.hour,
        .tm_mday  = utc.day,
        .tm_mon   = utc.month - 1,
        .tm_year  = utc.year - 1900,
        .tm_isdst = 0
    };

    time_t t = timegm(&tm_utc);

    if (t == (time_t)-1)
        return false;

    struct tm tm_local;

    if (localtime_r(&t, &tm_local) == NULL)
        return false;

    local->year  = tm_local.tm_year + 1900;
    local->month = tm_local.tm_mon + 1;
    local->day   = tm_local.tm_mday;
    local->hour  = tm_local.tm_hour;
    local->min   = tm_local.tm_min;
    local->sec   = tm_local.tm_sec;
    local->msec  = utc.msec;
    local->usec  = utc.usec;

    return true;
#endif
}

#endif // DATETIME_IMPLEMENTATION

#endif // DATETIME_H

