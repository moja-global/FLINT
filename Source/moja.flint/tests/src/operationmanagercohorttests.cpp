#include "moja/flint/ioperation.h"
#include "moja/flint/ipool.h"

#include <moja/flint/modulebase.h>
#include <moja/flint/operationmanagersimple.h>
#include <moja/flint/timing.h>

#include <moja/dynamic.h>

#include <boost/test/unit_test.hpp>

namespace flint {

using namespace moja;
using namespace moja::flint;

constexpr auto pct_tolerance = 0.0000001;

struct PoolInitValueAndResult {
   std::string name;
   double value;
   double result;

   IPool* pool;
};

struct CohortFixture {
   Timing timing;
   ModuleBase module;
   DynamicObject config;
   OperationManagerSimple manager;

   CohortFixture() : config({{"use_kahan", true}}), manager(timing, config) {
      timing.setStartDate(DateTime(2001, 1, 1));
      timing.setStartDate(DateTime(2013, 12, 31));
      timing.init();

      auto& metadata = module.metaData();
      metadata.setDefaults();
      metadata.moduleName = "testModuleCohort";
   }
};

BOOST_FIXTURE_TEST_SUITE(OperationManagerCohortTests, CohortFixture);

BOOST_AUTO_TEST_CASE(OperationManagerCohort_SingleStockTransfer) {
   const auto* A = manager.addPool("A", "", "", 1.0, 1, 100.0, nullptr);
   const auto* B = manager.addPool("B", "", "", 1.0, 1, 0.0, nullptr);
   manager.initialisePools();

   auto op1 = manager.createStockOperation(module);

   op1->addTransfer(A, B, 45);
   op1->submitOperation();

   manager.applyOperations();

   BOOST_CHECK_CLOSE(A->value(), 55.0, pct_tolerance);
   BOOST_CHECK_CLOSE(B->value(), 45.0, pct_tolerance);
}

BOOST_AUTO_TEST_CASE(OperationManagerCohort_SingleProportionalTransfer) {
   const auto* A = manager.addPool("A", "", "", 1.0, 1, 100.0, nullptr);
   const auto* B = manager.addPool("B", "", "", 1.0, 1, 0.0, nullptr);
   manager.initialisePools();

   auto op1 = manager.createProportionalOperation(module);

   op1->addTransfer(A, B, 1.0 / 3.0);
   op1->submitOperation();

   manager.applyOperations();

   BOOST_CHECK_CLOSE(A->value(), 100.0 * 2.0 / 3.0, pct_tolerance);
   BOOST_CHECK_CLOSE(B->value(), 100.0 * 1.0 / 3.0, pct_tolerance);
}

// BOOST_CHECK_THROW(conf::ConfigBlock(mockTile, 1, 1, size, 1), conf::LandscapeDefinitionException);

BOOST_AUTO_TEST_CASE(OperationManagerCohort_ParentWithInitValueThrows) {
   auto* A = manager.addPool("A", "", "", 1.0, 1, 100.0, nullptr);
   BOOST_CHECK_THROW(manager.addPool("B", "", "", 1.0, 1, 0.0, A), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(OperationManagerCohort_StockTransferFromParentThrows) {
   auto* parent = manager.addPool("parent", "", "", 1.0, 1, std::optional<double>{}, nullptr);
   auto* child = manager.addPool("child", "", "", 1.0, 1, 0.0, parent);
   manager.initialisePools();

   auto op1 = manager.createStockOperation(module);
   BOOST_CHECK_THROW(op1->addTransfer(parent, child, 45), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(OperationManagerCohort_ProportionalTransferFromParentThrows) {
   auto* parent = manager.addPool("parent", "", "", 1.0, 1, std::optional<double>{}, nullptr);
   auto* child = manager.addPool("child", "", "", 1.0, 1, 0.0, parent);
   manager.initialisePools();

   auto op1 = manager.createProportionalOperation(module);
   BOOST_CHECK_THROW(op1->addTransfer(parent, child, 1.0 / 3.0), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(OperationManagerCohort_StockTransferToParentThrows) {
   auto* parent = manager.addPool("parent", "", "", 1.0, 1, std::optional<double>{}, nullptr);
   auto* child = manager.addPool("child", "", "", 1.0, 1, 0.0, parent);
   manager.initialisePools();

   auto op1 = manager.createStockOperation(module);
   BOOST_CHECK_THROW(op1->addTransfer(child, parent, 45), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(OperationManagerCohort_ProportionalTransferToParentThrows) {
   auto* parent = manager.addPool("parent", "", "", 1.0, 1, std::optional<double>{}, nullptr);
   auto* child = manager.addPool("child", "", "", 1.0, 1, 0.0, parent);
   manager.initialisePools();

   auto op1 = manager.createProportionalOperation(module);
   BOOST_CHECK_THROW(op1->addTransfer(child, parent, 1.0 / 3.0), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(OperationManagerCohort_SetParentValueThrows) {
   auto* parent = manager.addPool("parent", "", "", 1.0, 1, std::optional<double>{}, nullptr);
   auto* child = manager.addPool("child", "", "", 1.0, 1, 0.0, parent);
   manager.initialisePools();

   BOOST_CHECK_THROW(parent->set_value(42.0), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(OperationManagerCohort_ChildToChildStockTransfer) {
   auto* parent_1 = manager.addPool("parent_1", "", "", 1.0, 1, std::optional<double>{}, nullptr);
   auto* parent_2 = manager.addPool("parent_2", "", "", 1.0, 1, std::optional<double>{}, nullptr);
   const auto* child_1_1 = manager.addPool("child_1_1", "", "", 1.0, 1, 10.0, parent_1);
   const auto* child_1_2 = manager.addPool("child_1_2", "", "", 1.0, 1, 10.0, parent_1);
   const auto* child_2_1 = manager.addPool("child_2_1", "", "", 1.0, 1, 10.0, parent_2);
   const auto* child_2_2 = manager.addPool("child_2_2", "", "", 1.0, 1, 10.0, parent_2);
   manager.initialisePools();

   auto op1 = manager.createStockOperation(module);

   op1->addTransfer(child_1_1, child_2_1, 5.0);
   op1->submitOperation();

   manager.applyOperations();

   BOOST_CHECK_CLOSE(child_1_1->value(), 5.0, pct_tolerance);
   BOOST_CHECK_CLOSE(child_2_1->value(), 15.0, pct_tolerance);
   BOOST_CHECK_CLOSE(parent_1->value(), 15.0, pct_tolerance);
   BOOST_CHECK_CLOSE(parent_2->value(), 25.0, pct_tolerance);
}

BOOST_AUTO_TEST_CASE(OperationManagerCohort_ChildToChildProportionalTransfer) {
   auto* parent_1 = manager.addPool("parent_1", "", "", 1.0, 1, std::optional<double>{}, nullptr);
   auto* parent_2 = manager.addPool("parent_2", "", "", 1.0, 1, std::optional<double>{}, nullptr);
   const auto* child_1_1 = manager.addPool("child_1_1", "", "", 1.0, 1, 10.0, parent_1);
   const auto* child_1_2 = manager.addPool("child_1_2", "", "", 1.0, 1, 10.0, parent_1);
   const auto* child_2_1 = manager.addPool("child_2_1", "", "", 1.0, 1, 10.0, parent_2);
   const auto* child_2_2 = manager.addPool("child_2_2", "", "", 1.0, 1, 10.0, parent_2);
   manager.initialisePools();

   auto op1 = manager.createProportionalOperation(module);

   op1->addTransfer(child_1_1, child_2_1, 1.0 / 2.0);
   op1->submitOperation();

   manager.applyOperations();

   BOOST_CHECK_CLOSE(child_1_1->value(), 5.0, pct_tolerance);
   BOOST_CHECK_CLOSE(child_2_1->value(), 15.0, pct_tolerance);
   BOOST_CHECK_CLOSE(parent_1->value(), 15.0, pct_tolerance);
   BOOST_CHECK_CLOSE(parent_2->value(), 25.0, pct_tolerance);
}

BOOST_AUTO_TEST_CASE(OperationManagerCohort_PoolNoInitValue) {
   auto* pool = manager.addPool("pool", "", "", 1.0, 1, std::optional<double>{}, nullptr);
   manager.initialisePools();

   BOOST_CHECK_CLOSE(pool->value(), 0.0, pct_tolerance);
}

BOOST_AUTO_TEST_CASE(OperationManagerCohort_PoolToChildStockTransfer) {
   const auto* pool = manager.addPool("pool", "", "", 1.0, 1, 10, nullptr);
   auto* parent_1 = manager.addPool("parent_1", "", "", 1.0, 1, std::optional<double>{}, nullptr);
   auto* parent_2 = manager.addPool("parent_2", "", "", 1.0, 1, std::optional<double>{}, nullptr);
   const auto* child_1_1 = manager.addPool("child_1_1", "", "", 1.0, 1, 10.0, parent_1);
   const auto* child_1_2 = manager.addPool("child_1_2", "", "", 1.0, 1, 10.0, parent_1);
   const auto* child_2_1 = manager.addPool("child_2_1", "", "", 1.0, 1, 10.0, parent_2);
   const auto* child_2_2 = manager.addPool("child_2_2", "", "", 1.0, 1, 10.0, parent_2);
   manager.initialisePools();

   auto op1 = manager.createStockOperation(module);

   op1->addTransfer(pool, child_2_1, 5.0);
   op1->submitOperation();

   manager.applyOperations();

   BOOST_CHECK_CLOSE(pool->value(), 5.0, pct_tolerance);
   BOOST_CHECK_CLOSE(child_1_1->value(), 10.0, pct_tolerance);
   BOOST_CHECK_CLOSE(child_2_1->value(), 15.0, pct_tolerance);
   BOOST_CHECK_CLOSE(parent_1->value(), 20.0, pct_tolerance);
   BOOST_CHECK_CLOSE(parent_2->value(), 25.0, pct_tolerance);
}

BOOST_AUTO_TEST_CASE(OperationManagerCohort_PoolToChildProportionalTransfer) {
   const auto* pool = manager.addPool("pool", "", "", 1.0, 1, 10, nullptr);
   auto* parent_1 = manager.addPool("parent_1", "", "", 1.0, 1, std::optional<double>{}, nullptr);
   auto* parent_2 = manager.addPool("parent_2", "", "", 1.0, 1, std::optional<double>{}, nullptr);
   const auto* child_1_1 = manager.addPool("child_1_1", "", "", 1.0, 1, 10.0, parent_1);
   const auto* child_1_2 = manager.addPool("child_1_2", "", "", 1.0, 1, 10.0, parent_1);
   const auto* child_2_1 = manager.addPool("child_2_1", "", "", 1.0, 1, 10.0, parent_2);
   const auto* child_2_2 = manager.addPool("child_2_2", "", "", 1.0, 1, 10.0, parent_2);
   manager.initialisePools();

   auto op1 = manager.createProportionalOperation(module);

   op1->addTransfer(pool, child_2_1, 1.0 / 2.0);
   op1->submitOperation();

   manager.applyOperations();

   BOOST_CHECK_CLOSE(pool->value(), 5.0, pct_tolerance);
   BOOST_CHECK_CLOSE(child_1_1->value(), 10.0, pct_tolerance);
   BOOST_CHECK_CLOSE(child_2_1->value(), 15.0, pct_tolerance);
   BOOST_CHECK_CLOSE(parent_1->value(), 20.0, pct_tolerance);
   BOOST_CHECK_CLOSE(parent_2->value(), 25.0, pct_tolerance);
}

BOOST_AUTO_TEST_CASE(OperationManagerCohort_DynamicAddPool) {
   const auto* pool = manager.addPool("pool", "", "", 1.0, 1, 10, nullptr);
   auto* parent_1 = manager.addPool("parent_1", "", "", 1.0, 1, std::optional<double>{}, nullptr);
   auto* parent_2 = manager.addPool("parent_2", "", "", 1.0, 1, std::optional<double>{}, nullptr);
   const auto* child_1_1 = manager.addPool("child_1_1", "", "", 1.0, 1, 10.0, parent_1);
   const auto* child_1_2 = manager.addPool("child_1_2", "", "", 1.0, 1, 10.0, parent_1);
   const auto* child_2_1 = manager.addPool("child_2_1", "", "", 1.0, 1, 10.0, parent_2);
   const auto* child_2_2 = manager.addPool("child_2_2", "", "", 1.0, 1, 10.0, parent_2);
   manager.initialisePools();

   auto op1 = manager.createStockOperation(module);

   op1->addTransfer(pool, child_2_1, 5.0);
   op1->submitOperation();

   manager.applyOperations();

   auto* child_1_3 = manager.addPool("child_1_3", "", "", 1.0, 1, 10.0, parent_1);
   child_1_3->init();

   auto op2 = manager.createProportionalOperation(module);

   op2->addTransfer(pool, child_1_3, 1.0 / 2.0);
   op2->submitOperation();

   manager.applyOperations();

   BOOST_CHECK_CLOSE(pool->value(), 2.5, pct_tolerance);
   BOOST_CHECK_CLOSE(child_1_1->value(), 10.0, pct_tolerance);
   BOOST_CHECK_CLOSE(child_1_2->value(), 10.0, pct_tolerance);
   BOOST_CHECK_CLOSE(child_1_3->value(), 12.5, pct_tolerance);
   BOOST_CHECK_CLOSE(child_2_1->value(), 15.0, pct_tolerance);
   BOOST_CHECK_CLOSE(child_2_2->value(), 10.0, pct_tolerance);
   BOOST_CHECK_CLOSE(parent_1->value(), 32.5, pct_tolerance);
   BOOST_CHECK_CLOSE(parent_2->value(), 25.0, pct_tolerance);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace flint
