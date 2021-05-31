#pragma once

#include <moja/flint/ioperation.h>
#include <moja/flint/ioperationmanager.h>
#include <moja/flint/itransform.h>
#include <moja/flint/operationresultcollection.h>

#include <turtle/mock.hpp>

namespace moja::test {

MOCK_BASE_CLASS(MockOperationManager, flint::IOperationManager) {

   MOCK_METHOD(createStockOperation, 1, std::shared_ptr<flint::IOperation>(flint::IModule&), createStockOperation)
   MOCK_METHOD(createStockOperation, 2, std::shared_ptr<flint::IOperation>(flint::IModule&, moja::DynamicVar&),
               createStockOperationWithData)
   MOCK_METHOD(createProportionalOperation, 1, std::shared_ptr<flint::IOperation>(flint::IModule&),
               createProportionalOperation)
   MOCK_METHOD(createProportionalOperation, 2, std::shared_ptr<flint::IOperation>(flint::IModule&, moja::DynamicVar&),
               createProportionalOperationWithData)

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

   MOCK_METHOD(addPool, 7, flint::IPool* (const std::string&, const std::string&, const std::string&, double, int, std::optional<double>,
                               flint::IPool*), addPool1)
   MOCK_METHOD(addPool, 7, flint::IPool* (const std::string&, const std::string&, const std::string&, double, int,
                               const std::shared_ptr<flint::ITransform>, flint::IPool*), addPool2)

   MOCK_CONST_METHOD(getPool, 1, const flint::IPool*(const std::string&), getPoolByName_const)
   MOCK_NON_CONST_METHOD(getPool, 1, flint::IPool* (const std::string&), getPoolByName)
   MOCK_METHOD(getPool, 1, const flint::IPool*(int), getPoolByIndex)

   // MOCK_METHOD(name, 0, std::string());
   // MOCK_METHOD(version, 0, std::string());
   // MOCK_METHOD(config, 0, std::string());

   std::string name() const override { return "MockOperationManager"; }
   std::string version() const override { return "MockOperationManager"; }
   std::string config() const override { return "MockOperationManager"; }

   MOCK_METHOD(submitOperation, 1, void(flint::IOperation*))
};

}  // namespace test
