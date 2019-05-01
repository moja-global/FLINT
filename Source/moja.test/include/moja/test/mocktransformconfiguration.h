#ifndef MOJA_TEST_MOCKTRANSFORMCONFIGURATION_H_
#define MOJA_TEST_MOCKTRANSFORMCONFIGURATION_H_

#include <turtle/mock.hpp>
#include "moja/flint/configuration/transform.h"
#include "moja/dynamic.h"

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockTransformConfiguration, moja::flint::configuration::Transform) {
	MockTransformConfiguration() : Transform("mock", "libraryTest", DynamicObject()) {}
	MOCK_METHOD(libraryName, 0, const std::string&())
	MOCK_METHOD(typeName, 0, const std::string&())
	MOCK_METHOD(settings, 0, const moja::DynamicObject&())
};

}
} // namespace moja::test

#endif // MOJA_TEST_MOCKTRANSFORMCONFIGURATION_H_