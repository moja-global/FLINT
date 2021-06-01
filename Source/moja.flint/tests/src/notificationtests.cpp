#include "notificationtests.h"

#include <moja/flint/flintexceptions.h>

#include <moja/flint/configuration/configuration.h>
#include <moja/flint/configuration/iterationbase.h>
#include <moja/flint/configuration/localdomain.h>

#include <moja/logging.h>

#include <boost/test/unit_test.hpp>

#include <fmt/format.h>

namespace flint {

using namespace moja::flint;

using moja::DateTime;
using moja::DynamicObject;

// --------------------------------------------------------------------------------------------
// -- Module Factory Stuff

struct Test_ObjectHolder {
   Test_ObjectHolder() {}

   std::vector<std::string> _callSequence;
};

Test_ObjectHolder testObjectHolder;

inline IModule* CreateTestSequencer01() { return new TestSequencer01(); }
inline IModule* CreateTestSequencer02() { return new TestSequencer02(); }
inline IModule* CreateTestSequencer03() { return new TestSequencer03(); }
inline IModule* CreateTestNotificationModule() { return new TestNotificationModule(testObjectHolder._callSequence); }

extern "C" int getModuleRegistrations(ModuleRegistration* outModuleRegistrations) {
   auto index = 0;
   outModuleRegistrations[index++] = ModuleRegistration{"TestSequencer01", &CreateTestSequencer01};
   outModuleRegistrations[index++] = ModuleRegistration{"TestSequencer02", &CreateTestSequencer02};
   outModuleRegistrations[index++] = ModuleRegistration{"TestSequencer03", &CreateTestSequencer03};
   outModuleRegistrations[index++] = ModuleRegistration{"TestNotificationModule01", &CreateTestNotificationModule};
   outModuleRegistrations[index++] = ModuleRegistration{"TestNotificationModule02", &CreateTestNotificationModule};
   outModuleRegistrations[index++] = ModuleRegistration{"TestNotificationModule03", &CreateTestNotificationModule};
   outModuleRegistrations[index++] = ModuleRegistration{"TestNotificationModule04", &CreateTestNotificationModule};
   outModuleRegistrations[index++] = ModuleRegistration{"TestNotificationModule05", &CreateTestNotificationModule};
   outModuleRegistrations[index++] = ModuleRegistration{"TestNotificationModule06", &CreateTestNotificationModule};
   outModuleRegistrations[index++] = ModuleRegistration{"TestNotificationModule07", &CreateTestNotificationModule};
   outModuleRegistrations[index++] = ModuleRegistration{"TestNotificationModule08", &CreateTestNotificationModule};
   outModuleRegistrations[index++] = ModuleRegistration{"TestNotificationModule09", &CreateTestNotificationModule};
   outModuleRegistrations[index++] = ModuleRegistration{"TestNotificationModule10", &CreateTestNotificationModule};
   return index;
}

extern "C" int getTransformRegistrations(TransformRegistration* outTransformRegistrations) {
   auto index = 0;
   return index;
}

extern "C" int getFlintDataRegistrations(moja::flint::FlintDataRegistration* outFlintDataRegistrations) {
   auto index = 0;
   return index;
}

extern "C" int getFlintDataFactoryRegistrations(
    moja::flint::FlintDataFactoryRegistration* outFlintDataFactoryRegistrations) {
   auto index = 0;
   return index;
}

extern "C" int getDataRepositoryProviderRegistrations(
    moja::flint::DataRepositoryProviderRegistration* outDataRepositoryProviderRegistration) {
   auto index = 0;
   return index;
}

// --------------------------------------------------------------------------------------------
// -- Fixture for BOOST tests

struct NotificationTests_Fixture {
   NotificationTests_Fixture() {
      testObjectHolder._callSequence.clear();

      // std::string s =
      //	"[Sinks.console]\n"
      //	"Destination=Console\n"
      //	"Asynchronous = false\n"
      //	"AutoFlush = true\n"
      //	"Format = \"<%TimeStamp%> (%Severity%) - %Message%\"\n";
      ////"Filter = \"%Severity% >= info\"" << std::endl;

      // moja::Logging::setConfigurationText(s);
   }
};

// --------------------------------------------------------------------------------------------
// -- Testing Modules

void TestNotificationModule::configure(const DynamicObject& config) {
   if (config.contains("debug_name")) {
      _debugName = config["debug_name"].extract<std::string>();
   } else {
      _debugName = "debug_name";
   }
}

void TestNotificationModule::subscribe(moja::NotificationCenter& notificationCenter) {
   notificationCenter.subscribe(moja::signals::SystemInit, &TestNotificationModule::onSystemInit, *this);
   notificationCenter.subscribe(moja::signals::SystemShutdown, &TestNotificationModule::onSystemShutdown, *this);
   notificationCenter.subscribe(moja::signals::LocalDomainInit, &TestNotificationModule::onLocalDomainInit, *this);
   notificationCenter.subscribe(moja::signals::LocalDomainShutdown, &TestNotificationModule::onLocalDomainShutdown,
                                *this);
   notificationCenter.subscribe(moja::signals::LocalDomainProcessingUnitInit,
                                &TestNotificationModule::onLocalDomainProcessingUnitInit, *this);
   notificationCenter.subscribe(moja::signals::LocalDomainProcessingUnitShutdown,
                                &TestNotificationModule::onLocalDomainProcessingUnitShutdown, *this);
   notificationCenter.subscribe(moja::signals::PreTimingSequence, &TestNotificationModule::onPreTimingSequence, *this);
   notificationCenter.subscribe(moja::signals::TimingInit, &TestNotificationModule::onTimingInit, *this);
   notificationCenter.subscribe(moja::signals::TimingPrePostInit, &TestNotificationModule::onTimingPrePostInit, *this);
   notificationCenter.subscribe(moja::signals::TimingPostInit, &TestNotificationModule::onTimingPostInit, *this);
   notificationCenter.subscribe(moja::signals::TimingPostInit2, &TestNotificationModule::onTimingPostInit2, *this);
   notificationCenter.subscribe(moja::signals::TimingShutdown, &TestNotificationModule::onTimingShutdown, *this);
   notificationCenter.subscribe(moja::signals::TimingStep, &TestNotificationModule::onTimingStep, *this);
   notificationCenter.subscribe(moja::signals::TimingPreEndStep, &TestNotificationModule::onTimingPreEndStep, *this);
   notificationCenter.subscribe(moja::signals::TimingEndStep, &TestNotificationModule::onTimingEndStep, *this);
   notificationCenter.subscribe(moja::signals::TimingPostStep, &TestNotificationModule::onTimingPostStep, *this);
   notificationCenter.subscribe(moja::signals::OutputStep, &TestNotificationModule::onOutputStep, *this);
   notificationCenter.subscribe(moja::signals::Error, &TestNotificationModule::onError, *this);
   notificationCenter.subscribe(moja::signals::DisturbanceEvent, &TestNotificationModule::onDisturbanceEvent, *this);
   notificationCenter.subscribe(moja::signals::PrePostDisturbanceEvent,
                                &TestNotificationModule::onPrePostDisturbanceEvent, *this);
   notificationCenter.subscribe(moja::signals::PostDisturbanceEvent, &TestNotificationModule::onPostDisturbanceEvent,
                                *this);
   notificationCenter.subscribe(moja::signals::PostDisturbanceEvent2, &TestNotificationModule::onPostDisturbanceEvent2,
                                *this);
   notificationCenter.subscribe(moja::signals::PostNotification, &TestNotificationModule::onPostNotification, *this);
}

void TestNotificationModule::onSystemInit() {
   _callCount++;
   _sharedCallSequence.push_back(fmt::format("Module = {}, Notification = {}", _debugName, moja::signals::SystemInit));
   _callSequence.emplace_back("moja::signals::SystemInit");
}
void TestNotificationModule::onSystemShutdown() {
   _callCount++;
   _sharedCallSequence.push_back(
       fmt::format("Module = {}, Notification = {}", _debugName, moja::signals::SystemShutdown));
   _callSequence.emplace_back("moja::signals::SystemShutdown");
}
void TestNotificationModule::onLocalDomainInit() {
   _callCount++;
   _sharedCallSequence.push_back(
       fmt::format("Module = {}, Notification = {}", _debugName, moja::signals::LocalDomainInit));
   _callSequence.emplace_back("moja::signals::LocalDomainInit");
}
void TestNotificationModule::onLocalDomainShutdown() {
   _callCount++;
   _sharedCallSequence.push_back(
       fmt::format("Module = {}, Notification = {}", _debugName, moja::signals::LocalDomainShutdown));
   _callSequence.emplace_back("moja::signals::LocalDomainShutdown");
}
void TestNotificationModule::onLocalDomainProcessingUnitInit() {
   _callCount++;
   _sharedCallSequence.push_back(
       fmt::format("Module = {}, Notification = {}", _debugName, moja::signals::LocalDomainProcessingUnitInit));
   _callSequence.emplace_back("moja::signals::LocalDomainProcessingUnitInit");
}
void TestNotificationModule::onLocalDomainProcessingUnitShutdown() {
   _callCount++;
   _sharedCallSequence.push_back(
       fmt::format("Module = {}, Notification = {}", _debugName, moja::signals::LocalDomainProcessingUnitShutdown));
   _callSequence.emplace_back("moja::signals::LocalDomainProcessingUnitShutdown");
}
void TestNotificationModule::onPreTimingSequence() {
   _callCount++;
   _sharedCallSequence.push_back(
       fmt::format("Module = {}, Notification = {}", _debugName, moja::signals::PreTimingSequence));
   _callSequence.emplace_back("moja::signals::PreTimingSequence");
}
void TestNotificationModule::onTimingInit() {
   _callCount++;
   _sharedCallSequence.push_back(fmt::format("Module = {}, Notification = {}", _debugName, moja::signals::TimingInit));
   _callSequence.emplace_back("moja::signals::TimingInit");
}
void TestNotificationModule::onTimingPrePostInit() {
   _callCount++;
   _sharedCallSequence.push_back(
       fmt::format("Module = {}, Notification = {}", _debugName, moja::signals::TimingPostInit));
   _callSequence.emplace_back("moja::signals::TimingPrePostInit");
}
void TestNotificationModule::onTimingPostInit() {
   _callCount++;
   _sharedCallSequence.push_back(
       fmt::format("Module = {}, Notification = {}", _debugName, moja::signals::TimingPostInit));
   _callSequence.emplace_back("moja::signals::TimingPostInit");
}
void TestNotificationModule::onTimingPostInit2() {
   _callCount++;
   _sharedCallSequence.push_back(
       fmt::format("Module = {}, Notification = {}", _debugName, moja::signals::TimingPostInit));
   _callSequence.emplace_back("moja::signals::TimingPostInit2");
}
void TestNotificationModule::onTimingShutdown() {
   _callCount++;
   _sharedCallSequence.push_back(
       fmt::format("Module = {}, Notification = {}", _debugName, moja::signals::TimingShutdown));
   _callSequence.emplace_back("moja::signals::TimingShutdown");
}
void TestNotificationModule::onTimingStep() {
   _callCount++;
   _sharedCallSequence.push_back(fmt::format("Module = {}, Notification = {}", _debugName, moja::signals::TimingStep));
   _callSequence.emplace_back("moja::signals::TimingStep");
}
void TestNotificationModule::onTimingPreEndStep() {
   _callCount++;
   _sharedCallSequence.push_back(
       fmt::format("Module = {}, Notification = {}", _debugName, moja::signals::TimingPreEndStep));
   _callSequence.emplace_back("moja::signals::TimingPreEndStep");
   MOJA_LOG_DEBUG << "Debug name: " << _debugName;
}
void TestNotificationModule::onTimingEndStep() {
   _callCount++;
   _sharedCallSequence.push_back(
       fmt::format("Module = {}, Notification = {}", _debugName, moja::signals::TimingEndStep));
   _callSequence.emplace_back("moja::signals::TimingEndStep");
}
void TestNotificationModule::onTimingPostStep() {
   _callCount++;
   _sharedCallSequence.push_back(
       fmt::format("Module = {}, Notification = {}", _debugName, moja::signals::TimingPostStep));
   _callSequence.emplace_back("moja::signals::TimingPostStep");
}
void TestNotificationModule::onOutputStep() {
   _callCount++;
   _sharedCallSequence.push_back(fmt::format("Module = {}, Notification = {}", _debugName, moja::signals::OutputStep));
   _callSequence.emplace_back("moja::signals::OutputStep");
}
void TestNotificationModule::onError(std::string msg) {
   _callCount++;
   _sharedCallSequence.push_back(fmt::format("Module = {}, Notification = {}", _debugName, moja::signals::Error));
   _callSequence.emplace_back("moja::signals::Error");
}
void TestNotificationModule::onDisturbanceEvent(moja::DynamicVar) {
   _callCount++;
   _sharedCallSequence.push_back(
       fmt::format("Module = {}, Notification = {}", _debugName, moja::signals::DisturbanceEvent));
   _callSequence.emplace_back("moja::signals::DisturbanceEvent");
}
void TestNotificationModule::onPrePostDisturbanceEvent() {
   _callCount++;
   _sharedCallSequence.push_back(
       fmt::format("Module = {}, Notification = {}", _debugName, moja::signals::DisturbanceEvent));
   _callSequence.emplace_back("moja::signals::PrePostDisturbanceEvent");
}
void TestNotificationModule::onPostDisturbanceEvent() {
   _callCount++;
   _sharedCallSequence.push_back(
       fmt::format("Module = {}, Notification = {}", _debugName, moja::signals::PostDisturbanceEvent));
   _callSequence.emplace_back("moja::signals::PostDisturbanceEvent");
}
void TestNotificationModule::onPostDisturbanceEvent2() {
   _callCount++;
   _sharedCallSequence.push_back(
       fmt::format("Module = {}, Notification = {}", _debugName, moja::signals::PostDisturbanceEvent));
   _callSequence.emplace_back("moja::signals::PostDisturbanceEvent2");
}
void TestNotificationModule::onPostNotification(short preMessageSignal) {
   _callCount++;
   _sharedCallSequence.push_back(
       fmt::format("Module = {}, Notification = {}", _debugName, moja::signals::PostNotification));
   _callSequence.emplace_back("moja::signals::PostNotification");
}

BOOST_FIXTURE_TEST_SUITE(NotificationTests, NotificationTests_Fixture)

BOOST_AUTO_TEST_CASE(Notification_AllNotificationsFire) {
   TestLocalDomainController ldc;
   configuration::Configuration config(DateTime(1920, 1, 1), DateTime(1924, 12, 31));

   config.setLocalDomain(configuration::LocalDomainType::Point, configuration::LocalDomainIterationType::NotAnIteration,
                         true, 1, "internal.moja.flint.test", "TestSequencer01", "simulateLandUnit",
                         "landUnitBuildSuccess", DynamicObject());

   config.addModule("internal.moja.flint.test", "TestSequencer01", 1, false);
   config.addModule("internal.moja.flint.test", "TestNotificationModule01", 2, false);

   int poolOrder = 0;
   for (auto pool : {"initialValues"}) {
      std::string desc(pool);
      desc += " description";
      std::string unit("units for ");
      unit += pool;
      config.addPool(pool, desc, unit, 1.0, poolOrder++);
   }

   config.addVariable("simulateLandUnit", true);
   config.addVariable("landUnitBuildSuccess", true);

   ldc.configure(config);
   ldc.run();

   LocalDomainControllerBase::ModuleMapKey key("internal.moja.flint.test", "TestNotificationModule01");
   auto modules = ldc.modules();
   const auto* module = dynamic_cast<const TestNotificationModule*>(modules[key]);

   BOOST_CHECK_EQUAL(module->_callCount, 23);
}

BOOST_AUTO_TEST_CASE(Notification_AllNotificationsFireWithFollowupNotification) {
   TestLocalDomainController ldc;
   configuration::Configuration config(DateTime(1920, 1, 1), DateTime(1924, 12, 31));

   config.setLocalDomain(configuration::LocalDomainType::Point, configuration::LocalDomainIterationType::NotAnIteration,
                         true, 1, "internal.moja.flint.test", "TestSequencer02", "simulateLandUnit",
                         "landUnitBuildSuccess", DynamicObject());

   config.addModule("internal.moja.flint.test", "TestSequencer02", 1, false);
   config.addModule("internal.moja.flint.test", "TestNotificationModule01", 2, false);

   int poolOrder = 0;
   for (auto pool : {"initialValues"}) {
      std::string desc(pool);
      desc += " description";
      std::string unit("units for ");
      unit += pool;
      config.addPool(pool, desc, unit, 1.0, poolOrder++);
   }
   config.addVariable("simulateLandUnit", true);
   config.addVariable("landUnitBuildSuccess", true);

   ldc.configure(config);
   ldc.run();

   LocalDomainControllerBase::ModuleMapKey key("internal.moja.flint.test", "TestNotificationModule01");
   auto modules = ldc.modules();
   auto module = static_cast<const TestNotificationModule*>(modules[key]);

   BOOST_CHECK_EQUAL(module->_callCount, 23 * 2);
}

BOOST_AUTO_TEST_CASE(Notification_AllNotificationsPostSequence) {
   TestLocalDomainController ldc;
   configuration::Configuration config(DateTime(1920, 1, 1), DateTime(1924, 12, 31));

   config.setLocalDomain(configuration::LocalDomainType::Point, configuration::LocalDomainIterationType::NotAnIteration,
                         true, 1, "internal.moja.flint.test", "TestSequencer01", "simulateLandUnit",
                         "landUnitBuildSuccess", DynamicObject());

   config.addModule("internal.moja.flint.test", "TestSequencer01", 1, false);
   config.addModule("internal.moja.flint.test", "TestNotificationModule01", 2, false);

   int poolOrder = 0;
   for (auto pool : {"initialValues"}) {
      std::string desc(pool);
      desc += " description";
      std::string unit("units for ");
      unit += pool;
      config.addPool(pool, desc, unit, 1.0, poolOrder++);
   }
   config.addVariable("simulateLandUnit", true);
   config.addVariable("landUnitBuildSuccess", true);

   ldc.configure(config);
   ldc.run();

   LocalDomainControllerBase::ModuleMapKey key("internal.moja.flint.test", "TestNotificationModule01");
   auto modules = ldc.modules();
   auto module = static_cast<const TestNotificationModule*>(modules[key]);
   auto sequencer = std::static_pointer_cast<TestSequencer01>(ldc._sequencer);

   BOOST_CHECK_EQUAL(sequencer->_postSequence.size(), 23);
   BOOST_CHECK_EQUAL(module->_callSequence.size(), sequencer->_postSequence.size());

   for (int i = 0; i < std::min(sequencer->_postSequence.size(), module->_callSequence.size()); i++) {
      auto str1 = sequencer->_postSequence[i];
      auto str2 = module->_callSequence[i];
      BOOST_CHECK_EQUAL(str1, str2);
   }
}

BOOST_AUTO_TEST_CASE(Notification_AllNotificationsPostSequenceWithPostNotify) {
   TestLocalDomainController ldc;
   configuration::Configuration config(DateTime(1920, 1, 1), DateTime(1924, 12, 31));

   config.setLocalDomain(configuration::LocalDomainType::Point, configuration::LocalDomainIterationType::NotAnIteration,
                         true, 1, "internal.moja.flint.test", "TestSequencer02", "simulateLandUnit",
                         "landUnitBuildSuccess", DynamicObject());

   config.addModule("internal.moja.flint.test", "TestSequencer02", 1, false);
   config.addModule("internal.moja.flint.test", "TestNotificationModule01", 2, false);

   int poolOrder = 0;
   for (auto pool : {"initialValues"}) {
      std::string desc(pool);
      desc += " description";
      std::string unit("units for ");
      unit += pool;
      config.addPool(pool, desc, unit, 1.0, poolOrder++);
   }
   config.addVariable("simulateLandUnit", true);
   config.addVariable("landUnitBuildSuccess", true);

   ldc.configure(config);
   ldc.run();

   LocalDomainControllerBase::ModuleMapKey key("internal.moja.flint.test", "TestNotificationModule01");
   auto modules = ldc.modules();
   auto module = static_cast<const TestNotificationModule*>(modules[key]);
   auto sequencer = std::static_pointer_cast<TestSequencer02>(ldc._sequencer);

   BOOST_CHECK_EQUAL(sequencer->_postSequence.size(), 23);
   BOOST_CHECK_EQUAL(module->_callSequence.size(), sequencer->_postSequence.size() * 2);

   for (int i = 0; i < sequencer->_postSequence.size(); i++) {
      auto str1 = sequencer->_postSequence[i];
      auto str2a = module->_callSequence[i * 2];      // original notify
      auto str2b = module->_callSequence[i * 2 + 1];  // post notify, notify
      BOOST_CHECK_EQUAL(str1, str2a);
      BOOST_CHECK_EQUAL(str2b, "moja::signals::PostNotification");
   }
}

BOOST_AUTO_TEST_CASE(Notification_AllNotificationsPostSequenceWithPostMultipleotify) {
   TestLocalDomainController ldc;
   configuration::Configuration config(DateTime(1920, 1, 1), DateTime(1924, 12, 31));

   config.setLocalDomain(configuration::LocalDomainType::Point, configuration::LocalDomainIterationType::NotAnIteration,
                         true, 1, "internal.moja.flint.test", "TestSequencer02", "simulateLandUnit",
                         "landUnitBuildSuccess", DynamicObject());

   config.addModule("internal.moja.flint.test", "TestSequencer02", 1, false);
   config.addModule("internal.moja.flint.test", "TestNotificationModule01", 2, false);
   config.addModule("internal.moja.flint.test", "TestNotificationModule02", 3, false);

   int poolOrder = 0;
   for (auto pool : {"initialValues"}) {
      std::string desc(pool);
      desc += " description";
      std::string unit("units for ");
      unit += pool;
      config.addPool(pool, desc, unit, 1.0, poolOrder++);
   }
   config.addVariable("simulateLandUnit", true);
   config.addVariable("landUnitBuildSuccess", true);

   ldc.configure(config);
   ldc.run();

   LocalDomainControllerBase::ModuleMapKey key("internal.moja.flint.test", "TestNotificationModule01");
   auto modules = ldc.modules();
   auto module = static_cast<const TestNotificationModule*>(modules[key]);
   auto sequencer = std::static_pointer_cast<TestSequencer02>(ldc._sequencer);

   BOOST_CHECK_EQUAL(sequencer->_postSequence.size(), 23);
   BOOST_CHECK_EQUAL(
       module->_callSequence.size(),
       sequencer->_postSequence.size() * 3);  // * 3 because there are 2 modules registered and postNotify gets called
                                              // after each module. 23 notifies, + 2 postNotifys for each

   for (int i = 0; i < sequencer->_postSequence.size(); i++) {
      auto str1 = sequencer->_postSequence[i];
      auto str2a = module->_callSequence[i * 3];      // original notify
      auto str2b = module->_callSequence[i * 3 + 1];  // post notify, notify 1
      auto str2c = module->_callSequence[i * 3 + 2];  // post notify, notify 2
      BOOST_CHECK_EQUAL(str1, str2a);
      BOOST_CHECK_EQUAL(str2b, "moja::signals::PostNotification");
      BOOST_CHECK_EQUAL(str2c, "moja::signals::PostNotification");
   }
}

BOOST_AUTO_TEST_CASE(Notification_DuplcateModuleException) {
   TestLocalDomainController ldc;
   configuration::Configuration config(DateTime(1920, 1, 1), DateTime(1924, 12, 31));

   config.setLocalDomain(configuration::LocalDomainType::Point, configuration::LocalDomainIterationType::NotAnIteration,
                         true, 1, "internal.moja.flint.test", "TestSequencer01", "simulateLandUnit",
                         "landUnitBuildSuccess", DynamicObject());

   config.addModule("internal.moja.flint.test", "TestSequencer01", 1, false);
   config.addModule("internal.moja.flint.test", "TestNotificationModule01", 4, false);
   config.addModule("internal.moja.flint.test", "TestNotificationModule01", 6, false);
   config.addModule("internal.moja.flint.test", "TestNotificationModule01", 8, false);

   int poolOrder = 0;
   for (auto pool : {"initialValues"}) {
      std::string desc(pool);
      desc += " description";
      std::string unit("units for ");
      unit += pool;
      config.addPool(pool, desc, unit, 1.0, poolOrder++);
   }
   config.addVariable("simulateLandUnit", true);
   config.addVariable("landUnitBuildSuccess", true);

   BOOST_CHECK_THROW(ldc.configure(config), flint::DuplicateModuleDefinedException);
}

BOOST_AUTO_TEST_CASE(Notification_AllNotificationsOrder) {
   TestLocalDomainController ldc;
   configuration::Configuration config(moja::DateTime(1920, 1, 1), moja::DateTime(1924, 12, 31));

   config.setLocalDomain(configuration::LocalDomainType::Point, configuration::LocalDomainIterationType::NotAnIteration,
                         true, 1, "internal.moja.flint.test", "TestSequencer03", "simulateLandUnit",
                         "landUnitBuildSuccess", moja::DynamicObject());

   config.addModule("internal.moja.flint.test", "TestSequencer03", 1, false);
   config.addModule("internal.moja.flint.test", "TestNotificationModule01", 3, false,
                    moja::DynamicObject({{"debug_name", std::string("TestNotificationModule01")}}));
   config.addModule("internal.moja.flint.test", "TestNotificationModule02", 4, false,
                    moja::DynamicObject({{"debug_name", std::string("TestNotificationModule02")}}));
   config.addModule("internal.moja.flint.test", "TestNotificationModule03", 2, false,
                    moja::DynamicObject({{"debug_name", std::string("TestNotificationModule03")}}));
   config.addModule("internal.moja.flint.test", "TestNotificationModule04", 5, false,
                    DynamicObject({{"debug_name", std::string("TestNotificationModule04")}}));
   config.addModule("internal.moja.flint.test", "TestNotificationModule05", 6, false,
                    DynamicObject({{"debug_name", std::string("TestNotificationModule05")}}));
   config.addModule("internal.moja.flint.test", "TestNotificationModule06", 7, false,
                    DynamicObject({{"debug_name", std::string("TestNotificationModule06")}}));
   config.addModule("internal.moja.flint.test", "TestNotificationModule07", 9, false,
                    DynamicObject({{"debug_name", std::string("TestNotificationModule07")}}));
   config.addModule("internal.moja.flint.test", "TestNotificationModule08", 8, false,
                    DynamicObject({{"debug_name", std::string("TestNotificationModule08")}}));

   std::vector<std::string> expectedSequence = {
       fmt::format("Module = {}, Notification = {}", "TestNotificationModule03", moja::signals::SystemInit),
       fmt::format("Module = {}, Notification = {}", "TestNotificationModule01", moja::signals::SystemInit),
       fmt::format("Module = {}, Notification = {}", "TestNotificationModule02", moja::signals::SystemInit),
       fmt::format("Module = {}, Notification = {}", "TestNotificationModule04", moja::signals::SystemInit),
       fmt::format("Module = {}, Notification = {}", "TestNotificationModule05", moja::signals::SystemInit),
       fmt::format("Module = {}, Notification = {}", "TestNotificationModule06", moja::signals::SystemInit),
       fmt::format("Module = {}, Notification = {}", "TestNotificationModule08", moja::signals::SystemInit),
       fmt::format("Module = {}, Notification = {}", "TestNotificationModule07", moja::signals::SystemInit)};

   int poolOrder = 0;
   for (auto pool : {"initialValues"}) {
      std::string desc(pool);
      desc += " description";
      std::string unit("units for ");
      unit += pool;
      config.addPool(pool, desc, unit, 1.0, poolOrder++);
   }
   config.addVariable("simulateLandUnit", true);
   config.addVariable("landUnitBuildSuccess", true);

   ldc.configure(config);
   ldc.run();

   // auto modules = ldc.modules();
   // for (auto& item : ldc.modules()) {
   //	LocalDomainControllerBase::ModuleMapKey key = item.first;
   //	auto moduleLib  = key.first;
   //	auto moduleName = key.second;
   //	auto module = item.second;
   //}

   // Order of messages should be TestNotificationModule03, TestNotificationModule01 and TestNotificationModule02
   for (int i = 0; i < testObjectHolder._callSequence.size(); i++) {
      BOOST_CHECK_EQUAL(testObjectHolder._callSequence[i], expectedSequence[i]);
   }
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace flint
