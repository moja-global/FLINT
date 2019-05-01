#ifndef MOJA_CORE_TIMESPAN_H_
#define MOJA_CORE_TIMESPAN_H_

#include "moja/_core_exports.h"

#include <chrono>

namespace moja {

/** <summary>    A class that represents time spans up to microsecond resolution. </summary> */
class CORE_API Timespan {
public:
    /** <summary>    Creates a zero Timespan. </summary> */
    Timespan();

    /**
     * <summary>    Creates a Timespan for a specified number of microseconds. </summary>
     */
    explicit Timespan(std::chrono::microseconds microseconds);

    /**
     * <summary>    Creates a Timespan. Useful for creating a Timespan from a struct timeval. </summary>
     *
     * <param name="seconds">       The seconds. </param>
     * <param name="microseconds">  The microseconds. </param>
     */
    Timespan(long seconds, long microseconds);

    /**
     * <summary>    Creates a Timespan. </summary>
     *
     * <param name="days">          The days. </param>
     * <param name="hours">         The hours. </param>
     * <param name="minutes">       The minutes. </param>
     * <param name="seconds">       (Optional) the seconds. </param>
     * <param name="microseconds">  (Optional) the microseconds. </param>
     */
    Timespan(int days, int hours, int minutes, int seconds = 0, int microseconds = 0);

    /**
     * <summary>    Creates a Timespan from another one. </summary>
     *
     * <param name="timespan">  The target timespan. </param>
     */
    Timespan(const Timespan& timespan);

    /** <summary>    Destroys the Timespan. </summary> */
    ~Timespan() = default;

    /**
     * <summary>    Assigns this Timespan the value of another Timespan. </summary>
     */
    Timespan& operator = (const Timespan& timespan);

    /**
     * <summary>    Assigns this Timespan a new value from a number of microseconds. </summary>
     */
    //Timespan& operator = (Poco::Timestamp::TimeDiff microseconds);

    /**
     * <summary>    Assigns new values to the Timespan. </summary>
     *
     * <param name="days">          The days. </param>
     * <param name="hours">         The hours. </param>
     * <param name="minutes">       The minutes. </param>
     * <param name="seconds">       The seconds. </param>
     * <param name="microseconds">  The microseconds. </param>
     *
     * <returns>    A reference to the Timespan. </returns>
     */
    Timespan& assign(int days, int hours, int minutes, int seconds, int microseconds);

    /**
     * <summary>    Assigns a new span. Useful for assigning from a struct timeval. </summary>
     *
     * <param name="seconds">       The seconds. </param>
     * <param name="microseconds">  The microseconds. </param>
     *
     * <returns>    A reference to the Timespan. </returns>
     */
    Timespan& assign(long seconds, long microseconds);

    /**
     * <summary>    Swaps the Timespan with another one. </summary>
     *
     * <param name="timespan">  The new timespan. </param>
     */
    void swap(Timespan& timespan) MOJA_NOEXCEPT;
    
    bool operator == (const Timespan& ts) const;
    bool operator != (const Timespan& ts) const;
    bool operator >  (const Timespan& ts) const;
    bool operator >= (const Timespan& ts) const;
    bool operator <  (const Timespan& ts) const;
    bool operator <= (const Timespan& ts) const;

    Timespan operator + (const Timespan& d) const;
    Timespan operator - (const Timespan& d) const;
    Timespan& operator += (const Timespan& d);
    Timespan& operator -= (const Timespan& d);

	bool operator == (const std::chrono::microseconds& ts) const;
	bool operator != (const std::chrono::microseconds& ts) const;
	bool operator >  (const std::chrono::microseconds& ts) const;
	bool operator >= (const std::chrono::microseconds& ts) const;
	bool operator <  (const std::chrono::microseconds& ts) const;
	bool operator <= (const std::chrono::microseconds& ts) const;

	Timespan operator + (const std::chrono::microseconds& d) const;
	Timespan operator - (const std::chrono::microseconds& d) const;
	Timespan& operator += (const std::chrono::microseconds& d);
	Timespan& operator -= (const std::chrono::microseconds& d);

    /**
     * <summary>    Returns the number of days. </summary>
     */
    int days() const;

	/**
	* <summary>    Returns the total number of days. </summary>
	*/
	double totalDays() const;

    /**
     * <summary>    Returns the number of hours (0 to 23). </summary>
     */
    int hours() const;

	/**
	* <summary>    Returns the total number of hours. </summary>
	*/
	double totalHours() const;

    /**
     * <summary>    Returns the number of minutes (0 to 59). </summary>
     */
    int minutes() const;

    /**
     * <summary>    Returns the total number of minutes. </summary>
     */
    double totalMinutes() const;

    /**
     * <summary>    Returns the total number of seconds. </summary>
     */
    double totalSeconds() const;
	
	/**
	* <summary>    Returns the number of seconds (0 to 59). </summary>
	*/
	long long seconds() const;

    /**
     * <summary>    Returns the number of milliseconds (0 to 999). </summary>
     */
	long long milliseconds() const;

	/**
	* <summary>    Returns the total number of milliseconds. </summary>
	*/
	double totalMilliseconds() const;

    /**
     * <summary>    Returns the fractions of a millisecond in microseconds (0 to 999). </summary>
     */
	long long microseconds() const;

	/**
	* <summary>    Returns the total number of milliseconds. </summary>
	*/
	double totalMicroseconds() const;

	std::chrono::microseconds duration() const { return _span; }

	//explicit operator std::chrono::duration<long long, std::micro>() const { return _span; }
	//operator const std::chrono::duration<long long, std::micro>&() { return _span; }

private:
	std::chrono::microseconds _span;
};

inline int Timespan::hours() const {
	return std::chrono::duration_cast<std::chrono::hours>(_span).count() % 24;
}

inline double Timespan::totalHours() const {
	using fhours = std::chrono::duration<double, std::chrono::hours::period>;
	return std::chrono::duration_cast<fhours>(_span).count();
}

inline int Timespan::minutes() const {
	return std::chrono::duration_cast<std::chrono::minutes>(_span).count() % 60;
}

inline double Timespan::totalMinutes() const {
	using fminutes = std::chrono::duration<double, std::chrono::minutes::period>;
	return std::chrono::duration_cast<fminutes>(_span).count();
}

inline long long Timespan::seconds() const {
	return std::chrono::duration_cast<std::chrono::seconds>(_span).count() % 60;
}

inline double Timespan::totalSeconds() const {
	using fseconds = std::chrono::duration<double, std::chrono::seconds::period>;
	return std::chrono::duration_cast<fseconds>(_span).count();
}

inline long long Timespan::milliseconds() const {
	return std::chrono::duration_cast<std::chrono::milliseconds>(_span).count() % 1000;
}

inline double Timespan::totalMilliseconds() const {
	using fmilliseconds = std::chrono::duration<double, std::chrono::milliseconds::period>;
	return std::chrono::duration_cast<fmilliseconds>(_span).count();
}

inline long long Timespan::microseconds() const {
	return _span.count() % 1000000;
}

inline double Timespan::totalMicroseconds() const {
	using fmicroseconds = std::chrono::duration<double, std::chrono::microseconds::period>;
	return std::chrono::duration_cast<fmicroseconds>(_span).count();
}

inline bool Timespan::operator == (const Timespan& ts) const {
    return _span == ts._span;
}

inline bool Timespan::operator != (const Timespan& ts) const {
    return _span != ts._span;
}

inline bool Timespan::operator >  (const Timespan& ts) const {
    return _span > ts._span;
}

inline bool Timespan::operator >= (const Timespan& ts) const {
    return _span >= ts._span;
}

inline bool Timespan::operator <  (const Timespan& ts) const {
    return _span < ts._span;
}

inline bool Timespan::operator <= (const Timespan& ts) const {
    return _span <= ts._span;
}

inline bool Timespan::operator == (const std::chrono::microseconds& ts) const {
	return _span == ts;
}

inline bool Timespan::operator != (const std::chrono::microseconds& ts) const {
	return _span != ts;
}

inline bool Timespan::operator >  (const std::chrono::microseconds& ts) const {
	return _span > ts;
}

inline bool Timespan::operator >= (const std::chrono::microseconds& ts) const {
	return _span >= ts;
}

inline bool Timespan::operator <  (const std::chrono::microseconds& ts) const {
	return _span < ts;
}

inline bool Timespan::operator <= (const std::chrono::microseconds& ts) const {
	return _span <= ts;
}

inline void swap(Timespan& s1, Timespan& s2) MOJA_NOEXCEPT {
    s1.swap(s2);
}

} // namespace moja

#endif // MOJA_CORE_TIMESPAN_H_
