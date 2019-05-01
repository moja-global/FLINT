#ifndef MOJA_TEST_MOCKEXTERNALVARIABLECONFIGURATION_H_
#define MOJA_TEST_MOCKEXTERNALVARIABLECONFIGURATION_H_

#include <turtle/mock.hpp>

#include "moja/flint/configuration/externalvariable.h"
#include "moja/flint/configuration/transform.h"
#include "moja/test/mocktransformconfiguration.h"

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockExternalVariableConfiguration, moja::flint::configuration::ExternalVariable) {
	MockExternalVariableConfiguration() : moja::flint::configuration::ExternalVariable("a", std::make_shared<moja::test::MockTransformConfiguration>()) {}

	MOCK_METHOD(name, 0, const std::string&())
	MOCK_METHOD(transform, 0, const moja::flint::configuration::Transform&())
};

}
} // namespace moja::test

#endif // MOJA_TEST_MOCKEXTERNALVARIABLECONFIGURATION_H_