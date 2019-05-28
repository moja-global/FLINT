#include <moja/flint/configuration/transform.h>

#include <boost/test/unit_test.hpp>

using moja::DynamicObject;
using moja::flint::configuration::Transform;

BOOST_AUTO_TEST_SUITE(TransformConfigurationTests);

BOOST_AUTO_TEST_CASE(flint_configuration_Transform_ConstructThrowsExceptionIfTypeNameIsEmpty) {
   auto badNames = {"", "  "};
   for (auto name : badNames) {
      BOOST_CHECK_THROW(Transform("test", name, DynamicObject()), std::invalid_argument);
   }
}

BOOST_AUTO_TEST_CASE(flint_configuration_Transform_ConstructThrowsExceptionIfLibraryNameIsEmpty) {
   auto badNames = {"", "  "};
   for (auto name : badNames) {
      BOOST_CHECK_THROW(Transform(name, "test", DynamicObject()), std::invalid_argument);
   }
}

BOOST_AUTO_TEST_CASE(flint_configuration_Transform_GetType) {
   auto typeName = "test";
   Transform transform("test", typeName, DynamicObject());
   BOOST_CHECK(typeName == transform.typeName());
}

BOOST_AUTO_TEST_CASE(flint_configuration_Transform_GetSettings) {
   DynamicObject settings;
   auto k1 = "key1";
   auto v1 = "b";
   auto k2 = "key2";
   auto v2 = 100.0;
   settings[k1] = v1;
   settings[k2] = v2;

   Transform transform("test", "TestTransform", settings);
   auto transformSettings = transform.settings();

   std::string transformV1 = transformSettings[k1];
   BOOST_CHECK_EQUAL(v1, transformV1);

   double transformV2 = transformSettings[k2];
   BOOST_CHECK_EQUAL(v2, transformV2);
}

BOOST_AUTO_TEST_SUITE_END();
