#include "moja/floatcmp.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(FloatCmpTests);

BOOST_AUTO_TEST_CASE(core_floatcmp_EqualTo_Exact) {
   double a = 10.32;
   double b = a;

   BOOST_CHECK(moja::FloatCmp::equalTo(a, b));
}

BOOST_AUTO_TEST_CASE(core_floatcmp_EqualTo_Tolerance) {
   const double Epsilon = 0.000000000000001;

   double a = 6.572;
   double b = a + Epsilon;

   BOOST_CHECK_NE(a, b);
   BOOST_CHECK(moja::FloatCmp::equalTo(a, b));
}

BOOST_AUTO_TEST_CASE(core_floatcmp_EqualTo_NotEqual) {
   double a = 3.1243;
   double b = 5.123;

   BOOST_CHECK(!moja::FloatCmp::equalTo(a, b));
}

BOOST_AUTO_TEST_CASE(core_floatcmp_GreaterThan_True) {
   double a = 5;
   double b = 1;

   BOOST_CHECK(moja::FloatCmp::greaterThan(a, b));
}

BOOST_AUTO_TEST_CASE(core_floatcmp_GreaterThan_False) {
   double a = 1;
   double b = 10;

   BOOST_CHECK(!moja::FloatCmp::greaterThan(a, b));
}

BOOST_AUTO_TEST_CASE(core_floatcmp_GreaterThan_NotEqualTo) {
   double a = 1;
   double b = a;

   BOOST_CHECK(!moja::FloatCmp::greaterThan(a, b));
}

BOOST_AUTO_TEST_CASE(core_floatcmp_GreaterThanOrEqualTo_True) {
   double a = 5;
   double b = 1;

   BOOST_CHECK(moja::FloatCmp::greaterThanOrEqualTo(a, b));
}

BOOST_AUTO_TEST_CASE(core_floatcmp_GreaterThanOrEqualTo_False) {
   double a = 1;
   double b = 10;

   BOOST_CHECK(!moja::FloatCmp::greaterThanOrEqualTo(a, b));
}

BOOST_AUTO_TEST_CASE(core_floatcmp_GreaterThanOrEqualTo_EqualTo_Exact) {
   double a = 1;
   double b = a;

   BOOST_CHECK(moja::FloatCmp::greaterThanOrEqualTo(a, b));
}

BOOST_AUTO_TEST_CASE(core_floatcmp_GreaterThanOrEqualTo_EqualTo_Tolerance) {
   const double Epsilon = 0.000000000000001;

   double a = 5.123;
   double b = a + Epsilon;

   BOOST_CHECK(moja::FloatCmp::greaterThanOrEqualTo(a, b));
}

BOOST_AUTO_TEST_CASE(core_floatcmp_LessThan_True) {
   double a = 1;
   double b = 5;

   BOOST_CHECK(moja::FloatCmp::lessThan(a, b));
}

BOOST_AUTO_TEST_CASE(core_floatcmp_LessThan_False) {
   double a = 10;
   double b = 1;

   BOOST_CHECK(!moja::FloatCmp::lessThan(a, b));
}

BOOST_AUTO_TEST_CASE(core_floatcmp_LessThan_NotEqualTo) {
   double a = 1;
   double b = a;

   BOOST_CHECK(!moja::FloatCmp::lessThan(a, b));
}

BOOST_AUTO_TEST_CASE(core_floatcmp_LessThanOrEqualTo_True) {
   double a = 1;
   double b = 5;

   BOOST_CHECK(moja::FloatCmp::lessThanOrEqualTo(a, b));
}

BOOST_AUTO_TEST_CASE(core_floatcmp_LessThanOrEqualTo_False) {
   double a = 10;
   double b = 1;

   BOOST_CHECK(!moja::FloatCmp::lessThanOrEqualTo(a, b));
}

BOOST_AUTO_TEST_CASE(core_floatcmp_LessThanOrEqualTo_EqualTo_Exact) {
   double a = 1;
   double b = a;

   BOOST_CHECK(moja::FloatCmp::lessThanOrEqualTo(a, b));
}

BOOST_AUTO_TEST_CASE(core_floatcmp_LessThanOrEqualTo_EqualTo_Tolerance) {
   const double Epsilon = 0.000000000000001;

   double a = 5.123;
   double b = a - Epsilon;

   BOOST_CHECK(moja::FloatCmp::lessThanOrEqualTo(a, b));
}

BOOST_AUTO_TEST_SUITE_END();