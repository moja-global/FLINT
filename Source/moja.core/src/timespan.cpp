#include "moja/timespan.h"
#undef min
#undef max
#include <date/date.h>

namespace moja {

Timespan::Timespan() :
	_span(0) {}

Timespan::Timespan(std::chrono::microseconds microSeconds) :
	_span(microSeconds) {}

Timespan::Timespan(long seconds, long microSeconds) :
	_span(std::chrono::seconds{ seconds } +
			std::chrono::microseconds{ microSeconds }) {}

Timespan::Timespan(int days, int hours, int minutes, int seconds, int microSeconds) :
	_span(std::chrono::microseconds { microSeconds }
		+ std::chrono::seconds { seconds }
		+ std::chrono::minutes { minutes }
		+ std::chrono::hours { hours }
		+ date::days{ days }) {}

Timespan::Timespan(const Timespan& timespan) :
	_span(timespan._span) {}

int Timespan::days() const {
	return std::chrono::duration_cast<date::days>(_span).count();
}

double Timespan::totalDays() const {
	using fdays = std::chrono::duration<double, date::days::period>;
	return std::chrono::duration_cast<fdays>(_span).count();
}

Timespan& Timespan::operator = (const Timespan& timespan) {
	_span = timespan._span;
	return *this;
}

Timespan& Timespan::assign(int days, int hours, int minutes, int seconds, int microSeconds) {
	_span = std::chrono::microseconds{ microSeconds }
		+std::chrono::seconds{ seconds }
		+std::chrono::minutes{ minutes }
		+std::chrono::hours{ hours }
	+date::days{ days };
	return *this;
}

Timespan& Timespan::assign(long seconds, long microSeconds) {
	_span = std::chrono::seconds { seconds } +
		std::chrono::microseconds { microSeconds };
	return *this;
}

void Timespan::swap(Timespan& timespan) MOJA_NOEXCEPT
{
	std::swap(_span, timespan._span);
}

Timespan Timespan::operator + (const Timespan& d) const {
	return Timespan(_span + d._span);
}

Timespan Timespan::operator - (const Timespan& d) const {
	return Timespan(_span - d._span);
}

Timespan& Timespan::operator += (const Timespan& d) {
	_span += d._span;
	return *this;
}

Timespan& Timespan::operator -= (const Timespan& d) {
	_span -= d._span;
	return *this;
}

Timespan Timespan::operator+(const std::chrono::microseconds& d) const {
	return Timespan(_span + d);
}

Timespan Timespan::operator - (const std::chrono::microseconds& d) const {
	return Timespan(_span - d);
}

Timespan& Timespan::operator += (const std::chrono::microseconds& d) {
	_span += d;
	return *this;
}

Timespan& Timespan::operator -= (const std::chrono::microseconds& d) {
	_span -= d;
	return *this;
}

} // namespace moja
