#include <moja/flint/configuration/variable.h>

#include <moja/dynamic.h>

#include <boost/test/unit_test.hpp>

namespace flint_configuration {

using moja::DynamicVar;
using moja::flint::configuration::Variable;

BOOST_AUTO_TEST_SUITE(VariableConfigurationTests);

BOOST_AUTO_TEST_CASE(Variable_ConstructorThrowsExceptionIfNameIsEmpty) {
   auto badNames = {"", "  "};
   for (auto name : badNames) {
      BOOST_CHECK_THROW(Variable(name, DynamicVar(1.0)), std::invalid_argument);
   }
}

BOOST_AUTO_TEST_CASE(Variable_GetName) {
   auto name = "test";
   Variable variable(name);
   BOOST_CHECK_EQUAL(name, variable.name());
}

BOOST_AUTO_TEST_CASE(Variable_GetValue) {
   DynamicVar value(100.0);
   Variable variable("test", value);
   BOOST_CHECK(value == variable.value());
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace flint_configuration
