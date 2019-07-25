#ifndef MOJA_TEST_MOCKOPERATIONRESULTFLUXCOLLECTION_H_
#define MOJA_TEST_MOCKOPERATIONRESULTFLUXCOLLECTION_H_

#include <moja/flint/operationresultfluxcollection.h>

#include <turtle/mock.hpp>

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockOperationResultFluxCollection, flint::OperationResultFluxCollection) {
   explicit MockOperationResultFluxCollection(std::shared_ptr<flint::IOperationResultFluxIterator> it)
       : flint::OperationResultFluxCollection(it) {}

   MOCK_METHOD(setSize, 1, void(int));
   MOCK_METHOD(count, 0, int());
   MOCK_METHOD(initialisePools, 0, void());

   MOCK_NON_CONST_METHOD(begin, 0, flint::OperationResultFluxCollection::_iterator<false>(), beginNormal);
   MOCK_CONST_METHOD(begin, 0, flint::OperationResultFluxCollection::_iterator<true>(), beginConst);

   MOCK_NON_CONST_METHOD(end, 0, flint::OperationResultFluxCollection::_iterator<false>(), endNormal);
   MOCK_CONST_METHOD(end, 0, flint::OperationResultFluxCollection::_iterator<true>(), endConst);
};

}  // namespace test
}  // namespace moja

#endif  // MOJA_TEST_MOCKOPERATIONRESULTFLUXCOLLECTION_H_