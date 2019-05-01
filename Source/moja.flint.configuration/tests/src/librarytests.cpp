#include "moja/flint/configuration/library.h"

#include <boost/test/unit_test.hpp>

using moja::flint::configuration::Library;
using moja::flint::configuration::LibraryType;

BOOST_AUTO_TEST_SUITE(LibraryConfigurationTests);

BOOST_AUTO_TEST_CASE(flint_configuration_Library_ConstructorThrowsExceptionIfNameIsEmpty) {
	auto badNames = { "", "  " };
	for (auto name : badNames) {
		BOOST_CHECK_THROW(Library(name, LibraryType::Internal),
						  std::invalid_argument);
	}
}

BOOST_AUTO_TEST_CASE(flint_configuration_Library_GetName) {
	auto name = "test";
	Library library(name, LibraryType::External);
	BOOST_CHECK_EQUAL(name, library.name());
}

BOOST_AUTO_TEST_CASE(flint_configuration_Library_GetType) {
	auto type = LibraryType::Managed;
	Library library("test", type);
	BOOST_CHECK(type == library.type());
}

BOOST_AUTO_TEST_SUITE_END();
