#include "moja/datarepository/datarepositoryexceptions.h"
#include "moja/datarepository/providerspatialrastertiled.h"
#include "moja/datarepository/tileblockcellindexer.h"

#include <moja/dynamicstruct.h>

#include <boost/test/unit_test.hpp>

#include <iomanip>
#include <iostream>
#include <vector>

using moja::datarepository::FileNotFoundException;
using moja::datarepository::ProviderSpatialRasterTiled;
using moja::datarepository::QueryException;
using namespace moja::datarepository;

struct ProviderSpatialRasterTiledTestsFixture {
   moja::DynamicObject settings;

   ProviderSpatialRasterTiledTestsFixture() {}

   ~ProviderSpatialRasterTiledTestsFixture() {}
};

BOOST_FIXTURE_TEST_SUITE(TileBlockCellIteratorTests, ProviderSpatialRasterTiledTestsFixture);

BOOST_AUTO_TEST_CASE(DataRepository_Sanity_On_Creation) {}

BOOST_AUTO_TEST_CASE(DataRepository_New_Iterator) {
   ProviderSpatialRasterTiled provider(settings);

   TileBlockCellIndexer indexer = {1.0, 1.0, 0.1, 0.1, 0.00025, 0.00025};  // 1.0 Degree Tiles
   moja::datarepository::Rectangle areaOfInterest{Point{0.5, 34.5}, {1.0, 1.0}};
   std::vector<std::vector<int>> tileBlockCellCount;
   tileBlockCellCount.resize(indexer.tileDesc.indexLimit);
   for (auto& tile : tileBlockCellCount) {
      tile.resize(indexer.blockDesc.indexLimit);
      for (auto& blockCount : tile) {
         blockCount = 0;
      }
   }

   // std::cout << "Tile\tblock\tcell\t" << std::endl;
   for (auto& cell : provider.cells(areaOfInterest)) {
      auto& count = tileBlockCellCount[cell.tileIdx][cell.blockIdx];
      count++;
      ////if (cell.cellIdx % 160000 == 0)
      //	std::cout << cell.tileIdx << "\t" << cell.blockIdx << "\t" << cell.cellIdx << std::endl;
   }

   std::cout << "Tile\tblock\tcell count\t" << std::endl;
   int tileIdx = 0;
   for (auto& tile : tileBlockCellCount) {
      int blockIdx = 0;
      for (auto& blockCount : tile) {
         if (blockCount > 0) std::cout << tileIdx << "\t" << blockIdx << "\t" << blockCount << std::endl;
         blockIdx++;
      }
      tileIdx++;
   }
}

BOOST_AUTO_TEST_SUITE_END();
