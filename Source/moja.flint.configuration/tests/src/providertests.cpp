#include "moja/flint/configuration/provider.h"
#include "moja/flint/configuration/configurationexceptions.h"

#include <boost/test/unit_test.hpp>

using moja::flint::configuration::Provider;
using moja::DynamicObject;

BOOST_AUTO_TEST_SUITE(ProviderConfigurationTests);

BOOST_AUTO_TEST_CASE(flint_configuration_Provider_ConstructorThrowsExceptionIfNameIsEmpty) {
	auto badNames = { "", "  " };
	auto badTypes = { "", "  " };
	for (auto name : badNames) {
		BOOST_CHECK_THROW(Provider(name, name, name, DynamicObject()), moja::flint::configuration::ProviderInvalidNameTypeException);
	}
}

BOOST_AUTO_TEST_CASE(flint_configuration_Provider_GetName_EmptySettings) {
	auto name = "testName";
	auto providerType = "testType";

	BOOST_CHECK_THROW(Provider(name, name, name, DynamicObject()), moja::flint::configuration::ProviderSettingsException);
}

BOOST_AUTO_TEST_CASE(flint_configuration_Provider_GetName_BadSettings) {
	auto name = "testName";
	auto providerType = "testType";
	DynamicObject settings;
	settings["missing_type"] = "testProviderType";

	BOOST_CHECK_THROW(Provider(name, name, name, settings), moja::flint::configuration::ProviderSettingsException);
}

BOOST_AUTO_TEST_CASE(flint_configuration_Provider_GetName) {
	auto name = "testName";
	auto lib = "testLib";
	auto providerType = "testType";
	DynamicObject settings;
	settings["type"] = "testProviderType";

	Provider provider(name, lib, providerType, settings);
	BOOST_CHECK_EQUAL(name, provider.name());
	BOOST_CHECK_EQUAL(lib, provider.library());
	BOOST_CHECK_EQUAL(providerType, provider.providerType());
}

BOOST_AUTO_TEST_CASE(flint_configuration_Provider_GetSettings) {
	DynamicObject settings;
	auto k1 = "key1";
	auto v1 = "b";
	auto k2 = "key2";
	auto v2 = 100.0;
	settings[k1] = v1;
	settings[k2] = v2;
	settings["type"] = "must have type";

	Provider provider("testName", "testLib", "testType", settings);
	auto providerSettings = provider.settings();

	std::string providerV1 = providerSettings[k1];
	BOOST_CHECK_EQUAL(v1, providerV1);

	double providerV2 = providerSettings[k2];
	BOOST_CHECK_EQUAL(v2, providerV2);
}

BOOST_AUTO_TEST_SUITE_END();
