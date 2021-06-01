#include <moja/flint/configuration/configuration.h>
#include <moja/flint/configuration/configurationexceptions.h>

#include <moja/test/mockexternalvariableconfiguration.h>
#include <moja/test/mocklibraryconfiguration.h>
#include <moja/test/mocklocaldomainconfiguration.h>
#include <moja/test/mockmoduleconfiguration.h>
#include <moja/test/mockpoolconfiguration.h>
#include <moja/test/mockproviderconfiguration.h>
#include <moja/test/mocktransformconfiguration.h>
#include <moja/test/mockvariableconfiguration.h>

#include <moja/datetime.h>
#include <moja/dynamic.h>

#include <boost/test/unit_test.hpp>

#include <turtle/mock.hpp>

namespace flint_configuration {

using moja::DateTime;
using moja::DynamicObject;
using moja::flint::configuration::Configuration;
using moja::flint::configuration::LibraryType;
using moja::flint::configuration::LocalDomainIterationType;
using moja::flint::configuration::LocalDomainType;
using moja::flint::configuration::ModuleOrderOverlapException;
using moja::test::MockExternalVariableConfiguration;
using moja::test::MockLibraryConfiguration;
using moja::test::MockLocalDomainConfiguration;
using moja::test::MockModuleConfiguration;
using moja::test::MockPoolConfiguration;
using moja::test::MockProviderConfiguration;
using moja::test::MockTransformConfiguration;
using moja::test::MockVariableConfiguration;

struct ConfigurationTestsFixture {
   Configuration config = Configuration(DateTime(2000, 1, 1), DateTime(2000, 1, 2));
};

BOOST_FIXTURE_TEST_SUITE(ConfigurationTests, ConfigurationTestsFixture);

BOOST_AUTO_TEST_CASE(Configuration_ConstructorThrowsExceptionIfEndDateIsNotAfterStartDate) {
   DateTime startDate(2000, 5, 5, 5, 5, 5, 5, 5);
   int y = startDate.year();
   int mo = startDate.month();
   int d = startDate.day();
   int h = startDate.hour();
   int mn = startDate.minute();
   int s = startDate.second();
   int ms = startDate.millisecond();
   int us = startDate.microsecond();

   auto badEndDates = {
       DateTime(y, mo, d, h, mn, s, ms, us),     DateTime(y - 1, mo, d, h, mn, s, ms, us),
       DateTime(y, mo - 1, d, h, mn, s, ms, us), DateTime(y, mo, d - 1, h, mn, s, ms, us),
       DateTime(y, mo, d, h - 1, mn, s, ms, us), DateTime(y, mo, d, h, mn - 1, s, ms, us),
       DateTime(y, mo, d, h, mn, s - 1, ms, us), DateTime(y, mo, d, h, mn, s, ms - 1, us),
       DateTime(y, mo, d, h, mn, s, ms, us - 1),
   };

   for (auto endDate : badEndDates) {
      BOOST_CHECK_THROW(Configuration(startDate, endDate), std::invalid_argument);
   }
}

BOOST_AUTO_TEST_CASE(Configuration_AddLibraryThrowsExceptionIfNameIsEmpty) {
   auto badNames = {"", "  "};
   for (auto name : badNames) {
      BOOST_CHECK_THROW(config.addLibrary(name, LibraryType::External), std::invalid_argument);
   }
}

BOOST_AUTO_TEST_CASE(Configuration_AddLibrary) {
   auto name = "test";
   auto type = LibraryType::External;

   config.addLibrary(name, type);
   auto addedLibrary = config.libraries()[0];
   BOOST_CHECK_EQUAL(name, addedLibrary->name());
   BOOST_CHECK(type == addedLibrary->type());
}

BOOST_AUTO_TEST_CASE(Configuration_AddProviderThrowsExceptionIfNameIsEmpty) {
   auto settings = DynamicObject();
   settings["a"] = "b";

   auto badNames = {"", "  "};
   for (auto name : badNames) {
      BOOST_CHECK_THROW(config.addProvider(name, name, name, settings),
                        moja::flint::configuration::ProviderInvalidNameTypeException);
   }
}

BOOST_AUTO_TEST_CASE(Configuration_AddProvider) {
   auto testName = "testName";
   auto testLib = "testLib";
   auto testType = "testType";
   auto settings = DynamicObject();
   settings["type"] = "must have type";

   config.addProvider(testName, testLib, testType, settings);
   auto addedProvider = config.providers()[0];
   BOOST_CHECK_EQUAL(testName, addedProvider->name());
   BOOST_CHECK_EQUAL(testLib, addedProvider->library());
   BOOST_CHECK_EQUAL(testType, addedProvider->providerType());

   std::string expectedSettingValue = settings["type"];
   std::string addedProviderSettingValue = addedProvider->settings()["type"];
   BOOST_CHECK_EQUAL(expectedSettingValue, addedProviderSettingValue);
}

BOOST_AUTO_TEST_CASE(Configuration_AddPoolThrowsExceptionIfNameIsEmpty) {
   auto badNames = {"", "  "};
   for (auto name : badNames) {
      BOOST_CHECK_THROW(config.addPool(name), std::invalid_argument);
   }
}

BOOST_AUTO_TEST_CASE(Configuration_AddPool) {
   auto name = "test";
   auto initValue = 1000.0;

   config.addPool(name, initValue);
   auto addedPool = config.pools()[0];
   BOOST_CHECK_EQUAL(name, addedPool->name());
   BOOST_CHECK_EQUAL(initValue, addedPool->initValue());
}

BOOST_AUTO_TEST_CASE(Configuration_AddVariable) {
   auto name = "test";
   auto value = 100;

   config.addVariable(name, value);
   auto addedVariable = config.variables()[0];
   BOOST_CHECK_EQUAL(name, addedVariable->name());

   int addedVariableValue = addedVariable->value();
   BOOST_CHECK_EQUAL(value, addedVariableValue);
}

BOOST_AUTO_TEST_CASE(Configuration_AddEmptyVariable) {
   auto name = "test";

   config.addVariable(name);
   auto addedVariable = config.variables()[0];
   BOOST_CHECK_EQUAL(name, addedVariable->name());

   auto addedVariableValue = addedVariable->value();
   BOOST_CHECK(addedVariableValue.isEmpty());
}

BOOST_AUTO_TEST_CASE(Configuration_AddExternalVariableThrowsExceptionIfNameIsEmpty) {
   auto badNames = {"", "  "};
   for (auto name : badNames) {
      BOOST_CHECK_THROW(config.addExternalVariable(name, "test", "test", DynamicObject()), std::invalid_argument);
   }
}

BOOST_AUTO_TEST_CASE(Configuration_AddExternalVariableThrowsExceptionIfTransformTypeNameIsEmpty) {
   auto badNames = {"", "  "};
   for (auto name : badNames) {
      BOOST_CHECK_THROW(config.addExternalVariable("test", "test", name, DynamicObject()), std::invalid_argument);
   }
}

BOOST_AUTO_TEST_CASE(Configuration_AddExternalVariable) {
   auto name = "test";
   config.addExternalVariable(name, "test", "transform type", DynamicObject());
   auto addedVariable = config.externalVariables()[0];
   BOOST_CHECK_EQUAL(name, addedVariable->name());
}

BOOST_AUTO_TEST_CASE(Configuration_AddModuleThrowsExceptionIfNameIsEmpty) {
   auto badNames = {"", "  "};
   for (auto name : badNames) {
      BOOST_CHECK_THROW(config.addModule("test", name, 1, false, DynamicObject()), std::invalid_argument);
   }
}

BOOST_AUTO_TEST_CASE(Configuration_AddModule) {
   auto name = "test";
   auto order = 5;
   auto isProxy = false;
   auto settings = DynamicObject();
   settings["a"] = "b";

   config.addModule("test", name, order, isProxy, settings);
   auto addedModule = config.modules()[0];
   BOOST_CHECK_EQUAL(name, addedModule->name());
   BOOST_CHECK_EQUAL(order, addedModule->order());

   std::string expectedSettingValue = settings["a"];
   std::string addedModuleSettingValue = addedModule->settings()["a"];
   BOOST_CHECK_EQUAL(expectedSettingValue, addedModuleSettingValue);
}

BOOST_AUTO_TEST_CASE(Configuration_AddModuleThrowsExceptionOnDuplicateOrder) {
   config.addModule("test", "first", 1, false);
   BOOST_CHECK_THROW(config.addModule("test", "also first", 1, false), ModuleOrderOverlapException);
}

BOOST_AUTO_TEST_CASE(Configuration_ReturnsModulesSortedByOrder) {
   config.addModule("test", "second", 2, false);
   config.addModule("test", "third", 3, false);
   config.addModule("test", "first", 1, false);

   auto actualFirst = config.modules()[0];
   BOOST_CHECK_EQUAL(actualFirst->name(), "first");

   auto actualSecond = config.modules()[1];
   BOOST_CHECK_EQUAL(actualSecond->name(), "second");

   auto actualThird = config.modules()[2];
   BOOST_CHECK_EQUAL(actualThird->name(), "third");
}

BOOST_AUTO_TEST_CASE(Configuration_SetLocalDomainThrowsExceptionIfSequencerIsEmpty) {
   auto badNames = {"", "  "};
   for (auto name : badNames) {
      BOOST_CHECK_THROW(
          config.setLocalDomain(LocalDomainType::SpatiallyReferencedSQL, LocalDomainIterationType::LandscapeTiles, true,
                                1, "sequencer_library", name, "dummy", "dummy", DynamicObject()),
          std::invalid_argument);
   }
}

BOOST_AUTO_TEST_CASE(Configuration_SetLocalDomainThrowsExceptionIfSimulateLandUnitIsEmpty) {
   auto badNames = {"", "  "};
   for (auto name : badNames) {
      BOOST_CHECK_THROW(
          config.setLocalDomain(LocalDomainType::SpatiallyReferencedSQL, LocalDomainIterationType::LandscapeTiles, true,
                                1, "sequencer_library", "dummy", name, "dummy", DynamicObject()),
          std::invalid_argument);
   }
}

BOOST_AUTO_TEST_CASE(Configuration_SetLocalDomain) {
   auto type = LocalDomainType::SpatiallyReferencedSQL;
   auto ittype = LocalDomainIterationType::LandscapeTiles;
   auto sequencerLibrary = "test_sequencer_library";
   auto sequencer = "test_sequencer";
   auto simulateLandUnit = "test_simulatelandunit";
   auto landUnitBuildSuccess = "test_landUnitBuildSuccess";
   auto settings = DynamicObject();
   settings["a"] = "b";

   config.setLocalDomain(type, ittype, true, 1, sequencerLibrary, sequencer, simulateLandUnit, landUnitBuildSuccess,
                         settings);
   auto configLocalDomain = config.localDomain();
   BOOST_CHECK(type == configLocalDomain->type());
   BOOST_CHECK_EQUAL(sequencer, configLocalDomain->sequencer());

   std::string expectedSettingValue = settings["a"];
   std::string actualSettingValue = configLocalDomain->settings()["a"];
   BOOST_CHECK_EQUAL(expectedSettingValue, actualSettingValue);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace flint_configuration
