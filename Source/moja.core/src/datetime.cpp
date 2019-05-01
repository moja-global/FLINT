#include "moja/datetime.h"
#include "moja/exception.h"

using namespace date;
using namespace std::chrono;

namespace moja {

DateTime::~DateTime() {}

DateTime DateTime::now() {
	auto tp = system_clock::now();
	auto dp = floor<days>(tp);
	DateTime date;
	date._date= dp;
	date._tod=make_time(duration_cast<microseconds>(tp - dp));
	return date;
}

DateTime& DateTime::operator+=(const Timespan& span) {
	add_micro( span.duration() );
	return *this;
}

DateTime& DateTime::addYears(int years) {
	_date += date::years(years);
	return *this;
}

// This is more complicated than it appears
// Simply adding the days in a month to the current DateTime will not be succifient.
// We need to increment only the month, keeping the day the same (if possible). 
// If the resulting date is not valid (ie going from Jan 31 -> Feb 31 shoudl equal Feb 28 (or 29 in a leap year)

// The AddMonths method calculates the resulting month and year, taking into account leap years and the number of days in a month, 
// then adjusts the day part of the resulting DateTime object.If the resulting day is not a valid day in the resulting month, the last 
// valid day of the resulting month is used.
// For example, March 31st + 1 month = April 30th, and March 31st - 1 month = February 28 for a non - leap year and February 29 for a leap year.
// The time - of - day part of the resulting DateTime object remains the same as this instance.

DateTime& DateTime::addMonths(int months) {
	_date += date::months(months);
	if (!_date.ok())
		_date = _date.year() / _date.month() / last;
	return *this;
}

DateTime& DateTime::addDays(double days) {
	auto tp = sys_days(_date) + _tod.to_duration() + duration_cast<microseconds>(duration<double, date::days::period>(days));
	auto dp = floor<date::days>(tp);
	_date = dp;
	_tod = make_time(tp - dp);
	return *this;
}

void DateTime::add(duration<double, std::micro> value) {
	auto tod = _tod.to_duration() + value;
	if (tod > microseconds{ 0 } && tod < days{ 1 }) {
		_tod = make_time(duration_cast<microseconds>(tod));
	} else {
		auto tp = time_point_cast<microseconds>(sys_days(_date) + tod);
		auto dp = floor<date::days>(tp);
		_date = dp;
		_tod = make_time(tp - dp);
	}
}

void DateTime::add_micro(duration<long long, std::micro> value) {
	auto tod = _tod.to_duration() + value;
	if (tod > microseconds{ 0 } && tod < days{ 1 }) {
		_tod = make_time(duration_cast<microseconds>(tod));
	} else {
		auto tp = sys_days(_date) + tod;
		auto dp = floor<date::days>(tp);
		_date = dp;
		_tod = make_time(duration_cast<microseconds>(tp - dp));
	}
}

DateTime& DateTime::addHours(double hours) {
	add(duration<double, std::chrono::hours::period>(hours));
	return *this;
}

DateTime& DateTime::addMinutes(double minutes) {
	add(duration<double, std::chrono::minutes::period>(minutes));
	return *this;
}

DateTime& DateTime::addSeconds(double seconds) {
	add(duration<double, std::chrono::seconds::period>(seconds));
	return *this;
}

DateTime& DateTime::addMilliseconds(double milliseconds) {
	add(duration<double, std::chrono::milliseconds::period>(milliseconds));
	return *this;
}

DateTime& DateTime::addMicroseconds(double microseconds) {
	add(duration<double, std::chrono::microseconds::period>(microseconds));
	return *this;
}

double DateTime::decimalYear() const {
	const auto year = _date.year();
	double days = (sys_days(_date) - sys_days{ year / jan / 0 }).count();
    double daysInYear = DateTime::daysInYear(int(year));
    return int(year) + (days / daysInYear);
}

} // namespace moja
