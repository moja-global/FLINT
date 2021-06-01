#include <moja/datetime.h>
#include <moja/dynamic.h>
#include <moja/types.h>

#include <boost/test/unit_test.hpp>

namespace flint_dynamicvar {

using moja::DynamicVar;

BOOST_AUTO_TEST_SUITE(DynamicTests);

BOOST_AUTO_TEST_CASE(DynamicVar_ObjectsAreEqualIfStringValuesAreEqual) {
   BOOST_CHECK(DynamicVar("test") == DynamicVar("test"));
}

BOOST_AUTO_TEST_CASE(DynamicVar_ObjectsAreNotEqualIfStringValuesAreNotEqual) {
   BOOST_CHECK(DynamicVar("test") != DynamicVar("other"));
   BOOST_CHECK(DynamicVar("test") != DynamicVar("Test"));
}

BOOST_AUTO_TEST_CASE(DynamicVar_ObjectsAreEqualIfIntValuesAreEqual) { BOOST_CHECK(DynamicVar(100) == DynamicVar(100)); }

BOOST_AUTO_TEST_CASE(DynamicVar_ObjectsAreNotEqualIfIntValuesAreNotEqual) {
   BOOST_CHECK(DynamicVar(10) != DynamicVar(57));
   BOOST_CHECK(DynamicVar(42) != DynamicVar(3));
   BOOST_CHECK(DynamicVar(-2) != DynamicVar(2));
}

BOOST_AUTO_TEST_CASE(DynamicVar_ObjectsAreEqualIfUIntValuesAreEqual) {
   BOOST_CHECK(DynamicVar(moja::UInt16(5)) == DynamicVar(moja::UInt16(5)));
}

BOOST_AUTO_TEST_CASE(DynamicVar_ObjectsAreNotEqualifUIntValuesAreNotEqual) {
   BOOST_CHECK(DynamicVar(moja::UInt16(1)) != DynamicVar(moja::UInt16(10)));
}

BOOST_AUTO_TEST_CASE(DynamicVar_ObjectsAreEqualIfUIntAndIntValuesAreEqual) {
   BOOST_CHECK(DynamicVar(moja::UInt16(1)) == DynamicVar(1));
}

BOOST_AUTO_TEST_CASE(DynamicVar_ObjectsAreNotEqualIfUIntAndIntValuesAreNotEqual) {
   BOOST_CHECK(DynamicVar(moja::UInt16(1)) != DynamicVar(-1));
}

BOOST_AUTO_TEST_CASE(DynamicVar_ObjectsAreEqualIfDoubleValuesAreEqual) {
   BOOST_CHECK(DynamicVar(1.1) == DynamicVar(1.1));
}

BOOST_AUTO_TEST_CASE(DynamicVar_ObjectsAreNotEqualIfDoubleValuesAreNotEqual) {
   BOOST_CHECK(DynamicVar(1.1) != DynamicVar(1.2));
}

BOOST_AUTO_TEST_CASE(DynamicVar_StoreAndUnpackString) {
   std::string value = "test";
   DynamicVar dynamic(value);
   BOOST_CHECK(dynamic.isString());

   auto extracted = dynamic.extract<const std::string>();
   BOOST_CHECK_EQUAL(value, extracted);

   auto converted = dynamic.convert<std::string>();
   BOOST_CHECK_EQUAL(value, converted);
}

BOOST_AUTO_TEST_CASE(DynamicVar_StoreAndUnpackInt) {
   int value = 42;
   DynamicVar dynamic(value);
   BOOST_CHECK(dynamic.isInteger());

   auto extracted = dynamic.extract<const int>();
   BOOST_CHECK_EQUAL(value, extracted);

   auto converted = dynamic.convert<int>();
   BOOST_CHECK_EQUAL(value, converted);
}

BOOST_AUTO_TEST_CASE(DynamicVar_StoreAndUnpackDouble) {
   double value = 42.42;
   DynamicVar dynamic(value);
   BOOST_CHECK(dynamic.isNumeric());

   auto extracted = dynamic.extract<const double>();
   BOOST_CHECK_EQUAL(value, extracted);

   auto converted = dynamic.convert<double>();
   BOOST_CHECK_EQUAL(value, converted);
}

BOOST_AUTO_TEST_CASE(DynamicVar_VectorOperations) {
   std::vector<DynamicVar> values{1.3, -5.4, 2.22};
   DynamicVar dynamic(values);
   BOOST_CHECK(dynamic.isVector());

   for (auto i = 0; i < dynamic.size(); i++) {
      double actual = dynamic[i];
      double expected = values[i];
      BOOST_CHECK_EQUAL(actual, expected);
   }
}

BOOST_AUTO_TEST_CASE(DynamicVar_AssignFromOther) {
   int value = 10;
   DynamicVar dynamic(value);
   DynamicVar other = dynamic;
   BOOST_CHECK(dynamic == other);
}

BOOST_AUTO_TEST_CASE(DynamicVar_DoubleToFloat) {
   float originalValue = 1.5;
   DynamicVar original(originalValue);
   double convertedValue = original;
   BOOST_CHECK_EQUAL(originalValue, convertedValue);
}

BOOST_AUTO_TEST_CASE(DynamicVar_FloatToDouble) {
   double originalValue = 1.5;
   DynamicVar original(originalValue);
   float convertedValue = original;
   BOOST_CHECK_EQUAL(originalValue, convertedValue);
}

BOOST_AUTO_TEST_CASE(DynamicVar_StoreAndUnpackDateTime) {
   moja::DateTime value;
   DynamicVar dynamic(value);

   auto extracted = dynamic.extract<moja::DateTime>();
   BOOST_CHECK_EQUAL(value, extracted);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace flint_dynamicvar