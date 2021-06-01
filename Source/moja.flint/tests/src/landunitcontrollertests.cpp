#include <moja/flint/flintexceptions.h>
#include <moja/flint/imodule.h>
#include <moja/flint/ioperationmanager.h>
#include <moja/flint/landunitcontroller.h>

#include <moja/test/mockmodule.h>
#include <moja/test/mockpool.h>
#include <moja/test/mockvariable.h>

#include <moja/dynamic.h>

#include <boost/test/unit_test.hpp>

#include <turtle/mock.hpp>

#include <map>
#include <memory>

namespace flint {

namespace mocks = moja::test;

struct LandUnitControllerTestsFixture {
   moja::flint::LandUnitController controller;
};

BOOST_FIXTURE_TEST_SUITE(LandUnitControllerTests, LandUnitControllerTestsFixture)

BOOST_AUTO_TEST_CASE(LandUnitController_AddPoolThrowsExceptionIfNameIsEmpty) {
   auto badNames = {"", "  "};
   for (auto name : badNames) {
      BOOST_CHECK_THROW(controller.operationManager()->addPool(name, "", "", 1.0, 1, 0.0, nullptr),
                        std::invalid_argument);
   }
}

BOOST_AUTO_TEST_CASE(LandUnitController_AddPoolThrowsExceptionIfValueIsNegative) {
   auto badValues = {-0.1, -100.0};
   for (auto value : badValues) {
      BOOST_CHECK_THROW(controller.operationManager()->addPool("test", "", "", 1.0, 1, value, nullptr),
                        std::invalid_argument);
   }
}

BOOST_AUTO_TEST_CASE(LandUnitController_AddPoolIncrementsPoolCount) {
   for (int i = 1; i < 100; i++) {
      controller.operationManager()->addPool("p" + std::to_string(i), "", "", 1.0, 1, 0.0, nullptr);
      BOOST_CHECK_EQUAL(i, controller.operationManager()->poolCount());
   }
}

BOOST_AUTO_TEST_CASE(LandUnitController_AddPoolAssignsSequentialIndices) {
   for (int i = 0; i < 100; i++) {
      auto name = "p" + std::to_string(i);
      controller.operationManager()->addPool(name, "", "", 1.0, 1, 0.0, nullptr);
      auto pool = controller.operationManager()->getPool(name);
      BOOST_CHECK_EQUAL(i, pool->idx());
   }
}

BOOST_AUTO_TEST_CASE(LandUnitController_GetPoolByName) {
   std::map<std::string, double> pools{{"p1", 0.0}, {"p2", 100.0}};
   for (auto [name, init_value] : pools) {
      controller.operationManager()->addPool(name, "", "", 1.0, 1, init_value, nullptr);
   }
   controller.initialiseData(true);

   for (auto expected : pools) {
      auto actual = controller.operationManager()->getPool(expected.first);
      BOOST_CHECK_EQUAL(expected.first, actual->name());
      BOOST_CHECK_EQUAL(expected.second, actual->value());
   }
}

BOOST_AUTO_TEST_CASE(LandUnitController_GetPoolByNameThrowsExceptionIfPoolNotFound) {
   controller.operationManager()->addPool("test", "", "", 1.0, 1, 0.0, nullptr);
   BOOST_CHECK_THROW(controller.operationManager()->getPool("invalid pool"), moja::flint::PoolNotFoundException);
}

BOOST_AUTO_TEST_CASE(LandUnitController_GetPoolByIndex) {
   std::vector<std::pair<std::string, double>> pools{{"p1", 0.0}, {"p2", 100.0}};
   for (auto [name, init_value] : pools) {
      controller.operationManager()->addPool(name, "", "", 1.0, 1, init_value, nullptr);
   }

   controller.initialiseData(true);

   for (int i = 0; i < pools.size(); i++) {
      auto expected = pools[i];
      auto actual = controller.operationManager()->getPool(i);
      BOOST_CHECK_EQUAL(expected.first, actual->name());
      BOOST_CHECK_EQUAL(expected.second, actual->value());
   }
}

BOOST_AUTO_TEST_CASE(LandUnitController_GetPoolByIndexThrowsExceptionIfIndexOutOfRange) {
   controller.operationManager()->addPool("test", "", "", 1.0, 1, 0.0, nullptr);
   for (int i : {-100, -1, 1, 100}) {
      BOOST_CHECK_THROW(controller.operationManager()->getPool(i), moja::flint::PoolNotFoundException);
   }
}

BOOST_AUTO_TEST_CASE(LandUnitController_PoolValueIterator) {
   std::vector<std::pair<std::string, double>> pools{{"p1", 0.0}, {"p2", 100.0}};
   for (auto& [name, init_value] : pools) {
      controller.operationManager()->addPool(name, "", "", 1.0, 1, init_value, nullptr);
   }

   controller.initialiseData(true);

   for (auto pool : controller.operationManager()->poolCollection()) {
      auto expectedValue = pools[pool->idx()].second;
      auto actualValue = pool->value();
      BOOST_CHECK_EQUAL(expectedValue, actualValue);
   }
}

BOOST_AUTO_TEST_CASE(LandUnitController_PoolObjIterator) {
   std::vector<std::pair<std::string, double>> pools{{"p1", 0.0}, {"p2", 100.0}};
   for (auto [name, init_value] : pools) {
      controller.operationManager()->addPool(name, "", "", 1.0, 1, init_value, nullptr);
   }

   controller.initialiseData(true);

   for (auto pool : controller.operationManager()->poolCollection()) {
      auto& [name, value] = pools[pool->idx()];
      BOOST_CHECK_EQUAL(name, pool->name());
      BOOST_CHECK_EQUAL(value, pool->value());
   }
}

BOOST_AUTO_TEST_CASE(LandUnitController_AddVariableThrowsExceptionIfNameIsEmpty) {
   auto var = std::make_shared<mocks::MockVariable>();
   MOCK_EXPECT(var->isExternal).returns(false);
   MOCK_EXPECT(var->isFlintData).returns(false);
   MOCK_EXPECT(var->value).returns(moja::DynamicVar(0));

   auto badNames = {"", "  "};
   for (auto name : badNames) {
      BOOST_CHECK_THROW(controller.addVariable(name, var), std::invalid_argument);
   }
}

BOOST_AUTO_TEST_CASE(LandUnitController_AddVariable) {
   auto var = std::make_shared<mocks::MockVariable>();
   MOCK_EXPECT(var->isExternal).returns(false);
   MOCK_EXPECT(var->isFlintData).returns(false);
   MOCK_EXPECT(var->value).returns(moja::DynamicVar(0));

   controller.addVariable("test", var);
   auto has_variable = controller.hasVariable("test");
   BOOST_CHECK(has_variable);
}

BOOST_AUTO_TEST_CASE(LandUnitController_GetVariableThrowsExceptionIfVariableNotFound) {
   BOOST_CHECK_THROW(controller.getVariable("not found"), moja::flint::VariableNotFoundException);
}

BOOST_AUTO_TEST_CASE(LandUnitController_GetVariable) {
   int expectedValue = 100;
   auto var = std::make_shared<mocks::MockVariable>();
   MOCK_EXPECT(var->isExternal).returns(false);
   MOCK_EXPECT(var->isFlintData).returns(false);
   MOCK_EXPECT(var->value).returns(moja::DynamicVar(expectedValue));

   controller.addVariable("test", var);
   auto actual = controller.getVariable("test");
   BOOST_CHECK_EQUAL(expectedValue, actual->value().convert<int>());
}

BOOST_AUTO_TEST_CASE(LandUnitController_CreateProportionalOperation) {
   auto module = std::make_shared<mocks::MockModule>();
   MOCK_EXPECT(module->metaData).returns(moja::flint::ModuleMetaData());
   controller.createProportionalOperation(*module.get());
}

BOOST_AUTO_TEST_CASE(LandUnitController_CreateStockOperation) {
   auto module = std::make_shared<mocks::MockModule>();
   MOCK_EXPECT(module->metaData).returns(moja::flint::ModuleMetaData());
   controller.createStockOperation(*module.get());
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace flint
