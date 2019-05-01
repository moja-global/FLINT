#ifndef MOJA_TEST_MOCKOPERATIONRESULTFLUX_H_
#define MOJA_TEST_MOCKOPERATIONRESULTFLUX_H_

#include <turtle/mock.hpp>
#include "moja/flint/ioperationresultflux.h"
#include "moja/flint/imodule.h"

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockOperationResultFlux, flint::IOperationResultFlux) {

	MOCK_METHOD(source, 0, int())
	MOCK_METHOD(sink, 0, int())
	MOCK_METHOD(value, 0, double())

	MOCK_METHOD(transferType, 0, flint::OperationTransferType())
	MOCK_METHOD(metaData, 0, flint::ModuleMetaData*(void))
};

}
} // namespace moja::test

#endif // MOJA_TEST_MOCKOPERATIONRESULTFLUX_H_