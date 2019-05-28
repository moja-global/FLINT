#ifndef MOJA_TEST_MOCKOPERATION_H_
#define MOJA_TEST_MOCKOPERATION_H_

#include <moja/flint/ioperation.h>
#include <moja/flint/ioperationmanager.h>
#include <moja/flint/ioperationresult.h>

#include <turtle/mock.hpp>

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockOperation, flint::IOperation) {
   MockOperation(const flint::ModuleMetaData* md) : IOperation(md) {}

   MOCK_METHOD(addTransfer, 3, flint::IOperation * (const flint::IPool*, const flint::IPool*, double),
               addTransferShared);

   MOCK_METHOD(computeOperation, 1, std::shared_ptr<flint::IOperationResult>(flint::ITiming&));

   MOCK_METHOD(submitOperation, 0, void());

   MOCK_METHOD(transferType, 0, flint::OperationTransferType());
   MOCK_METHOD(metaData, 0, flint::ModuleMetaData * (void));

   MOCK_METHOD(dataPackage, 0, moja::DynamicVar & (void));
   MOCK_METHOD(hasDataPackage, 0, bool(void));

   MOCK_METHOD(transferHandlingType, 0, flint::OperationTransferHandlingType());

   static std::shared_ptr<MockOperation> makeMockOperation(const flint::ModuleMetaData& md) {
      return std::make_shared<MockOperation>(&md);
   }
};

}  // namespace test
}  // namespace moja

#endif  // MOJA_TEST_MOCKOPERATION_H_