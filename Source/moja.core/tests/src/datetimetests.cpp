#include <moja/datetime.h>

#include <boost/test/unit_test.hpp>

#include <ctime>
#include <chrono>
#include <time.h>


using moja::DateTime;

BOOST_AUTO_TEST_SUITE(DateTimeTests);

BOOST_AUTO_TEST_CASE(core_datetime_Default_ctor) {
   DateTime dt;

   BOOST_CHECK_EQUAL(dt.year(), 0);
   BOOST_CHECK_EQUAL(dt.month(), 0);
   BOOST_CHECK_EQUAL(dt.day(), 0);
   BOOST_CHECK_EQUAL(dt.hour(), 0);
   BOOST_CHECK_EQUAL(dt.minute(), 0);
   BOOST_CHECK_EQUAL(dt.second(), 0);
   BOOST_CHECK_EQUAL(dt.microsecond(), 0);
   BOOST_CHECK_EQUAL(dt.millisecond(), 0);
}

BOOST_AUTO_TEST_CASE(core_datetime_ctor_full) {
   DateTime dt(1, 1, 1, 1, 1, 1, 1, 1);

   BOOST_CHECK_EQUAL(dt.year(), 1);
   BOOST_CHECK_EQUAL(dt.month(), 1);
   BOOST_CHECK_EQUAL(dt.day(), 1);
   BOOST_CHECK_EQUAL(dt.hour(), 1);
   BOOST_CHECK_EQUAL(dt.minute(), 1);
   BOOST_CHECK_EQUAL(dt.second(), 1);
   BOOST_CHECK_EQUAL(dt.microsecond(), 1);
   BOOST_CHECK_EQUAL(dt.millisecond(), 1);
}

BOOST_AUTO_TEST_CASE(core_datetime_DecimalYear) {
   int year = 2015;

   int daysInYear = 365;
   int targetDay = 73;
   double expectedDecimalYear = year + (double)targetDay / daysInYear;

   DateTime dt(year, 1, 1);
   dt.addDays(targetDay - 1);

   BOOST_CHECK_EQUAL(dt.decimalYear(), expectedDecimalYear);
}

BOOST_AUTO_TEST_CASE(core_datetime_DecimalYear_Leap) {
   int year = 2016;

   int daysInLeapYear = 366;
   int targetDay = 183;
   double expectedDecimalYear = year + double(targetDay) / daysInLeapYear;

   DateTime dt(year, 1, 1);
   dt.addDays(targetDay - 1);

   BOOST_CHECK_EQUAL(dt.decimalYear(), expectedDecimalYear);
}

// BOOST_AUTO_TEST_CASE(core_datetime_Julian) {
//	DateTime dt(2440587.5); // unix epoch as Julian day
//
//	BOOST_CHECK_EQUAL(dt.year(), 1970);
//    BOOST_CHECK_EQUAL(dt.month(), 1);
//    BOOST_CHECK_EQUAL(dt.day(), 1);
//    BOOST_CHECK_EQUAL(dt.hour(), 0);
//    BOOST_CHECK_EQUAL(dt.minute(), 0);
//    BOOST_CHECK_EQUAL(dt.second(), 0);
//    BOOST_CHECK_EQUAL(dt.millisecond(), 0);
//    BOOST_CHECK_EQUAL(dt.dayOfWeek(), 4);
//    BOOST_CHECK_EQUAL(dt.julianDay(), 2440587.5);
//}

BOOST_AUTO_TEST_CASE(core_datetime_AddYears) {
   DateTime dt(2000, 1, 1);
   dt.addYears(10);

   BOOST_CHECK_EQUAL(dt.year(), 2010);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMonths) {
   DateTime dt(1970, 3, 25);
   dt.addMonths(5);

   BOOST_CHECK_EQUAL(dt.month(), 8);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMonths_YearRollover) {
   DateTime dt(1950, 1, 1);
   dt.addMonths(12);

   BOOST_CHECK_EQUAL(dt.year(), 1951);
   BOOST_CHECK_EQUAL(dt.month(), 1);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMonths_With_Negitive) {
   DateTime dt(1970, 5, 25);
   dt.addMonths(-4);

   BOOST_CHECK_EQUAL(dt.year(), 1970);
   BOOST_CHECK_EQUAL(dt.month(), 1);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMonths_YearRollover_With_Negitive) {
   DateTime dt(1950, 1, 1);
   dt.addMonths(-12);

   BOOST_CHECK_EQUAL(dt.year(), 1949);
   BOOST_CHECK_EQUAL(dt.month(), 1);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddDays) {
   DateTime dt(1970, 3, 25);
   dt.addDays(3);

   BOOST_CHECK_EQUAL(dt.day(), 28);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddDays_MonthRollover) {
   DateTime dt(1970, 3, 25);
   dt.addDays(7);

   BOOST_CHECK_EQUAL(dt.month(), 4);
   BOOST_CHECK_EQUAL(dt.day(), 1);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddDays_YearRollover) {
   DateTime dt(1970, 12, 31);
   dt.addDays(1);

   BOOST_CHECK_EQUAL(dt.year(), 1971);
   BOOST_CHECK_EQUAL(dt.month(), 1);
   BOOST_CHECK_EQUAL(dt.day(), 1);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddDays_Leap) {
   DateTime dt(1972, 2, 28);
   dt.addDays(1);

   BOOST_CHECK_EQUAL(dt.month(), 2);
   BOOST_CHECK_EQUAL(dt.day(), 29);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddDays_MonthRollover_Leap) {
   DateTime dt(1972, 2, 28);
   dt.addDays(2);

   BOOST_CHECK_EQUAL(dt.month(), 3);
   BOOST_CHECK_EQUAL(dt.day(), 1);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddDays_YearRollover_Leap) {
   DateTime dt(1972, 2, 28);
   dt.addDays(365);

   BOOST_CHECK_EQUAL(dt.year(), 1973);
   BOOST_CHECK_EQUAL(dt.month(), 2);
   BOOST_CHECK_EQUAL(dt.day(), 27);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddHours) {
   DateTime dt(1970, 3, 25);
   dt.addHours(7);

   BOOST_CHECK_EQUAL(dt.hour(), 7);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddHours_DayRollover) {
   DateTime dt(1970, 3, 25, 12);
   dt.addHours(24);

   BOOST_CHECK_EQUAL(dt.day(), 26);
   BOOST_CHECK_EQUAL(dt.hour(), 12);
}

BOOST_AUTO_TEST_CASE(core_datetime_SubtractHours_DayRollover) {
   DateTime dt(1970, 3, 25, 12);
   dt.addHours(-24);

   BOOST_CHECK_EQUAL(dt.day(), 24);
   BOOST_CHECK_EQUAL(dt.hour(), 12);
}

BOOST_AUTO_TEST_CASE(core_datetime_SubtractHours) {
   DateTime dt(1970, 3, 25, 14);
   dt.addHours(-7);

   BOOST_CHECK_EQUAL(dt.hour(), 7);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddHours_MonthRollover) {
   DateTime dt(1970, 3, 31, 12);
   dt.addHours(24);

   BOOST_CHECK_EQUAL(dt.month(), 4);
   BOOST_CHECK_EQUAL(dt.day(), 1);
   BOOST_CHECK_EQUAL(dt.hour(), 12);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddHours_YearRollover) {
   DateTime dt(1970, 12, 31, 12);
   dt.addHours(24);

   BOOST_CHECK_EQUAL(dt.year(), 1971);
   BOOST_CHECK_EQUAL(dt.month(), 1);
   BOOST_CHECK_EQUAL(dt.day(), 1);
   BOOST_CHECK_EQUAL(dt.hour(), 12);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddHours_DayRollover_Leap) {
   DateTime dt(1972, 2, 28, 12);
   dt.addHours(24);

   BOOST_CHECK_EQUAL(dt.day(), 29);
   BOOST_CHECK_EQUAL(dt.hour(), 12);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddHours_MonthRollover_Leap) {
   DateTime dt(1972, 2, 29, 12);
   dt.addHours(24);

   BOOST_CHECK_EQUAL(dt.month(), 3);
   BOOST_CHECK_EQUAL(dt.day(), 1);
   BOOST_CHECK_EQUAL(dt.hour(), 12);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddHours_YearRollover_Leap) {
   DateTime dt(1972, 2, 28, 12);
   int oneYear = 24 * 365;
   dt.addHours(oneYear);

   BOOST_CHECK_EQUAL(dt.year(), 1973);
   BOOST_CHECK_EQUAL(dt.month(), 2);
   BOOST_CHECK_EQUAL(dt.day(), 27);
   BOOST_CHECK_EQUAL(dt.hour(), 12);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMinutes) {
   DateTime dt(1970, 3, 25);
   dt.addMinutes(42);

   BOOST_CHECK_EQUAL(dt.minute(), 42);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMinutes_HourRollover) {
   DateTime dt(1970, 3, 25, 1, 30);
   dt.addMinutes(60);

   BOOST_CHECK_EQUAL(dt.hour(), 2);
   BOOST_CHECK_EQUAL(dt.minute(), 30);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMinutes_DayRollover) {
   DateTime dt(1970, 3, 25, 1, 30);
   int oneDay = 60 * 24;
   dt.addMinutes(oneDay);

   BOOST_CHECK_EQUAL(dt.day(), 26);
   BOOST_CHECK_EQUAL(dt.hour(), 1);
   BOOST_CHECK_EQUAL(dt.minute(), 30);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMinutes_MonthRollover) {
   DateTime dt(1970, 3, 25, 1, 30);
   int oneMonth = 60 * 24 * 31;
   dt.addMinutes(oneMonth);

   BOOST_CHECK_EQUAL(dt.month(), 4);
   BOOST_CHECK_EQUAL(dt.day(), 25);
   BOOST_CHECK_EQUAL(dt.hour(), 1);
   BOOST_CHECK_EQUAL(dt.minute(), 30);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMinutes_YearRollover) {
   DateTime dt(1970, 03, 25, 1, 30);
   int oneYear = 60 * 24 * 365;
   dt.addMinutes(oneYear);

   BOOST_CHECK_EQUAL(dt.year(), 1971);
   BOOST_CHECK_EQUAL(dt.month(), 3);
   BOOST_CHECK_EQUAL(dt.day(), 25);
   BOOST_CHECK_EQUAL(dt.hour(), 1);
   BOOST_CHECK_EQUAL(dt.minute(), 30);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMinutes_DayRollover_Leap) {
   DateTime dt(1972, 2, 28, 1, 30);
   int oneDay = 60 * 24;
   dt.addMinutes(oneDay);

   BOOST_CHECK_EQUAL(dt.day(), 29);
   BOOST_CHECK_EQUAL(dt.hour(), 1);
   BOOST_CHECK_EQUAL(dt.minute(), 30);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMinutes_MonthRollover_Leap) {
   DateTime dt(1972, 2, 29, 1, 30);
   int oneDay = 60 * 24;
   dt.addMinutes(oneDay);

   BOOST_CHECK_EQUAL(dt.month(), 3);
   BOOST_CHECK_EQUAL(dt.day(), 1);
   BOOST_CHECK_EQUAL(dt.hour(), 1);
   BOOST_CHECK_EQUAL(dt.minute(), 30);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMinutes_YearRollover_Leap) {
   DateTime dt(1972, 2, 28, 1, 30);
   int oneYear = 60 * 24 * 365;
   dt.addMinutes(oneYear);

   BOOST_CHECK_EQUAL(dt.year(), 1973);
   BOOST_CHECK_EQUAL(dt.month(), 2);
   BOOST_CHECK_EQUAL(dt.day(), 27);
   BOOST_CHECK_EQUAL(dt.hour(), 1);
   BOOST_CHECK_EQUAL(dt.minute(), 30);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddSeconds) {
   DateTime dt(1970, 3, 25);
   dt.addSeconds(42);

   BOOST_CHECK_EQUAL(dt.second(), 42);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddSeconds_MinuteRollover) {
   DateTime dt(1970, 3, 25, 1, 30, 30);
   dt.addSeconds(60);

   BOOST_CHECK_EQUAL(dt.minute(), 31);
   BOOST_CHECK_EQUAL(dt.second(), 30);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddSeconds_HourRollover) {
   DateTime dt(1970, 3, 25, 1, 30, 30);
   double oneHour = 60 * 60;
   dt.addSeconds(oneHour);

   BOOST_CHECK_EQUAL(dt.hour(), 2);
   BOOST_CHECK_EQUAL(dt.minute(), 30);
   BOOST_CHECK_EQUAL(dt.second(), 30);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddSeconds_DayRollover) {
   DateTime dt(1970, 3, 25, 1, 30, 30);
   double oneDay = 60 * 60 * 24;
   dt.addSeconds(oneDay);

   BOOST_CHECK_EQUAL(dt.day(), 26);
   BOOST_CHECK_EQUAL(dt.hour(), 1);
   BOOST_CHECK_EQUAL(dt.minute(), 30);
   BOOST_CHECK_EQUAL(dt.second(), 30);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddSeconds_MonthRollover) {
   DateTime dt(1970, 3, 25, 1, 30, 30);
   double oneMonth = 60 * 60 * 24 * 31;
   dt.addSeconds(oneMonth);

   BOOST_CHECK_EQUAL(dt.month(), 4);
   BOOST_CHECK_EQUAL(dt.day(), 25);
   BOOST_CHECK_EQUAL(dt.hour(), 1);
   BOOST_CHECK_EQUAL(dt.minute(), 30);
   BOOST_CHECK_EQUAL(dt.second(), 30);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddSeconds_YearRollover) {
   DateTime dt(1970, 03, 25, 1, 30, 30);
   double oneYear = 60 * 60 * 24 * 365;
   dt.addSeconds(oneYear);

   BOOST_CHECK_EQUAL(dt.year(), 1971);
   BOOST_CHECK_EQUAL(dt.month(), 3);
   BOOST_CHECK_EQUAL(dt.day(), 25);
   BOOST_CHECK_EQUAL(dt.hour(), 1);
   BOOST_CHECK_EQUAL(dt.minute(), 30);
   BOOST_CHECK_EQUAL(dt.second(), 30);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddSeconds_DayRollover_Leap) {
   DateTime dt(1972, 2, 28, 1, 30, 30);
   double oneDay = 60 * 60 * 24;
   dt.addSeconds(oneDay);

   BOOST_CHECK_EQUAL(dt.day(), 29);
   BOOST_CHECK_EQUAL(dt.hour(), 1);
   BOOST_CHECK_EQUAL(dt.minute(), 30);
   BOOST_CHECK_EQUAL(dt.second(), 30);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddSeconds_MonthRollover_Leap) {
   DateTime dt(1972, 2, 29, 1, 30, 30);
   double oneDay = 60 * 60 * 24;
   dt.addSeconds(oneDay);

   BOOST_CHECK_EQUAL(dt.month(), 3);
   BOOST_CHECK_EQUAL(dt.day(), 1);
   BOOST_CHECK_EQUAL(dt.hour(), 1);
   BOOST_CHECK_EQUAL(dt.minute(), 30);
   BOOST_CHECK_EQUAL(dt.second(), 30);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddSeconds_YearRollover_Leap) {
   DateTime dt(1972, 2, 28, 1, 30, 30);
   double oneYear = 60 * 60 * 24 * 365;
   dt.addSeconds(oneYear);

   BOOST_CHECK_EQUAL(dt.year(), 1973);
   BOOST_CHECK_EQUAL(dt.month(), 2);
   BOOST_CHECK_EQUAL(dt.day(), 27);
   BOOST_CHECK_EQUAL(dt.hour(), 1);
   BOOST_CHECK_EQUAL(dt.minute(), 30);
   BOOST_CHECK_EQUAL(dt.second(), 30);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMilliseconds) {
   DateTime dt(1970, 3, 25);
   dt.addMilliseconds(42);

   BOOST_CHECK_EQUAL(dt.millisecond(), 42);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMilliseconds_SecondRollover) {
   DateTime dt(1970, 3, 25, 1, 1, 1, 500);
   dt.addMilliseconds(1000);

   BOOST_CHECK_EQUAL(dt.second(), 2);
   BOOST_CHECK_EQUAL(dt.millisecond(), 500);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMilliseconds_MinuteRollover) {
   DateTime dt(1970, 3, 25, 1, 1, 1, 500);
   double oneMinute = 1000.0 * 60.0;
   dt.addMilliseconds(oneMinute);

   BOOST_CHECK_EQUAL(dt.minute(), 2);
   BOOST_CHECK_EQUAL(dt.second(), 1);
   BOOST_CHECK_EQUAL(dt.millisecond(), 500);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMilliseconds_HourRollover) {
   DateTime dt(1970, 3, 25, 1, 1, 1, 500);
   double oneHour = 1000.0 * 60.0 * 60.0;
   dt.addMilliseconds(oneHour);

   BOOST_CHECK_EQUAL(dt.hour(), 2);
   BOOST_CHECK_EQUAL(dt.minute(), 1);
   BOOST_CHECK_EQUAL(dt.second(), 1);
   BOOST_CHECK_EQUAL(dt.millisecond(), 500);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMilliseconds_DayRollover) {
   DateTime dt(1970, 3, 25, 1, 1, 1, 500);
   double oneDay = 1000.0 * 60.0 * 60.0 * 24.0;
   dt.addMilliseconds(oneDay);

   BOOST_CHECK_EQUAL(dt.day(), 26);
   BOOST_CHECK_EQUAL(dt.hour(), 1);
   BOOST_CHECK_EQUAL(dt.minute(), 1);
   BOOST_CHECK_EQUAL(dt.second(), 1);
   BOOST_CHECK_EQUAL(dt.millisecond(), 500);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMilliseconds_MonthRollover) {
   DateTime dt(1970, 3, 25, 1, 1, 1, 500);
   double oneMonth = 1000.0 * 60.0 * 60.0 * 24.0 * 31.0;
   dt.addMilliseconds(oneMonth);

   BOOST_CHECK_EQUAL(dt.month(), 4);
   BOOST_CHECK_EQUAL(dt.day(), 25);
   BOOST_CHECK_EQUAL(dt.hour(), 1);
   BOOST_CHECK_EQUAL(dt.minute(), 1);
   BOOST_CHECK_EQUAL(dt.second(), 1);
   BOOST_CHECK_EQUAL(dt.millisecond(), 500);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMilliseconds_YearRollover) {
   DateTime dt(1970, 3, 25, 1, 1, 1, 500);
   double oneYear = 1000.0 * 60.0 * 60.0 * 24.0 * 365.0;
   dt.addMilliseconds(oneYear);

   BOOST_CHECK_EQUAL(dt.year(), 1971);
   BOOST_CHECK_EQUAL(dt.month(), 3);
   BOOST_CHECK_EQUAL(dt.day(), 25);
   BOOST_CHECK_EQUAL(dt.hour(), 1);
   BOOST_CHECK_EQUAL(dt.minute(), 1);
   BOOST_CHECK_EQUAL(dt.second(), 1);
   BOOST_CHECK_EQUAL(dt.millisecond(), 500);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMilliseconds_DayRollover_Leap) {
   DateTime dt(1972, 2, 28, 1, 1, 1, 500);
   double oneDay = 1000.0 * 60.0 * 60.0 * 24.0;
   dt.addMilliseconds(oneDay);

   BOOST_CHECK_EQUAL(dt.day(), 29);
   BOOST_CHECK_EQUAL(dt.hour(), 1);
   BOOST_CHECK_EQUAL(dt.minute(), 1);
   BOOST_CHECK_EQUAL(dt.second(), 1);
   BOOST_CHECK_EQUAL(dt.millisecond(), 500);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMilliseconds_MonthRollover_Leap) {
   DateTime dt(1972, 2, 29, 1, 1, 1, 500);
   double oneDay = 1000.0 * 60.0 * 60.0 * 24.0;
   dt.addMilliseconds(oneDay);

   BOOST_CHECK_EQUAL(dt.month(), 3);
   BOOST_CHECK_EQUAL(dt.day(), 1);
   BOOST_CHECK_EQUAL(dt.hour(), 1);
   BOOST_CHECK_EQUAL(dt.minute(), 1);
   BOOST_CHECK_EQUAL(dt.second(), 1);
   BOOST_CHECK_EQUAL(dt.millisecond(), 500);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMilliseconds_YearRollover_Leap) {
   DateTime dt(1972, 2, 28, 1, 1, 1, 500);
   double oneYear = 1000.0 * 60.0 * 60.0 * 24.0 * 365.0;
   dt.addMilliseconds(oneYear);

   BOOST_CHECK_EQUAL(dt.year(), 1973);
   BOOST_CHECK_EQUAL(dt.month(), 2);
   BOOST_CHECK_EQUAL(dt.day(), 27);
   BOOST_CHECK_EQUAL(dt.hour(), 1);
   BOOST_CHECK_EQUAL(dt.minute(), 1);
   BOOST_CHECK_EQUAL(dt.second(), 1);
   BOOST_CHECK_EQUAL(dt.millisecond(), 500);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMicroseconds) {
   DateTime dt(1970, 3, 25);
   dt.addMicroseconds(42);

   BOOST_CHECK_EQUAL(dt.microsecond(), 42);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMicroseconds_MillisecondRollover) {
   DateTime dt(1970, 3, 25, 1, 1, 1, 1, 500);
   dt.addMicroseconds(1000);

   BOOST_CHECK_EQUAL(dt.millisecond(), 2);
   BOOST_CHECK_EQUAL(dt.microsecond(), 500);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMicroseconds_SecondRollover) {
   DateTime dt(1970, 3, 25, 1, 1, 1, 1, 500);
   double oneSecond = 1000.0 * 1000.0;
   dt.addMicroseconds(oneSecond);

   BOOST_CHECK_EQUAL(dt.second(), 2);
   BOOST_CHECK_EQUAL(dt.millisecond(), 1);
   BOOST_CHECK_EQUAL(dt.microsecond(), 500);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMicroseconds_MinuteRollover) {
   DateTime dt(1970, 3, 25, 1, 1, 1, 1, 500);
   double oneMinute = 1000.0 * 1000.0 * 60.0;
   dt.addMicroseconds(oneMinute);

   BOOST_CHECK_EQUAL(dt.minute(), 2);
   BOOST_CHECK_EQUAL(dt.second(), 1);
   BOOST_CHECK_EQUAL(dt.millisecond(), 1);
   BOOST_CHECK_EQUAL(dt.microsecond(), 500);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMicroseconds_HourRollover) {
   DateTime dt(1970, 3, 25, 1, 1, 1, 1, 500);
   double oneHour = 1000.0 * 1000.0 * 60.0 * 60.0;
   dt.addMicroseconds(oneHour);

   BOOST_CHECK_EQUAL(dt.hour(), 2);
   BOOST_CHECK_EQUAL(dt.minute(), 1);
   BOOST_CHECK_EQUAL(dt.second(), 1);
   BOOST_CHECK_EQUAL(dt.millisecond(), 1);
   BOOST_CHECK_EQUAL(dt.microsecond(), 500);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMicroseconds_DayRollover) {
   DateTime dt(1970, 3, 25, 1, 1, 1, 1, 500);
   double oneDay = 1000.0 * 1000.0 * 60.0 * 60.0 * 24.0;
   dt.addMicroseconds(oneDay);

   BOOST_CHECK_EQUAL(dt.day(), 26);
   BOOST_CHECK_EQUAL(dt.hour(), 1);
   BOOST_CHECK_EQUAL(dt.minute(), 1);
   BOOST_CHECK_EQUAL(dt.second(), 1);
   BOOST_CHECK_EQUAL(dt.millisecond(), 1);
   BOOST_CHECK_EQUAL(dt.microsecond(), 500);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMicroseconds_MonthRollover) {
   DateTime dt(1970, 3, 25, 1, 1, 1, 1, 500);
   double oneMonth = 1000.0 * 1000.0 * 60.0 * 60.0 * 24.0 * 31.0;
   dt.addMicroseconds(oneMonth);

   BOOST_CHECK_EQUAL(dt.month(), 4);
   BOOST_CHECK_EQUAL(dt.day(), 25);
   BOOST_CHECK_EQUAL(dt.hour(), 1);
   BOOST_CHECK_EQUAL(dt.minute(), 1);
   BOOST_CHECK_EQUAL(dt.second(), 1);
   BOOST_CHECK_EQUAL(dt.millisecond(), 1);
   BOOST_CHECK_EQUAL(dt.microsecond(), 500);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMicroseconds_YearRollover) {
   DateTime dt(1970, 3, 25, 1, 1, 1, 1, 500);
   double oneYear = 1000.0 * 1000.0 * 60.0 * 60.0 * 24.0 * 365.0;
   dt.addMicroseconds(oneYear);

   BOOST_CHECK_EQUAL(dt.year(), 1971);
   BOOST_CHECK_EQUAL(dt.month(), 3);
   BOOST_CHECK_EQUAL(dt.day(), 25);
   BOOST_CHECK_EQUAL(dt.hour(), 1);
   BOOST_CHECK_EQUAL(dt.minute(), 1);
   BOOST_CHECK_EQUAL(dt.second(), 1);
   BOOST_CHECK_EQUAL(dt.millisecond(), 1);
   BOOST_CHECK_EQUAL(dt.microsecond(), 500);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMicroseconds_DayRollover_Leap) {
   DateTime dt(1972, 2, 28, 1, 1, 1, 1, 500);
   double oneDay = 1000.0 * 1000.0 * 60.0 * 60.0 * 24.0;
   dt.addMicroseconds(oneDay);

   BOOST_CHECK_EQUAL(dt.day(), 29);
   BOOST_CHECK_EQUAL(dt.hour(), 1);
   BOOST_CHECK_EQUAL(dt.minute(), 1);
   BOOST_CHECK_EQUAL(dt.second(), 1);
   BOOST_CHECK_EQUAL(dt.millisecond(), 1);
   BOOST_CHECK_EQUAL(dt.microsecond(), 500);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMicroseconds_MonthRollover_Leap) {
   DateTime dt(1972, 2, 29, 1, 1, 1, 1, 500);
   double oneDay = 1000.0 * 1000.0 * 60.0 * 60.0 * 24.0;
   dt.addMicroseconds(oneDay);

   BOOST_CHECK_EQUAL(dt.month(), 3);
   BOOST_CHECK_EQUAL(dt.day(), 1);
   BOOST_CHECK_EQUAL(dt.hour(), 1);
   BOOST_CHECK_EQUAL(dt.minute(), 1);
   BOOST_CHECK_EQUAL(dt.second(), 1);
   BOOST_CHECK_EQUAL(dt.millisecond(), 1);
   BOOST_CHECK_EQUAL(dt.microsecond(), 500);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddMicroseconds_YearRollover_Leap) {
   DateTime dt(1972, 2, 28, 1, 1, 1, 1, 500);
   double oneYear = 1000.0 * 1000.0 * 60.0 * 60.0 * 24.0 * 365.0;
   dt.addMicroseconds(oneYear);

   BOOST_CHECK_EQUAL(dt.year(), 1973);
   BOOST_CHECK_EQUAL(dt.month(), 2);
   BOOST_CHECK_EQUAL(dt.day(), 27);
   BOOST_CHECK_EQUAL(dt.hour(), 1);
   BOOST_CHECK_EQUAL(dt.minute(), 1);
   BOOST_CHECK_EQUAL(dt.second(), 1);
   BOOST_CHECK_EQUAL(dt.millisecond(), 1);
   BOOST_CHECK_EQUAL(dt.microsecond(), 500);
}

BOOST_AUTO_TEST_CASE(core_datetime_AddChained) {
   DateTime dt(1970, 3, 25);
   dt.addYears(1).addMonths(1).addDays(1).addHours(1).addMinutes(1).addSeconds(1).addMilliseconds(1).addMicroseconds(1);

   BOOST_CHECK_EQUAL(dt.year(), 1971);
   BOOST_CHECK_EQUAL(dt.month(), 4);
   BOOST_CHECK_EQUAL(dt.day(), 26);
   BOOST_CHECK_EQUAL(dt.hour(), 1);
   BOOST_CHECK_EQUAL(dt.minute(), 1);
   BOOST_CHECK_EQUAL(dt.second(), 1);
   BOOST_CHECK_EQUAL(dt.millisecond(), 1);
   BOOST_CHECK_EQUAL(dt.microsecond(), 1);
}

BOOST_AUTO_TEST_CASE(core_datetime_DaysInYear) { BOOST_CHECK_EQUAL(DateTime::daysInYear(1970), 365); }

BOOST_AUTO_TEST_CASE(core_datetime_DaysInYear_Leap) { BOOST_CHECK_EQUAL(DateTime::daysInYear(1972), 366); }

BOOST_AUTO_TEST_CASE(core_datetime_Now) {
   // Could try to compare this now() to another now() a second later, but
   // the minute could roll over, etc. - so just exercise the method.
   auto now = DateTime::now();
   BOOST_CHECK(now.year() > 2020);
}

BOOST_AUTO_TEST_CASE(core_datetime_OStream) {
   DateTime dt;
   std::ostringstream oss;
   oss << dt;
   const auto str = oss.str();
   BOOST_CHECK(str.length() > 0);
}

BOOST_AUTO_TEST_CASE(core_datetime_localtime) {
   const auto now = moja::systemtime_now();
   auto t1 = moja::localtime(now);
   const auto str = moja::put_time(&t1, "%c %Z");
   BOOST_CHECK(str.length() > 0);
}

BOOST_AUTO_TEST_CASE(core_datetime_put_time_now) {
   const auto now = moja::systemtime_now();
   auto t1 = moja::localtime(now);
   auto str = moja::put_time(&t1, "%c %Z");
   BOOST_CHECK(str.length() > 0);
}

BOOST_AUTO_TEST_CASE(core_datetime_put_time) {
   for (short x = 1969; x > 1900; --x) {
      DateTime dt(x, 11, 30);
      std::ostringstream oss;
      oss << dt;
      auto str = oss.str();
      BOOST_CHECK(str.length() > 0);
   }
}

BOOST_AUTO_TEST_SUITE_END();