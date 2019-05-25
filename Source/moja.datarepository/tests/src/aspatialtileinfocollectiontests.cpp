#include "moja/datarepository/aspatialtileinfocollection.h"
#include "moja/datarepository/datarepositoryexceptions.h"

#include <moja/test/mockproviderrelationalinterface.h>

#include <moja/dynamic.h>

#include <boost/test/unit_test.hpp>

using moja::DynamicObject;
using moja::datarepository::AspatialTileInfoCollection;
using moja::datarepository::LandscapeDefinitionException;
using moja::test::MockProviderRelational;

BOOST_AUTO_TEST_SUITE(AspatialTileInfoCollectionTests);

BOOST_AUTO_TEST_CASE(datarepository_ConstructorThrowsExceptionIfMaxTileSizeIsLessThanOne) {
   auto badTileSizes = {0, -1, -100};
   for (auto tileSize : badTileSizes) {
      auto mockProvider = MockProviderRelational();
      BOOST_CHECK_THROW(AspatialTileInfoCollection(mockProvider, tileSize), LandscapeDefinitionException);
   }
}

BOOST_AUTO_TEST_CASE(datarepository_ConstructorThrowsExceptionIfTileCacheSizeIsLessThanOne) {
   auto badCacheSizes = {0, -1, -100};
   for (auto cacheSize : badCacheSizes) {
      auto mockProvider = MockProviderRelational();
      BOOST_CHECK_THROW(AspatialTileInfoCollection(mockProvider, 100, cacheSize), LandscapeDefinitionException);
   }
}

// See AspatialTileInfoCollectionIntegrationTests for remainder of tests.

BOOST_AUTO_TEST_SUITE_END();
