#ifndef MOJA_TEST_MOCKVARIABLE_H_
#define MOJA_TEST_MOCKVARIABLE_H_

#include <moja/flint/ivariable.h>

#include <moja/dynamic.h>

#include <turtle/mock.hpp>

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockVariable, flint::IVariable) {
   MOCK_METHOD(info, 0, const flint::VariableInfo&(void));
   MOCK_METHOD(value, 0, const DynamicVar&(void));
   MOCK_METHOD(set_value, 1, void(DynamicVar));
   MOCK_METHOD(reset_value, 0, void(void));
   MOCK_METHOD(isExternal, 0, bool(void));
   MOCK_METHOD(isFlintData, 0, bool(void));
};

}  // namespace test
}  // namespace moja

#endif  // MOJA_TEST_MOCKVARIABLE_H_
