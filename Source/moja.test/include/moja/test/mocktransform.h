#ifndef MOJA_TEST_MOCKTRANSFORM_H_
#define MOJA_TEST_MOCKTRANSFORM_H_

#include <turtle/mock.hpp>
#include "moja/dynamic.h"
#include "moja/flint/itransform.h"

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockTransform, flint::ITransform) {
	MOCK_METHOD(value, 0, const DynamicVar&(void));
};

}
} // namespace moja::test

#endif // MOJA_TEST_MOCKTRANSFORM_H_