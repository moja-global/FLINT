#ifndef MOJA_TEST_MOCKOPERATIONMANAGER_H_
#define MOJA_TEST_MOCKOPERATIONMANAGER_H_

#include <turtle/mock.hpp>
#include "moja/flint/ioperation.h"
#include "moja/flint/ioperationmanager.h"
#include "moja/flint/operationresultcollection.h"
#include "moja/flint/itransform.h"

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockOperationManager, flint::IOperationManager) {

	//MOCK_METHOD(createStockOperation, 1,		std::shared_ptr<flint::IOperation>(flint::IModule&))
	//MOCK_METHOD(createProportionalOperation, 1, std::shared_ptr<flint::IOperation>(flint::IModule&))

	MOCK_METHOD(createStockOperation, 1,		std::shared_ptr<flint::IOperation>(flint::IModule&), createStockOperation)
	MOCK_METHOD(createStockOperation, 2,		std::shared_ptr<flint::IOperation>(flint::IModule&, moja::DynamicVar&), createStockOperationWithData)
	MOCK_METHOD(createProportionalOperation, 1, std::shared_ptr<flint::IOperation>(flint::IModule&), createProportionalOperation)
	MOCK_METHOD(createProportionalOperation, 2, std::shared_ptr<flint::IOperation>(flint::IModule&, moja::DynamicVar&), createProportionalOperationWithData)

	MOCK_METHOD(applyOperations, 0, void())

	MOCK_METHOD(clearAllOperationResults, 0, void())
	MOCK_METHOD(clearLastAppliedOperationResults, 0, void())

	MOCK_METHOD(operationResultsPending, 0, const flint::OperationResultCollection&())
	MOCK_METHOD(operationResultsLastApplied, 0, const flint::OperationResultCollection&())
	MOCK_METHOD(operationResultsCommitted, 0, const flint::OperationResultCollection&())

	MOCK_METHOD(hasOperationResultsLastApplied, 0, bool())

	MOCK_METHOD(poolCollection, 0, flint::PoolCollection())

	MOCK_METHOD(initialisePools, 0, void())
	MOCK_METHOD(poolCount, 0, int())

	MOCK_METHOD(addPool, 2, const flint::IPool*(const std::string&, double), addPool1)
	MOCK_METHOD(addPool, 6, const flint::IPool*(const std::string&, const std::string&, const std::string&, double, int, double), addPool2)
	MOCK_METHOD(addPool, 2, const flint::IPool*(flint::PoolMetaData&, double), addPool3)
    MOCK_METHOD(addPool, 6, const flint::IPool*(const std::string&, const std::string&, const std::string&, double, int, const std::shared_ptr<flint::ITransform>), addPool4)

	MOCK_METHOD(getPool, 1, const flint::IPool*(const std::string&), getPoolByName)
	MOCK_METHOD(getPool, 1, const flint::IPool*(int), getPoolByIndex)

	//MOCK_METHOD(name, 0, std::string())
	//MOCK_METHOD(version, 0, std::string())
	//MOCK_METHOD(config, 0, std::string())

	std::string name() const override { return "MockOperationManager"; }
	std::string version() const override { return "MockOperationManager"; }
	std::string config() const override { return "MockOperationManager"; }

	MOCK_METHOD(submitOperation, 1, void(flint::IOperation*))
};

}
} // namespace moja::test

#endif // MOJA_TEST_MOCKOPERATIONMANAGER_H_