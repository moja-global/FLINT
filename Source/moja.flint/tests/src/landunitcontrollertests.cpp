#include <boost/test/unit_test.hpp>
#include <turtle/mock.hpp>

#include "moja/dynamic.h"
#include "moja/flint/imodule.h"
#include "moja/flint/landunitcontroller.h"
#include "moja/flint/flintexceptions.h"
#include "moja/test/mockpool.h"
#include "moja/test/mockvariable.h"
#include "moja/test/mockproviderrelationalinterface.h"
#include "moja/test/mocktransform.h"
#include "moja/test/mockmodule.h"

#include <memory>
#include <map>
#include "moja/flint/ioperationmanager.h"

namespace mf = moja::flint;
using moja::DynamicVar;
namespace mocks = moja::test;

struct LandUnitControllerTestsFixture {
	mf::LandUnitController controller;
};

BOOST_FIXTURE_TEST_SUITE(LandUnitControllerTests, LandUnitControllerTestsFixture)

BOOST_AUTO_TEST_CASE(flint_LandUnitController_AddPoolThrowsExceptionIfNameIsEmpty) {
	auto badNames = { "", "  " };
	for (auto name : badNames) {
		BOOST_CHECK_THROW(controller.operationManager()->addPool(name), std::invalid_argument);
	}
}

BOOST_AUTO_TEST_CASE(flint_LandUnitController_AddPoolThrowsExceptionIfValueIsNegative) {
	auto badValues = { -0.1, -100.0 };
	for (auto value : badValues) {
		BOOST_CHECK_THROW(controller.operationManager()->addPool("test", value), std::invalid_argument);
	}
}

BOOST_AUTO_TEST_CASE(flint_LandUnitController_AddPoolIncrementsPoolCount) {
	for (int i = 1; i < 100; i++) {
		controller.operationManager()->addPool("p" + std::to_string(i));
		BOOST_CHECK_EQUAL(i, controller.operationManager()->poolCount());
	}
}

BOOST_AUTO_TEST_CASE(flint_LandUnitController_AddPoolAssignsSequentialIndices) {
	for (int i = 0; i < 100; i++) {
		auto name = "p" + std::to_string(i);
		controller.operationManager()->addPool(name);
		auto pool = controller.operationManager()->getPool(name);
		BOOST_CHECK_EQUAL(i, pool->idx());
	}
}

BOOST_AUTO_TEST_CASE(flint_LandUnitController_GetPoolByName) {
	std::map<std::string, double> pools { { "p1", 0.0 }, { "p2", 100.0 } };
	for (auto pool : pools) {
		controller.operationManager()->addPool(pool.first, pool.second);
	}
	controller.initialiseData(true);

	for (auto expected : pools) {
		auto actual = controller.operationManager()->getPool(expected.first);
		BOOST_CHECK_EQUAL(expected.first, actual->name());
		BOOST_CHECK_EQUAL(expected.second, actual->value());
	}
}

BOOST_AUTO_TEST_CASE(flint_LandUnitController_GetPoolByNameThrowsExceptionIfPoolNotFound) {
	controller.operationManager()->addPool("test");
	BOOST_CHECK_THROW(controller.operationManager()->getPool("invalid pool"), mf::PoolNotFoundException);
}

BOOST_AUTO_TEST_CASE(flint_LandUnitController_GetPoolByIndex) {
	std::vector<std::pair<std::string, double>> pools { { "p1", 0.0 }, { "p2", 100.0 } };
	for (auto pool : pools) {
		controller.operationManager()->addPool(pool.first, pool.second);
	}

	controller.initialiseData(true);

	for (int i = 0; i < pools.size(); i++) {
		auto expected = pools[i];
		auto actual = controller.operationManager()->getPool(i);
		BOOST_CHECK_EQUAL(expected.first, actual->name());
		BOOST_CHECK_EQUAL(expected.second, actual->value());
	}
}

BOOST_AUTO_TEST_CASE(flint_LandUnitController_GetPoolByIndexThrowsExceptionIfIndexOutOfRange) {
	controller.operationManager()->addPool("test");
	for (int i : { -100, -1, 1, 100 }) {
		BOOST_CHECK_THROW(controller.operationManager()->getPool(i), mf::PoolNotFoundException);
	}
}

BOOST_AUTO_TEST_CASE(flint_LandUnitController_PoolValueIterator) {
	std::vector<std::pair<std::string, double>> pools { { "p1", 0.0 }, { "p2", 100.0 } };
	for (auto pool : pools) {
		controller.operationManager()->addPool(pool.first, pool.second);
	}

	controller.initialiseData(true);

	for (auto pool : controller.operationManager()->poolCollection()) {
		auto expectedValue = pools[pool->idx()].second;
		auto actualValue = pool->value();
		BOOST_CHECK_EQUAL(expectedValue, actualValue);
	}
}

BOOST_AUTO_TEST_CASE(flint_LandUnitController_PoolObjIterator) {
	std::vector<std::pair<std::string, double>> pools{ { "p1", 0.0 }, { "p2", 100.0 } };
	for (auto pool : pools) {
		controller.operationManager()->addPool(pool.first, pool.second);
	}

	controller.initialiseData(true);

	for (auto pool : controller.operationManager()->poolCollection()) {
		auto expected = pools[pool->idx()];
		BOOST_CHECK_EQUAL(expected.first, pool->name());
		BOOST_CHECK_EQUAL(expected.second, pool->value());
	}
}

BOOST_AUTO_TEST_CASE(flint_LandUnitController_AddVariableThrowsExceptionIfNameIsEmpty) {
	auto var = std::make_shared<mocks::MockVariable>();
	MOCK_EXPECT(var->isExternal).returns(false);
	MOCK_EXPECT(var->isFlintData).returns(false);
	MOCK_EXPECT(var->value).returns(DynamicVar(0));

	auto badNames = { "", "  " };
	for (auto name : badNames) {
		BOOST_CHECK_THROW(controller.addVariable(name, var), std::invalid_argument);
	}
}

BOOST_AUTO_TEST_CASE(flint_LandUnitController_AddVariable) {
	auto var = std::make_shared<mocks::MockVariable>();
	MOCK_EXPECT(var->isExternal).returns(false);
	MOCK_EXPECT(var->isFlintData).returns(false);
	MOCK_EXPECT(var->value).returns(DynamicVar(0));

	controller.addVariable("test", var);
}

BOOST_AUTO_TEST_CASE(flint_LandUnitController_GetVariableThrowsExceptionIfVariableNotFound) {
	BOOST_CHECK_THROW(controller.getVariable("not found"), mf::VariableNotFoundException);
}

BOOST_AUTO_TEST_CASE(flint_LandUnitController_GetVariable) {
	int expectedValue = 100;
	auto var = std::make_shared<mocks::MockVariable>();
	MOCK_EXPECT(var->isExternal).returns(false);
	MOCK_EXPECT(var->isFlintData).returns(false);
	MOCK_EXPECT(var->value).returns(DynamicVar(expectedValue));

	controller.addVariable("test", var);
	auto actual = controller.getVariable("test");
	BOOST_CHECK_EQUAL(expectedValue, actual->value().convert<int>());
}

BOOST_AUTO_TEST_CASE(flint_LandUnitController_CreateProportionalOperation) {
	auto module = std::make_shared<mocks::MockModule>();
	MOCK_EXPECT(module->metaData).returns(mf::ModuleMetaData());
	controller.createProportionalOperation(*module.get());
}

BOOST_AUTO_TEST_CASE(flint_LandUnitController_CreateStockOperation) {
	auto module = std::make_shared<mocks::MockModule>();
	MOCK_EXPECT(module->metaData).returns(mf::ModuleMetaData());
	controller.createStockOperation(*module.get());
}

BOOST_AUTO_TEST_SUITE_END();
