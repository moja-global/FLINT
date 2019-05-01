#include "moja/test/mocktransformconfiguration.h"

#include "moja/flint/configuration/externalvariable.h"

#include "moja/dynamic.h"

#include <boost/test/unit_test.hpp>

using moja::flint::configuration::ExternalVariable;
using moja::DynamicObject;
using moja::test::MockTransformConfiguration;

BOOST_AUTO_TEST_SUITE(ExternalVariableConfigurationTests);

BOOST_AUTO_TEST_CASE(flint_configuration_ExternalVariable_ConstructorThrowsExceptionIfNameIsEmpty) {
	auto badNames = { "", "  " };
	for (auto name : badNames) {
		BOOST_CHECK_THROW(
				ExternalVariable(name, std::make_shared<MockTransformConfiguration>()),
				std::invalid_argument);
	}
}

BOOST_AUTO_TEST_CASE(flint_configuration_ExternalVariable_GetName) {
	auto name = "test";
	ExternalVariable variable(name, std::make_shared<MockTransformConfiguration>());
	BOOST_CHECK_EQUAL(name, variable.name());
}
BOOST_AUTO_TEST_SUITE_END();
