#include "moja/test/mockaspatialtileinfocollection.h"

#include "moja/datarepository/aspatialtileinfo.h"
#include "moja/datarepository/datarepositoryexceptions.h"

#include "moja/dynamic.h"
#include "moja/types.h"

#include <boost/test/unit_test.hpp>

using moja::DynamicVar;
using moja::DynamicObject;
using moja::Int64;
using moja::datarepository::AspatialTileInfo;
using moja::datarepository::LandscapeDefinitionException;
using moja::test::MockAspatialTileInfoCollection;

BOOST_AUTO_TEST_SUITE(AspatialTileInfoTests);

BOOST_AUTO_TEST_CASE(datarepository_ConstructorThrowsExceptionIfIdIsLessThanOne) {
	MockAspatialTileInfoCollection mockCollection;
	auto badIds = { 0, -1, -100 };
	for (auto id : badIds) {
		BOOST_CHECK_THROW(AspatialTileInfo(mockCollection, id),
						  LandscapeDefinitionException);
	}
}

BOOST_AUTO_TEST_CASE(datarepository_InitialSizeSetFromCollection) {
	MockAspatialTileInfoCollection mockCollection;
	std::vector<int> sizes = { 1, 50, 75, 200 };
	for (int i = 0; i < sizes.size(); i++) {
		int tileId = i + 1;
		MOCK_EXPECT(mockCollection.luCount).with(tileId).returns(sizes[i]);
		AspatialTileInfo tile(mockCollection, tileId);
		BOOST_CHECK_EQUAL(tile.size(), sizes[i]);
	}
}

BOOST_AUTO_TEST_CASE(datarepository_LandUnitAtIndexReturnsExpectedLandUnit) {
	int numTiles = 4;
	int tileSize = 5;
    
    std::shared_ptr<std::map<Int64, DynamicVar>> luData =
        std::make_shared<std::map<Int64, DynamicVar>>();

    for (int i = 0; i < numTiles * tileSize; i++) {
        luData->insert(std::make_pair(i, DynamicObject({ { "area", 1 } })));
    }

	MockAspatialTileInfoCollection mockCollection;
	MOCK_EXPECT(mockCollection.tileCount).returns(numTiles);
	MOCK_EXPECT(mockCollection.luCount).returns(tileSize);
    MOCK_EXPECT(mockCollection.fetchLUBlock).returns(luData);

	int tileId = 2;
	AspatialTileInfo tile(mockCollection, tileId);
	for (int i = 0; i < tileSize; i++) {
		auto lu = tile.landUnitAtIndex(i);
		Int64 expectedId = tileId + i * numTiles;
		BOOST_CHECK_EQUAL(lu->id(), expectedId);
	}
}

BOOST_AUTO_TEST_CASE(datarepository_IterateLandUnits) {
	int numTiles = 3;
	int tileSize = 10;

    std::shared_ptr<std::map<Int64, DynamicVar>> luData =
        std::make_shared<std::map<Int64, DynamicVar>>();

    for (int i = 0; i < numTiles * tileSize; i++) {
        luData->insert(std::make_pair(i, DynamicObject({ { "area", 1 } })));
    }

	MockAspatialTileInfoCollection mockCollection;
	MOCK_EXPECT(mockCollection.tileCount).returns(numTiles);
	MOCK_EXPECT(mockCollection.luCount).returns(tileSize);
    MOCK_EXPECT(mockCollection.fetchLUBlock).returns(luData);

	int tileId = 1;
	AspatialTileInfo tile(mockCollection, 1);
	Int64 expectedId = tileId;
	int count = 0;
	for (auto lu : tile) {
		BOOST_CHECK_EQUAL(lu.id(), expectedId);
		expectedId += numTiles;
		count++;
	}

	BOOST_CHECK_EQUAL(count, tileSize);
}

BOOST_AUTO_TEST_SUITE_END();
