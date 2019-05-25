#ifndef MOJA_TEST_MOCKOPERATIONRESULTFLUX_H_
#define MOJA_TEST_MOCKOPERATIONRESULTFLUX_H_

#include <moja/flint/imodule.h>
#include <moja/flint/ioperationresultflux.h>

#include <turtle/mock.hpp>

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockOperationResultFlux, flint::IOperationResultFlux){

   MOCK_METHOD(source, 0, int()) MOCK_METHOD(sink, 0, int()) MOCK_METHOD(value, 0, double())

   MOCK_METHOD(transferType, 0, flint::OperationTransferType())
   MOCK_METHOD(metaData, 0, flint::ModuleMetaData*(void))};

}
}  // namespace moja

#endif  // MOJA_TEST_MOCKOPERATIONRESULTFLUX_H_