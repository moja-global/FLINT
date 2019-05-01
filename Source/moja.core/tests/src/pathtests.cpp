#include <moja/environment.h>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(PathTests);

BOOST_AUTO_TEST_CASE(core_path_startProcessFolder) {
	auto path = moja::Environment::startProcessFolder();
}

BOOST_AUTO_TEST_SUITE_END();