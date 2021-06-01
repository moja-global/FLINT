#include <moja/flint/configuration/module.h>

#include <moja/dynamic.h>

#include <boost/test/unit_test.hpp>

namespace flint_configuration {

using moja::DynamicObject;
using moja::flint::configuration::Module;

BOOST_AUTO_TEST_SUITE(ModuleConfigurationTests);

BOOST_AUTO_TEST_CASE(Module_ConstructorThrowsExceptionIfNameIsEmpty) {
   auto badNames = {"", "  "};
   for (auto name : badNames) {
      BOOST_CHECK_THROW(Module("test", name, 1, false, DynamicObject()), std::invalid_argument);
   }
}

BOOST_AUTO_TEST_CASE(Module_GetName) {
   auto name = "test";
   Module module("test", name, 1, false, DynamicObject());
   BOOST_CHECK_EQUAL(name, module.name());
}

BOOST_AUTO_TEST_CASE(Module_GetOrder) {
   int order = 99;
   Module module("test", "test", order, false, DynamicObject());
   BOOST_CHECK_EQUAL(order, module.order());
}

BOOST_AUTO_TEST_CASE(Module_GetSettings) {
   moja::DynamicObject settings;
   auto k1 = "key1";
   auto v1 = "b";
   auto k2 = "key2";
   auto v2 = 100.0;
   settings[k1] = v1;
   settings[k2] = v2;

   Module module("test", "test", 1, false, settings);
   auto moduleSettings = module.settings();

   std::string moduleV1 = moduleSettings[k1];
   BOOST_CHECK_EQUAL(v1, moduleV1);

   double moduleV2 = moduleSettings[k2];
   BOOST_CHECK_EQUAL(v2, moduleV2);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace flint_configuration
