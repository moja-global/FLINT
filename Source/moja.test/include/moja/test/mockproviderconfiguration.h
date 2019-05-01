#ifndef MOJA_TEST_MOCKPROVIDERCONFIGURATION_H_
#define MOJA_TEST_MOCKPROVIDERCONFIGURATION_H_

#include <turtle/mock.hpp>

#include "moja/flint/configuration/provider.h"
#include "moja/dynamic.h"

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockProviderConfiguration, moja::flint::configuration::Provider) {
	MockProviderConfiguration() : moja::flint::configuration::Provider("a", "b", "c", moja::DynamicObject()) {}

	MOCK_METHOD(name, 0, const std::string&())
	MOCK_METHOD(library, 0, const std::string&())
	MOCK_METHOD(providerType, 0, const std::string&())
	MOCK_METHOD(settings, 0, const moja::DynamicObject&())
};

}
} // namespace moja::test

#endif // MOJA_TEST_MOCKPROVIDERCONFIGURATION_H_