#ifndef MOJA_TEST_MOCKOPERATIONRESULTCOLLECTION_H_
#define MOJA_TEST_MOCKOPERATIONRESULTCOLLECTION_H_

#include <moja/flint/operationresultcollection.h>

#include <turtle/mock.hpp>

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockOperationResultCollection, flint::OperationResultCollection) {

   explicit MockOperationResultCollection() : flint::OperationResultCollection() {};

   //MOCK_METHOD(setSize, 1, void(int))
   //MOCK_METHOD(count, 0, int())
   //MOCK_METHOD(initialisePools, 0, void())

   MOCK_METHOD(size, 0, int())
   MOCK_METHOD(empty, 0, bool())

   MOCK_NON_CONST_METHOD(begin, 0, flint::OperationResultCollection::iterator(), beginNormal)
   MOCK_CONST_METHOD(begin, 0, flint::OperationResultCollection::const_iterator(), beginConst)

   MOCK_NON_CONST_METHOD(end, 0, flint::OperationResultCollection::iterator(), endNormal)
   MOCK_CONST_METHOD(end, 0, flint::OperationResultCollection::const_iterator(), endConst)

};

}
} // namespace moja::test

#endif // MOJA_TEST_MOCKOPERATIONRESULTCOLLECTION_H_