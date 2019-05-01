#include <moja/environment.h>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(EnvironmentTests);

BOOST_AUTO_TEST_CASE(core_environment_StartProcessFolder) {
    // Just exercise the function.
    moja::Environment::startProcessFolder();
}

BOOST_AUTO_TEST_SUITE_END();