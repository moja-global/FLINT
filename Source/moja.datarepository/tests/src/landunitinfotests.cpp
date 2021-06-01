#include <moja/datarepository/datarepositoryexceptions.h>
#include <moja/datarepository/landunitinfo.h>

#include <moja/test/mockaspatialtileinfo.h>

#include <boost/test/unit_test.hpp>

namespace flint_datarepository {

using moja::datarepository::LandscapeDefinitionException;
using moja::datarepository::LandUnitInfo;
using moja::test::MockAspatialTileInfo;

BOOST_AUTO_TEST_SUITE(LandUnitInfoTests);

BOOST_AUTO_TEST_CASE(LandUnitInfo_ConstructorThrowsExceptionIfIdIsLessThanOne) {
   MockAspatialTileInfo mockTile;
   auto badIds = {0, -1, -100};
   for (auto id : badIds) {
      BOOST_CHECK_THROW(LandUnitInfo(mockTile, id, 1.0), LandscapeDefinitionException);
   }
}

BOOST_AUTO_TEST_CASE(LandUnitInfo_ConstructorThrowsExceptionIfAreaIsZeroOrNegative) {
   MockAspatialTileInfo mockTile;
   auto badAreas = {0.0, -0.1, -100.0};
   for (auto area : badAreas) {
      BOOST_CHECK_THROW(LandUnitInfo(mockTile, 1, area), LandscapeDefinitionException);
   }
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace flint_datarepository
