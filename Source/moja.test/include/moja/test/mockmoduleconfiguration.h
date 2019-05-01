#ifndef MOJA_TEST_MOCKMODULECONFIGURATION_H_
#define MOJA_TEST_MOCKMODULECONFIGURATION_H_

#include <turtle/mock.hpp>

#include "moja/flint/configuration/module.h"
#include "moja/dynamic.h"

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockModuleConfiguration, moja::flint::configuration::Module) {
	MockModuleConfiguration() : moja::flint::configuration::Module("libraryMock", "a", 1, false, moja::DynamicObject()) {}

	MOCK_METHOD(libraryName, 0, const std::string&())
	MOCK_METHOD(name, 0, const std::string&())
	MOCK_METHOD(order, 0, int())
	MOCK_METHOD(settings, 0, moja::DynamicObject&())
};

}
} // namespace moja::test

#endif // MOJA_TEST_MOCKMODULECONFIGURATION_H_