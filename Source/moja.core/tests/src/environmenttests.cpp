#include <moja/environment.h>

#include <boost/test/unit_test.hpp>

namespace flint_environment {

BOOST_AUTO_TEST_SUITE(EnvironmentTests);

BOOST_AUTO_TEST_CASE(Environment_StartProcessFolder) {
   // Just exercise the function.
   const auto folder = moja::Environment::startProcessFolder();
   BOOST_CHECK(folder.length() > 0);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace flint_environment