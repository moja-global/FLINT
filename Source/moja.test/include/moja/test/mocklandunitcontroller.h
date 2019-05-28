#ifndef MOJA_TEST_MOCKLANDUNITCONTROLLER_H_
#define MOJA_TEST_MOCKLANDUNITCONTROLLER_H_

#include <moja/flint/ilandunitcontroller.h>
#include <moja/flint/ioperationmanager.h>
#include <moja/flint/ivariable.h>
#include <moja/flint/librarymanager.h>

#include <moja/dynamic.h>

#include <turtle/mock.hpp>

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockLandUnitController, flint::ILandUnitController) {
   MOCK_METHOD(configure, 1, void(const moja::flint::configuration::Configuration& config));
   MOCK_METHOD(run, 0, void());
   MOCK_METHOD(id, 0, int());

   MOCK_METHOD(createStockOperation, 1, std::shared_ptr<flint::IOperation>(flint::IModule&), createStockOperation);
   MOCK_METHOD(createStockOperation, 2, std::shared_ptr<flint::IOperation>(flint::IModule&, moja::DynamicVar&),
               createStockOperationWithData);
   MOCK_METHOD(createProportionalOperation, 1, std::shared_ptr<flint::IOperation>(flint::IModule&),
               createProportionalOperation);
   MOCK_METHOD(createProportionalOperation, 2, std::shared_ptr<flint::IOperation>(flint::IModule&, moja::DynamicVar&),
               createProportionalOperationWithData);

   MOCK_METHOD(submitOperation, 1, void(std::shared_ptr<flint::IOperation>));
   MOCK_METHOD(applyOperations, 0, void());

   MOCK_METHOD(clearLastAppliedOperationResults, 0, void());
   MOCK_METHOD(clearAllOperationResults, 0, void());

   MOCK_NON_CONST_METHOD(operationManager, 0, flint::IOperationManager * (), getOpMan);
   MOCK_CONST_METHOD(operationManager, 0, const flint::IOperationManager*(), getOpManConst);

   MOCK_METHOD(getVariable, 1, flint::IVariable * (const std::string&));
   MOCK_METHOD(hasVariable, 1, bool(const std::string&));
   MOCK_METHOD(variables, 0, std::vector<std::shared_ptr<flint::IVariable>>(void));

   MOCK_METHOD(timing, 0, flint::ITiming & ());

   MOCK_METHOD(config, 0, flint::configuration::Configuration * ());
};

}  // namespace test
}  // namespace moja

#endif  // MOJA_TEST_MOCKLANDUNITCONTROLLER_H_