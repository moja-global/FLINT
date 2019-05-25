#include <moja/flint/configuration/configtile.h>
#include <moja/flint/configuration/configurationexceptions.h>

#include <boost/test/unit_test.hpp>

#include <turtle/mock.hpp>

namespace conf = moja::flint::configuration;

BOOST_AUTO_TEST_SUITE(ConfigTileTests)

BOOST_AUTO_TEST_CASE(flint_configuration_ConfigTile_ConstructorThrowsExceptionIfXSizeIsZeroOrNegative) {
   auto badXSizes = {0.0, -1.0, -100.0};
   for (double xSize : badXSizes) {
      BOOST_CHECK_THROW(conf::ConfigTile(1, 1, xSize, 1.0, 100, 100), conf::LandscapeDefinitionException);
   }
}

BOOST_AUTO_TEST_CASE(flint_configuration_ConfigTile_ConstructorThrowsExceptionIfYSizeIsZeroOrNegative) {
   auto badYSizes = {0.0, -1.0, -100.0};
   for (double ySize : badYSizes) {
      BOOST_CHECK_THROW(conf::ConfigTile(1, 1, 1.0, ySize, 100, 100), conf::LandscapeDefinitionException);
   }
}

BOOST_AUTO_TEST_CASE(flint_configuration_ConfigTile_ConstructorThrowsExceptionIfXPixelsIsLessThanOne) {
   auto badXPixels = {0, -1, -100};
   for (int xPixels : badXPixels) {
      BOOST_CHECK_THROW(conf::ConfigTile(1, 1, 1.0, 1.0, xPixels, 100), conf::LandscapeDefinitionException);
   }
}

BOOST_AUTO_TEST_CASE(flint_configuration_ConfigTile_ConstructorThrowsExceptionIfYPixelsIsLessThanOne) {
   auto badYPixels = {0, -1, -100};
   for (int yPixels : badYPixels) {
      BOOST_CHECK_THROW(conf::ConfigTile(1, 1, 1.0, 1.0, 100, yPixels), conf::LandscapeDefinitionException);
   }
}

BOOST_AUTO_TEST_CASE(flint_configuration_ConfigTile_XPixelSize) {
   conf::ConfigTile tile(1, 1, 50.0, 1.0, 5, 1);
   double expectedXPixelSize = 10.0;
   BOOST_CHECK_EQUAL(expectedXPixelSize, tile.xPixelSize());
}

BOOST_AUTO_TEST_CASE(flint_configuration_ConfigTile_YPixelSize) {
   conf::ConfigTile tile(1, 1, 1.0, 27.0, 1, 3);
   double expectedYPixelSize = 9.0;
   BOOST_CHECK_EQUAL(expectedYPixelSize, tile.yPixelSize());
}

BOOST_AUTO_TEST_CASE(flint_configuration_ConfigTile_SplitReturnsExpectedNumberOfBlocksForEvenlyDivisibleLandscape) {
   conf::ConfigTile tile(0, 0, 1.0, 1.0, 100, 100);
   auto blocks = tile.split(10, 10);
   BOOST_CHECK_EQUAL(blocks.size(), 100);
}

BOOST_AUTO_TEST_CASE(flint_configuration_ConfigTile_SplitReturnsExpectedNumberOfBlocksForLandscapeWithRemainders) {
   conf::ConfigTile tile(0, 0, 1.0, 1.0, 100, 100);
   auto blocks = tile.split(75, 75);
   BOOST_CHECK_EQUAL(blocks.size(), 4);
}

BOOST_AUTO_TEST_SUITE_END();
