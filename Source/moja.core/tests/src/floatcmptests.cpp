#include <moja/floatcmp.h>

#include <boost/test/unit_test.hpp>

namespace flint_floatcompare {

BOOST_AUTO_TEST_SUITE(FloatCompareTests);

BOOST_AUTO_TEST_CASE(FloatCmp_EqualToExact) {
   double a = 10.32;
   double b = a;

   BOOST_CHECK(moja::FloatCmp::equalTo(a, b));
}

BOOST_AUTO_TEST_CASE(FloatCmp_EqualToTolerance) {
   const double Epsilon = 0.000000000000001;

   double a = 6.572;
   double b = a + Epsilon;

   BOOST_CHECK_NE(a, b);
   BOOST_CHECK(moja::FloatCmp::equalTo(a, b));
}

BOOST_AUTO_TEST_CASE(FloatCmp_EqualToNotEqual) {
   double a = 3.1243;
   double b = 5.123;

   BOOST_CHECK(!moja::FloatCmp::equalTo(a, b));
}

BOOST_AUTO_TEST_CASE(FloatCmp_GreaterThanTrue) {
   double a = 5;
   double b = 1;

   BOOST_CHECK(moja::FloatCmp::greaterThan(a, b));
}

BOOST_AUTO_TEST_CASE(FloatCmp_GreaterThanFalse) {
   double a = 1;
   double b = 10;

   BOOST_CHECK(!moja::FloatCmp::greaterThan(a, b));
}

BOOST_AUTO_TEST_CASE(FloatCmp_GreaterThanNotEqualTo) {
   double a = 1;
   double b = a;

   BOOST_CHECK(!moja::FloatCmp::greaterThan(a, b));
}

BOOST_AUTO_TEST_CASE(FloatCmp_GreaterThanOrEqualToTrue) {
   double a = 5;
   double b = 1;

   BOOST_CHECK(moja::FloatCmp::greaterThanOrEqualTo(a, b));
}

BOOST_AUTO_TEST_CASE(FloatCmp_GreaterThanOrEqualToFalse) {
   double a = 1;
   double b = 10;

   BOOST_CHECK(!moja::FloatCmp::greaterThanOrEqualTo(a, b));
}

BOOST_AUTO_TEST_CASE(FloatCmp_GreaterThanOrEqualToEqualToExact) {
   double a = 1;
   double b = a;

   BOOST_CHECK(moja::FloatCmp::greaterThanOrEqualTo(a, b));
}

BOOST_AUTO_TEST_CASE(FloatCmp_GreaterThanOrEqualToEqualToTolerance) {
   const double Epsilon = 0.000000000000001;

   double a = 5.123;
   double b = a + Epsilon;

   BOOST_CHECK(moja::FloatCmp::greaterThanOrEqualTo(a, b));
}

BOOST_AUTO_TEST_CASE(FloatCmp_LessThanTrue) {
   double a = 1;
   double b = 5;

   BOOST_CHECK(moja::FloatCmp::lessThan(a, b));
}

BOOST_AUTO_TEST_CASE(FloatCmp_LessThanFalse) {
   double a = 10;
   double b = 1;

   BOOST_CHECK(!moja::FloatCmp::lessThan(a, b));
}

BOOST_AUTO_TEST_CASE(FloatCmp_LessThanNotEqualTo) {
   double a = 1;
   double b = a;

   BOOST_CHECK(!moja::FloatCmp::lessThan(a, b));
}

BOOST_AUTO_TEST_CASE(FloatCmp_LessThanOrEqualToTrue) {
   double a = 1;
   double b = 5;

   BOOST_CHECK(moja::FloatCmp::lessThanOrEqualTo(a, b));
}

BOOST_AUTO_TEST_CASE(FloatCmp_LessThanOrEqualToFalse) {
   double a = 10;
   double b = 1;

   BOOST_CHECK(!moja::FloatCmp::lessThanOrEqualTo(a, b));
}

BOOST_AUTO_TEST_CASE(FloatCmp_LessThanOrEqualToEqualToExact) {
   double a = 1;
   double b = a;

   BOOST_CHECK(moja::FloatCmp::lessThanOrEqualTo(a, b));
}

BOOST_AUTO_TEST_CASE(FloatCmp_LessThanOrEqualToEqualToTolerance) {
   const double Epsilon = 0.000000000000001;

   double a = 5.123;
   double b = a - Epsilon;

   BOOST_CHECK(moja::FloatCmp::lessThanOrEqualTo(a, b));
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace flint_floatcompare