#ifndef MOJA_TEST_MOCKOPERATION_H_
#define MOJA_TEST_MOCKOPERATION_H_

#include <turtle/mock.hpp>
#include "moja/flint/ioperationmanager.h"
#include "moja/flint/ioperation.h"
#include "moja/flint/ioperationresult.h"

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockOperation, flint::IOperation) {
	MockOperation(const flint::ModuleMetaData* md) : IOperation(md) {}

	MOCK_METHOD(addTransfer, 3, flint::IOperation*(const flint::IPool*, const flint::IPool*, double), addTransferShared)

	MOCK_METHOD(computeOperation, 1, std::shared_ptr<flint::IOperationResult>(flint::ITiming&))

	MOCK_METHOD(submitOperation, 0, void())
	
	MOCK_METHOD(transferType, 0, flint::OperationTransferType())
	MOCK_METHOD(metaData, 0, flint::ModuleMetaData*(void))

	MOCK_METHOD(dataPackage, 0, moja::DynamicVar&(void))
	MOCK_METHOD(hasDataPackage, 0, bool(void))
	
	MOCK_METHOD(transferHandlingType, 0, flint::OperationTransferHandlingType())

	static std::shared_ptr<MockOperation> makeMockOperation(const flint::ModuleMetaData& md) {
		return std::make_shared<MockOperation>(&md);
	}
};

}
} // namespace moja::test

#endif // MOJA_TEST_MOCKOPERATION_H_