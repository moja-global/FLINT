#include "moja/flint/configuration/json2configurationprovider.h"
#include "moja/flint/configuration/configurationexceptions.h"
#include "moja/flint/configuration/localdomain.h"
#include "moja/flint/configuration/transform.h"
#include "moja/flint/configuration/provider.h"
#include "moja/flint/configuration/variable.h"
#include "moja/flint/configuration/externalvariable.h"
#include "moja/flint/configuration/module.h"
#include "moja/flint/configuration/pool.h"
#include "moja/flint/configuration/spinup.h"

#include "moja/datetime.h"

#include <Poco/TemporaryFile.h>
#include <Poco/FileStream.h>

#include <boost/test/unit_test.hpp>
#include <boost/format.hpp>

using moja::flint::configuration::JSON2ConfigurationProvider;
using moja::flint::configuration::FileNotFoundException;
using moja::flint::configuration::LocalDomainType;
using moja::flint::configuration::LibraryType;
using Poco::TemporaryFile;
using Poco::FileOutputStream;

template<class T>
struct EnumMapping {
	std::string asString;
	T asEnum;
};

struct Library2ConfigurationTestsFixture {
	const std::string configTemplate =
		"{\n"
		"    \"LocalDomain\": {\n"
		"%1%\n"
		"    },\n"
		"    \"Libraries\": {\n"
		"%2%\n"
		"    },\n"
		"    \"Spinup\": {\n"
		"%3%\n"
		"    },\n"
		"    \"SpinupModules\": {\n"
		"%4%\n"
		"    },\n"
		"    \"SpinupVariables\": {\n"
		"%5%\n"
		"    },\n"
		"    \"Pools\": {\n"
		"%6%\n"
		"    },\n"
		"    \"Variables\": {\n"
		"%7%\n"
		"    },\n"
		"    \"Modules\": {\n"
		"%8%\n"
		"    }\n"
		"}";

	const std::string configTemplate2 =		// Pools in Array
		"{\n"
		"    \"LocalDomain\": {\n"
		"%1%\n"
		"    },\n"
		"    \"Libraries\": {\n"
		"%2%\n"
		"    },\n"
		"    \"Spinup\": {\n"
		"%3%\n"
		"    },\n"
		"    \"SpinupModules\": {\n"
		"%4%\n"
		"    },\n"
		"    \"SpinupVariables\": {\n"
		"%5%\n"
		"    },\n"
		"    \"Pools\": [\n"
		"%6%\n"
		"    ],\n"
		"    \"Variables\": {\n"
		"%7%\n"
		"    },\n"
		"    \"Modules\": {\n"
		"%8%\n"
		"    }\n"
		"}";

	const std::string configTemplate3 =		// Variables in array
		"{\n"
		"    \"LocalDomain\": {\n"
		"%1%\n"
		"    },\n"
		"    \"Libraries\": {\n"
		"%2%\n"
		"    },\n"
		"    \"Spinup\": {\n"
		"%3%\n"
		"    },\n"
		"    \"SpinupModules\": {\n"
		"%4%\n"
		"    },\n"
		"    \"SpinupVariables\": {\n"
		"%5%\n"
		"    },\n"
		"    \"Pools\": {\n"
		"%6%\n"
		"    },\n"
		"    \"Variables\": [\n"
		"%7%\n"
		"    ],\n"
		"    \"Modules\": {\n"
		"%8%\n"
		"    }\n"
		"}";

	const std::string configProviderTemplate =
		"{\n"
		"    \"Providers\": {\n"
		"%1%\n"
		"    }\n"
		"}";

	const std::string validLocalDomain =
		"        \"type\": \"spatial_tiled\",\n"
		"        \"start_date\": \"2001/01/01\",\n"
		"        \"end_date\": \"2013/12/31\",\n"
		"        \"sequencer_library\": \"internal.flint\",\n"
		"        \"sequencer\": \"CalendarAndEventSequencer\",\n"
		//"        \"landunitcontroller\": \"Eigen\",\n"
		"        \"simulateLandUnit\": \"Dummy\",\n"
		"        \"landUnitBuildSuccess\": \"Dummy\",\n"
		"        \"landscape\": {\n"
		"            \"provider\": \"Test\",\n"
		"            \"tile_size_x\": 1.0,\n"
		"            \"tile_size_y\": 1.0,\n"
		"            \"x_pixels\": 4000,\n"
		"            \"y_pixels\": 4000,\n"
		"            \"tiles\": [\n"
		"                { \"x\": 34, \"y\": 0 }\n"
		"            ]\n"
		"        }";

	const std::string validLibraries =
		"        \"moja.modules.sleek\": \"external\"";

	const std::string validLibrariesCBM =
		"        \"moja.modules.cbm\": \"external\"";

	const std::string validSpinupEnabled =
		"        \"enabled\": true,\n"
		"        \"simulateLandUnit\": \"Dummy\",\n"
		"        \"landUnitBuildSuccess\": \"Dummy\",\n"
		"        \"sequencer_library\": \"internal.flint\",\n"
		"        \"sequencer\": \"CalendarAndEventSequencer\"\n";

	const std::string validSpinupDisabled =
		"        \"enabled\": false";	

	const std::string validSpinupVariables =
		"        \"TestSpinUpVariable\": 1";

	const std::string validSpinupModules =
		"        \"CalendarAndEventSequencer\" : {\n"
		"            \"library\": \"moja.modules.sleek\",\n"
		"            \"order\": 1,\n"
		"            \"create_new\": true\n"
		"        }";

	const std::string validSpinupModulesCBM =
		"        \"CBMSequencer\" : {\n"
		"            \"library\": \"moja.modules.cbm\",\n"
		"            \"order\": 2,\n"
		"            \"create_new\": true\n"
		"        }";

	const std::string validProviders =
		"        \"raster\" : {\n"
		"            \"type\": \"raster_tiled\"\n"
		"        }";

	const std::string validPools =
		"        \"atmosphere\": 100.0";

	const std::string validPoolsPeastland =		
		"        \"Catotelm_O\": 10.0";

	const std::string validPoolsArray =
		"        { \"C_atmosphere\": 101.0 },"
		"        { \"A_atmosphere\": 102.0 },"
		"        { \"B_atmosphere\": 103.0 }";

	const std::string validPoolsArray2 =
		"        {\n"
		"            \"name\": \"C_atmosphere\",\n"
		"            \"value\": 100.0,\n"
		"            \"description\": \"testDesc\",\n"
		"            \"units\": \"testUnits\",\n"
		"            \"scale\": 1.0,\n"
		"            \"order\": 1\n"
		"		 },\n"
		"        {\n"
		"            \"name\": \"A_atmosphere\",\n"
		"            \"value\": 100.0,\n"
		"            \"description\": \"testDesc\",\n"
		"            \"units\": \"testUnits\",\n"
		"            \"scale\": 1.0,\n"
		"            \"order\": 1\n"
		"		 },\n"
		"        {\n"
		"            \"name\": \"B_atmosphere\",\n"
		"            \"value\": 100.0,\n"
		"            \"description\": \"testDesc\",\n"
		"            \"units\": \"testUnits\",\n"
		"            \"scale\": 1.0,\n"
		"            \"order\": 1\n"
		"		 }\n";

	const std::string validPoolsArray3 =
		"        \"C_atmosphere\","
		"        \"A_atmosphere\","
		"        \"B_atmosphere\"";

	const std::string validVariables =
		"        \"localDomainId\": 1";

	const std::string validVariablesD =
		"        \"D_variable\": 102.2";

	const std::string validVariablesArray1 =
		"        { \"C_variable\": 101.0 },"
		"        { \"A_variable\": 102.0 },"
		"        { \"B_variable\": 103.0 }";

	const std::string validVariablesArray2 =
		"        { \"localDomainId\": 1 }";

	const std::string validVariablesArray3 =
		"        \"localDomainId\"";

	const std::string validModules =
		"        \"CalendarAndEventSequencer\" : {\n"
		"            \"library\": \"moja.modules.sleek\",\n"
		"            \"order\": 1,\n"
		"            \"settings\": { }\n"
		"        }";

	std::shared_ptr<TemporaryFile> writeConfigFile(
			const std::string& confTemplate,
			const std::string& localDomain, const std::string& libraries,
			const std::string& spinup, const std::string& spinupModules,
			const std::string& spinupVariables, const std::string& pools,
			const std::string& variables, const std::string& modules) const {

		auto configFile = std::make_shared<TemporaryFile>();
		if (configFile->exists()) {
			configFile->remove();
		}

		configFile->createFile();
		FileOutputStream configWriter{{configFile->path()} };
		configWriter << boost::format(confTemplate) % localDomain % libraries
			% spinup % spinupModules % spinupVariables % pools % variables % modules;

		configWriter.close();
		return configFile;
	}

	std::shared_ptr<TemporaryFile> writeProviderConfigFile(const std::string& providers) const {
		auto configFile = std::make_shared<TemporaryFile>();
		if (configFile->exists()) {
			configFile->remove();
		}

		configFile->createFile();
		FileOutputStream configWriter{{configFile->path()} };
		configWriter << boost::format(configProviderTemplate) % providers;
		configWriter.close();
		return configFile;
	}
};

BOOST_FIXTURE_TEST_SUITE(Library2ConfigurationTests, Library2ConfigurationTestsFixture);

BOOST_AUTO_TEST_CASE(flint_configuration_JSON2ConfigurationProvider_ConstructorThrowsExceptionIfConfigFileNotFound) {
	BOOST_CHECK_THROW(JSON2ConfigurationProvider({ "notfound.json", "notfound.json" }, { "notfound.json", "notfound.json" }),
					  FileNotFoundException);
}

BOOST_AUTO_TEST_CASE(flint_configuration_JSON2ConfigurationProvider_BuildsExpectedConfiguration) {
	auto configFile = writeConfigFile(configTemplate, validLocalDomain, validLibraries, validSpinupDisabled,
									  validSpinupModules, validSpinupVariables, validPools,
									  validVariables, validModules);

	auto configProviderFile = writeProviderConfigFile(validProviders);

	JSON2ConfigurationProvider provider{{configFile->path()}, {configProviderFile->path()}};
	auto config = provider.createConfiguration();
	BOOST_CHECK_EQUAL(config->startDate().year(), 2001);
	BOOST_CHECK_EQUAL(config->startDate().month(), 1);
	BOOST_CHECK_EQUAL(config->startDate().day(), 1);
	BOOST_CHECK_EQUAL(config->endDate().year(), 2013);
	BOOST_CHECK_EQUAL(config->endDate().month(), 12);
	BOOST_CHECK_EQUAL(config->endDate().day(), 31);
}

BOOST_AUTO_TEST_CASE(flint_configuration_JSON2ConfigurationProvider_BuildsExpectedConfiguration2_PoolArray) {
	auto configFile = writeConfigFile(configTemplate2, 
		validLocalDomain, validLibraries, validSpinupDisabled,
		validSpinupModules, validSpinupVariables, validPoolsArray,
		validVariables, validModules);

	auto configProviderFile = writeProviderConfigFile(validProviders);

	JSON2ConfigurationProvider provider{{configFile->path()}, {configProviderFile->path()}};
	auto config = provider.createConfiguration();

	BOOST_CHECK_EQUAL(config->startDate().year(), 2001);
	BOOST_CHECK_EQUAL(config->startDate().month(), 1);
	BOOST_CHECK_EQUAL(config->startDate().day(), 1);
	BOOST_CHECK_EQUAL(config->endDate().year(), 2013);
	BOOST_CHECK_EQUAL(config->endDate().month(), 12);
	BOOST_CHECK_EQUAL(config->endDate().day(), 31);

	// Check order of Pools
	auto pools = config->pools();

	auto pool01 = pools[0];
	auto pool02 = pools[1];
	auto pool03 = pools[2];

	BOOST_CHECK_EQUAL(pool01->name(), "C_atmosphere");
	BOOST_CHECK_EQUAL(pool02->name(), "A_atmosphere");
	BOOST_CHECK_EQUAL(pool03->name(), "B_atmosphere");
}

BOOST_AUTO_TEST_CASE(flint_configuration_JSON2ConfigurationProvider_BuildsExpectedConfiguration3_PoolArrayObjects) {
	auto configFile = writeConfigFile(configTemplate2, validLocalDomain, validLibraries, validSpinupDisabled,
		validSpinupModules, validSpinupVariables, validPoolsArray2,
		validVariables, validModules);

	auto configProviderFile = writeProviderConfigFile(validProviders);

	JSON2ConfigurationProvider provider{{configFile->path()}, {configProviderFile->path()}};
	auto config = provider.createConfiguration();
	BOOST_CHECK_EQUAL(config->startDate().year(), 2001);
	BOOST_CHECK_EQUAL(config->startDate().month(), 1);
	BOOST_CHECK_EQUAL(config->startDate().day(), 1);
	BOOST_CHECK_EQUAL(config->endDate().year(), 2013);
	BOOST_CHECK_EQUAL(config->endDate().month(), 12);
	BOOST_CHECK_EQUAL(config->endDate().day(), 31);

	// Check order of Pools
	auto pools = config->pools();

	auto pool01 = pools[0];
	auto pool02 = pools[1];
	auto pool03 = pools[2];

	BOOST_CHECK_EQUAL(pool01->name(), "C_atmosphere");
	BOOST_CHECK_EQUAL(pool02->name(), "A_atmosphere");
	BOOST_CHECK_EQUAL(pool03->name(), "B_atmosphere");
}

BOOST_AUTO_TEST_CASE(flint_configuration_JSON2ConfigurationProvider_BuildsExpectedConfiguration4_PoolArrayObjects) {
	auto configFile = writeConfigFile(configTemplate2, validLocalDomain, validLibraries, validSpinupDisabled,
		validSpinupModules, validSpinupVariables, validPoolsArray3,
		validVariables, validModules);

	auto configProviderFile = writeProviderConfigFile(validProviders);

	JSON2ConfigurationProvider provider{{configFile->path()}, {configProviderFile->path()}};
	auto config = provider.createConfiguration();
	BOOST_CHECK_EQUAL(config->startDate().year(), 2001);
	BOOST_CHECK_EQUAL(config->startDate().month(), 1);
	BOOST_CHECK_EQUAL(config->startDate().day(), 1);
	BOOST_CHECK_EQUAL(config->endDate().year(), 2013);
	BOOST_CHECK_EQUAL(config->endDate().month(), 12);
	BOOST_CHECK_EQUAL(config->endDate().day(), 31);

	// Check order of Pools
	auto pools = config->pools();

	auto pool01 = pools[0];
	auto pool02 = pools[1];
	auto pool03 = pools[2];

	BOOST_CHECK_EQUAL(pool01->name(), "C_atmosphere");
	BOOST_CHECK_EQUAL(pool02->name(), "A_atmosphere");
	BOOST_CHECK_EQUAL(pool03->name(), "B_atmosphere");
}

BOOST_AUTO_TEST_CASE(flint_configuration_JSON2ConfigurationProvider_BuildsExpectedLocalDomainConfiguration) {
	auto configFile = writeConfigFile(configTemplate, validLocalDomain, validLibraries, validSpinupDisabled,
									  validSpinupModules, validSpinupVariables, validPools,
									  validVariables, validModules);

	auto configProviderFile = writeProviderConfigFile(validProviders);

	JSON2ConfigurationProvider provider{{configFile->path()}, {configProviderFile->path()}};
	auto config = provider.createConfiguration();
	auto localDomain = config->localDomain();
	
	auto expectedType = LocalDomainType::SpatialTiled;
	auto actualType = localDomain->type();
	BOOST_CHECK(expectedType == actualType);

	auto expectedSequencer = "CalendarAndEventSequencer";
	auto actualSequencer = localDomain->sequencer();
	BOOST_CHECK_EQUAL(expectedSequencer, actualSequencer);

	BOOST_CHECK(localDomain->landscapeObject()->iterationLandscapeTiles()->tileSizeX() == 1.0);
	BOOST_CHECK(localDomain->landscapeObject()->iterationLandscapeTiles()->tileSizeY() == 1.0);
	BOOST_CHECK(localDomain->landscapeObject()->iterationLandscapeTiles()->xPixels() == 4000);
	BOOST_CHECK(localDomain->landscapeObject()->iterationLandscapeTiles()->yPixels() == 4000);
	
	auto& tiles = localDomain->landscapeObject()->iterationLandscapeTiles()->landscapeTiles();
	auto tile = tiles[0];
	BOOST_CHECK(tile.xIndex() == 34);
	BOOST_CHECK(tile.yIndex() == 0);
}

BOOST_AUTO_TEST_CASE(flint_configuration_JSON2ConfigurationProvider_BuildsExpectedProviderConfiguration) {
	auto configFile = writeConfigFile(configTemplate, validLocalDomain, validLibraries, validSpinupDisabled,
									  validSpinupModules, validSpinupVariables, validPools,
									  validVariables, validModules);

	auto configProviderFile = writeProviderConfigFile(validProviders);

	JSON2ConfigurationProvider provider{{configFile->path()}, {configProviderFile->path()}};
	auto config = provider.createConfiguration();
	auto dataProvider = config->providers()[0];
	BOOST_CHECK_EQUAL(dataProvider->name(), "raster");
	auto dataProviderSettings = dataProvider->settings();
	BOOST_CHECK(dataProviderSettings["type"].convert<std::string>() == "raster_tiled");
}

BOOST_AUTO_TEST_CASE(flint_configuration_JSON2ConfigurationProvider_BuildsExpectedPoolConfiguration) {
	auto configFile = writeConfigFile(configTemplate, validLocalDomain, validLibraries, validSpinupDisabled,
									  validSpinupModules, validSpinupVariables, validPools,
									  validVariables, validModules);

	auto configProviderFile = writeProviderConfigFile(validProviders);

	JSON2ConfigurationProvider provider{{configFile->path()}, {configProviderFile->path()}};
	auto config = provider.createConfiguration();
	auto pool = config->pools()[0];
	BOOST_CHECK_EQUAL(pool->name(), "atmosphere");
	BOOST_CHECK_EQUAL(pool->initValue(), 100.0);
}

BOOST_AUTO_TEST_CASE(flint_configuration_JSON2ConfigurationProvider_BuildsExpectedInternalVariableConfiguration) {
	auto configFile = writeConfigFile(configTemplate, validLocalDomain, validLibraries, validSpinupDisabled,
									  validSpinupModules, validSpinupVariables, validPools,
									  validVariables, validModules);

	auto configProviderFile = writeProviderConfigFile(validProviders);

	JSON2ConfigurationProvider provider{{configFile->path()}, {configProviderFile->path()}};
	auto config = provider.createConfiguration();
	auto variable = config->variables()[0];
	BOOST_CHECK_EQUAL(variable->name(), "localDomainId");
	BOOST_CHECK(variable->value() == 1);
}

BOOST_AUTO_TEST_CASE(flint_configuration_JSON2ConfigurationProvider_BuildsExpectedModuleConfiguration) {
	auto moduleConfig =
		"\"testmodule\": {\n"
		"    \"library\": \"moja.modules.sleek\",\n"
		"    \"order\": 1,\n"
		"    \"settings\": {\n"
		"        \"foo\": \"bar\"\n"
		"    }\n"
		"}";

	auto configFile = writeConfigFile(configTemplate, validLocalDomain, validLibraries, validSpinupDisabled,
									  validSpinupModules, validSpinupVariables, validPools,
									  validVariables, moduleConfig);

	auto configProviderFile = writeProviderConfigFile(validProviders);

	JSON2ConfigurationProvider provider{{configFile->path()}, {configProviderFile->path()}};
	auto config = provider.createConfiguration();
	auto module = config->modules()[0];
	BOOST_CHECK_EQUAL(module->name(), "testmodule");
	auto settings = module->settings();
	BOOST_CHECK(settings["foo"].convert<std::string>() == "bar");
}

BOOST_AUTO_TEST_CASE(flint_configuration_JSON2ConfigurationProvider_BuildsExpectedModuleConfiguration2) {
	auto moduleConfig =
		"\"testmoduleNotEnabled\": {\n"
		"    \"enabled\": false,\n"
		"    \"library\": \"moja.modules.sleek\",\n"
		"    \"order\": 1,\n"
		"    \"settings\": {\n"
		"        \"foo\": \"bar\"\n"
		"    }\n"
		"},\n"
		"\"testmodule\": {\n"
		"    \"enabled\": true,\n"
		"    \"library\": \"moja.modules.sleek\",\n"
		"    \"order\": 1,\n"
		"    \"settings\": {\n"
		"        \"foo\": \"bar\"\n"
		"    }\n"
		"}";

	auto configFile = writeConfigFile(configTemplate, validLocalDomain, validLibraries, validSpinupDisabled,
		validSpinupModules, validSpinupVariables, validPools,
		validVariables, moduleConfig);

	auto configProviderFile = writeProviderConfigFile(validProviders);

	JSON2ConfigurationProvider provider{{configFile->path()}, {configProviderFile->path()}};
	auto config = provider.createConfiguration();
	auto module = config->modules()[0];
	BOOST_CHECK_EQUAL(module->name(), "testmodule");
	auto settings = module->settings();
	BOOST_CHECK(settings["foo"].convert<std::string>() == "bar");
}

BOOST_AUTO_TEST_CASE(flint_configuration_JSON2ConfigurationProvider_BuildsExpectedModuleConfiguration3) {
	auto moduleConfig =
		"\"testmoduleEnabled\": {\n"
		"    \"enabled\": true,\n"
		"    \"library\": \"moja.modules.sleek\",\n"
		"    \"order\": 1,\n"
		"    \"settings\": {\n"
		"        \"foo\": \"bar\"\n"
		"    }\n"
		"},\n"
		"\"testmodule\": {\n"
		"    \"enabled\": false,\n"
		"    \"library\": \"moja.modules.sleek\",\n"
		"    \"order\": 1,\n"
		"    \"settings\": {\n"
		"        \"foo\": \"bar\"\n"
		"    }\n"
		"}";

	auto configFile = writeConfigFile(configTemplate, validLocalDomain, validLibraries, validSpinupDisabled,
		validSpinupModules, validSpinupVariables, validPools,
		validVariables, moduleConfig);

	auto configProviderFile = writeProviderConfigFile(validProviders);

	JSON2ConfigurationProvider provider{{configFile->path()}, {configProviderFile->path()}};
	auto config = provider.createConfiguration();
	auto module = config->modules()[0];
	BOOST_CHECK_EQUAL(module->name(), "testmoduleEnabled");
	auto settings = module->settings();
	BOOST_CHECK(settings["foo"].convert<std::string>() == "bar");
}

BOOST_AUTO_TEST_CASE(flint_configuration_JSON2ConfigurationProvider_BuildsExpectedModuleConfiguration4) {
	auto moduleConfig =
		"\"testmoduleNotEnabled\": {\n"
		"    \"enabled\": false,\n"
		"    \"library\": \"moja.modules.sleek\",\n"
		"    \"order\": 1,\n"
		"    \"settings\": {\n"
		"        \"foo\": \"bar\"\n"
		"    }\n"
		"},\n"
		"\"testmodule\": {\n"
		"    \"enabled\": false,\n"
		"    \"library\": \"moja.modules.sleek\",\n"
		"    \"order\": 1,\n"
		"    \"settings\": {\n"
		"        \"foo\": \"bar\"\n"
		"    }\n"
		"}";

	auto configFile = writeConfigFile(configTemplate, validLocalDomain, validLibraries, validSpinupDisabled,
		validSpinupModules, validSpinupVariables, validPools,
		validVariables, moduleConfig);

	auto configProviderFile = writeProviderConfigFile(validProviders);

	JSON2ConfigurationProvider provider{{configFile->path()}, {configProviderFile->path()}};
	auto config = provider.createConfiguration();
	BOOST_CHECK_EQUAL(config->modules().size(), 0);	// all modules were set enabled = false
}

BOOST_AUTO_TEST_CASE(flint_configuration_JSON2ConfigurationProvider_BuildsExpectedMultiValueInternalVariableConfiguration) {
	auto variableConfig =
		"\"soil\": {\n"
		"    \"fracBlah\": 0.32,\n"
		"    \"fracWhatevs\": 0.54\n"
		"}";

	auto configFile = writeConfigFile(configTemplate, validLocalDomain, validLibraries, validSpinupDisabled,
									  validSpinupModules, validSpinupVariables, validPools,
									  variableConfig, validModules);

	auto configProviderFile = writeProviderConfigFile(validProviders);

	JSON2ConfigurationProvider provider{{configFile->path()}, {configProviderFile->path()}};
	auto config = provider.createConfiguration();
	auto variable = config->variables()[0];
	BOOST_CHECK_EQUAL(variable->name(), "soil");
	auto variableTable = variable->value();
	BOOST_CHECK(variableTable["fracBlah"] == 0.32);
	BOOST_CHECK(variableTable["fracWhatevs"] == 0.54);
}

BOOST_AUTO_TEST_CASE(flint_configuration_JSON2ConfigurationProvider_BuildsExpectedVectorInternalVariableConfiguration) {
	auto variableConfig = "\"stuff\": [1, 7]\n";
	auto configFile = writeConfigFile(configTemplate, validLocalDomain, validLibraries, validSpinupDisabled,
									  validSpinupModules, validSpinupVariables, validPools,
									  variableConfig, validModules);

	auto configProviderFile = writeProviderConfigFile(validProviders);

	JSON2ConfigurationProvider provider{{configFile->path()}, {configProviderFile->path()}};
	auto config = provider.createConfiguration();
	auto variable = config->variables()[0];
	BOOST_CHECK_EQUAL(variable->name(), "stuff");
	auto values = variable->value();
	BOOST_CHECK(values[0] == 1);
	BOOST_CHECK(values[1] == 7);
}

BOOST_AUTO_TEST_CASE(flint_configuration_JSON2ConfigurationProvider_BuildsExpectedVectorOfStructInternalVariableConfiguration) {
	auto variableConfig =
		"\"stuff\": [\n"
		"    {\n"
		"        \"thing\": 1\n"
		"    },\n"
		"    {\n"
		"        \"thing\": 2\n"
		"    }\n"
		"]";

	auto configFile = writeConfigFile(configTemplate, validLocalDomain, validLibraries, validSpinupDisabled,
									  validSpinupModules, validSpinupVariables, validPools,
									  variableConfig, validModules);

	auto configProviderFile = writeProviderConfigFile(validProviders);

	JSON2ConfigurationProvider provider{{configFile->path()}, {configProviderFile->path()}};
	auto config = provider.createConfiguration();
	auto variable = config->variables()[0];
	BOOST_CHECK_EQUAL(variable->name(), "stuff");
	auto values = variable->value();
	BOOST_CHECK(values.isVector());

	int expectedValue = 1;
	for (auto row : values) {
		BOOST_CHECK(row["thing"].convert<int>() == expectedValue);
		expectedValue++;
	}
}

BOOST_AUTO_TEST_CASE(flint_configuration_JSON2ConfigurationProvider_BuildsExpectedDateVariable) {
	auto variableConfig = "\"testDate\": { \"$date\": \"2000/10/25\" }\n";

	auto configFile = writeConfigFile(configTemplate, validLocalDomain, validLibraries, validSpinupDisabled,
		validSpinupModules, validSpinupVariables, validPools,
		variableConfig, validModules);

	auto configProviderFile = writeProviderConfigFile(validProviders);

	JSON2ConfigurationProvider provider{{configFile->path()}, {configProviderFile->path()}};
	auto config = provider.createConfiguration();
	auto variable = config->variables()[0];
	BOOST_CHECK_EQUAL(variable->name(), "testDate");
	const auto& test = variable->value().extract<moja::DateTime>();
	BOOST_CHECK_EQUAL(test.year(), 2000);
	BOOST_CHECK_EQUAL(test.month(), 10);
	BOOST_CHECK_EQUAL(test.day(), 25);
}

BOOST_AUTO_TEST_CASE(flint_configuration_JSON2ConfigurationProvider_BuildsExpectedDateArrayVariable) {
	auto variableConfig =
		"\"testDateArray\": [\n"
		"    { \"$date\": \"2000/10/25\" },\n"
		"    { \"$date\": \"2001/02/01\" },\n"
		"    { \"$date\": \"2002/12/05\" },\n"
		"    { \"$date\": \"2003/11/15\" }\n"
		"]";

	auto configFile = writeConfigFile(configTemplate, validLocalDomain, validLibraries, validSpinupDisabled,
		validSpinupModules, validSpinupVariables, validPools,
		variableConfig, validModules);

	auto configProviderFile = writeProviderConfigFile(validProviders);

	JSON2ConfigurationProvider provider{{configFile->path()}, {configProviderFile->path()}};
	auto config = provider.createConfiguration();
	auto variable = config->variables()[0];

	BOOST_CHECK_EQUAL(variable->name(), "testDateArray");
	auto dynamicArray = variable->value().extract<moja::DynamicVector>();

	for (auto& dateDynamic : dynamicArray) {
		moja::DateTime test = dateDynamic.extract<moja::DateTime>();
	}
}

BOOST_AUTO_TEST_CASE(flint_configuration_JSON2ConfigurationProvider_BuildsExpectedDateVariable_inArray) {
	auto variableConfig = "{ \"testDate\": { \"$date\": \"2000/10/25\" }}\n";

	auto configFile = writeConfigFile(configTemplate3, validLocalDomain, validLibraries, validSpinupDisabled,
		validSpinupModules, validSpinupVariables, validPools,
		variableConfig, validModules);

	auto configProviderFile = writeProviderConfigFile(validProviders);

	JSON2ConfigurationProvider provider{{configFile->path()}, {configProviderFile->path()}};
	auto config = provider.createConfiguration();
	auto variable = config->variables()[0];
	BOOST_CHECK_EQUAL(variable->name(), "testDate");
	moja::DateTime test = variable->value().extract<moja::DateTime>();
	BOOST_CHECK_EQUAL(test.year(), 2000);
	BOOST_CHECK_EQUAL(test.month(), 10);
	BOOST_CHECK_EQUAL(test.day(), 25);
}

BOOST_AUTO_TEST_CASE(flint_configuration_JSON2ConfigurationProvider_BuildsExpectedDateArrayVariable_inArray) {
	auto variableConfig =
		"{\"testDateArray\": [\n"
		"    { \"$date\": \"2000/10/25\" },\n"
		"    { \"$date\": \"2001/02/01\" },\n"
		"    { \"$date\": \"2002/12/05\" },\n"
		"    { \"$date\": \"2003/11/15\" }\n"
		"]}";

	auto configFile = writeConfigFile(configTemplate3, validLocalDomain, validLibraries, validSpinupDisabled,
		validSpinupModules, validSpinupVariables, validPools,
		variableConfig, validModules);

	auto configProviderFile = writeProviderConfigFile(validProviders);

	JSON2ConfigurationProvider provider{{configFile->path()}, {configProviderFile->path()}};
	auto config = provider.createConfiguration();
	auto variable = config->variables()[0];

	BOOST_CHECK_EQUAL(variable->name(), "testDateArray");
	auto dynamicArray = variable->value().extract<moja::DynamicVector>();

	for (const auto& dateDynamic : dynamicArray) {
		moja::DateTime test = dateDynamic.extract<moja::DateTime>();
	}
}

BOOST_AUTO_TEST_CASE(flint_configuration_JSON2ConfigurationProvider_BuildsExpectedConfiguration2_VariableArray) {
	auto configFile = writeConfigFile(configTemplate3,
		validLocalDomain, validLibraries, validSpinupDisabled,
		validSpinupModules, validSpinupVariables, validPools,
		validVariablesArray1, validModules);

	auto configProviderFile = writeProviderConfigFile(validProviders);

	JSON2ConfigurationProvider provider{{configFile->path()}, {configProviderFile->path()}};
	auto config = provider.createConfiguration();

	// Check order of Variables
	auto variables = config->variables();

	auto variables01 = variables[0];
	auto variables02 = variables[1];
	auto variables03 = variables[2];

	BOOST_CHECK_EQUAL(variables01->name(), "C_variable");
	BOOST_CHECK_EQUAL(variables02->name(), "A_variable");
	BOOST_CHECK_EQUAL(variables03->name(), "B_variable");
}


BOOST_AUTO_TEST_CASE(flint_configuration_JSON2ConfigurationProvider_CheckModuleOrderAfterLoadIsSameAsDefinitionInJSON) {
	const int moduleCount = 9;
	std::string moduleName[moduleCount] = {
		"TestModule0",
		"TestModule1",
		"TestModule2",
		"TestModule3",
		"AABB",
		"AAAA",
		"ZZZZZZ",
		"aaaaaaa",
		"12345"
	};

	auto moduleConfig =
		"\"" + moduleName[0] + "\" : {\n"
		"    \"library\": \"moja.modules.sleek\",\n"
		"    \"order\": 1,\n"
		"    \"settings\": { }\n"
		"},\n"
		"\"" + moduleName[1] + "\" : {\n"
		"    \"library\": \"moja.modules.sleek\",\n"
		"    \"order\": 2,\n"
		"    \"settings\": { }\n"
		"},\n"
		"\"" + moduleName[2] + "\" : {\n"
		"    \"library\": \"moja.modules.sleek\",\n"
		"    \"order\": 3,\n"
		"    \"settings\": { }\n"
		"},\n"
		"\"" + moduleName[3] + "\" : {\n"
		"    \"library\": \"moja.modules.sleek\",\n"
		"    \"order\": 4,\n"
		"    \"settings\": { }\n"
		"},\n"
		"\"" + moduleName[4] + "\" : {\n"
		"    \"library\": \"moja.modules.sleek\",\n"
		"    \"order\": 5,\n"
		"    \"settings\": { }\n"
		"},\n"
		"\"" + moduleName[5] + "\" : {\n"
		"    \"library\": \"moja.modules.sleek\",\n"
		"    \"order\": 6\n"
		"},\n"
		"\"" + moduleName[6] + "\" : {\n"
		"    \"library\": \"moja.modules.sleek\",\n"
		"    \"order\": 7\n"
		"},\n"
		"\"" + moduleName[7] + "\" : {\n"
		"    \"library\": \"moja.modules.sleek\",\n"
		"    \"order\": 8\n"
		"},\n"
		"\"" + moduleName[8] + "\" : {\n"
		"    \"library\": \"moja.modules.sleek\",\n"
		"    \"order\": 9\n"
		"}";

	auto configFile = writeConfigFile(configTemplate, validLocalDomain, validLibraries, validSpinupDisabled,
									  validSpinupModules, validSpinupVariables, validPools,
									  validVariables, moduleConfig);

	auto configProviderFile = writeProviderConfigFile(validProviders);

	JSON2ConfigurationProvider provider{{configFile->path()}, {configProviderFile->path()}};
	auto config = provider.createConfiguration();
	auto modules = config->modules();

	BOOST_CHECK_EQUAL(modules.size(), moduleCount);

	for (int i = 0; i < moduleCount; i++) {
		auto module = modules[i];
		BOOST_CHECK_EQUAL(module->name(), moduleName[i]);
		BOOST_CHECK_EQUAL(module->order(), i + 1);
	}
}

BOOST_AUTO_TEST_CASE(flint_configuration_JSON2ConfigurationProvider_BuildsExpectedSpinupConfiguration) {
	auto configFile = writeConfigFile(configTemplate, validLocalDomain, validLibraries, validSpinupEnabled,
									  validSpinupModules, validSpinupVariables, validPools,
									  validVariables, validModules);

	auto configProviderFile = writeProviderConfigFile(validProviders);
	JSON2ConfigurationProvider provider{{configFile->path()}, {configProviderFile->path()}};
	auto config = provider.createConfiguration();

	auto variable = config->variables()[0];
	BOOST_CHECK_EQUAL(config->spinup()->enabled(), true);
	BOOST_CHECK_EQUAL(config->spinup()->sequencerName(), "CalendarAndEventSequencer");
	BOOST_CHECK_EQUAL(config->spinup()->sequencerLibrary(), "internal.flint");
}

BOOST_AUTO_TEST_CASE(flint_configuration_JSON2ConfigurationProvider_BuildsExpectedConfigurationMergeOne) {	
	auto moduleConfig =
		"		\"testmoduleEnabled\": {\n"
		"			\"enabled\": true,\n"
		"			\"library\": \"moja.modules.sleek\",\n"
		"			\"order\": 1,\n"
		"			\"settings\": {\n"
		"				\"foo1\": \"bar1\"\n"
		"			}\n"
		"		},\n"
		"		\"testmodule\": {\n"
		"			\"enabled\": true,\n"
		"			\"library\": \"moja.modules.cbm\",\n"
		"			\"order\": 2,\n"
		"			\"settings\": {\n"
		"				\"foo\": \"bar\"\n"
		"			}\n"
		"		}";

	auto configFile = writeConfigFile(configTemplate, validLocalDomain, validLibraries, validSpinupDisabled,
		validSpinupModules, validSpinupVariables, validPools,
		validVariables, moduleConfig);	

	auto configProviderFile = writeProviderConfigFile(validProviders);

	JSON2ConfigurationProvider provider{ {configFile->path(), configFile->path() }, { configProviderFile->path() } };

	auto config = provider.createConfiguration();

	auto moduesSize = config->modules().size();
	BOOST_CHECK_EQUAL(moduesSize, 2);

	auto module = config->modules()[0];
	BOOST_CHECK_EQUAL(module->name(), "testmoduleEnabled");
	auto settings = module->settings();
	BOOST_CHECK(settings["foo1"].convert<std::string>() == "bar1");
}

BOOST_AUTO_TEST_CASE(flint_configuration_JSON2ConfigurationProvider_BuildsExpectedConfigurationMergeTwo) {	
	auto configFile = writeConfigFile(configTemplate, validLocalDomain, validLibraries, validSpinupDisabled,
		validSpinupModules, validSpinupVariables, validPools,
		validVariables, validModules);	

	auto configFile1 = writeConfigFile(configTemplate,
		validLocalDomain, validLibrariesCBM, validSpinupDisabled,
		validSpinupModules, validSpinupVariables, validPoolsPeastland,
		validVariablesD, validModules);

	auto configProviderFile = writeProviderConfigFile(validProviders);

	JSON2ConfigurationProvider provider{ { configFile->path(), configFile1->path() },{ configProviderFile->path() } };

	auto config = provider.createConfiguration();

	auto moduesSize = config->modules().size();
	BOOST_CHECK_EQUAL(moduesSize, 1);

	auto module = config->modules()[0];
	BOOST_CHECK_EQUAL(module->name(), "CalendarAndEventSequencer");

	auto variables = config->variables();
	auto var1Name = variables[0]->name();
	auto var2Name = variables[1]->name();
	BOOST_CHECK_EQUAL(var2Name, "localDomainId");
	BOOST_CHECK_EQUAL(var1Name, "D_variable");	

	auto pools = config->pools();
	BOOST_CHECK_EQUAL(pools[0]->name(), "Catotelm_O");
	BOOST_CHECK_EQUAL(pools[1]->name(), "atmosphere");

	auto librarieSize = config->libraries().size();
	BOOST_CHECK_EQUAL(librarieSize, 2);
}


BOOST_AUTO_TEST_SUITE_END();
