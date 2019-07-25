#ifndef MOJA_TEST_MOCKVARIABLECONFIGURATION_H_
#define MOJA_TEST_MOCKVARIABLECONFIGURATION_H_

#include <moja/flint/configuration/variable.h>

#include <turtle/mock.hpp>

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockVariableConfiguration, moja::flint::configuration::Variable) {
   MockVariableConfiguration() : moja::flint::configuration::Variable("a", 0) {}

   MOCK_METHOD(name, 0, const std::string&());
   MOCK_METHOD(value, 0, const moja::DynamicVar&());
};

}  // namespace test
}  // namespace moja

#endif  // MOJA_TEST_MOCKVARIABLECONFIGURATION_H_