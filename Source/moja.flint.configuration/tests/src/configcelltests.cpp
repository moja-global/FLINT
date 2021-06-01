#include <moja/flint/configuration/configcell.h>
#include <moja/flint/configuration/configurationexceptions.h>

#include <moja/test/mockblock.h>

#include <boost/test/unit_test.hpp>

#include <turtle/mock.hpp>

namespace flint_configuration {

namespace conf = moja::flint::configuration;
using moja::test::MockBlock;

struct CellTestsFixture {
   MockBlock mockBlock;
};

BOOST_FIXTURE_TEST_SUITE(ConfigCellTests, CellTestsFixture)

BOOST_AUTO_TEST_CASE(ConfigCell_ConstructorThrowsExceptionIfRowIsNegative) {
   MOCK_EXPECT(mockBlock.xPixels).returns(1);
   BOOST_CHECK_THROW(conf::ConfigCell(mockBlock, -1, 1), conf::LandscapeDefinitionException);
}

BOOST_AUTO_TEST_CASE(ConfigCell_ConstructorThrowsExceptionIfColIsNegative) {
   MOCK_EXPECT(mockBlock.xPixels).returns(1);
   BOOST_CHECK_THROW(conf::ConfigCell(mockBlock, 1, -1), conf::LandscapeDefinitionException);
}

BOOST_AUTO_TEST_CASE(ConfigCell_AreaReturnsExpectedValue) {
   MOCK_EXPECT(mockBlock.xPixels).returns(1);
   MOCK_EXPECT(mockBlock.top).returns(1);
   MOCK_EXPECT(mockBlock.yPixelSize).returns(0.00025);

   conf::ConfigCell cell(mockBlock, 1, 1);
   BOOST_CHECK_EQUAL(cell.area(), 0.077438397195488895);
}

BOOST_AUTO_TEST_CASE(ConfigCell_IdReturnsExpectedValue) {
   MOCK_EXPECT(mockBlock.xPixels).returns(3);

   // ...
   // .*. <-- row 1, col 1 / 5th cell / 1-based index = 5
   conf::ConfigCell cell(mockBlock, 1, 1);
   BOOST_CHECK_EQUAL(cell.id(), 5);
}

BOOST_AUTO_TEST_CASE(ConfigCell_LatReturnsExpectedValue) {
   MOCK_EXPECT(mockBlock.xPixels).returns(1);
   MOCK_EXPECT(mockBlock.top).returns(1);
   MOCK_EXPECT(mockBlock.yPixelSize).returns(0.25);
   conf::ConfigCell cell(mockBlock, 0, 0);
   BOOST_CHECK_EQUAL(cell.lat(), 0.75);
}

BOOST_AUTO_TEST_CASE(ConfigCell_LonReturnsExpectedValue) {
   MOCK_EXPECT(mockBlock.xPixels).returns(1);
   MOCK_EXPECT(mockBlock.left).returns(5);
   MOCK_EXPECT(mockBlock.xPixelSize).returns(0.25);
   conf::ConfigCell cell(mockBlock, 0, 0);
   BOOST_CHECK_EQUAL(cell.lon(), 5.0);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace flint_configuration