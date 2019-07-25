#include <moja/timespan.h>

#include <boost/test/unit_test.hpp>

#include <chrono>
#include <cmath>

using namespace std::chrono;

BOOST_AUTO_TEST_SUITE(TimespanTests);

BOOST_AUTO_TEST_CASE(core_timespan_DefaultConstructor) {
   moja::Timespan ts;

   BOOST_CHECK_EQUAL(ts.days(), 0);
   BOOST_CHECK_EQUAL(ts.hours(), 0);
   BOOST_CHECK_EQUAL(ts.minutes(), 0);
   BOOST_CHECK_EQUAL(ts.seconds(), 0);
   BOOST_CHECK_EQUAL(ts.milliseconds(), 0);
   BOOST_CHECK_EQUAL(ts.microseconds(), 0);
}

BOOST_AUTO_TEST_CASE(core_timespan_Constructor_Microseconds) {
   moja::Timespan ts(microseconds{30000000});

   BOOST_CHECK_EQUAL(ts.days(), 0);
   BOOST_CHECK_EQUAL(ts.hours(), 0);
   BOOST_CHECK_EQUAL(ts.minutes(), 0);
   BOOST_CHECK_EQUAL(ts.seconds(), 30);
   BOOST_CHECK_EQUAL(ts.milliseconds(), 0);
   BOOST_CHECK_EQUAL(ts.microseconds(), 0);
}

BOOST_AUTO_TEST_CASE(core_timespan_Constructor_SecondsAndMicroseconds) {
   moja::Timespan ts(50, 100);

   BOOST_CHECK_EQUAL(ts.days(), 0);
   BOOST_CHECK_EQUAL(ts.hours(), 0);
   BOOST_CHECK_EQUAL(ts.minutes(), 0);
   BOOST_CHECK_EQUAL(ts.seconds(), 50);
   BOOST_CHECK_EQUAL(ts.milliseconds(), 0);
   BOOST_CHECK_EQUAL(ts.microseconds(), 100);
}

BOOST_AUTO_TEST_CASE(core_timespan_Constructor_AllComponents) {
   moja::Timespan ts(5, 4, 3, 2, 1);

   BOOST_CHECK_EQUAL(ts.days(), 5);
   BOOST_CHECK_EQUAL(ts.hours(), 4);
   BOOST_CHECK_EQUAL(ts.minutes(), 3);
   BOOST_CHECK_EQUAL(ts.seconds(), 2);
   BOOST_CHECK_EQUAL(ts.milliseconds(), 0);
   BOOST_CHECK_EQUAL(ts.microseconds(), 1);
}

BOOST_AUTO_TEST_CASE(core_timespan_Constructor_Timespan) {
   moja::Timespan first(5, 4, 3, 2, 1);
   moja::Timespan second(first);

   BOOST_CHECK_EQUAL(second.days(), first.days());
   BOOST_CHECK_EQUAL(second.hours(), first.hours());
   BOOST_CHECK_EQUAL(second.minutes(), first.minutes());
   BOOST_CHECK_EQUAL(second.seconds(), first.seconds());
   BOOST_CHECK_EQUAL(second.milliseconds(), first.milliseconds());
   BOOST_CHECK_EQUAL(second.microseconds(), first.microseconds());
}

BOOST_AUTO_TEST_CASE(core_timespan_AssignmentOperator) {
   moja::Timespan first(5, 4, 3, 2, 1);
   moja::Timespan second;
   second = first;

   BOOST_CHECK_EQUAL(second.days(), first.days());
   BOOST_CHECK_EQUAL(second.hours(), first.hours());
   BOOST_CHECK_EQUAL(second.minutes(), first.minutes());
   BOOST_CHECK_EQUAL(second.seconds(), first.seconds());
   BOOST_CHECK_EQUAL(second.milliseconds(), first.milliseconds());
   BOOST_CHECK_EQUAL(second.microseconds(), first.microseconds());
}

BOOST_AUTO_TEST_CASE(core_timespan_Assign_AllComponents) {
   moja::Timespan ts(50, 40, 30, 20, 10);
   ts.assign(5, 4, 3, 2, 1);

   BOOST_CHECK_EQUAL(ts.days(), 5);
   BOOST_CHECK_EQUAL(ts.hours(), 4);
   BOOST_CHECK_EQUAL(ts.minutes(), 3);
   BOOST_CHECK_EQUAL(ts.seconds(), 2);
   BOOST_CHECK_EQUAL(ts.milliseconds(), 0);
   BOOST_CHECK_EQUAL(ts.microseconds(), 1);
}

BOOST_AUTO_TEST_CASE(core_timespan_Assign_SecondsAndMicroseconds) {
   moja::Timespan ts(50, 40, 30, 20, 10);
   ts.assign(2, 1);

   BOOST_CHECK_EQUAL(ts.days(), 0);
   BOOST_CHECK_EQUAL(ts.hours(), 0);
   BOOST_CHECK_EQUAL(ts.minutes(), 0);
   BOOST_CHECK_EQUAL(ts.seconds(), 2);
   BOOST_CHECK_EQUAL(ts.milliseconds(), 0);
   BOOST_CHECK_EQUAL(ts.microseconds(), 1);
}

BOOST_AUTO_TEST_CASE(core_timespan_Swap) {
   moja::Timespan first(5, 4, 3, 2, 1);
   moja::Timespan second;

   auto firstDays = first.days();
   auto firstHours = first.hours();
   auto firstMinutes = first.minutes();
   auto firstSeconds = first.seconds();
   auto firstMilliseconds = first.milliseconds();
   auto firstMicroseconds = first.microseconds();

   auto secondDays = second.days();
   auto secondHours = second.hours();
   auto secondMinutes = second.minutes();
   auto secondSeconds = second.seconds();
   auto secondMilliseconds = second.milliseconds();
   auto secondMicroseconds = second.microseconds();

   second.swap(first);

   BOOST_CHECK_EQUAL(first.days(), secondDays);
   BOOST_CHECK_EQUAL(first.hours(), secondHours);
   BOOST_CHECK_EQUAL(first.minutes(), secondMinutes);
   BOOST_CHECK_EQUAL(first.seconds(), secondSeconds);
   BOOST_CHECK_EQUAL(first.milliseconds(), secondMilliseconds);
   BOOST_CHECK_EQUAL(first.microseconds(), secondMicroseconds);

   BOOST_CHECK_EQUAL(second.days(), firstDays);
   BOOST_CHECK_EQUAL(second.hours(), firstHours);
   BOOST_CHECK_EQUAL(second.minutes(), firstMinutes);
   BOOST_CHECK_EQUAL(second.seconds(), firstSeconds);
   BOOST_CHECK_EQUAL(second.milliseconds(), firstMilliseconds);
   BOOST_CHECK_EQUAL(second.microseconds(), firstMicroseconds);
}

BOOST_AUTO_TEST_CASE(core_timespan_EqualityOperator_Equal) {
   moja::Timespan first(5, 4, 3, 2, 1);
   moja::Timespan second(5, 4, 3, 2, 1);

   BOOST_CHECK(first == second);
}

BOOST_AUTO_TEST_CASE(core_timespan_EqualityOperator_NotEqual) {
   moja::Timespan first(microseconds{1});
   moja::Timespan second;

   BOOST_CHECK(!(first == second));
}

BOOST_AUTO_TEST_CASE(core_timespan_InequalityOperator_NotEqual) {
   moja::Timespan first(microseconds{1});
   moja::Timespan second;

   BOOST_CHECK(first != second);
}

BOOST_AUTO_TEST_CASE(core_timespan_InequalityOperator_Equal) {
   moja::Timespan first;
   moja::Timespan second;

   BOOST_CHECK(!(first != second));
}

BOOST_AUTO_TEST_CASE(core_timespan_GreaterThan_Greater) {
   moja::Timespan first(microseconds{100});
   moja::Timespan second(microseconds{1});

   BOOST_CHECK(first > second);
}

BOOST_AUTO_TEST_CASE(core_timespan_GreaterThan_NotGreater) {
   moja::Timespan first(microseconds{1});
   moja::Timespan second(microseconds{100});

   BOOST_CHECK(!(first > second));
}

BOOST_AUTO_TEST_CASE(core_timespan_GreaterThanOrEqualTo_Greater) {
   moja::Timespan first(microseconds{100});
   moja::Timespan second(microseconds{1});

   BOOST_CHECK(first >= second);
}

BOOST_AUTO_TEST_CASE(core_timespan_GreaterThanOrEqualTo_Equal) {
   moja::Timespan first(microseconds{1});
   moja::Timespan second(microseconds{1});

   BOOST_CHECK(first >= second);
}

BOOST_AUTO_TEST_CASE(core_timespan_GreaterThanOrEqualTo_NotGreaterOrEqual) {
   moja::Timespan first(microseconds{1});
   moja::Timespan second(microseconds{100});

   BOOST_CHECK(!(first >= second));
}

BOOST_AUTO_TEST_CASE(core_timespan_LessThan_Less) {
   moja::Timespan first(microseconds{1});
   moja::Timespan second(microseconds{100});

   BOOST_CHECK(first < second);
}

BOOST_AUTO_TEST_CASE(core_timespan_LessThan_NotLess) {
   moja::Timespan first(microseconds{100});
   moja::Timespan second(microseconds{1});

   BOOST_CHECK(!(first < second));
}

BOOST_AUTO_TEST_CASE(core_timespan_LessThanOrEqualTo_Less) {
   moja::Timespan first(microseconds{1});
   moja::Timespan second(microseconds{100});

   BOOST_CHECK(first <= second);
}

BOOST_AUTO_TEST_CASE(core_timespan_LessThanOrEqualTo_Equal) {
   moja::Timespan first(microseconds{1});
   moja::Timespan second(microseconds{1});

   BOOST_CHECK(first <= second);
}

BOOST_AUTO_TEST_CASE(core_timespan_LessThanOrEqualTo_NotLessOrEqual) {
   moja::Timespan first(microseconds{100});
   moja::Timespan second(microseconds{1});

   BOOST_CHECK(!(first <= second));
}

BOOST_AUTO_TEST_CASE(core_timespan_EqualityOperator_Microseconds_Equal) {
   moja::Timespan ts(microseconds{1000});
   microseconds usec(1000);

   BOOST_CHECK(ts.duration() == usec);
}

BOOST_AUTO_TEST_CASE(core_timespan_EqualityOperator_Microseconds_NotEqual) {
   moja::Timespan ts(microseconds{1000});
   microseconds usec(1);

   BOOST_CHECK(!(ts.duration() == usec));
}

BOOST_AUTO_TEST_CASE(core_timespan_InequalityOperator_Microseconds_NotEqual) {
   moja::Timespan ts(microseconds{1000});
   microseconds usec(1);

   BOOST_CHECK(ts.duration() != usec);
}

BOOST_AUTO_TEST_CASE(core_timespan_InequalityOperator_Microseconds_Equal) {
   moja::Timespan ts(microseconds{1000});
   microseconds usec(1000);

   BOOST_CHECK(!(ts.duration() != usec));
}

BOOST_AUTO_TEST_CASE(core_timespan_GreaterThan_Microseconds_Greater) {
   moja::Timespan ts(microseconds{1000});
   microseconds usec(1);

   BOOST_CHECK(ts.duration() > usec);
}

BOOST_AUTO_TEST_CASE(core_timespan_GreaterThan_Microseconds_NotGreater) {
   moja::Timespan ts(microseconds{1});
   microseconds usec(1000);

   BOOST_CHECK(!(ts.duration() > usec));
}

BOOST_AUTO_TEST_CASE(core_timespan_GreaterThanOrEqualTo_Microseconds_Greater) {
   moja::Timespan ts(microseconds{1000});
   microseconds usec(1);

   BOOST_CHECK(ts.duration() >= usec);
}

BOOST_AUTO_TEST_CASE(core_timespan_GreaterThanOrEqualTo_Microseconds_Equal) {
   moja::Timespan ts(microseconds{1});
   microseconds usec(1);

   BOOST_CHECK(ts.duration() >= usec);
}

BOOST_AUTO_TEST_CASE(core_timespan_GreaterThanOrEqualTo_Microseconds_NotGreaterOrEqual) {
   moja::Timespan ts(microseconds{1});
   microseconds usec(1000);

   BOOST_CHECK(!(ts.duration() >= usec));
}

BOOST_AUTO_TEST_CASE(core_timespan_LessThan_Microseconds_Less) {
   moja::Timespan ts(microseconds{1});
   microseconds usec(1000);

   BOOST_CHECK(ts.duration() < usec);
}

BOOST_AUTO_TEST_CASE(core_timespan_LessThan_Microseconds_NotLess) {
   moja::Timespan ts(microseconds{1000});
   microseconds usec(1);

   BOOST_CHECK(!(ts.duration() < usec));
}

BOOST_AUTO_TEST_CASE(core_timespan_LessThanOrEqualTo_Microseconds_Less) {
   moja::Timespan ts(microseconds{1});
   microseconds usec(1000);

   BOOST_CHECK(ts.duration() <= usec);
}

BOOST_AUTO_TEST_CASE(core_timespan_LessThanOrEqualTo_Microseconds_Equal) {
   moja::Timespan ts(microseconds{1});
   microseconds usec(1);

   BOOST_CHECK(ts.duration() <= usec);
}

BOOST_AUTO_TEST_CASE(core_timespan_LessThanOrEqualTo_Microseconds_NotLessOrEqual) {
   moja::Timespan ts(microseconds{1000});
   microseconds usec(1);

   BOOST_CHECK(!(ts.duration() <= usec));
}

BOOST_AUTO_TEST_CASE(core_timespan_Addition) {
   moja::Timespan first(1, 1, 1, 1, 1);
   moja::Timespan second(1, 1, 1, 1, 1);

   auto result = first + second;

   BOOST_CHECK_EQUAL(result.days(), first.days() + second.days());
   BOOST_CHECK_EQUAL(result.hours(), first.hours() + second.hours());
   BOOST_CHECK_EQUAL(result.minutes(), first.minutes() + second.minutes());
   BOOST_CHECK_EQUAL(result.seconds(), first.seconds() + second.seconds());
   BOOST_CHECK_EQUAL(result.milliseconds(), first.milliseconds() + second.milliseconds());
   BOOST_CHECK_EQUAL(result.microseconds(), first.microseconds() + second.microseconds());
}

BOOST_AUTO_TEST_CASE(core_timespan_Subtraction) {
   moja::Timespan first(1, 1, 1, 1, 1);
   moja::Timespan second(1, 1, 1, 1, 1);

   auto result = first - second;

   BOOST_CHECK_EQUAL(result.days(), first.days() - second.days());
   BOOST_CHECK_EQUAL(result.hours(), first.hours() - second.hours());
   BOOST_CHECK_EQUAL(result.minutes(), first.minutes() - second.minutes());
   BOOST_CHECK_EQUAL(result.seconds(), first.seconds() - second.seconds());
   BOOST_CHECK_EQUAL(result.milliseconds(), first.milliseconds() - second.milliseconds());
   BOOST_CHECK_EQUAL(result.microseconds(), first.microseconds() - second.microseconds());
}

BOOST_AUTO_TEST_CASE(core_timespan_Addition_InPlace) {
   moja::Timespan first(1, 1, 1, 1, 1);
   moja::Timespan second(1, 1, 1, 1, 1);

   first += second;

   auto first_hours = first.hours();
   auto second_hours = second.hours();

   BOOST_CHECK_EQUAL(first.days(), 2);
   BOOST_CHECK_EQUAL(first.hours(), 2);
   BOOST_CHECK_EQUAL(first.minutes(), 2);
   BOOST_CHECK_EQUAL(first.seconds(), 2);
   BOOST_CHECK_EQUAL(first.milliseconds(), 0);
   BOOST_CHECK_EQUAL(first.microseconds(), 2);
}

BOOST_AUTO_TEST_CASE(core_timespan_Subtraction_InPlace) {
   moja::Timespan first(1, 1, 1, 1, 1);
   moja::Timespan second(1, 1, 1, 1, 1);

   first -= second;

   BOOST_CHECK_EQUAL(first.days(), 0);
   BOOST_CHECK_EQUAL(first.hours(), 0);
   BOOST_CHECK_EQUAL(first.minutes(), 0);
   BOOST_CHECK_EQUAL(first.seconds(), 0);
   BOOST_CHECK_EQUAL(first.milliseconds(), 0);
   BOOST_CHECK_EQUAL(first.microseconds(), 0);
}

BOOST_AUTO_TEST_CASE(core_timespan_Addition_Microseconds) {
   moja::Timespan ts(microseconds{0});
   microseconds usec(1);

   auto result = ts + usec;

   BOOST_CHECK_EQUAL(result.duration().count(), usec.count());
}

BOOST_AUTO_TEST_CASE(core_timespan_Subtraction_Microseconds) {
   moja::Timespan ts(microseconds{10});
   microseconds usec(1);

   auto result = ts - usec;

   BOOST_CHECK_EQUAL(result.microseconds(), 9);
}

BOOST_AUTO_TEST_CASE(core_timespan_Addition_Microseconds_InPlace) {
   moja::Timespan ts(microseconds{0});
   microseconds usec(1);

   ts += usec;

   BOOST_CHECK_EQUAL(ts.microseconds(), usec.count());
}

BOOST_AUTO_TEST_CASE(core_timespan_Subtraction_Microseconds_InPlace) {
   moja::Timespan ts(microseconds{10});
   microseconds usec(1);

   ts -= usec;

   BOOST_CHECK_EQUAL(ts.microseconds(), 9);
}

BOOST_AUTO_TEST_CASE(core_timespan_TotalHours) {
   moja::Timespan ts(1, 1, 0);

   BOOST_CHECK_EQUAL(ts.totalHours(), ts.days() * 24 + ts.hours());
}

BOOST_AUTO_TEST_CASE(core_timespan_TotalMinutes) {
   moja::Timespan ts(1, 1, 1);
   BOOST_CHECK(std::abs(ts.totalMinutes() - (ts.days() * 24 * 60 + ts.hours() * 60 + ts.minutes())) < 0.000000001);
}

BOOST_AUTO_TEST_CASE(core_timespan_TotalSeconds) {
   moja::Timespan ts(1, 1, 1, 1, 0);

   BOOST_CHECK(std::abs(ts.totalSeconds() - (ts.days() * 24 * 60 * 60 + ts.hours() * 60 * 60 + ts.minutes() * 60 +
                                             ts.seconds())) < 0.000000001);
}

BOOST_AUTO_TEST_CASE(core_timespan_TotalMilliseconds) {
   moja::Timespan ts(0, 0, 1, 1, 0);

   BOOST_CHECK(std::abs(ts.totalMilliseconds() - (ts.days() * 24 * 60 * 60 * 1000 + ts.hours() * 60 * 60 * 1000 +
                                                  ts.minutes() * 60 * 1000 + ts.seconds() * 1000 + ts.milliseconds())) <
               0.000000001);
}

BOOST_AUTO_TEST_CASE(core_timespan_totalMicroseconds) {
   moja::Timespan ts(0, 0, 0, 1, 1);

   BOOST_CHECK(
       std::abs(ts.totalMicroseconds() - (ts.days() * 24 * 60 * 60 * 1000 * 1000 + ts.hours() * 60 * 60 * 1000 * 1000 +
                                          ts.minutes() * 60 * 1000 * 1000 + ts.seconds() * 1000 * 1000 +
                                          ts.milliseconds() * 1000 + ts.microseconds())) < 0.000000001);
}

BOOST_AUTO_TEST_SUITE_END();