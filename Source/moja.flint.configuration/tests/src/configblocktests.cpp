#include "moja/test/mocktile.h"

#include "moja/flint/configuration/configblock.h"
#include "moja/flint/configuration/configurationexceptions.h"

#include <boost/test/unit_test.hpp>
#include <turtle/mock.hpp>

namespace conf = moja::flint::configuration;
using moja::test::MockTile;

struct BlockTestsFixture {
	MockTile mockTile;
	BlockTestsFixture() {
		MOCK_EXPECT(mockTile.xSize).returns(1.0);
		MOCK_EXPECT(mockTile.ySize).returns(1.0);
		MOCK_EXPECT(mockTile.xOrigin).returns(10);
		MOCK_EXPECT(mockTile.yOrigin).returns(5);
		MOCK_EXPECT(mockTile.xPixelSize).returns(1.0 / 100.0);
		MOCK_EXPECT(mockTile.yPixelSize).returns(1.0 / 50.0);
		MOCK_EXPECT(mockTile.xPixels).returns(100);
		MOCK_EXPECT(mockTile.yPixels).returns(50);
	}
};

BOOST_FIXTURE_TEST_SUITE(ConfigBlockTests, BlockTestsFixture)

BOOST_AUTO_TEST_CASE(flint_configuration_ConfigBlock_ConstructorThrowsExceptionIfRowIsNegative) {
	BOOST_CHECK_THROW(conf::ConfigBlock(mockTile, -1, 1, 1, 1), conf::LandscapeDefinitionException);
}

BOOST_AUTO_TEST_CASE(flint_configuration_ConfigBlock_ConstructorThrowsExceptionIfColIsNegative) {
	BOOST_CHECK_THROW(conf::ConfigBlock(mockTile, 1, -1, 1, 1), conf::LandscapeDefinitionException);
}

BOOST_AUTO_TEST_CASE(flint_configuration_ConfigBlock_ConstructorThrowsExceptionIfBlockSizeXIsZeroOrNegative) {
	auto badSizes = { 0, -1, -100 };
	for (auto size : badSizes) {
		BOOST_CHECK_THROW(conf::ConfigBlock(mockTile, 1, 1, size, 1), conf::LandscapeDefinitionException);
	}
}

BOOST_AUTO_TEST_CASE(flint_configuration_ConfigBlock_ConstructorThrowsExceptionIfBlockSizeYIsZeroOrNegative) {
	auto badSizes = { 0, -1, -100 };
	for (auto size : badSizes) {
		BOOST_CHECK_THROW(conf::ConfigBlock(mockTile, 1, 1, 1, size), conf::LandscapeDefinitionException);
	}
}

BOOST_AUTO_TEST_CASE(flint_configuration_ConfigBlock_LeftReturnsExpectedLeftSideCoordinate) {
	// First block of a 100x50px tile split into 50 10x5 blocks.
	conf::ConfigBlock r0c0(mockTile, 0, 0, 10, 5);

	// The left side of the first block should be the tile origin.
	BOOST_CHECK_EQUAL(r0c0.left(), 10);

	// The left side of the second block in the first row should be the tile
	// origin + (first block width / tile width).
	conf::ConfigBlock r0c1(mockTile, 0, 1, 10, 5);
	BOOST_CHECK_EQUAL(r0c1.left(), 10.1);
}

BOOST_AUTO_TEST_CASE(flint_configuration_ConfigBlock_TopReturnsExpectedTopCoordinate) {
	// First block of a 100x50px tile split into 50 10x5 blocks.
	conf::ConfigBlock r0c0(mockTile, 0, 0, 10, 5);

	// The top of the first block should be the tile origin + tile height (tile
	// origin is the bottom left corner).
	BOOST_CHECK_EQUAL(r0c0.top(), 6);

	// The top of the second block should be the tile origin + tile height -
	// (first block height / tile height).
	conf::ConfigBlock r1c0(mockTile, 1, 0, 10, 5);
	BOOST_CHECK_EQUAL(r1c0.top(), 5.9);
}

BOOST_AUTO_TEST_CASE(flint_configuration_ConfigBlock_XPixelSizeMatchesTileXPixelSize) {
	conf::ConfigBlock block(mockTile, 1, 1, 1, 1);
	BOOST_CHECK_EQUAL(block.xPixelSize(), mockTile.xPixelSize());
}

BOOST_AUTO_TEST_CASE(flint_configuration_ConfigBlock_YPixelSizeMatchesTileYPixelSize) {
	conf::ConfigBlock block(mockTile, 1, 1, 1, 1);
	BOOST_CHECK_EQUAL(block.yPixelSize(), mockTile.yPixelSize());
}

BOOST_AUTO_TEST_CASE(flint_configuration_ConfigBlock_XPixelsReturnsExpectedValueForFullyContainedBlock) {
	int xPixels = 25;
	conf::ConfigBlock block(mockTile, 1, 1, xPixels, 1);
	BOOST_CHECK_EQUAL(block.xPixels(), xPixels);
}

BOOST_AUTO_TEST_CASE(flint_configuration_ConfigBlock_XPixelsReturnsExpectedValueForPartiallyContainedBlock) {
	int xPixels = 80;
	conf::ConfigBlock block(mockTile, 1, 1, xPixels, 1);
	BOOST_CHECK_EQUAL(block.xPixels(), mockTile.xPixels() - xPixels);
}

BOOST_AUTO_TEST_CASE(flint_configuration_ConfigBlock_YPixelsReturnsExpectedValueForFullyContainedBlock) {
	int yPixels = 10;
	conf::ConfigBlock block(mockTile, 1, 1, 1, yPixels);
	BOOST_CHECK_EQUAL(block.yPixels(), yPixels);
}

BOOST_AUTO_TEST_CASE(flint_configuration_ConfigBlock_YPixelsReturnsExpectedValueForPartiallyContainedBlock) {
	int yPixels = 40;
	conf::ConfigBlock block(mockTile, 1, 1, 1, yPixels);
	BOOST_CHECK_EQUAL(block.yPixels(), mockTile.yPixels() - yPixels);
}

BOOST_AUTO_TEST_SUITE_END();
