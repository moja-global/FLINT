#include "operationmanagertestutils.h"

#include <moja/flint/modulebase.h>
#include <moja/flint/operationmanagersimple.h>
#include <moja/flint/timing.h>

#include <moja/dynamic.h>

#include <boost/test/unit_test.hpp>

namespace flint {

using namespace moja;
using namespace moja::flint;

struct SimpleFixture {
   Timing timing;
   ModuleBase module;
   DynamicObject config;
   OperationManagerSimple manager;

   SimpleFixture() : config({{"use_kahan", true}}), manager(timing, config) {
      timing.setStartDate(DateTime(2001, 1, 1));
      timing.setStartDate(DateTime(2013, 12, 31));
      timing.init();

      auto& metadata = module.metaData();
      metadata.setDefaults();
      metadata.moduleName = "testModuleSimple";
   }
};

BOOST_FIXTURE_TEST_SUITE(OperationManagerTests, SimpleFixture);

BOOST_AUTO_TEST_CASE(OperationManager_NoPoolIteration) { test_NoPoolIteration(manager, module); }

BOOST_AUTO_TEST_CASE(OperationManager_SinglePoolIteration) { test_SinglePoolIteration(manager, module); }

BOOST_AUTO_TEST_CASE(OperationManager_MultiplePoolIteration) { test_MultiplePoolIteration(manager, module); }

BOOST_AUTO_TEST_CASE(OperationManager_NoResultIteration) { test_NoResultIteration(manager, module); }

BOOST_AUTO_TEST_CASE(OperationManager_SingleResultIteration) { test_SingleResultIteration(manager, module); }

BOOST_AUTO_TEST_CASE(OperationManager_MultipleResultIteration) { test_MultipleResultIteration(manager, module); }

BOOST_AUTO_TEST_CASE(OperationManager_NoResultFluxIteration) { test_NoResultFluxIteration(manager, module); }

BOOST_AUTO_TEST_CASE(OperationManager_SingleResultFluxIteration) { test_SingleResultFluxIteration(manager, module); }

BOOST_AUTO_TEST_CASE(OperationManager_MultipleResultFluxIteration) { test_MultipleResultFluxIteration(manager, module); }

BOOST_AUTO_TEST_CASE(OperationManager_SingleProportionTransfer) { test_SingleProportionTransfer(manager, module); }

BOOST_AUTO_TEST_CASE(OperationManager_SingleStockTransfer) { test_SingleStockTransfer(manager, module); }

BOOST_AUTO_TEST_CASE(OperationManager_DoubleProportionalTransfer) { test_DoubleProportionalTransfer(manager, module); }

BOOST_AUTO_TEST_CASE(OperationManager_DoubleStockTransfer) { test_DoubleStockTransfer(manager, module); }

BOOST_AUTO_TEST_CASE(OperationManager_DoubleStockAndApplyTransfer) { test_DoubleStockAndApplyTransfer(manager, module); }

BOOST_AUTO_TEST_CASE(OperationManager_TwoOperationsStockAndProportional) {
   test_TwoOperationsStockAndProportional(manager, module);
}

BOOST_AUTO_TEST_CASE(OperationManager_Kahan_summation_issues_Proportion) {
   test_Kahan_summation_issues_Proportion(manager, module);
}

BOOST_AUTO_TEST_CASE(OperationManager_Kahan_summation_issues_Proportion_with_SpinUp) {
   test_Kahan_summation_issues_Proportion_with_SpinUp(manager, module);
}

BOOST_AUTO_TEST_CASE(OperationManager_PerformanceTestProportionalSLEEK) {
   test_PerformanceTestProportionalSLEEK(manager, module);
}

BOOST_AUTO_TEST_CASE(OperationManager_PerformanceTestStockSLEEK) { test_PerformanceTestStockSLEEK(manager, module); }

BOOST_AUTO_TEST_CASE(OperationManager_PerformanceTestCBM) { test_PerformanceTestCBM(manager, module); }

BOOST_AUTO_TEST_CASE(OperationManager_SubmitOperationAddsToPendingQueue) { SubmitOperationAddsToPendingQueue(manager, module); }

BOOST_AUTO_TEST_CASE(OperationManager_ClearLastAppliedOperationResults) { ClearLastAppliedOperationResults(manager, module); }

BOOST_AUTO_TEST_CASE(OperationManager_ApplyOperationsAppendsToCommittedQueue) {
   ApplyOperationsAppendsToCommittedQueue(manager, module);
}

BOOST_AUTO_TEST_CASE(OperationManager_ApplyOperationsCorrectlyUpdatesPoolsForSimpleCase) {
   ApplyOperationsCorrectlyUpdatesPoolsForSimpleCase(manager, module);
}

BOOST_AUTO_TEST_CASE(OperationManager_ApplyOperationsCorrectlyUpdatesPoolsForComplexCase) {
   ApplyOperationsCorrectlyUpdatesPoolsForComplexCase(manager, module);
}

BOOST_AUTO_TEST_CASE(OperationManager_ApplyAndGetOperationsLastApplied) { ApplyAndGetOperationsLastApplied(manager, module); }

BOOST_AUTO_TEST_CASE(OperationManager_ApplyOperationsClearsPendingQueue) { ApplyOperationsClearsPendingQueue(manager, module); }

BOOST_AUTO_TEST_SUITE_END()

}  // namespace flint
