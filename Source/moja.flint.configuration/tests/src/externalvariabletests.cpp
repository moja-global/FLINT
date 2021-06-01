#include <moja/flint/configuration/externalvariable.h>

#include <moja/test/mocktransformconfiguration.h>

#include <moja/dynamic.h>

#include <boost/test/unit_test.hpp>

namespace flint_configuration {

using moja::DynamicObject;
using moja::flint::configuration::ExternalVariable;
using moja::test::MockTransformConfiguration;

BOOST_AUTO_TEST_SUITE(ExternalVariableConfigurationTests);

BOOST_AUTO_TEST_CASE(ExternalVariable_ConstructorThrowsExceptionIfNameIsEmpty) {
   auto badNames = {"", "  "};
   for (auto name : badNames) {
      BOOST_CHECK_THROW(ExternalVariable(name, std::make_shared<MockTransformConfiguration>()), std::invalid_argument);
   }
}

BOOST_AUTO_TEST_CASE(ExternalVariable_GetName) {
   auto name = "test";
   ExternalVariable variable(name, std::make_shared<MockTransformConfiguration>());
   BOOST_CHECK_EQUAL(name, variable.name());
}
BOOST_AUTO_TEST_SUITE_END()

}  // namespace flint_configuration
