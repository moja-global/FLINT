#include "flinttests.h"
#include "operationmanagertestutils.h"

#include <moja/flint/modulebase.h>
#include <moja/flint/operationmanagerublas.h>
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

#define OP_MANAGER_TO_USE OperationManagerUblas

// --------------------------------------------------------------------------------------------

struct UblasFixture {
   Timing timing;
   ModuleBase module;
   DynamicObject config;
   OperationManagerUblas manager;

   UblasFixture() : config({{"use_kahan", true}}), manager(timing, config) {
      timing.setStartDate(DateTime(2001, 1, 1));
      timing.setStartDate(DateTime(2013, 12, 31));
      timing.init();

      auto& metadata = module.metaData();
      metadata.setDefaults();
      metadata.moduleName = "testModuleUblas";
   }
};

BOOST_FIXTURE_TEST_SUITE(Ublas_operationmanagertests, UblasFixture);

// // --------------------------------------------------------------------------------------------
// BOOST_AUTO_TEST_CASE(Ublas_NoPoolIteration) { test_NoPoolIteration(manager, module); }

// // --------------------------------------------------------------------------------------------
// BOOST_AUTO_TEST_CASE(Ublas_SinglePoolIteration) { test_SinglePoolIteration(manager, module); }

// // --------------------------------------------------------------------------------------------
// BOOST_AUTO_TEST_CASE(Ublas_MultiplePoolIteration) { test_MultiplePoolIteration(manager, module); }

// // --------------------------------------------------------------------------------------------
// BOOST_AUTO_TEST_CASE(Ublas_NoResultIteration) { test_NoResultIteration(manager, module); }

// // --------------------------------------------------------------------------------------------
// BOOST_AUTO_TEST_CASE(Ublas_SingleResultIteration) { test_SingleResultIteration(manager, module); }

// // --------------------------------------------------------------------------------------------
// BOOST_AUTO_TEST_CASE(Ublas_MultipleResultIteration) { test_MultipleResultIteration(manager, module); }

// // --------------------------------------------------------------------------------------------
// BOOST_AUTO_TEST_CASE(Ublas_NoResultFluxIteration) { test_NoResultFluxIteration(manager, module); }

// // --------------------------------------------------------------------------------------------
// BOOST_AUTO_TEST_CASE(Ublas_SingleResultFluxIteration) { test_SingleResultFluxIteration(manager, module); }

// // --------------------------------------------------------------------------------------------
// BOOST_AUTO_TEST_CASE(Ublas_MultipleResultFluxIteration) { test_MultipleResultFluxIteration(manager, module); }

// // --------------------------------------------------------------------------------------------
// BOOST_AUTO_TEST_CASE(Ublas_SingleProportionTransfer) { test_SingleProportionTransfer(manager, module); }

// // --------------------------------------------------------------------------------------------
// BOOST_AUTO_TEST_CASE(Ublas_SingleStockTransfer) { test_SingleStockTransfer(manager, module); }

// // --------------------------------------------------------------------------------------------
// BOOST_AUTO_TEST_CASE(Ublas_DoubleProportionalTransfer) { test_DoubleProportionalTransfer(manager, module); }

// // --------------------------------------------------------------------------------------------
// BOOST_AUTO_TEST_CASE(Ublas_DoubleStockTransfer) { test_DoubleStockTransfer(manager, module); }

// // --------------------------------------------------------------------------------------------
// BOOST_AUTO_TEST_CASE(Ublas_DoubleStockAndApplyTransfer) { test_DoubleStockAndApplyTransfer(manager, module); }

// // --------------------------------------------------------------------------------------------
// BOOST_AUTO_TEST_CASE(Ublas_TwoOperationsStockAndProportional) {
//    test_TwoOperationsStockAndProportional(manager, module);
// }

// // --------------------------------------------------------------------------------------------
// BOOST_AUTO_TEST_CASE(Ublas_Kahan_summation_issues_Proportion) {
//    test_Kahan_summation_issues_Proportion(manager, module);
// }

// // --------------------------------------------------------------------------------------------
// BOOST_AUTO_TEST_CASE(Ublas_Kahan_summation_issues_Proportion_with_SpinUp) {
//    test_Kahan_summation_issues_Proportion_with_SpinUp(manager, module);
// }

#if 0  // don't do performance testing, too slow for blas

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Ublas_PerformanceTestProportionalSLEEK) {
	test_PerformanceTestProportionalSLEEK(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Ublas_PerformanceTestStockSLEEK) {
	test_PerformanceTestStockSLEEK(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Ublas_PerformanceTestCBM) {
	test_PerformanceTestCBM(manager, module);
}

#endif

// // --------------------------------------------------------------------------------------------
// BOOST_AUTO_TEST_CASE(Ublas_SubmitOperationAddsToPendingQueue) { SubmitOperationAddsToPendingQueue(manager, module); }

// // --------------------------------------------------------------------------------------------
// BOOST_AUTO_TEST_CASE(Ublas_ClearLastAppliedOperationResults) { ClearLastAppliedOperationResults(manager, module); }

// // --------------------------------------------------------------------------------------------
// BOOST_AUTO_TEST_CASE(Ublas_ApplyOperationsAppendsToCommittedQueue) {
//    ApplyOperationsAppendsToCommittedQueue(manager, module);
// }

// // --------------------------------------------------------------------------------------------
// BOOST_AUTO_TEST_CASE(Ublas_ApplyOperationsCorrectlyUpdatesPoolsForSimpleCase) {
//    ApplyOperationsCorrectlyUpdatesPoolsForSimpleCase(manager, module);
// }

// // --------------------------------------------------------------------------------------------
// BOOST_AUTO_TEST_CASE(Ublas_ApplyOperationsCorrectlyUpdatesPoolsForComplexCase) {
//    ApplyOperationsCorrectlyUpdatesPoolsForComplexCase(manager, module);
// }

// // --------------------------------------------------------------------------------------------
// BOOST_AUTO_TEST_CASE(Ublas_ApplyAndGetOperationsLastApplied) { ApplyAndGetOperationsLastApplied(manager, module); }

// // --------------------------------------------------------------------------------------------
// BOOST_AUTO_TEST_CASE(Ublas_ApplyOperationsClearsPendingQueue) { ApplyOperationsClearsPendingQueue(manager, module); }

BOOST_AUTO_TEST_SUITE_END();
