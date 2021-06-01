#include <moja/datarepository/datarepository.h>
#include <moja/datarepository/providerspatialrastertiled.h>
#include <moja/datarepository/rasterreader.h>

#include <moja/dynamic.h>
#include <moja/types.h>

#include <boost/test/unit_test.hpp>

#include <filesystem>

namespace flint_datarepository {

using moja::datarepository::ProviderSpatialRasterTiled;
namespace fs = std::filesystem;

struct ProviderSpatialRasterTiledTestsFixture {
   const std::string test_tile_path = "./data/TestTile/TestTile_034_001.blk";
   const std::string test_stack_path = "./data/TestTile/TestStack_034_001.blk";

   moja::DynamicObject settings;

   ProviderSpatialRasterTiledTestsFixture() {
      _tiles = {{34.0, 1.0}};

      const auto test_tile = fs::path(test_tile_path);
      const auto test_stack = fs::path(test_stack_path);
      testTileExists = (fs::exists(test_tile) && fs::is_regular_file(test_tile)) ? true : false;
      testStackExists = (fs::exists(test_stack) && fs::is_regular_file(test_stack)) ? true : false;
   }

   ~ProviderSpatialRasterTiledTestsFixture() {}

   ProviderSpatialRasterTiled _provider{
       std::make_shared<moja::datarepository::FlintRasterReaderFactory>(),
       moja::DynamicObject(
           {{"type", std::string("raster_tiled")},
            {"tileLatSize", 1.0},
            {"tileLonSize", 1.0},
            {"blockLatSize", 0.1},
            {"blockLonSize", 0.1},
            {"cellLatSize", 0.00025},
            {"cellLonSize", 0.00025},
            {"layers", moja::DynamicVector({moja::DynamicObject({{"name", std::string("testdata")},
                                                                 {"layer_type", std::string("GridLayer")},
                                                                 {"layer_data", std::string("Int32")},
                                                                 {"layer_path", std::string("./data/TestTile")},
                                                                 {"layer_prefix", std::string("TestTile")},
                                                                 {"tileLatSize", 1.0},
                                                                 {"tileLonSize", 1.0},
                                                                 {"blockLatSize", 0.1},
                                                                 {"blockLonSize", 0.1},
                                                                 {"cellLatSize", 0.00025},
                                                                 {"cellLonSize", 0.00025},
                                                                 {"nodata", -1}}),
                                            moja::DynamicObject({{"name", "teststack"},
                                                                 {"layer_type", "StackLayer"},
                                                                 {"layer_data", "UInt8"},
                                                                 {"layer_path", "./data/TestTile"},
                                                                 {"layer_prefix", "TestStack"},
                                                                 {"nLayers", 14},
                                                                 {"tileLatSize", 1.0},
                                                                 {"tileLonSize", 1.0},
                                                                 {"blockLatSize", 0.1},
                                                                 {"blockLonSize", 0.1},
                                                                 {"cellLatSize", 0.00025},
                                                                 {"cellLonSize", 0.00025},
                                                                 {"nodata", 0}})})}})};

   std::vector<moja::Point> _tiles;
   bool testTileExists;
   bool testStackExists;
};

BOOST_FIXTURE_TEST_SUITE(ProviderSpatialRasterTiledTests, ProviderSpatialRasterTiledTestsFixture)

BOOST_AUTO_TEST_CASE(ProviderSpatialRasterTiled_BlockIteratorTest1) {
   auto blockCount = 0;
   for (auto tile : _tiles) {
      moja::datarepository::Rectangle areaOfInterest(moja::Point{double(tile.lon), double(tile.lat)}, moja::Size{1.0, 1.0});
      for (const auto& block : _provider.blocks(areaOfInterest)) {
         if (testTileExists) {
         }
         blockCount++;

         if (testStackExists) {
         }
      }
   }
   BOOST_CHECK_EQUAL(blockCount, 100);
}

BOOST_AUTO_TEST_CASE(ProviderSpatialRasterTiled_CellIteratorUsingBlockIteratorTest) {
   auto blockCount = 0;
   for (auto tile : _tiles) {
      moja::datarepository::Rectangle areaOfInterest(moja::Point{double(tile.lon), double(tile.lat)}, moja::Size{1.0, 1.0});
      for (const auto& block : _provider.blocks(areaOfInterest)) {
         if (testTileExists) {
            int cellCount = 0;
            for (const auto& cell : _provider.cells(block)) {
               cellCount++;
            }
            BOOST_CHECK_EQUAL(cellCount, _provider.indexer().cellDesc.indexLimit);
         }
         blockCount++;
      }
   }
   BOOST_CHECK_EQUAL(blockCount, _provider.indexer().blockDesc.indexLimit);
}

#if !defined(_DEBUG)
BOOST_AUTO_TEST_CASE(ProviderSpatialRasterTiled_CheckCreatedDataFileThatHasIndexValuesWrittenToFile_Tile) {
   if (testTileExists) {
      auto cellCount = 0;
      for (auto tile : _tiles) {
         const moja::datarepository::Rectangle area_of_interest(moja::Point{double(tile.lon), double(tile.lat)},
                                                                moja::Size{1.0, 1.0});
         for (const auto& cell : _provider.cells(area_of_interest)) {
            auto cellIdx =
                (cell.blockY() * 400 * 400 * 10) + (cell.blockX() * 400) + (cell.cellY() * 400 * 10) + cell.cellX();
            auto retVal = _provider.GetValue("testdata", cell);
            if (retVal.isEmpty()) {
               // Got a nodata value!
               BOOST_CHECK_EQUAL(1, 2);
            } else {
               int value = retVal;
               BOOST_CHECK_EQUAL(value, cellIdx);
            }

            cellCount++;
         }
      }
   }
}
#endif

BOOST_AUTO_TEST_CASE(ProviderSpatialRasterTiled_CheckCreatedDataFileThatHasIndexValuesWrittenToFile_Stack) {
   if (testStackExists) {
      auto cellCount = 0;
      for (const auto& tile : _tiles) {
         const moja::datarepository::Rectangle area_of_interest(moja::Point{double(tile.lon), double(tile.lat)},
                                                                moja::Size{1.0, 1.0});
         for (const auto& cell : _provider.cells(area_of_interest)) {
            auto retVal = _provider.GetValueStack("teststack", cell).extract<std::vector<moja::UInt8>>();
            cellCount++;
         }
      }
      BOOST_CHECK_EQUAL(cellCount, 160000);
   }
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace flint_datarepository
