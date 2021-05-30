#include <moja/flint/configuration/pool.h>

#include <boost/test/unit_test.hpp>

using moja::flint::configuration::Pool;

namespace pool_configuration {


BOOST_AUTO_TEST_SUITE(PoolConfigurationTests);

BOOST_AUTO_TEST_CASE(flint_configuration_Pool_ConstructorThrowsExceptionIfNameIsEmpty) {
   auto badNames = {"", "  "};
   for (auto name : badNames) {
      BOOST_CHECK_THROW(Pool(name, 1.0), std::invalid_argument);
   }
}

BOOST_AUTO_TEST_CASE(flint_configuration_Pool_GetName) {
   auto name = "test";
   Pool pool(name, 1.0);
   BOOST_CHECK_EQUAL(name, pool.name());
}

BOOST_AUTO_TEST_CASE(flint_configuration_Pool_GetInitValue) {
   auto initValue = 100.0;
   Pool pool("test", initValue);
   BOOST_CHECK_EQUAL(initValue, pool.initValue());
}


BOOST_AUTO_TEST_CASE(flint_configuration_Pool_get_child_default) {
   Pool child_pool("child", 42.0);
   BOOST_CHECK_EQUAL(false, child_pool.parent().has_value());
}

BOOST_AUTO_TEST_CASE(flint_configuration_Pool_get_child) {
   auto parent_pool = "parent";
   Pool child_pool("child", 42.0, parent_pool);
   BOOST_CHECK_EQUAL(true, child_pool.parent().has_value());
   BOOST_CHECK_EQUAL(parent_pool, child_pool.parent().value());
}

}  // namespace pool_configuration
BOOST_AUTO_TEST_SUITE_END();
