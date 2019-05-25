#ifndef MOJA_CORE_DATETIME_H_
#define MOJA_CORE_DATETIME_H_

#include "_core_exports.h"

#undef min
#undef max
#include "moja/timespan.h"

#include <date/date.h>

#include <chrono>
#include <iomanip>

namespace moja {

typedef std::chrono::time_point<std::chrono::system_clock> system_time_point;

inline tm localtime(const std::time_t& time) {
   std::tm tm_snapshot;
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
   localtime_s(&tm_snapshot, &time);
#else
   localtime_r(&time, &tm_snapshot);  // POSIX
#endif
   return tm_snapshot;
}

// To simplify things the return value is just a string. I.e. by design!
inline std::string put_time(const std::tm* date_time, const char* c_time_format) {
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
   std::ostringstream oss;

   // BOGUS hack done for VS2012: C++11 non-conformant since it SHOULD take a "const struct tm*  "
   // ref. C++11 standard: ISO/IEC 14882:2011, § 27.7.1,
   oss << std::put_time(const_cast<std::tm*>(date_time), c_time_format);
   return oss.str();

#else  // LINUX
   const size_t size = 1024;
   char buffer[size];
   auto success = std::strftime(buffer, size, c_time_format, date_time);

   if (0 == success) return c_time_format;

   return buffer;
#endif
}

// extracting std::time_t from std:chrono for "now"
inline std::time_t systemtime_now() {
   system_time_point system_now = std::chrono::system_clock::now();
   return std::chrono::system_clock::to_time_t(system_now);
}

/**
 * <summary>    DateTime with convenience methods for adding time. </summary>
 */
class CORE_API DateTime {
  public:
   /** <summary>    Default constructor - date and time. </summary> */
   DateTime()
       : _date(),
         _tod(std::chrono::hours{0}, std::chrono::minutes{0}, std::chrono::seconds{0}, std::chrono::microseconds{0},
              0) {}
   ~DateTime();

   /**
    * <summary>    Constructs a DateTime from component times. </summary>
    *
    * <param name="year">          The year. </param>
    * <param name="month">         The month. </param>
    * <param name="day">           The day. </param>
    * <param name="hour">          (Optional) the hour. </param>
    * <param name="minute">        (Optional) the minute. </param>
    * <param name="second">        (Optional) the second. </param>
    * <param name="millisecond">   (Optional) the millisecond. </param>
    * <param name="microsecond">   (Optional) the microsecond. </param>
    */
   DateTime(short year, unsigned month, unsigned day)
       : _date(date::year{year}, date::month{month}, date::day{day}),
         _tod(std::chrono::hours{0}, std::chrono::minutes{0}, std::chrono::seconds{0}, std::chrono::microseconds{0},
              0) {}

   DateTime(short year, unsigned month, unsigned day, int hour, int minute = 0, int second = 0)
       : _date(date::year{year}, date::month{month}, date::day{day}),
         _tod(std::chrono::hours{hour}, std::chrono::minutes{minute}, std::chrono::seconds{second},
              std::chrono::microseconds{0}, 0) {}

   DateTime(short year, unsigned month, unsigned day, int hour, int minute, int second, int millisecond,
            int microsecond = 0)
       : _date(date::year{year}, date::month{month}, date::day{day}),
         _tod(std::chrono::hours{hour}, std::chrono::minutes{minute}, std::chrono::seconds{second},
              std::chrono::milliseconds{millisecond} + std::chrono::microseconds{microsecond}, 0) {}

   /**
    * <summary>
    * Return the current day and year in decimal format (i.e. 1970.1095890410958904 (day 40))
    * </summary>
    */
   double decimalYear() const;

   /**
    * <summary>    Increments the date by a number of years. </summary>
    *
    * <param name="years"> The number of years to add. </param>
    *
    * <returns>    A reference to this DateTime for chained calls. </returns>
    */
   DateTime& addYears(int years);

   /**
    * <summary>    Increments the date by a number of months. </summary>
    *
    * <param name="months">    The number of months to add. </param>
    *
    * <returns>    A reference to this DateTime for chained calls. </returns>
    */
   DateTime& addMonths(int months);

   /**
    * <summary>    Adds a fractional number of days. </summary>
    *
    * <param name="days">  The number of days to add. </param>
    *
    * <returns>    A reference to this DateTime for chained calls. </returns>
    */
   DateTime& addDays(double days);

   /**
    * <summary>    Adds a fractional number of hours. </summary>
    *
    * <param name="hours"> The number of hours to add. </param>
    *
    * <returns>    A reference to this DateTime for chained calls. </returns>
    */
   DateTime& addHours(double hours);

   /**
    * <summary>    Adds a fractional number of minutes. </summary>
    *
    * <param name="minutes">   The number of minutes to add. </param>
    *
    * <returns>    A reference to this DateTime for chained calls. </returns>
    */
   DateTime& addMinutes(double minutes);

   /**
    * <summary>    Adds a fractional number of seconds. </summary>
    *
    * <param name="seconds">   The number of seconds to add. </param>
    *
    * <returns>    A reference to this DateTime for chained calls. </returns>
    */
   DateTime& addSeconds(double seconds);

   /**
    * <summary>    Adds a given number of milliseconds. </summary>
    *
    * <param name="milliseconds">  The number of milliseconds to add. </param>
    *
    * <returns>    A reference to this DateTime for chained calls. </returns>
    */
   DateTime& addMilliseconds(double milliseconds);

   /**
    * <summary>    Adds a given number of microseconds. </summary>
    *
    * <param name="microseconds">  The number of microseconds to add. </param>
    *
    * <returns>    A reference to this DateTime for chained calls. </returns>
    */
   DateTime& addMicroseconds(double microseconds);
   /**
    * <summary>    Returns the number of days in a given year. </summary>
    *
    * <param name="year">  The year. </param>
    */
   static short daysInYear(int year);

   /**
    * <summary>    constructs a DateTime from the current system date and time. </summary>
    */
   static DateTime now();

   /**
    * <summary>    Inserts the string representation of this datetime into a stream. </summary>
    */
   friend std::ostream& operator<<(std::ostream& output, const DateTime& dt) {
      output << dt._date << ":" << dt._tod;

      // output << dt.year() << std::setfill('0') << std::setw(2) << dt.month()
      //       << std::setfill('0') << std::setw(2) << dt.day() << ":" << dt.hour()
      //       << ":" << dt.minute() << ":" << dt.second() << ":" << dt.millisecond();

      return output;
   }

   std::string toString() const {
      std::stringstream ss;
      ss << *this;
      return ss.str();
   }

   int year() const { return int(_date.year()); }
   int month() const { return unsigned(_date.month()); }
   int day() const { return unsigned(_date.day()); }

   int hour() const { return _tod.hours().count(); }
   int minute() const { return _tod.minutes().count(); }
   int second() const { return int(_tod.seconds().count()); }
   int millisecond() const {
      return int(std::chrono::duration_cast<std::chrono::milliseconds>(_tod.subseconds()).count());
   }
   int microsecond() const {
      auto ms = date::floor<std::chrono::milliseconds>(_tod.subseconds());
      return int(std::chrono::duration_cast<std::chrono::microseconds>(_tod.subseconds() - ms).count());
   }

   DateTime& operator+=(const moja::Timespan& span);

   static int daysOfMonth(int year, int month);
   int dayOfYear() const;
   int dayOfWeek() const;
   static bool isLeapYear(int y) MOJA_NOEXCEPT;

   // double julianDay() const;
   DateTime& assign(short year, unsigned month, unsigned day, int hour, int minute, int second, int millisecond,
                    int microsecond);

   bool operator==(const DateTime& dateTime) const;
   bool operator!=(const DateTime& dateTime) const;
   bool operator<(const DateTime& dateTime) const;
   bool operator<=(const DateTime& dateTime) const;
   bool operator>(const DateTime& dateTime) const;
   bool operator>=(const DateTime& dateTime) const;

   moja::Timespan operator-(const DateTime& dateTime) const;

  protected:
   /**
    * <summary>    Adds a duration time units to this DateTime. </summary>
    *
    * <param name="value"> The number of units to add. </param>
    */
   void add(std::chrono::duration<double, std::micro> value);
   void add_micro(std::chrono::duration<long long, std::micro> value);

  private:
   static unsigned last_day_of_month_common_year(unsigned m) MOJA_NOEXCEPT {
      constexpr unsigned char a[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
      return a[m - 1];
   }

   date::year_month_day _date;
   date::time_of_day<std::chrono::microseconds> _tod;
};

inline bool DateTime::isLeapYear(int y) MOJA_NOEXCEPT { return y % 4 == 0 && (y % 100 != 0 || y % 400 == 0); }
inline short DateTime::daysInYear(int year) { return isLeapYear(year) ? 366 : 365; }

inline int DateTime::daysOfMonth(int y, int m) {
   return m != 2 || !isLeapYear(y) ? last_day_of_month_common_year(m) : 29u;
}
inline DateTime& DateTime::assign(short year, unsigned month, unsigned day, int hour, int minute, int second,
                                  int millisecond, int microsecond) {
   using namespace date;
   _date = year_month_day(date::year{year}, date::month{month}, date::day{day});
   _tod = make_time(std::chrono::hours{hour} + std::chrono::minutes{minute} + std::chrono::seconds{second} +
                    std::chrono::milliseconds{millisecond} + std::chrono::microseconds{microsecond});
   return *this;
}

inline int DateTime::dayOfYear() const {
   using namespace date;

   // calculating the year and the day of the year
   const auto year = _date.year();
   return (sys_days(_date) - sys_days{year / jan / 0}).count();
}

inline int DateTime::dayOfWeek() const {
   using namespace date;

   sys_days dp = _date;
   weekday wd(dp);
   return unsigned(wd);
}

inline bool DateTime::operator==(const DateTime& dateTime) const {
   return _date == dateTime._date && _tod.to_duration() == dateTime._tod.to_duration();
}
inline bool DateTime::operator!=(const DateTime& dateTime) const { return !(operator==(dateTime)); }
inline bool DateTime::operator<(const DateTime& dateTime) const {
   return _date < dateTime._date
              ? true
              : (_date > dateTime._date ? false : (_tod.to_duration() < dateTime._tod.to_duration()));
}
inline bool DateTime::operator<=(const DateTime& dateTime) const { return !(dateTime < *this); }
inline bool DateTime::operator>(const DateTime& dateTime) const { return dateTime < *this; }
inline bool DateTime::operator>=(const DateTime& dateTime) const { return !(*this < dateTime); }

inline Timespan DateTime::operator-(const DateTime& dateTime) const {
   using namespace date;
   using namespace std::chrono;

   auto dp_this = sys_days(_date) + _tod.to_duration();
   auto dp_rhs = sys_days(dateTime._date) + dateTime._tod.to_duration();
   auto span = dp_this - dp_rhs;
   return Timespan(span);
}

}  // namespace moja

#endif  // MOJA_CORE_DATETIME_H_