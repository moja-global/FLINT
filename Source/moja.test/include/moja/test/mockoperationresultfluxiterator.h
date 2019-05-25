#ifndef MOJA_TEST_MOCKOPERATIONRESULTFLUXITERATOR_H_
#define MOJA_TEST_MOCKOPERATIONRESULTFLUXITERATOR_H_

#include <moja/flint/ioperationresultflux.h>
#include <moja/flint/ioperationresultfluxiterator.h>

#include <turtle/mock.hpp>

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockOperationResultFluxIterator, flint::IOperationResultFluxIterator){

   MOCK_METHOD(createIterator, 1, std::shared_ptr<IOperationResultFluxIterator>(bool))

   MOCK_METHOD(dereference, 0, flint::IOperationResultFlux*(void))

   MOCK_METHOD(increment, 0, void()) MOCK_METHOD(decrement, 0, void())

   MOCK_METHOD(isEqual, 1, bool(flint::IOperationResultFluxIterator&))

};

}
}  // namespace moja

#endif  // MOJA_TEST_MOCKOPERATIONRESULTFLUXITERATOR_H_
