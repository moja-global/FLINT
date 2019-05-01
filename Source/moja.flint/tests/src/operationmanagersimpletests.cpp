#include <boost/test/unit_test.hpp>
#include "flinttests.h"
#include "moja/dynamic.h"
#include "moja/flint/timing.h"

#include "moja/flint/operationmanagersimple.h"
#include "operationmanagertestutils.h"
#include <moja/flint/modulebase.h>

#include <memory>

using namespace moja;
using namespace moja::flint;
namespace conf = moja::flint::configuration;

// --------------------------------------------------------------------------------------------
struct PoolInitValueAndResult {
	std::string name;
	double value;
	double result;

	IPool* poolHandle;
};

// --------------------------------------------------------------------------------------------

struct SimpleFixture {
	Timing timing;
	ModuleBase module;
	DynamicObject config;
	OperationManagerSimple manager;

	SimpleFixture() : config({ {"use_kahan", true} }), manager(timing, config) {
		timing.setStartDate(DateTime(2001, 1, 1));
		timing.setStartDate(DateTime(2013, 12, 31));
		timing.init();

		auto& metadata = module.metaData();
		metadata.setDefaults();
		metadata.moduleName = "testModuleSimple";
	}
};

BOOST_FIXTURE_TEST_SUITE(Simple_operationmanagertests, SimpleFixture);

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Simple_NoPoolIteration) {
	test_NoPoolIteration(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Simple_SinglePoolIteration) {
	test_SinglePoolIteration(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Simple_MultiplePoolIteration) {
	test_MultiplePoolIteration(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Simple_NoResultIteration) {
	test_NoResultIteration(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Simple_SingleResultIteration) {
	test_SingleResultIteration(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Simple_MultipleResultIteration) {
	test_MultipleResultIteration(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Simple_NoResultFluxIteration) {
	test_NoResultFluxIteration(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Simple_SingleResultFluxIteration) {
	test_SingleResultFluxIteration(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Simple_MultipleResultFluxIteration) {
	test_MultipleResultFluxIteration(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Simple_SingleProportionTransfer) {
	test_SingleProportionTransfer(manager,module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Simple_SingleStockTransfer) {
	test_SingleStockTransfer(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Simple_DoubleProportionalTransfer) {
	test_DoubleProportionalTransfer(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Simple_DoubleStockTransfer) {
	test_DoubleStockTransfer(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Simple_DoubleStockAndApplyTransfer) {
	test_DoubleStockAndApplyTransfer(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Simple_TwoOperationsStockAndProportional) {
	test_TwoOperationsStockAndProportional(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Simple_Kahan_summation_issues_Proportion) {
	test_Kahan_summation_issues_Proportion(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Simple_Kahan_summation_issues_Proportion_with_SpinUp) {
	test_Kahan_summation_issues_Proportion_with_SpinUp(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Simple_PerformanceTestProportionalSLEEK) {
	test_PerformanceTestProportionalSLEEK(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Simple_PerformanceTestStockSLEEK) {
	test_PerformanceTestStockSLEEK(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Simple_PerformanceTestCBM) {
	test_PerformanceTestCBM(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Simple_SubmitOperationAddsToPendingQueue) {
	SubmitOperationAddsToPendingQueue(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Simple_ClearLastAppliedOperationResults) {
	ClearLastAppliedOperationResults(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Simple_ApplyOperationsAppendsToCommittedQueue) {
	ApplyOperationsAppendsToCommittedQueue(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Simple_ApplyOperationsCorrectlyUpdatesPoolsForSimpleCase) {
	ApplyOperationsCorrectlyUpdatesPoolsForSimpleCase(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Simple_ApplyOperationsCorrectlyUpdatesPoolsForComplexCase) {
	ApplyOperationsCorrectlyUpdatesPoolsForComplexCase(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Simple_ApplyAndGetOperationsLastApplied) {
	ApplyAndGetOperationsLastApplied(manager, module);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Simple_ApplyOperationsClearsPendingQueue) {
	ApplyOperationsClearsPendingQueue(manager, module);
}

BOOST_AUTO_TEST_SUITE_END();
