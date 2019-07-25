#include "flinttests.h"
#include "operationmanagertestutils.h"

#include <moja/flint/modulebase.h>
#include <moja/flint/operationmanagersimplecache.h>
#include <moja/flint/timing.h>

#include <moja/dynamic.h>

#include <boost/test/unit_test.hpp>

#include <memory>

using namespace moja;
using namespace moja::flint;
namespace conf = moja::flint::configuration;

// --------------------------------------------------------------------------------------------
struct PoolInitValueAndResult {
   std::string name;
   double value;
   double result;

   std::shared_ptr<IPool> poolHandle;
};

// --------------------------------------------------------------------------------------------

struct SimpleCacheFixture {
   Timing timing;
   ModuleBase module;
   DynamicObject config;
   OperationManagerSimpleCache manager;

   SimpleCacheFixture() : config({{"use_kahan", true}}), manager(timing, config) {
      timing.setStartDate(DateTime(2001, 1, 1));
      timing.setStartDate(DateTime(2013, 12, 31));
      timing.init();

      auto& metadata = module.metaData();
      metadata.setDefaults();
      metadata.moduleName = "testModuleSimpleCache";
   }
};

BOOST_FIXTURE_TEST_SUITE(SimpleCache_operationmanagertests, SimpleCacheFixture);

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SimpleCache_NoPoolIteration) { test_NoPoolIteration(manager, module); }

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SimpleCache_SinglePoolIteration) { test_SinglePoolIteration(manager, module); }

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SimpleCache_MultiplePoolIteration) { test_MultiplePoolIteration(manager, module); }

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SimpleCache_NoResultIteration) { test_NoResultIteration(manager, module); }

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SimpleCache_SingleResultIteration) { test_SingleResultIteration(manager, module); }

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SimpleCache_MultipleResultIteration) { test_MultipleResultIteration(manager, module); }

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SimpleCache_NoResultFluxIteration) { test_NoResultFluxIteration(manager, module); }

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SimpleCache_SingleResultFluxIteration) { test_SingleResultFluxIteration(manager, module); }

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SimpleCache_MultipleResultFluxIteration) { test_MultipleResultFluxIteration(manager, module); }

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SimpleCache_SingleProportionTransfer) { test_SingleProportionTransfer(manager, module); }

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SimpleCache_SingleStockTransfer) { test_SingleStockTransfer(manager, module); }

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SimpleCache_DoubleProportionalTransfer) { test_DoubleProportionalTransfer(manager, module); }

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SimpleCache_DoubleStockTransfer) { test_DoubleStockTransfer(manager, module); }

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SimpleCache_DoubleStockAndApplyTransfer) { test_DoubleStockAndApplyTransfer(manager, module); }

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SimpleCache_TwoOperationsStockAndProportional) {
   test_TwoOperationsStockAndProportional(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SimpleCache_Kahan_summation_issues_Proportion) {
   test_Kahan_summation_issues_Proportion(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SimpleCache_Kahan_summation_issues_Proportion_with_SpinUp) {
   test_Kahan_summation_issues_Proportion_with_SpinUp(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SimpleCache_PerformanceTestProportionalSLEEK) {
   test_PerformanceTestProportionalSLEEK(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SimpleCache_PerformanceTestStockSLEEK) { test_PerformanceTestStockSLEEK(manager, module); }

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SimpleCache_PerformanceTestCBM) { test_PerformanceTestCBM(manager, module); }

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SimpleCache_SubmitOperationAddsToPendingQueue) {
   SubmitOperationAddsToPendingQueue(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SimpleCache_ClearLastAppliedOperationResults) {
   ClearLastAppliedOperationResults(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SimpleCache_ApplyOperationsAppendsToCommittedQueue) {
   ApplyOperationsAppendsToCommittedQueue(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SimpleCache_ApplyOperationsCorrectlyUpdatesPoolsForSimpleCacheCase) {
   ApplyOperationsCorrectlyUpdatesPoolsForSimpleCase(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Simple_ApplyOperationsCorrectlyUpdatesPoolsForComplexCase) {
   ApplyOperationsCorrectlyUpdatesPoolsForComplexCase(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SimpleCache_ApplyAndGetOperationsLastApplied) {
   ApplyAndGetOperationsLastApplied(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SimpleCache_ApplyOperationsClearsPendingQueue) {
   ApplyOperationsClearsPendingQueue(manager, module);
}

BOOST_AUTO_TEST_SUITE_END();
