#ifndef MOJA_TEST_MOCKLANDUNITDATAWRAPPER_H_
#define MOJA_TEST_MOCKLANDUNITDATAWRAPPER_H_

#include <moja/flint/ilandunitdatawrapper.h>
#include <moja/flint/ioperation.h>

#include <moja/flint/configuration/configuration.h>

#include <turtle/mock.hpp>

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockLandUnitDataWrapper, flint::ILandUnitDataWrapper) {
   // MOCK_METHOD(createProportionalOperation, 0, std::shared_ptr<flint::IOperation>())
   // MOCK_METHOD(createStockOperation, 0, std::shared_ptr<flint::IOperation>())

   MOCK_METHOD(createStockOperation, 0, std::shared_ptr<flint::IOperation>(), createStockOperation);
   MOCK_METHOD(createStockOperation, 1, std::shared_ptr<flint::IOperation>(moja::DynamicVar&),
               createStockOperationWithData);
   MOCK_METHOD(createProportionalOperation, 0, std::shared_ptr<flint::IOperation>(), createProportionalOperation);
   MOCK_METHOD(createProportionalOperation, 1, std::shared_ptr<flint::IOperation>(moja::DynamicVar&),
               createProportionalOperationWithData);

   MOCK_METHOD(submitOperation, 1, void(std::shared_ptr<flint::IOperation>));
   MOCK_METHOD(applyOperations, 0, void());

   MOCK_METHOD(getOperationPendingIterator, 0, const flint::OperationResultCollection&());
   MOCK_METHOD(getOperationLastAppliedIterator, 0, const flint::OperationResultCollection&());
   MOCK_METHOD(getOperationCommittedIterator, 0, const flint::OperationResultCollection&());

   MOCK_METHOD(hasLastAppliedOperationResults, 0, bool());

   MOCK_METHOD(clearLastAppliedOperationResults, 0, void());
   MOCK_METHOD(clearAllOperationResults, 0, void());

   MOCK_METHOD(poolCollection, 0, flint::PoolCollection());

   MOCK_METHOD(getPoolCount, 0, int());
   MOCK_METHOD(getPool, 1, const flint::IPool*(const std::string&), getPoolByName);
   MOCK_METHOD(getPool, 1, const flint::IPool*(int), getPoolByIndex);

   MOCK_METHOD(hasVariable, 1, bool(const std::string&));
   MOCK_METHOD(getVariable, 1, flint::IVariable * (const std::string&));
   MOCK_METHOD(variables, 0, std::vector<std::shared_ptr<flint::IVariable>>(void));

   MOCK_METHOD(operationManager, 0, flint::IOperationManager * ());

   MOCK_METHOD(timing, 0, ITiming * ());

   MOCK_METHOD(config, 0, flint::configuration::Configuration * ());

   MOCK_METHOD(setLandUnitController, 1, void(flint::ILandUnitController*));
};

}  // namespace test
}  // namespace moja

#endif  // MOJA_TEST_MOCKLANDUNITDATAWRAPPER_H_