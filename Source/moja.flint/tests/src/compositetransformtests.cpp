#include <moja/flint/compositetransform.h>
#include <moja/flint/flintexceptions.h>

#include <moja/test/mockdatarepository.h>
#include <moja/test/mocklandunitcontroller.h>
#include <moja/test/mockvariable.h>

#include <moja/dynamic.h>

#include <boost/test/unit_test.hpp>

#include <turtle/mock.hpp>

namespace moja::flint {

using moja::DynamicObject;
using moja::DynamicVar;
using test::MockDataRepository;
using test::MockLandUnitController;
using test::MockVariable;

struct CompositeTransformTestsFixture {
   CompositeTransform transform;
   MockDataRepository mockRepository;
   MockLandUnitController mockController;
   std::unique_ptr<MockVariable> var1 = std::make_unique<MockVariable>();
   std::unique_ptr<MockVariable> var2 = std::make_unique<MockVariable>();
   DynamicObject mockConfiguration;

   CompositeTransformTestsFixture() {
      mockConfiguration.insert("vars", std::vector<DynamicVar>{"var1", "var2"});
      MOCK_EXPECT(mockController.getVariable).with("var1").returns(var1.get());
      MOCK_EXPECT(mockController.getVariable).with("var2").returns(var2.get());
      MOCK_EXPECT(mockController.getVariable).returns(nullptr);
      MOCK_EXPECT(var1->info).returns(VariableInfo{"var1"});
      MOCK_EXPECT(var2->info).returns(VariableInfo{"var2"});
   }
};

BOOST_FIXTURE_TEST_SUITE(CompositeTransformTests, CompositeTransformTestsFixture)

BOOST_AUTO_TEST_CASE(CompositeTransform_ConfigureThrowsExceptionIfConfigurationIsIncomplete) {
   // Requires an item called "vars" which is a list of at least one variable name.
   // { "a", "b" } is just a kludge for the DynamicObject constructor - ignore.
   auto badConfigurations = {
       DynamicObject(),
       DynamicObject({{"vars", std::vector<DynamicVar>()}, {"a", "b"}}),
   };

   for (auto configuration : badConfigurations) {
      BOOST_CHECK_THROW(transform.configure(configuration, mockController, mockRepository),
                        IncompleteConfigurationException);
   }
}

BOOST_AUTO_TEST_CASE(CompositeTransform_ConfigureThrowsExceptionOnDuplicateVariableNames) {
   DynamicObject config;
   config.insert("vars", std::vector<DynamicVar>{"duplicate", "duplicate"});
   BOOST_CHECK_THROW(transform.configure(config, mockController, mockRepository), IncompleteConfigurationException);
}

BOOST_AUTO_TEST_CASE(CompositeTransform_ValueThrowsExceptionIfVariableNotFound) {
   DynamicObject config;
   config.insert("vars", std::vector<DynamicVar>{"not found 1", "not found 2"});
   transform.configure(config, mockController, mockRepository);
   BOOST_CHECK_THROW(transform.value(), VariableNotFoundException);
}

BOOST_AUTO_TEST_CASE(CompositeTransform_ValueConsolidatesReferencedSingleValueVariables) {
   transform.configure(mockConfiguration, mockController, mockRepository);

   DynamicVar var1Value{"var1value"};
   DynamicVar var2Value{"var2value"};
   MOCK_EXPECT(var1->value).returns(var1Value);
   MOCK_EXPECT(var2->value).returns(var2Value);

   auto& result = transform.value().extract<DynamicObject>();
   BOOST_CHECK(result["var1"] == var1Value);
   BOOST_CHECK(result["var2"] == var2Value);
}

BOOST_AUTO_TEST_CASE(CompositeTransform_ValueConsolidatesReferencedMultiValueVariables) {
   transform.configure(mockConfiguration, mockController, mockRepository);

   DynamicVar var1Value = DynamicObject({{"a", 1}, {"b", 2}});
   DynamicVar var2Value = DynamicObject({{"c", 3}, {"d", 4}, {"e", 5}});
   MOCK_EXPECT(var1->value).returns(var1Value);
   MOCK_EXPECT(var2->value).returns(var2Value);

   auto& result = transform.value().extract<DynamicObject>();
   BOOST_CHECK(result["a"] == 1);
   BOOST_CHECK(result["b"] == 2);
   BOOST_CHECK(result["c"] == 3);
   BOOST_CHECK(result["d"] == 4);
   BOOST_CHECK(result["e"] == 5);
}

BOOST_AUTO_TEST_CASE(CompositeTransform_ValueConsolidatesReferencedSingleAndMultiValueVariables) {
   transform.configure(mockConfiguration, mockController, mockRepository);

   DynamicVar var1Value{1};
   DynamicVar var2Value = DynamicObject({{"b", 2}, {"c", 3}, {"d", 4}});
   MOCK_EXPECT(var1->value).returns(var1Value);
   MOCK_EXPECT(var2->value).returns(var2Value);

   auto& result = transform.value().extract<DynamicObject>();
   BOOST_CHECK(result["var1"] == 1);
   BOOST_CHECK(result["b"] == 2);
   BOOST_CHECK(result["c"] == 3);
   BOOST_CHECK(result["d"] == 4);
}

BOOST_AUTO_TEST_CASE(CompositeTransform_ValueConsolidatesReferencedListVariables) {
   // For variables coming from relational datasources - it is expected that
   // these variables only contain a single row. The row's key/value pairs are
   // extracted and flattened into the result.
   transform.configure(mockConfiguration, mockController, mockRepository);

   DynamicVar var1Value = std::vector<DynamicObject>{DynamicObject({{"a", 1}, {"b", 2}})};
   DynamicVar var2Value = std::vector<DynamicObject>{DynamicObject({{"c", 3}, {"d", 4}, {"e", 5}})};
   MOCK_EXPECT(var1->value).returns(var1Value);
   MOCK_EXPECT(var2->value).returns(var2Value);

   auto& result = transform.value().extract<DynamicObject>();
   BOOST_CHECK(result["a"] == 1);
   BOOST_CHECK(result["b"] == 2);
   BOOST_CHECK(result["c"] == 3);
   BOOST_CHECK(result["d"] == 4);
   BOOST_CHECK(result["e"] == 5);
}

BOOST_AUTO_TEST_CASE(CompositeTransform_ValueIsEmptyIfAnyReferencedVariableIsEmpty) {
   transform.configure(mockConfiguration, mockController, mockRepository);

   DynamicVar var1Value{"var1value"};
   DynamicVar var2Value;
   MOCK_EXPECT(var1->value).returns(var1Value);
   MOCK_EXPECT(var2->value).returns(var2Value);

   auto& result = transform.value();
   BOOST_CHECK(result.isEmpty());
}

BOOST_AUTO_TEST_CASE(CompositeTransform_AllowsNullsWhenConfigured) {
   mockConfiguration.insert("allow_nulls", true);
   transform.configure(mockConfiguration, mockController, mockRepository);

   DynamicVar var1Value{1};
   DynamicVar var2Value;
   MOCK_EXPECT(var1->value).returns(var1Value);
   MOCK_EXPECT(var2->value).returns(var2Value);

   auto& result = transform.value();
   BOOST_CHECK(result["var1"] == 1);
   BOOST_CHECK(result["var2"].isEmpty());
}

BOOST_AUTO_TEST_CASE(CompositeTransform_LongFormat) {
   // Long-format composite transform expects participating variables to have the
   // same attributes. Each participating variable provides a row of data, instead
   // of the default format where each variable provides one or more columns to
   // compose a single row of data.
   mockConfiguration.insert("format", "long");
   transform.configure(mockConfiguration, mockController, mockRepository);

   DynamicVar var1Value = std::vector<DynamicObject>{DynamicObject({{"a", 1}, {"b", 2}})};
   DynamicVar var2Value =
       std::vector<DynamicObject>{DynamicObject({{"a", 3}, {"b", 4}}), DynamicObject({{"a", 5}, {"b", 6}})};
   MOCK_EXPECT(var1->value).returns(var1Value);
   MOCK_EXPECT(var2->value).returns(var2Value);

   auto result = transform.value().extract<std::vector<DynamicObject>>();
   int expectedValue = 1;
   for (auto row : result) {
      for (auto col : {"a", "b"}) {
         int value = row[col];
         BOOST_CHECK_EQUAL(value, expectedValue++);
      }
   }
}

BOOST_AUTO_TEST_CASE(CompositeTransform_LongFormat_ValueIsEmptyIfNoRows) {
   mockConfiguration.insert("format", "long");
   transform.configure(mockConfiguration, mockController, mockRepository);

   DynamicVar var1Value;
   DynamicVar var2Value;
   MOCK_EXPECT(var1->value).returns(var1Value);
   MOCK_EXPECT(var2->value).returns(var2Value);

   auto& result = transform.value();
   BOOST_CHECK(result.isEmpty());
}

BOOST_AUTO_TEST_CASE(CompositeTransform_LongFormat_HasValueIfAtLeastOneRow) {
   mockConfiguration.insert("format", "long");
   transform.configure(mockConfiguration, mockController, mockRepository);

   DynamicVar var1Value;
   DynamicVar var2Value = std::vector<DynamicObject>{DynamicObject({{"a", 1}, {"b", 2}})};
   MOCK_EXPECT(var1->value).returns(var1Value);
   MOCK_EXPECT(var2->value).returns(var2Value);

   auto result = transform.value().extract<std::vector<DynamicObject>>();
   BOOST_CHECK_EQUAL(result.size(), 1);

   auto row = result[0];
   BOOST_CHECK(row["a"] == 1);
   BOOST_CHECK(row["b"] == 2);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace moja::flint
