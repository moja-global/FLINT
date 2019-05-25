#include "moja/datarepository/datarepository.h"
#include "moja/datarepository/datarepositoryexceptions.h"
#include "moja/datarepository/providerspatialrastertiled.h"
#include "moja/datarepository/rasterreader.h"
#include "moja/datarepository/tileblockcellindexer.h"

#include <moja/dynamic.h>
#include <moja/types.h>

#include <Poco/File.h>
#include <Poco/FileStream.h>
#include <Poco/TemporaryFile.h>

#include <boost/format.hpp>
#include <boost/test/unit_test.hpp>

#include <iostream>

using moja::Int64;
using moja::Point;
using moja::Size;
using moja::datarepository::FileNotFoundException;
using moja::datarepository::ProviderSpatialRasterTiled;
using moja::datarepository::QueryException;

using Poco::FileOutputStream;
using Poco::TemporaryFile;

struct ProviderSpatialRasterTiledTestsFixture {
   const std::string test_tile_path = "./data/TestTile/TestTile_034_001.blk";
   const std::string test_stack_path = "./data/TestTile/TestStack_034_001.blk";

   moja::DynamicObject settings;

   ProviderSpatialRasterTiledTestsFixture() {
      _tiles = {{34.0, 1.0}};

      Poco::File TestTile(test_tile_path);
      Poco::File TestStack(test_stack_path);
      testTileExists = (TestTile.exists() && TestTile.isFile()) ? true : false;
      testStackExists = (TestStack.exists() && TestStack.isFile()) ? true : false;
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

   std::vector<Point> _tiles;
   bool testTileExists;
   bool testStackExists;
};

BOOST_FIXTURE_TEST_SUITE(ProviderSpatialRasterTiledTests, ProviderSpatialRasterTiledTestsFixture)

BOOST_AUTO_TEST_CASE(datarepository_BlockIteratorTest1) {
   auto blockCount = 0;
   for (auto tile : _tiles) {
      moja::datarepository::Rectangle areaOfInterest(Point{double(tile.lon), double(tile.lat)}, Size{1.0, 1.0});
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

BOOST_AUTO_TEST_CASE(datarepository_CellIteratorUsingBlockIteratorTest) {
   auto blockCount = 0;
   for (auto tile : _tiles) {
      moja::datarepository::Rectangle areaOfInterest(Point{double(tile.lon), double(tile.lat)}, Size{1.0, 1.0});
      for (const auto& block : _provider.blocks(areaOfInterest)) {
         if (testTileExists) {
            int cellCount = 0;
            for (const auto& cell : _provider.cells(block)) {
               // std::cout << "Block cell iteration:" << cell.toString() << std::endl;
               cellCount++;
            }
            BOOST_CHECK_EQUAL(cellCount, _provider.indexer().cellDesc.indexLimit);
         }
         blockCount++;

         if (testStackExists) {
         }
      }
   }
   BOOST_CHECK_EQUAL(blockCount, _provider.indexer().blockDesc.indexLimit);
}

BOOST_AUTO_TEST_CASE(datarepository_TileIteratorTest) {
   auto tileCount = 0;
   for (auto tile : _tiles) {
      moja::datarepository::Rectangle areaOfInterest(Point{double(tile.lon), double(tile.lat)}, Size{1.0, 1.0});
      for (const auto& tile : _provider.tiles(areaOfInterest)) {
         if (testTileExists) {
         }
         tileCount++;

         if (testStackExists) {
         }
      }
   }
   BOOST_CHECK_EQUAL(tileCount, 1);
}

BOOST_AUTO_TEST_CASE(datarepository_CheckCreatedDataFileThatHasIndexValuesWrittenToFile_Tile) {
   if (testTileExists) {
      moja::Stopwatch<> sw;
      sw.start();
      auto cellCount = 0;
      for (auto tile : _tiles) {
         const moja::datarepository::Rectangle area_of_interest(Point{double(tile.lon), double(tile.lat)},
                                                                Size{1.0, 1.0});
         for (const auto& cell : _provider.cells(area_of_interest)) {
            auto cellIdx =
                (cell.blockY() * 400 * 400 * 10) + (cell.blockX() * 400) + (cell.cellY() * 400 * 10) + cell.cellX();
            auto retVal = _provider.GetValue("testdata", cell);
            if (retVal.isEmpty()) {
               // Got a nodata value!
               BOOST_CHECK_EQUAL(1, 2);
            } else {
               int value = retVal;
               if (value != cellIdx)
                  std::cout << "TestTile: cell count (" << cellCount << "), value (" << value << "), calculated index ("
                            << cellIdx << ")\t\t" << cell.toString() << std::endl;
               BOOST_CHECK_EQUAL(value, cellIdx);
            }

            cellCount++;
         }
         sw.stop();
         std::cerr << "datarepository_CheckCreatedDataFileThatHasIndexValuesWrittenToFile_Tile: " << sw.elapsedSeconds()
                   << std::endl;
      }
   }
}

BOOST_AUTO_TEST_CASE(datarepository_CheckCreatedDataFileThatHasIndexValuesWrittenToFile_Stack) {
   if (testStackExists) {
      moja::Stopwatch<> sw;
      sw.start();
      auto cellCount = 0;
      for (auto tile : _tiles) {
         const moja::datarepository::Rectangle area_of_interest(Point{double(tile.lon), double(tile.lat)},
                                                                Size{1.0, 1.0});
         for (const auto& cell : _provider.cells(area_of_interest)) {
            auto retVal = _provider.GetValueStack("teststack", cell).extract<std::vector<moja::UInt8>>();
            cellCount++;
         }
         sw.stop();
         std::cout << "datarepository_CheckCreatedDataFileThatHasIndexValuesWrittenToFile_Stack: "
                   << sw.elapsedSeconds() << std::endl;
      }
   }
}

BOOST_AUTO_TEST_SUITE_END();
