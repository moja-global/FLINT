#include "notificationtests.h"


#include "moja/flint/flintexceptions.h"
#include "moja/flint/configuration/localdomain.h"
#include "moja/flint/configuration/configuration.h"
#include "moja/flint/configuration/iterationbase.h"

#include "moja/logging.h"

#include <boost/test/unit_test.hpp>
#include <boost/format.hpp>

// --------------------------------------------------------------------------------------------
// -- Module Factory Stuff

struct Test_ObjectHolder {
	Test_ObjectHolder() {
	}

	std::vector<std::string> _callSequence;
};

Test_ObjectHolder testObjectHolder;


inline IModule* CreateTestSequencer01()			{ return new TestSequencer01(); }
inline IModule* CreateTestSequencer02()			{ return new TestSequencer02(); }
inline IModule* CreateTestSequencer03()			{ return new TestSequencer03(); }
inline IModule* CreateTestNotificationModule()	{ return new TestNotificationModule(testObjectHolder._callSequence); }

extern "C" int getModuleRegistrations(ModuleRegistration* outModuleRegistrations) {
	auto index = 0;
	outModuleRegistrations[index++] = ModuleRegistration{ "TestSequencer01",			&CreateTestSequencer01 };
	outModuleRegistrations[index++] = ModuleRegistration{ "TestSequencer02",			&CreateTestSequencer02 };
	outModuleRegistrations[index++] = ModuleRegistration{ "TestSequencer03",			&CreateTestSequencer03 };
	outModuleRegistrations[index++] = ModuleRegistration{ "TestNotificationModule01",	&CreateTestNotificationModule };
	outModuleRegistrations[index++] = ModuleRegistration{ "TestNotificationModule02",	&CreateTestNotificationModule };
	outModuleRegistrations[index++] = ModuleRegistration{ "TestNotificationModule03",	&CreateTestNotificationModule };
	outModuleRegistrations[index++] = ModuleRegistration{ "TestNotificationModule04",	&CreateTestNotificationModule };
	outModuleRegistrations[index++] = ModuleRegistration{ "TestNotificationModule05",	&CreateTestNotificationModule };
	outModuleRegistrations[index++] = ModuleRegistration{ "TestNotificationModule06",	&CreateTestNotificationModule };
	outModuleRegistrations[index++] = ModuleRegistration{ "TestNotificationModule07",	&CreateTestNotificationModule };
	outModuleRegistrations[index++] = ModuleRegistration{ "TestNotificationModule08",	&CreateTestNotificationModule };
	outModuleRegistrations[index++] = ModuleRegistration{ "TestNotificationModule09",	&CreateTestNotificationModule };
	outModuleRegistrations[index++] = ModuleRegistration{ "TestNotificationModule10",	&CreateTestNotificationModule };
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

extern "C" int getFlintDataFactoryRegistrations(moja::flint::FlintDataFactoryRegistration* outFlintDataFactoryRegistrations) {
	auto index = 0;
	return index;
}

extern "C" int getDataRepositoryProviderRegistrations(moja::flint::DataRepositoryProviderRegistration*	outDataRepositoryProviderRegistration) {
	auto index = 0;
	return index;
}

// --------------------------------------------------------------------------------------------
// -- Fixture for BOOST tests

struct NotificationTests_Fixture {
	NotificationTests_Fixture() {
		testObjectHolder._callSequence.clear();

		//std::string s =
		//	"[Sinks.console]\n"
		//	"Destination=Console\n"
		//	"Asynchronous = false\n"
		//	"AutoFlush = true\n"
		//	"Format = \"<%TimeStamp%> (%Severity%) - %Message%\"\n";
		////"Filter = \"%Severity% >= info\"" << std::endl;

		//moja::Logging::setConfigurationText(s);
	}
};

// --------------------------------------------------------------------------------------------
// -- Testing Modules

void TestNotificationModule::configure(const DynamicObject& config) {
	if (config.contains("debug_name"))
		_debugName = config["debug_name"].extract<std::string>();
	else
		_debugName = "debug_name";
}

void TestNotificationModule::subscribe(NotificationCenter& notificationCenter) {
	notificationCenter.subscribe(signals::SystemInit,							&TestNotificationModule::onSystemInit,							*this);
	notificationCenter.subscribe(signals::SystemShutdown,						&TestNotificationModule::onSystemShutdown,						*this);
	notificationCenter.subscribe(signals::LocalDomainInit,						&TestNotificationModule::onLocalDomainInit,						*this);
	notificationCenter.subscribe(signals::LocalDomainShutdown,					&TestNotificationModule::onLocalDomainShutdown,					*this);
	notificationCenter.subscribe(signals::LocalDomainProcessingUnitInit,		&TestNotificationModule::onLocalDomainProcessingUnitInit,		*this);
	notificationCenter.subscribe(signals::LocalDomainProcessingUnitShutdown,	&TestNotificationModule::onLocalDomainProcessingUnitShutdown,	*this);
	notificationCenter.subscribe(signals::PreTimingSequence,					&TestNotificationModule::onPreTimingSequence,					*this);
	notificationCenter.subscribe(signals::TimingInit,							&TestNotificationModule::onTimingInit,							*this);
	notificationCenter.subscribe(signals::TimingPrePostInit,					&TestNotificationModule::onTimingPrePostInit,					*this);
	notificationCenter.subscribe(signals::TimingPostInit,						&TestNotificationModule::onTimingPostInit,						*this);
	notificationCenter.subscribe(signals::TimingPostInit2,						&TestNotificationModule::onTimingPostInit2,						*this);
	notificationCenter.subscribe(signals::TimingShutdown,						&TestNotificationModule::onTimingShutdown,						*this);
	notificationCenter.subscribe(signals::TimingStep,							&TestNotificationModule::onTimingStep,							*this);
	notificationCenter.subscribe(signals::TimingPreEndStep,						&TestNotificationModule::onTimingPreEndStep,					*this);
	notificationCenter.subscribe(signals::TimingEndStep,						&TestNotificationModule::onTimingEndStep,						*this);
	notificationCenter.subscribe(signals::TimingPostStep,						&TestNotificationModule::onTimingPostStep,						*this);
	notificationCenter.subscribe(signals::OutputStep,							&TestNotificationModule::onOutputStep,							*this);
	notificationCenter.subscribe(signals::Error,								&TestNotificationModule::onError,								*this);
	notificationCenter.subscribe(signals::DisturbanceEvent,						&TestNotificationModule::onDisturbanceEvent,					*this);
	notificationCenter.subscribe(signals::PrePostDisturbanceEvent,				&TestNotificationModule::onPrePostDisturbanceEvent,				*this);
	notificationCenter.subscribe(signals::PostDisturbanceEvent,					&TestNotificationModule::onPostDisturbanceEvent,				*this);
	notificationCenter.subscribe(signals::PostDisturbanceEvent2,				&TestNotificationModule::onPostDisturbanceEvent2,				*this);
	notificationCenter.subscribe(signals::PostNotification,						&TestNotificationModule::onPostNotification,					*this);
}

void TestNotificationModule::onSystemInit()									{ 	_callCount++;	_sharedCallSequence.push_back((boost::format("Module = %1%,\t\tNotification = %2%") % _debugName % signals::SystemInit							).str());_callSequence.push_back("signals::SystemInit");							MOJA_LOG_DEBUG << "Debug name: " << _debugName; }
void TestNotificationModule::onSystemShutdown()								{	_callCount++;	_sharedCallSequence.push_back((boost::format("Module = %1%,\t\tNotification = %2%") % _debugName % signals::SystemShutdown						).str());_callSequence.push_back("signals::SystemShutdown");						MOJA_LOG_DEBUG << "Debug name: " << _debugName; }
void TestNotificationModule::onLocalDomainInit()							{	_callCount++;	_sharedCallSequence.push_back((boost::format("Module = %1%,\t\tNotification = %2%") % _debugName % signals::LocalDomainInit						).str());_callSequence.push_back("signals::LocalDomainInit");						MOJA_LOG_DEBUG << "Debug name: " << _debugName; }
void TestNotificationModule::onLocalDomainShutdown()						{	_callCount++;	_sharedCallSequence.push_back((boost::format("Module = %1%,\t\tNotification = %2%") % _debugName % signals::LocalDomainShutdown					).str());_callSequence.push_back("signals::LocalDomainShutdown");					MOJA_LOG_DEBUG << "Debug name: " << _debugName; }
void TestNotificationModule::onLocalDomainProcessingUnitInit()				{	_callCount++;	_sharedCallSequence.push_back((boost::format("Module = %1%,\t\tNotification = %2%") % _debugName % signals::LocalDomainProcessingUnitInit		).str());_callSequence.push_back("signals::LocalDomainProcessingUnitInit");			MOJA_LOG_DEBUG << "Debug name: " << _debugName; }
void TestNotificationModule::onLocalDomainProcessingUnitShutdown()			{	_callCount++;	_sharedCallSequence.push_back((boost::format("Module = %1%,\t\tNotification = %2%") % _debugName % signals::LocalDomainProcessingUnitShutdown	).str());_callSequence.push_back("signals::LocalDomainProcessingUnitShutdown");		MOJA_LOG_DEBUG << "Debug name: " << _debugName; }
void TestNotificationModule::onPreTimingSequence()							{	_callCount++;	_sharedCallSequence.push_back((boost::format("Module = %1%,\t\tNotification = %2%") % _debugName % signals::PreTimingSequence					).str());_callSequence.push_back("signals::PreTimingSequence");						MOJA_LOG_DEBUG << "Debug name: " << _debugName; }
void TestNotificationModule::onTimingInit()									{	_callCount++;	_sharedCallSequence.push_back((boost::format("Module = %1%,\t\tNotification = %2%") % _debugName % signals::TimingInit							).str());_callSequence.push_back("signals::TimingInit");							MOJA_LOG_DEBUG << "Debug name: " << _debugName; }
void TestNotificationModule::onTimingPrePostInit()							{	_callCount++;	_sharedCallSequence.push_back((boost::format("Module = %1%,\t\tNotification = %2%") % _debugName % signals::TimingPostInit						).str());_callSequence.push_back("signals::TimingPrePostInit");						MOJA_LOG_DEBUG << "Debug name: " << _debugName; }
void TestNotificationModule::onTimingPostInit()								{	_callCount++;	_sharedCallSequence.push_back((boost::format("Module = %1%,\t\tNotification = %2%") % _debugName % signals::TimingPostInit						).str());_callSequence.push_back("signals::TimingPostInit");						MOJA_LOG_DEBUG << "Debug name: " << _debugName; }
void TestNotificationModule::onTimingPostInit2()							{	_callCount++;	_sharedCallSequence.push_back((boost::format("Module = %1%,\t\tNotification = %2%") % _debugName % signals::TimingPostInit						).str());_callSequence.push_back("signals::TimingPostInit2");						MOJA_LOG_DEBUG << "Debug name: " << _debugName; }
void TestNotificationModule::onTimingShutdown()								{	_callCount++;	_sharedCallSequence.push_back((boost::format("Module = %1%,\t\tNotification = %2%") % _debugName % signals::TimingShutdown						).str());_callSequence.push_back("signals::TimingShutdown");						MOJA_LOG_DEBUG << "Debug name: " << _debugName; }
void TestNotificationModule::onTimingStep()									{	_callCount++;	_sharedCallSequence.push_back((boost::format("Module = %1%,\t\tNotification = %2%") % _debugName % signals::TimingStep							).str());_callSequence.push_back("signals::TimingStep");							MOJA_LOG_DEBUG << "Debug name: " << _debugName; }
void TestNotificationModule::onTimingPreEndStep()							{	_callCount++;	_sharedCallSequence.push_back((boost::format("Module = %1%,\t\tNotification = %2%") % _debugName % signals::TimingPreEndStep					).str());_callSequence.push_back("signals::TimingPreEndStep");						MOJA_LOG_DEBUG << "Debug name: " << _debugName; }
void TestNotificationModule::onTimingEndStep()								{	_callCount++;	_sharedCallSequence.push_back((boost::format("Module = %1%,\t\tNotification = %2%") % _debugName % signals::TimingEndStep						).str());_callSequence.push_back("signals::TimingEndStep");							MOJA_LOG_DEBUG << "Debug name: " << _debugName; }
void TestNotificationModule::onTimingPostStep()								{	_callCount++;	_sharedCallSequence.push_back((boost::format("Module = %1%,\t\tNotification = %2%") % _debugName % signals::TimingPostStep						).str());_callSequence.push_back("signals::TimingPostStep");						MOJA_LOG_DEBUG << "Debug name: " << _debugName; }
void TestNotificationModule::onOutputStep()									{	_callCount++;	_sharedCallSequence.push_back((boost::format("Module = %1%,\t\tNotification = %2%") % _debugName % signals::OutputStep							).str());_callSequence.push_back("signals::OutputStep");							MOJA_LOG_DEBUG << "Debug name: " << _debugName; }
void TestNotificationModule::onError(std::string msg)						{	_callCount++;	_sharedCallSequence.push_back((boost::format("Module = %1%,\t\tNotification = %2%") % _debugName % signals::Error								).str());_callSequence.push_back("signals::Error");									MOJA_LOG_DEBUG << "Debug name: " << _debugName; }
void TestNotificationModule::onDisturbanceEvent(DynamicVar)					{	_callCount++;	_sharedCallSequence.push_back((boost::format("Module = %1%,\t\tNotification = %2%") % _debugName % signals::DisturbanceEvent					).str());_callSequence.push_back("signals::DisturbanceEvent");						MOJA_LOG_DEBUG << "Debug name: " << _debugName; }
void TestNotificationModule::onPrePostDisturbanceEvent()					{	_callCount++;	_sharedCallSequence.push_back((boost::format("Module = %1%,\t\tNotification = %2%") % _debugName % signals::DisturbanceEvent					).str());_callSequence.push_back("signals::PrePostDisturbanceEvent");				MOJA_LOG_DEBUG << "Debug name: " << _debugName; }
void TestNotificationModule::onPostDisturbanceEvent()						{	_callCount++;	_sharedCallSequence.push_back((boost::format("Module = %1%,\t\tNotification = %2%") % _debugName % signals::PostDisturbanceEvent				).str());_callSequence.push_back("signals::PostDisturbanceEvent");					MOJA_LOG_DEBUG << "Debug name: " << _debugName; }
void TestNotificationModule::onPostDisturbanceEvent2()						{	_callCount++;	_sharedCallSequence.push_back((boost::format("Module = %1%,\t\tNotification = %2%") % _debugName % signals::PostDisturbanceEvent				).str());_callSequence.push_back("signals::PostDisturbanceEvent2");					MOJA_LOG_DEBUG << "Debug name: " << _debugName; }
void TestNotificationModule::onPostNotification(short preMessageSignal)		{	_callCount++;	_sharedCallSequence.push_back((boost::format("Module = %1%,\t\tNotification = %2%") % _debugName % signals::PostNotification					).str());_callSequence.push_back("signals::PostNotification");						MOJA_LOG_DEBUG << "Debug name: " << _debugName << " : Following notification: "<< preMessageSignal;}

// --------------------------------------------------------------------------------------------

BOOST_FIXTURE_TEST_SUITE(notificationtests, NotificationTests_Fixture);

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(notificationtests_TestAllNotificationsFire) {
	TestLocalDomainController ldc;
	conf::Configuration config(DateTime(1920, 1, 1), DateTime(1924, 12, 31));

	config.setLocalDomain(conf::LocalDomainType::Point, conf::LocalDomainIterationType::NotAnIteration, true, 1, "internal.moja.flint.test", "TestSequencer01", "simulateLandUnit", "landUnitBuildSuccess", DynamicObject());

	config.addModule("internal.moja.flint.test", "TestSequencer01",				1, false);
	config.addModule("internal.moja.flint.test", "TestNotificationModule01",	2, false);

	int poolOrder = 0;
	for (auto pool : { "initialValues" }) {
		std::string desc(pool); desc += " description";
		std::string unit("units for "); unit += pool;
		config.addPool(pool, desc, unit, 1.0, poolOrder++);
	}

	config.addVariable("simulateLandUnit", true);
	config.addVariable("landUnitBuildSuccess", true);

	ldc.configure(config);
	ldc.run();

	LocalDomainControllerBase::ModuleMapKey key("internal.moja.flint.test", "TestNotificationModule01");
	auto modules = ldc.modules();
	auto module = static_cast<const TestNotificationModule*>(modules[key]);
	
	BOOST_CHECK_EQUAL(module->_callCount, 23);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(notificationtests_test_all_notifications_fire_with_followup_notification) {
	TestLocalDomainController ldc;
	conf::Configuration config(DateTime(1920, 1, 1), DateTime(1924, 12, 31));

	config.setLocalDomain(conf::LocalDomainType::Point, conf::LocalDomainIterationType::NotAnIteration, true, 1, "internal.moja.flint.test", "TestSequencer02", "simulateLandUnit", "landUnitBuildSuccess", DynamicObject());

	config.addModule("internal.moja.flint.test", "TestSequencer02",				1, false);
	config.addModule("internal.moja.flint.test", "TestNotificationModule01",	2, false);

	int poolOrder = 0;
	for (auto pool : { "initialValues" }) {
		std::string desc(pool); desc += " description";
		std::string unit("units for "); unit += pool;
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

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(notificationtests_testallnotifications_post_sequence) {
	TestLocalDomainController ldc;
	conf::Configuration config(DateTime(1920, 1, 1), DateTime(1924, 12, 31));

	config.setLocalDomain(conf::LocalDomainType::Point, conf::LocalDomainIterationType::NotAnIteration, true, 1, "internal.moja.flint.test", "TestSequencer01", "simulateLandUnit", "landUnitBuildSuccess", DynamicObject());

	config.addModule("internal.moja.flint.test", "TestSequencer01",				1, false);
	config.addModule("internal.moja.flint.test", "TestNotificationModule01",	2, false);

	int poolOrder = 0;
	for (auto pool : { "initialValues" }) {
		std::string desc(pool); desc += " description";
		std::string unit("units for "); unit += pool;
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

	for (int i = 0; i < std::min(sequencer->_postSequence.size(), module->_callSequence.size()); i++ ) {
		auto str1 = sequencer->_postSequence[i];
		auto str2 = module->_callSequence[i];
		BOOST_CHECK_EQUAL(str1, str2);
	}
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(notificationtests_testallnotifications_post_sequence_with_post_notify) {
	TestLocalDomainController ldc;
	conf::Configuration config(DateTime(1920, 1, 1), DateTime(1924, 12, 31));

	config.setLocalDomain(conf::LocalDomainType::Point, conf::LocalDomainIterationType::NotAnIteration, true, 1, "internal.moja.flint.test", "TestSequencer02", "simulateLandUnit", "landUnitBuildSuccess", DynamicObject());

	config.addModule("internal.moja.flint.test", "TestSequencer02",				1, false);
	config.addModule("internal.moja.flint.test", "TestNotificationModule01",	2, false);

	int poolOrder = 0;
	for (auto pool : { "initialValues" }) {
		std::string desc(pool); desc += " description";
		std::string unit("units for "); unit += pool;
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
		auto str2a = module->_callSequence[i * 2];			// original notify
		auto str2b = module->_callSequence[i * 2 + 1];		// post notify, notify
		BOOST_CHECK_EQUAL(str1, str2a);
		BOOST_CHECK_EQUAL(str2b, "signals::PostNotification");
	}
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(notificationtests_testallnotifications_post_sequence_with_post_Multiple_notify) {
	TestLocalDomainController ldc;
	conf::Configuration config(DateTime(1920, 1, 1), DateTime(1924, 12, 31));

	config.setLocalDomain(conf::LocalDomainType::Point, conf::LocalDomainIterationType::NotAnIteration, true, 1, "internal.moja.flint.test", "TestSequencer02", "simulateLandUnit", "landUnitBuildSuccess", DynamicObject());

	config.addModule("internal.moja.flint.test", "TestSequencer02",				1, false);
	config.addModule("internal.moja.flint.test", "TestNotificationModule01",	2, false);
	config.addModule("internal.moja.flint.test", "TestNotificationModule02",	3, false);

	int poolOrder = 0;
	for (auto pool : { "initialValues" }) {
		std::string desc(pool); desc += " description";
		std::string unit("units for "); unit += pool;
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
	BOOST_CHECK_EQUAL(module->_callSequence.size(), sequencer->_postSequence.size() * 3);	// * 3 because there are 2 modules registered and postNotify gets called after each module. 23 notifies, + 2 postNotifys for each

	for (int i = 0; i < sequencer->_postSequence.size(); i++) {
		auto str1 = sequencer->_postSequence[i];
		auto str2a = module->_callSequence[i * 3];			// original notify
		auto str2b = module->_callSequence[i * 3 + 1];		// post notify, notify 1
		auto str2c = module->_callSequence[i * 3 + 2];		// post notify, notify 2
		BOOST_CHECK_EQUAL(str1, str2a);
		BOOST_CHECK_EQUAL(str2b, "signals::PostNotification");
		BOOST_CHECK_EQUAL(str2c, "signals::PostNotification");
	}
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(notificationtests_test_duplcate_module_exception) {
	TestLocalDomainController ldc;
	conf::Configuration config(DateTime(1920, 1, 1), DateTime(1924, 12, 31));

	config.setLocalDomain(conf::LocalDomainType::Point, conf::LocalDomainIterationType::NotAnIteration, true, 1, "internal.moja.flint.test", "TestSequencer01", "simulateLandUnit", "landUnitBuildSuccess", DynamicObject());

	config.addModule("internal.moja.flint.test", "TestSequencer01",				1, false);
	config.addModule("internal.moja.flint.test", "TestNotificationModule01",	4, false);
	config.addModule("internal.moja.flint.test", "TestNotificationModule01",	6, false);
	config.addModule("internal.moja.flint.test", "TestNotificationModule01",	8, false);

	int poolOrder = 0;
	for (auto pool : { "initialValues" }) {
		std::string desc(pool); desc += " description";
		std::string unit("units for "); unit += pool;
		config.addPool(pool, desc, unit, 1.0, poolOrder++);
	}
	config.addVariable("simulateLandUnit", true);
	config.addVariable("landUnitBuildSuccess", true);

	BOOST_CHECK_THROW(ldc.configure(config), flint::DuplicateModuleDefinedException);
}

// --------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(notificationtests_test_all_notifications_order) {
	TestLocalDomainController ldc;
	conf::Configuration config(DateTime(1920, 1, 1), DateTime(1924, 12, 31));

	config.setLocalDomain(conf::LocalDomainType::Point, conf::LocalDomainIterationType::NotAnIteration, true, 1, "internal.moja.flint.test", "TestSequencer03", "simulateLandUnit", "landUnitBuildSuccess", DynamicObject());

	config.addModule("internal.moja.flint.test",	"TestSequencer03",			1, false);
	config.addModule("internal.moja.flint.test",	"TestNotificationModule01",	3, false, DynamicObject({ { "debug_name", std::string("TestNotificationModule01") } }));
	config.addModule("internal.moja.flint.test",	"TestNotificationModule02",	4, false, DynamicObject({ { "debug_name", std::string("TestNotificationModule02") } }));
	config.addModule("internal.moja.flint.test",	"TestNotificationModule03",	2, false, DynamicObject({ { "debug_name", std::string("TestNotificationModule03") } }));
	config.addModule("internal.moja.flint.test",	"TestNotificationModule04",	5, false, DynamicObject({ { "debug_name", std::string("TestNotificationModule04") } }));
	config.addModule("internal.moja.flint.test",	"TestNotificationModule05",	6, false, DynamicObject({ { "debug_name", std::string("TestNotificationModule05") } }));
	config.addModule("internal.moja.flint.test",	"TestNotificationModule06",	7, false, DynamicObject({ { "debug_name", std::string("TestNotificationModule06") } }));
	config.addModule("internal.moja.flint.test",	"TestNotificationModule07",	9, false, DynamicObject({ { "debug_name", std::string("TestNotificationModule07") } }));
	config.addModule("internal.moja.flint.test",	"TestNotificationModule08",	8, false, DynamicObject({ { "debug_name", std::string("TestNotificationModule08") } }));

	std::vector<std::string> expectedSequence = {
		(boost::format("Module = %1%,\t\tNotification = %2%") % "TestNotificationModule03" % signals::SystemInit).str(),
		(boost::format("Module = %1%,\t\tNotification = %2%") % "TestNotificationModule01" % signals::SystemInit).str(),
		(boost::format("Module = %1%,\t\tNotification = %2%") % "TestNotificationModule02" % signals::SystemInit).str(), 
		(boost::format("Module = %1%,\t\tNotification = %2%") % "TestNotificationModule04" % signals::SystemInit).str(),
		(boost::format("Module = %1%,\t\tNotification = %2%") % "TestNotificationModule05" % signals::SystemInit).str(),
		(boost::format("Module = %1%,\t\tNotification = %2%") % "TestNotificationModule06" % signals::SystemInit).str(),
		(boost::format("Module = %1%,\t\tNotification = %2%") % "TestNotificationModule08" % signals::SystemInit).str(),
		(boost::format("Module = %1%,\t\tNotification = %2%") % "TestNotificationModule07" % signals::SystemInit).str()
	};

	int poolOrder = 0;
	for (auto pool : { "initialValues" }) {
		std::string desc(pool); desc += " description";
		std::string unit("units for "); unit += pool;
		config.addPool(pool, desc, unit, 1.0, poolOrder++);
	}
	config.addVariable("simulateLandUnit", true);
	config.addVariable("landUnitBuildSuccess", true);

	ldc.configure(config);
	ldc.run();

	//auto modules = ldc.modules();
	//for (auto& item : ldc.modules()) {
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

BOOST_AUTO_TEST_SUITE_END();
