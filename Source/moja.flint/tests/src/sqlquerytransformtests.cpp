#include <moja/flint/sqlquerytransform.h>

#include <moja/test/mockdatarepository.h>
#include <moja/test/mocklandunitcontroller.h>
#include <moja/test/mockoperationmanager.h>
#include <moja/test/mockpool.h>
#include <moja/test/mockpoolcollection.h>
#include <moja/test/mockproviderrelationalinterface.h>
#include <moja/test/mockvariable.h>

#include <moja/dynamic.h>

#include <boost/test/unit_test.hpp>

#include <turtle/mock.hpp>

#include <memory>

BOOST_AUTO_TEST_SUITE(SQLQueryTransformTests)

using moja::DynamicObject;
using moja::DynamicVar;
namespace mf = moja::flint;
namespace mocks = moja::test;

struct ItemVar {
   std::string name;
   DynamicVar val;
   std::string valStr;
};

#if 1

BOOST_AUTO_TEST_CASE(flint_SQLQueryTransform_IssuesExpectedQueriesUsingVarsAndPools) {
   auto mockLandUnitController = std::make_unique<mocks::MockLandUnitController>();
   auto mockOperationManager = std::make_unique<mocks::MockOperationManager>();
   mocks::MockPoolCollection mockPoolCollection;  // = std::make_shared<mocks::MockPoolCollection>();

   // Setup Mock Variables
   std::vector<ItemVar> testVariables = {
       {"LandUnitId", 247, "247"}, {"TestVar01", 99, "99"}, {"TestVar01_Dummy", 101010101, "101010101"},
       {"a", "test", "'test'"},    {"11", 1.1, "1.1"},      {"&^%", true, "true"},
   };

   std::vector<std::unique_ptr<mocks::MockVariable>> mockVariables;
   for (auto data : testVariables) {
      mockVariables.push_back(std::make_unique<mocks::MockVariable>());
      MOCK_EXPECT(mockLandUnitController->getVariable).with(data.name).returns(mockVariables.back().get());
      MOCK_EXPECT(mockVariables.back().get()->value).returns(data.val);
   }

   struct itemPool {
      std::string name;
      std::string description;
      std::string units;
      double scale;
      int order;
      double val;
      std::string valStr;
   };

   // Setup Mock Pools
   std::vector<itemPool> testPools = {{"Pool1", "mock pool", "n/a", 1.0, 0, 1.1111, "1.1111"},
                                      {"Pool222222", "mock pool", "n/a", 1.0, 1, 2.22222, "2.22222"},
                                      {"3", "mock pool", "n/a", 1.0, 2, 333.3333, "333.3333"}};

   std::map<std::string, std::unique_ptr<mocks::MockPool>> mockPools;
   for (auto& data : testPools) {
      std::string& name = data.name;
      mockPools[data.name] =
          std::make_unique<mocks::MockPool>(data.name, data.description, data.units, data.scale, data.order);
      MOCK_EXPECT(mockPools[data.name]->value).returns(data.val);
   }

   MOCK_EXPECT(mockLandUnitController->getOpMan).returns(mockOperationManager.get());
   MOCK_EXPECT(mockLandUnitController->getOpManConst).returns(mockOperationManager.get());

   MOCK_EXPECT(mockOperationManager->poolCollection).returns(mockPoolCollection);
   MOCK_EXPECT(mockOperationManager->poolCount).returns(static_cast<int>(testPools.size()));

   MOCK_EXPECT(mockPoolCollection.getPoolByName).calls([&mockPools](const std::string name) -> mocks::MockPool* {
      return mockPools[name].get();
   });

   MOCK_EXPECT(mockOperationManager->getPoolByName).calls([&mockPools](const std::string name) -> mocks::MockPool* {
      return mockPools[name].get();
   });

   // Setup Query and Expected results
   std::vector<std::pair<std::string, std::string>> testQueryAndExpected = {
       {
           "",
           "",
       },
       {"{var:}", "{var:}"},
       {"{pool:}", "{pool:}"},
       {
           "{var:" + testVariables[0].name + "} {var:" + testVariables[1].name + "} {var:" + testVariables[2].name +
               "} {var:" + testVariables[3].name + "} {var:" + testVariables[4].name +
               "} {var:" + testVariables[5].name + "}",
           testVariables[0].valStr + " " + testVariables[1].valStr + " " + testVariables[2].valStr + " " +
               testVariables[3].valStr + " " + testVariables[4].valStr + " " + testVariables[5].valStr,
       },
       {"{var:LandUnitId} {var:TestVar01} {var:TestVar01_Dummy} {var:a} {var:11} {var:&^%}",
        "247 99 101010101 'test' 1.1 true"},
       {"{pool:Pool1} {pool:Pool222222} {pool:3}", "1.1111 2.22222 333.3333"},
       {
           "{pool:" + testPools[0].name + "} {pool:" + testPools[1].name + "} {pool:" + testPools[2].name + "}",
           testPools[0].valStr + " " + testPools[1].valStr + " " + testPools[2].valStr,
       },
       {
           "{pool:" + testPools[2].name + "} {pool:" + testPools[1].name + "} {pool:" + testPools[0].name + "}",
           testPools[2].valStr + " " + testPools[1].valStr + " " + testPools[0].valStr,
       },
       {"{var:&^%} {var:11} {var:a} {var:TestVar01_Dummy} {var:TestVar01} {var:LandUnitId}",
        "true 1.1 'test' 101010101 99 247"},
       {"{pool:3} {pool:Pool222222} {pool:Pool1}", "333.3333 2.22222 1.1111"},
       {"SELECT * FROM SPECIES WHERE LandUnitId = {var:LandUnitId}", "SELECT * FROM SPECIES WHERE LandUnitId = 247"},
       {"Var '{var:&^%}' should = 'true', '{var:11}' should = '1.1' and so on...",
        "Var 'true' should = 'true', '1.1' should = '1.1' and so on..."},
       {"SELECT * FROM SPECIES WHERE LandUnitId = {pool:Pool1}", "SELECT * FROM SPECIES WHERE LandUnitId = 1.1111"},
       {"SELECT {var:a} AS name, SUM(amount) FROM widgets WHERE LandUnitId = {var:LandUnitId} GROUP BY {var:a}",
        "SELECT 'test' AS name, SUM(amount) FROM widgets WHERE LandUnitId = 247 GROUP BY 'test'"}};

   // Setup Mock Provider
   std::string& currentExpectedResult = testQueryAndExpected.front().second;
   auto mockProvider = std::make_shared<mocks::MockProviderRelational>();

   MOCK_EXPECT(mockProvider->GetDataSet).calls([&currentExpectedResult](std::string query) -> DynamicVar {
      BOOST_CHECK_EQUAL(query, currentExpectedResult);
      return DynamicVar();
   });

   auto mockRepository = std::make_unique<mocks::MockDataRepository>();
   MOCK_EXPECT(mockRepository->getProvider).returns(mockProvider);

   // Run Test
   for (auto test : testQueryAndExpected) {
      currentExpectedResult = test.second;
      DynamicObject config({{"queryString", test.first}, {"provider", "test"}});

      auto testSQLTransform = std::make_shared<mf::SQLQueryTransform>();
      testSQLTransform->configure(config, *mockLandUnitController.get(), *mockRepository.get());
   }
}

#endif

BOOST_AUTO_TEST_CASE(flint_SQLQueryTransform_QuotesStringVariablesInSQL) {
   auto mockLandUnitController = std::make_unique<mocks::MockLandUnitController>();

   // Setup Mock Variables
   auto mockVariable = std::make_unique<mocks::MockVariable>();
   MOCK_EXPECT(mockVariable.get()->value).returns("test admin boundary");
   MOCK_EXPECT(mockLandUnitController->getVariable).with("AdminBoundary").returns(mockVariable.get());

   // Setup Query and Expected results
   std::pair<std::string, std::string> testQueryAndExpected = {
       "SELECT * FROM species WHERE name = {var:AdminBoundary}",
       "SELECT * FROM species WHERE name = 'test admin boundary'"};

   // Setup Mock Provider
   std::string& currentExpectedResult = testQueryAndExpected.second;
   auto mockProvider = std::make_shared<mocks::MockProviderRelational>();
   MOCK_EXPECT(mockProvider->GetDataSet).calls([&currentExpectedResult](std::string query) -> DynamicVar {
      BOOST_CHECK_EQUAL(query, currentExpectedResult);
      return DynamicVar();
   });

   auto mockRepository = std::make_unique<mocks::MockDataRepository>();
   MOCK_EXPECT(mockRepository->getProvider).returns(mockProvider);

   // Run Test
   DynamicObject config({{"queryString", testQueryAndExpected.first}, {"provider", "test"}});

   auto testSQLTransform = std::make_shared<mf::SQLQueryTransform>();
   testSQLTransform->configure(config, *mockLandUnitController.get(), *mockRepository.get());
   testSQLTransform->value();
}

BOOST_AUTO_TEST_CASE(flint_SQLQueryTransform_AddressSyntaxForMultiValueVariables) {
   auto mockLandUnitController = std::make_unique<mocks::MockLandUnitController>();

   // Setup Mock Variables
   std::string varName = "Boundaries";
   auto varValue =
       std::make_unique<DynamicVar>(DynamicObject({{"admin", "test admin boundary"}, {"eco", "test eco boundary"}}));

   auto mockVariable = std::make_unique<mocks::MockVariable>();
   MOCK_EXPECT(mockVariable.get()->value).returns(*varValue);
   MOCK_EXPECT(mockLandUnitController->getVariable).with(varName).returns(mockVariable.get());

   // Setup Query and Expected results
   std::pair<std::string, std::string> testQueryAndExpected = {
       "SELECT * FROM species WHERE admin_name = {var:Boundaries.admin} AND eco_name = {var:Boundaries.eco}",
       "SELECT * FROM species WHERE admin_name = 'test admin boundary' AND eco_name = 'test eco boundary'"};

   // Setup Mock Provider
   std::string& currentExpectedResult = testQueryAndExpected.second;
   auto mockProvider = std::make_shared<mocks::MockProviderRelational>();
   MOCK_EXPECT(mockProvider->GetDataSet).calls([&currentExpectedResult](std::string query) -> DynamicVar {
      BOOST_CHECK_EQUAL(query, currentExpectedResult);
      return DynamicVar();
   });

   auto mockRepository = std::make_unique<mocks::MockDataRepository>();
   MOCK_EXPECT(mockRepository.get()->getProvider).returns(mockProvider);

   // Run Test
   DynamicObject config({{"queryString", testQueryAndExpected.first}, {"provider", "test"}});

   auto testSQLTransform = std::make_shared<mf::SQLQueryTransform>();
   testSQLTransform->configure(config, *mockLandUnitController.get(), *mockRepository.get());
   testSQLTransform->value();
}

BOOST_AUTO_TEST_CASE(flint_SQLQueryTransform_AddressSyntaxForTableVariablesReturningMultipleRows) {
   auto mockLandUnitController = std::make_unique<mocks::MockLandUnitController>();

   // Setup Mock Variables
   std::string varName = "Species";
   DynamicObject row1;
   row1.insert("species_name", "Pine");
   DynamicObject row2;
   row2.insert("species_name", "Spruce");
   std::vector<DynamicObject> table{row1, row2};
   auto varValue = std::make_unique<DynamicVar>(table);

   auto mockVariable = std::make_unique<mocks::MockVariable>();
   MOCK_EXPECT(mockVariable.get()->value).returns(*varValue);
   MOCK_EXPECT(mockLandUnitController->getVariable).with(varName).returns(mockVariable.get());

   // Setup Query and Expected results
   std::pair<std::string, std::string> testQueryAndExpected = {
       "SELECT * FROM foo WHERE species IN ({var:Species.species_name})",
       "SELECT * FROM foo WHERE species IN ('Pine', 'Spruce')",
   };

   // Setup Mock Provider
   std::string& currentExpectedResult = testQueryAndExpected.second;
   auto mockProvider = std::make_shared<mocks::MockProviderRelational>();
   MOCK_EXPECT(mockProvider->GetDataSet).calls([&currentExpectedResult](std::string query) -> DynamicVar {
      BOOST_CHECK_EQUAL(query, currentExpectedResult);
      return DynamicVar();
   });

   auto mockRepository = std::make_unique<mocks::MockDataRepository>();
   MOCK_EXPECT(mockRepository.get()->getProvider).returns(mockProvider);

   // Run Test
   DynamicObject config({{"queryString", testQueryAndExpected.first}, {"provider", "test"}});

   auto testSQLTransform = std::make_shared<mf::SQLQueryTransform>();
   testSQLTransform->configure(config, *mockLandUnitController.get(), *mockRepository.get());
   testSQLTransform->value();
}

BOOST_AUTO_TEST_CASE(flint_SQLQueryTransform_ValueReturnsPlainValueIfSingleColumnSingleRowResult) {
   auto mockLandUnitController = std::make_unique<mocks::MockLandUnitController>();

   auto mockProvider = std::make_shared<mocks::MockProviderRelational>();
   auto result = std::make_shared<DynamicVar>(std::vector<DynamicObject>{DynamicObject({{"a", 1}})});

   MOCK_EXPECT(mockProvider->GetDataSet).returns(*result);

   auto mockRepository = std::make_unique<mocks::MockDataRepository>();
   MOCK_EXPECT(mockRepository.get()->getProvider).returns(mockProvider);

   DynamicObject config({{"queryString", "select a from foo"}, {"provider", "test"}});

   auto sqlTransform = std::make_shared<mf::SQLQueryTransform>();
   sqlTransform->configure(config, *mockLandUnitController.get(), *mockRepository.get());
   int value = sqlTransform->value();
   BOOST_CHECK_EQUAL(value, 1);
}

BOOST_AUTO_TEST_CASE(flint_SQLQueryTransform_ValueReturnsPlainValueVectorIfSingleColumnMultiRowResult) {
   auto mockLandUnitController = std::make_unique<mocks::MockLandUnitController>();

   auto mockProvider = std::make_shared<mocks::MockProviderRelational>();
   DynamicVar result{std::vector<DynamicObject>{DynamicObject({{"a", 1}}), DynamicObject({{"a", 2}})}};

   MOCK_EXPECT(mockProvider->GetDataSet).returns(result);

   auto mockRepository = std::make_unique<mocks::MockDataRepository>();
   MOCK_EXPECT(mockRepository.get()->getProvider).returns(mockProvider);

   DynamicObject config({{"queryString", "select a from foo"}, {"provider", "test"}});

   auto sqlTransform = std::make_shared<mf::SQLQueryTransform>();
   sqlTransform->configure(config, *mockLandUnitController.get(), *mockRepository.get());
   auto values = sqlTransform->value().extract<std::vector<DynamicVar>>();
   int value1 = values[0];
   int value2 = values[1];
   BOOST_CHECK_EQUAL(value1, 1);
   BOOST_CHECK_EQUAL(value2, 2);
}

BOOST_AUTO_TEST_CASE(flint_SQLQueryTransform_ValueReturnsStructIfMultiColumnSingleRowResult) {
   auto mockLandUnitController = std::make_unique<mocks::MockLandUnitController>();

   auto mockProvider = std::make_shared<mocks::MockProviderRelational>();
   DynamicVar result{std::vector<DynamicObject>{DynamicObject({{"a", 1}, {"b", 2}})}};

   MOCK_EXPECT(mockProvider->GetDataSet).returns(result);

   auto mockRepository = std::make_unique<mocks::MockDataRepository>();
   MOCK_EXPECT(mockRepository.get()->getProvider).returns(mockProvider);

   DynamicObject config({{"queryString", "select a, b from foo"}, {"provider", "test"}});

   auto sqlTransform = std::make_shared<mf::SQLQueryTransform>();
   sqlTransform->configure(config, *mockLandUnitController.get(), *mockRepository.get());
   const auto& value = sqlTransform->value().extract<DynamicObject>();
   BOOST_CHECK(value["a"] == 1);
   BOOST_CHECK(value["b"] == 2);
}

BOOST_AUTO_TEST_CASE(flint_SQLQueryTransform_ValueReturnsStructVectorIfMultiColumnMultiRowResult) {
   auto mockLandUnitController = std::make_unique<mocks::MockLandUnitController>();

   auto mockProvider = std::make_shared<mocks::MockProviderRelational>();
   DynamicVar result{
       std::vector<DynamicObject>{DynamicObject({{"a", 1}, {"b", 2}}), DynamicObject({{"a", 3}, {"b", 4}})}};

   MOCK_EXPECT(mockProvider->GetDataSet).returns(result);

   auto mockRepository = std::make_unique<mocks::MockDataRepository>();
   MOCK_EXPECT(mockRepository.get()->getProvider).returns(mockProvider);

   DynamicObject config({{"queryString", "select a from foo"}, {"provider", "test"}});

   auto sqlTransform = std::make_shared<mf::SQLQueryTransform>();
   sqlTransform->configure(config, *mockLandUnitController.get(), *mockRepository.get());
   auto values = sqlTransform->value().extract<std::vector<DynamicObject>>();
   BOOST_CHECK(values[0]["a"] == 1);
   BOOST_CHECK(values[0]["b"] == 2);
   BOOST_CHECK(values[1]["a"] == 3);
   BOOST_CHECK(values[1]["b"] == 4);
}

BOOST_AUTO_TEST_CASE(flint_SQLQueryTransform_ValueReturnsEmptyDynamicIfNoResult) {
   auto mockLandUnitController = std::make_unique<mocks::MockLandUnitController>();
   auto mockProvider = std::make_shared<mocks::MockProviderRelational>();
   MOCK_EXPECT(mockProvider->GetDataSet).returns(DynamicVar());

   auto mockRepository = std::make_unique<mocks::MockDataRepository>();
   MOCK_EXPECT(mockRepository.get()->getProvider).returns(mockProvider);

   DynamicObject config({{"queryString", "select a from foo"}, {"provider", "test"}});

   auto sqlTransform = std::make_shared<mf::SQLQueryTransform>();
   sqlTransform->configure(config, *mockLandUnitController.get(), *mockRepository.get());

   auto result = sqlTransform->value();
   BOOST_CHECK(result.isEmpty());
}

BOOST_AUTO_TEST_CASE(
    flint_SQLQueryTransform_regression_ValueReturnsEmptyDynamicIfNoResultAfterPreviousSuccessfulQuery) {
   auto mockLandUnitController = std::make_unique<mocks::MockLandUnitController>();
   auto mockProvider = std::make_shared<mocks::MockProviderRelational>();

   auto firstResult = std::make_shared<DynamicVar>(std::vector<DynamicObject>{DynamicObject({{"a", 1}})});

   MOCK_EXPECT(mockProvider->GetDataSet).once().returns(*firstResult);
   MOCK_EXPECT(mockProvider->GetDataSet).returns(DynamicVar());

   auto mockRepository = std::make_unique<mocks::MockDataRepository>();
   MOCK_EXPECT(mockRepository.get()->getProvider).returns(mockProvider);

   DynamicObject config({{"queryString", "select a from foo"}, {"provider", "test"}});

   auto sqlTransform = std::make_shared<mf::SQLQueryTransform>();
   sqlTransform->configure(config, *mockLandUnitController.get(), *mockRepository.get());

   // Check to make sure the first valid result isn't cached and returned when
   // it shouldn't be.
   int first = sqlTransform->value();
   auto next = sqlTransform->value();
   BOOST_CHECK(next.isEmpty());
}

BOOST_AUTO_TEST_SUITE_END();
