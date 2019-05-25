#ifndef MOJA_TEST_MOCKOPERATIONRESULT_H_
#define MOJA_TEST_MOCKOPERATIONRESULT_H_

#include <moja/flint/imodule.h>
#include <moja/flint/ioperationresult.h>
#include <moja/flint/timing.h>

#include <turtle/mock.hpp>
 
namespace moja {
namespace test {

MOCK_BASE_CLASS(MockOperationResult, flint::IOperationResult) {

   MOCK_METHOD(operationResultFluxCollection, 0, flint::OperationResultFluxCollection(void))

   MOCK_METHOD(transferType, 0, flint::OperationTransferType())
   MOCK_METHOD(metaData, 0, flint::ModuleMetaData*(void))
   MOCK_METHOD(dataPackage, 0, moja::DynamicVar&(void))
   MOCK_METHOD(hasDataPackage, 0, bool(void))

   MOCK_METHOD(timingWhenApplied, 0, flint::Timing&(void))
   MOCK_METHOD(setTimingWhenApplied, 1, void(const flint::Timing&))

};

}
} // namespace moja::test

#endif // MOJA_TEST_MOCKOPERATIONRESULT_H_