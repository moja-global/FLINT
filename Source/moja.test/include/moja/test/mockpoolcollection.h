#ifndef MOJA_TEST_MOCKPOOLCOLLECTION_H_
#define MOJA_TEST_MOCKPOOLCOLLECTION_H_

#include <moja/flint/ipool.h>
#include <moja/flint/poolcollection.h>

#include <turtle/mock.hpp>

#include <vector>

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockPoolCollection, flint::PoolCollection) {
   MockPoolCollection() : flint::PoolCollection(_pools) {}

   MOCK_METHOD(setSize, 1, void(int))
   MOCK_METHOD(count, 0, int())
   MOCK_METHOD(initialisePools, 0, void())

   MOCK_METHOD(push_back, 1, void(const std::shared_ptr<flint::IPool>& val))
   MOCK_METHOD(pop_back, 0, void())
   MOCK_METHOD(clear, 0, void())

   MOCK_METHOD(size, 0, flint::PoolCollection::size_type(void))

   MOCK_NON_CONST_METHOD(begin, 0, std::vector<std::shared_ptr<flint::IPool>>::iterator(), beginNormal)
   MOCK_CONST_METHOD(begin, 0, std::vector<std::shared_ptr<flint::IPool>>::const_iterator(), beginConst)

   MOCK_NON_CONST_METHOD(end, 0, flint::PoolCollection::iterator(void), endNormal)
   MOCK_CONST_METHOD(end, 0, flint::PoolCollection::const_iterator(void), endConst)

   MOCK_NON_CONST_METHOD(operator[], 1, flint::PoolCollection::reference(size_t), opBrackNormal)
   MOCK_CONST_METHOD(operator[], 1, flint::PoolCollection::const_reference(size_t), opBrackConst)

   MOCK_NON_CONST_METHOD(findPool, 1, const flint::IPool*(std::string&), getPoolByName)
   MOCK_CONST_METHOD(findPool, 1, const flint::IPool*(std::string&), getPoolByNameConst)

   std::vector<std::shared_ptr<flint::IPool>> _pools;
};

}  // namespace test
}  // namespace moja

#endif  // MOJA_TEST_MOCKPOOLCOLLECTION_H_
