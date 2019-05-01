#include "moja/flint/configuration/localdomain.h"

#include <Poco/Nullable.h>

#include <boost/test/unit_test.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/format.hpp>

using moja::flint::configuration::LocalDomain;
using moja::flint::configuration::LocalDomainType;
using moja::flint::configuration::LocalDomainIterationType;
using moja::DynamicObject;

BOOST_AUTO_TEST_SUITE(LocalDomainConfigurationTests);

BOOST_AUTO_TEST_CASE(flint_configuration_LocalDomain_ConstructorThrowsExceptionIfSequencerIsEmpty) {
	auto type = LocalDomainType::SpatialTiled;
	auto ittype = LocalDomainIterationType::LandscapeTiles;
	auto badSequencerNames = { "", "  " };
	for (auto sequencerName : badSequencerNames) {
		BOOST_CHECK_THROW(LocalDomain(type, ittype, true, 1, "sequencer_library", sequencerName, "dummy", "dummy", DynamicObject()), std::invalid_argument);
	}
}

BOOST_AUTO_TEST_CASE(flint_configuration_LocalDomain_ConstructorThrowsExceptionIfSimulateLandUnitIsEmpty) {
	auto type = LocalDomainType::SpatialTiled;
	auto ittype = LocalDomainIterationType::LandscapeTiles;
	auto badSimLUNames = { "", "  " };
	for (auto simLUName : badSimLUNames) {
		BOOST_CHECK_THROW(LocalDomain(type, ittype, true, 1, "sequencer_library", "dummy", simLUName, "dummy", DynamicObject()), std::invalid_argument);
	}
}

BOOST_AUTO_TEST_CASE(flint_configuration_LocalDomain_GetType) {
	auto type = LocalDomainType::SpatiallyReferencedSQL;
	auto ittype = LocalDomainIterationType::LandscapeTiles;
	LocalDomain localDomain(type, ittype, true, 1, "sequencer_library", "sequencer", "dummy", "dummy", DynamicObject());
	BOOST_CHECK(type == localDomain.type());
}

BOOST_AUTO_TEST_CASE(flint_configuration_LocalDomain_GetSequencer) {
	auto type = LocalDomainType::SpatialTiled;
	auto ittype = LocalDomainIterationType::LandscapeTiles;
	auto sequencerName = "test";
	LocalDomain localDomain(type, ittype, true, 1, "sequencer_library", sequencerName, "dummy", "dummy", DynamicObject());
	BOOST_CHECK_EQUAL(sequencerName, localDomain.sequencer());
}

BOOST_AUTO_TEST_CASE(flint_configuration_LocalDomain_GetSimulateLandUnit) {
	auto type = LocalDomainType::SpatialTiled;
	auto ittype = LocalDomainIterationType::LandscapeTiles;
	auto simLUName = "test";
	LocalDomain localDomain(type, ittype, true, 1, "sequencer_library", "dummy", simLUName, "dummy", DynamicObject());
	BOOST_CHECK_EQUAL(simLUName, localDomain.simulateLandUnit());
}

BOOST_AUTO_TEST_CASE(flint_configuration_LocalDomain_GetLandscape) {
	DynamicObject landscapeSettings;
	auto k1 = "key1";
	auto v1 = "b";
	auto k2 = "key2";
	auto v2 = 100;
	landscapeSettings[k1] = v1;
	landscapeSettings[k2] = v2;

	auto type = LocalDomainType::SpatialTiled;
	auto ittype = LocalDomainIterationType::LandscapeTiles;
	LocalDomain localDomain(type, ittype, true, 1, "sequencer_library", "sequencer", "dummy", "dummy", landscapeSettings);
	auto ldLandscapeSettings = localDomain.landscapeObject();

	// TODO: Have removed landscape stuff, all classes now
	//std::string ldV1 = ldLandscapeSettings[k1];
	//BOOST_CHECK_EQUAL(v1, ldV1);

	//int ldV2 = ldLandscapeSettings[k2];
	//BOOST_CHECK_EQUAL(v2, ldV2);
}

BOOST_AUTO_TEST_CASE(flint_configuration_LocalDomain_Settings_RandomSeeds) {
	DynamicObject settings;
	auto k1 = "key1";
	auto v1 = "b";
	auto k2 = "key2";
	auto v2 = 100;
	settings[k1] = v1;
	settings[k2] = v2;
	Poco::UInt32 randomSeedGlobal	= 4120625605;
	Poco::UInt32 randomSeedTile		= 0;
	Poco::UInt32 randomSeedBlock	= 1074808114;
	Poco::UInt32 randomSeedCell		= 1158849901;
	settings["randomSeedGlobal" ] = randomSeedGlobal;
	settings["randomSeedTile"	] = randomSeedTile;
	settings["randomSeedBlock"	] = randomSeedBlock;
	settings["randomSeedCell"	] = randomSeedCell;

	Poco::UInt32  extractedRandomSeedGlobal = settings["randomSeedGlobal"].extract<const Poco::UInt32>();
	Poco::UInt32  extractedRandomSeedTile	= settings["randomSeedTile"	 ].extract<const Poco::UInt32>();
	Poco::UInt32  extractedRandomSeedBlock	= settings["randomSeedBlock" ].extract<const Poco::UInt32>();
	Poco::UInt32  extractedRandomSeedCell	= settings["randomSeedCell"	 ].extract<const Poco::UInt32>();
}

BOOST_AUTO_TEST_CASE(flint_configuration_LocalDomain_Settings_Misc) {
	DynamicObject settings;
	Poco::UInt32 value	= 4120625605;
	Poco::Nullable<Poco::UInt32> valueNullPoco;
	boost::optional<Poco::UInt32> valueNullBoost;

	settings["value" ]	= value;

	auto x1 = (boost::format("%1%") % value).str();
	auto x2 = (boost::format("%1%") % valueNullPoco).str();
	auto x3 = (boost::format("%1%") % valueNullBoost).str();

	std::cout << "x1" << x1 << std::endl;
	std::cout << "x2" << x2 << std::endl;
	std::cout << "x3" << x3 << std::endl;
}


BOOST_AUTO_TEST_SUITE_END();
