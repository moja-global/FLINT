#include "moja/test/mockaspatialtileinfo.h"

#include "moja/datarepository/landunitinfo.h"
#include "moja/datarepository/datarepositoryexceptions.h"

#include <boost/test/unit_test.hpp>

using moja::test::MockAspatialTileInfo;
using moja::datarepository::LandUnitInfo;
using moja::datarepository::LandscapeDefinitionException;

BOOST_AUTO_TEST_SUITE(LandUnitInfoTests);

BOOST_AUTO_TEST_CASE(datarepository_LandUnitInfo_ConstructorThrowsExceptionIfIdIsLessThanOne) {
	MockAspatialTileInfo mockTile;
	auto badIds = { 0, -1, -100 };
	for (auto id : badIds) {
		BOOST_CHECK_THROW(LandUnitInfo(mockTile, id, 1.0),
						  LandscapeDefinitionException);
	}
}

BOOST_AUTO_TEST_CASE(datarepository_LandUnitInfo_ConstructorThrowsExceptionIfAreaIsZeroOrNegative) {
	MockAspatialTileInfo mockTile;
	auto badAreas = { 0.0, -0.1, -100.0 };
	for (auto area : badAreas) {
		BOOST_CHECK_THROW(LandUnitInfo(mockTile, 1, area),
						  LandscapeDefinitionException);
	}
}

BOOST_AUTO_TEST_SUITE_END();
