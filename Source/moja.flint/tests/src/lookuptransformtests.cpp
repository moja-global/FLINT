#include <moja/flint/flintexceptions.h>
#include <moja/flint/lookuptransform.h>

#include <moja/test/mockdatarepository.h>
#include <moja/test/mocklandunitcontroller.h>
#include <moja/test/mockvariable.h>

#include <moja/dynamic.h>

#include <boost/test/unit_test.hpp>

#include <turtle/mock.hpp>

namespace flint {

namespace mf = moja::flint;
using moja::DynamicObject;
using moja::DynamicVar;
using moja::test::MockDataRepository;
using moja::test::MockLandUnitController;
using moja::test::MockVariable;

struct LookupTransformTestsFixture {
   mf::LookupTransform transform;
   MockDataRepository mockRepository;
   MockLandUnitController mockController;
   std::unique_ptr<MockVariable> varFrom = std::make_unique<MockVariable>();
   std::unique_ptr<MockVariable> varTo = std::make_unique<MockVariable>();
   DynamicObject mockConfiguration;

   LookupTransformTestsFixture() {
      mockConfiguration = DynamicObject({{"from", "var1"}, {"to", "var2"}});
      MOCK_EXPECT(mockController.getVariable).with("var1").returns(varFrom.get());
      MOCK_EXPECT(mockController.getVariable).with("var2").returns(varTo.get());
      MOCK_EXPECT(mockController.getVariable).returns(nullptr);
   }
};

BOOST_FIXTURE_TEST_SUITE(LookupTransformTests, LookupTransformTestsFixture)

BOOST_AUTO_TEST_CASE(LookupTransform_ConfigureThrowsExceptionIfConfigurationIsIncomplete) {
   auto badConfigurations = {DynamicObject(),
                             DynamicObject({{"from", "test"}, {"a", "b"}}),
                             DynamicObject({{"to", "test"}, {"a", "b"}}),
                             DynamicObject({{"from", ""}, {"to", "test"}}),
                             DynamicObject({{"from", "  "}, {"to", "test"}}),
                             DynamicObject({{"from", "test"}, {"to", ""}}),
                             DynamicObject({{"from", "test"}, {"to", "  "}})};

   for (auto configuration : badConfigurations) {
      BOOST_CHECK_THROW(transform.configure(configuration, mockController, mockRepository),
                        mf::IncompleteConfigurationException);
   }
}

BOOST_AUTO_TEST_CASE(LookupTransform_ValueThrowsExceptionIfVariableNotFound) {
   auto badConfigurations = {
       DynamicObject({{"from", "var1"}, {"to", "some_missing_var"}}),
       DynamicObject({{"from", "some_missing_var"}, {"to", "var2"}}),
   };

   for (auto configuration : badConfigurations) {
      transform.configure(configuration, mockController, mockRepository);
      BOOST_CHECK_THROW(transform.value(), mf::VariableNotFoundException);
   }
}

BOOST_AUTO_TEST_CASE(LookupTransform_ValueReturnsEmptyIfNoMatchFound) {
   auto varFromValue = std::make_unique<DynamicVar>(1);
   auto varToValue = std::make_unique<DynamicVar>(DynamicObject({{"2", 0}}));
   MOCK_EXPECT(varFrom->value).returns(*varFromValue);
   MOCK_EXPECT(varTo->value).returns(*varToValue);
   transform.configure(mockConfiguration, mockController, mockRepository);
   auto result = transform.value();
   BOOST_CHECK(result.isEmpty());
}

// A simple 1:1 single-column lookup should return a keyless Dynamic (i.e. not
// a DynamicObject).
BOOST_AUTO_TEST_CASE(LookupTransform_ValueReturnsExpectedResultForDynamicToStruct) {
   std::string expectedKey = "b";
   int expectedValue = 2;
   auto varFromValue = std::make_unique<DynamicVar>(expectedKey);
   auto varToValue = std::make_unique<DynamicVar>(DynamicObject({{"a", 1}, {expectedKey, expectedValue}, {"c", 3}}));

   MOCK_EXPECT(varFrom->value).returns(*varFromValue);
   MOCK_EXPECT(varTo->value).returns(*varToValue);

   transform.configure(mockConfiguration, mockController, mockRepository);
   auto& wrappedResult = transform.value();
   int result = wrappedResult;
   BOOST_CHECK_EQUAL(result, expectedValue);
}

// A single-column lookup with multiple resulting value columns should return a
// DynamicObject.
BOOST_AUTO_TEST_CASE(LookupTransform_ValueReturnsExpectedResultForDynamicToMultiStruct) {
   auto varFromValue = std::make_unique<DynamicVar>("testvalue2");
   MOCK_EXPECT(varFrom->value).returns(*varFromValue);

   auto varToValue = std::make_unique<DynamicVar>(std::vector<DynamicVar>{
       DynamicObject({{"var1", "testvalue1"}, {"column1", "c1v1"}, {"column2", "c2v1"}}),
       DynamicObject({{"var1", "testvalue2"}, {"column1", "c1v2"}, {"column2", "c2v2"}})  // <--
   });
   MOCK_EXPECT(varTo->value).returns(*varToValue);

   transform.configure(mockConfiguration, mockController, mockRepository);
   auto& result = transform.value().extract<DynamicObject>();

   std::map<std::string, std::string> expectedResult{{"column1", "c1v2"}, {"column2", "c2v2"}};

   for (auto column : result.members()) {
      BOOST_CHECK(expectedResult.find(column) != expectedResult.end());
   }

   for (auto item : expectedResult) {
      BOOST_CHECK_EQUAL(item.second, result[item.first].convert<std::string>());
   }
}

// A single-column lookup with multiple resulting value columns should return a
// DynamicObject.
BOOST_AUTO_TEST_CASE(LookupTransform_ValueReturnsExpectedResultForStructToMultiStruct) {
   auto varFromValue = std::make_unique<DynamicVar>(DynamicObject({{"key1", "k1v1"}, {"key2", "k2v2"}}));
   MOCK_EXPECT(varFrom->value).returns(*varFromValue);

   auto varToValue = std::make_unique<DynamicVar>(std::vector<DynamicVar>{
       DynamicObject({{"key1", "k1v1"}, {"key2", "k2v1"}, {"value1", 1}, {"value2", "a"}}),
       DynamicObject({{"key1", "k1v1"}, {"key2", "k2v2"}, {"value1", 2}, {"value2", "b"}}),  // <--
       DynamicObject({{"key1", "k1v2"}, {"key2", "k2v1"}, {"value1", 3}, {"value2", "c"}}),
       DynamicObject({{"key1", "k1v2"}, {"key2", "k2v2"}, {"value1", 4}, {"value2", "d"}})});
   MOCK_EXPECT(varTo->value).returns(*varToValue);

   transform.configure(mockConfiguration, mockController, mockRepository);
   DynamicObject result = transform.value().extract<DynamicObject>();

   std::map<std::string, DynamicVar> expectedResult{{"value1", 2}, {"value2", "b"}};

   for (auto column : result.members()) {
      BOOST_CHECK(expectedResult.find(column) != expectedResult.end());
   }

   for (auto item : expectedResult) {
      BOOST_CHECK(item.second == result[item.first]);
   }
}

// A multi-column lookup with only a single resulting value column should
// return a keyless DynamicVar (i.e. not in DynamicObject form).
BOOST_AUTO_TEST_CASE(LookupTransform_ValueReturnsExpectedResultForStructToMultiStructWithSingleValueColumn) {
   auto varFromValue = std::make_unique<DynamicVar>(DynamicObject({{"key1", "k1v2"}, {"key2", "k2v1"}}));
   MOCK_EXPECT(varFrom->value).returns(*varFromValue);

   auto varToValue = std::make_unique<DynamicVar>(
       std::vector<DynamicVar>{DynamicObject({{"key1", "k1v1"}, {"key2", "k2v1"}, {"value1", 1}}),
                               DynamicObject({{"key1", "k1v1"}, {"key2", "k2v2"}, {"value1", 2}}),
                               DynamicObject({{"key1", "k1v2"}, {"key2", "k2v1"}, {"value1", 3}}),  // <--
                               DynamicObject({{"key1", "k1v2"}, {"key2", "k2v2"}, {"value1", 4}})});
   MOCK_EXPECT(varTo->value).returns(*varToValue);

   transform.configure(mockConfiguration, mockController, mockRepository);
   int result = transform.value();
   BOOST_CHECK_EQUAL(result, 3);
}

// A single-column lookup with only a single resulting value column should
// return a keyless DynamicVar (i.e. not in DynamicObject form).
BOOST_AUTO_TEST_CASE(LookupTransform_ValueReturnsExpectedResultForDynamicToMultiStructWithSingleValueColumn) {
   auto varFromValue = std::make_unique<DynamicVar>("k1v4");
   MOCK_EXPECT(varFrom->value).returns(*varFromValue);

   auto varToValue = std::make_unique<DynamicVar>(std::vector<DynamicVar>{
       DynamicObject({{"var1", "k1v1"}, {"value1", "test1"}}), DynamicObject({{"var1", "k1v2"}, {"value1", "test2"}}),
       DynamicObject({{"var1", "k1v3"}, {"value1", "test3"}}),
       DynamicObject({{"var1", "k1v4"}, {"value1", "test4"}})  // <--
   });
   MOCK_EXPECT(varTo->value).returns(*varToValue);

   transform.configure(mockConfiguration, mockController, mockRepository);
   std::string result = transform.value().convert<std::string>();
   BOOST_CHECK_EQUAL(result, "test4");
}

// A struct can match against a "to" struct that only has a subset of the "from"
// struct's keys.
BOOST_AUTO_TEST_CASE(LookupTransform_ValueReturnsExpectedResultForStructToSubsetMultiStructWithSingleValueColumn) {
   auto varFromValue = std::make_unique<DynamicVar>(DynamicObject({{"key1", "k1v2"}, {"key2", "k2v3"}}));
   MOCK_EXPECT(varFrom->value).returns(*varFromValue);

   auto varToValue = std::make_unique<DynamicVar>(std::vector<DynamicVar>{
       DynamicObject({{"key2", "k2v1"}, {"value1", 1}}), DynamicObject({{"key2", "k2v2"}, {"value1", 2}}),
       DynamicObject({{"key2", "k2v3"}, {"value1", 3}}),  // <--
       DynamicObject({{"key2", "k2v4"}, {"value1", 4}})});
   MOCK_EXPECT(varTo->value).returns(*varToValue);

   transform.configure(mockConfiguration, mockController, mockRepository);
   int result = transform.value();
   BOOST_CHECK_EQUAL(result, 3);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace flint
