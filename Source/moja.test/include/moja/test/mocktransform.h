#ifndef MOJA_TEST_MOCKTRANSFORM_H_
#define MOJA_TEST_MOCKTRANSFORM_H_

#include <moja/flint/itransform.h>

#include <moja/dynamic.h>

#include <turtle/mock.hpp>

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockTransform, flint::ITransform)
{
   MOCK_METHOD(value, 0, const DynamicVar&(void))
};

}  // namespace test
}  // namespace moja

#endif  // MOJA_TEST_MOCKTRANSFORM_H_