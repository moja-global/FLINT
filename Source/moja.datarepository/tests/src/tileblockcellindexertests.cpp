#include <moja/datarepository/datarepositoryexceptions.h>
#include <moja/datarepository/providerspatialrastertiled.h>
#include <moja/datarepository/tileblockcellindexer.h>

#include <moja/dynamic.h>
#include <moja/logging.h>

#include <boost/test/unit_test.hpp>

namespace flint_datarepository {

using moja::XYIndex;
using moja::datarepository::CellIdx;
using moja::datarepository::FileNotFoundException;
using moja::datarepository::ProviderSpatialRasterTiled;
using moja::datarepository::QueryException;
using moja::datarepository::TileBlockCellIndexer;

struct TileBlockCellIndexerTestsFixture {
   moja::DynamicObject settings;

   TileBlockCellIndexerTestsFixture() {}

   ~TileBlockCellIndexerTestsFixture() {}
};

BOOST_FIXTURE_TEST_SUITE(TileBlockCellIndexerTests, TileBlockCellIndexerTestsFixture);

#if 0
BOOST_AUTO_TEST_CASE(TileBlockCellIndexer_SanityOnCreation) {
   BOOST_CHECK_THROW(TileBlockCellIndexer indexer(1.3, 1.0, 0.1, 0.1, 0.00025, 0.00025),
                    moja::datarepository::TileBlockCellIndexerInvalidParameterException);
   BOOST_CHECK_THROW(TileBlockCellIndexer indexer(120U, 360U, 10U, 10U, 400U, 400U),
                    moja::datarepository::TileBlockCellIndexerInvalidParameterException);
}
#endif

BOOST_AUTO_TEST_CASE(TileBlockCellIndexer_GetCellIndexFromLatLon) {
   const TileBlockCellIndexer indexer = {1.0, 1.0, 0.1, 0.1, 0.00025, 0.00025};  // 1.0 Degree Tiles

   const moja::Point point = {-25.97063, -56.614};
   const auto index = indexer.getCellIndexFromLatLon(point);
   BOOST_CHECK_EQUAL(index.tileIdx, 41523U);
   BOOST_CHECK_EQUAL(index.blockIdx, 93U);
   BOOST_CHECK_EQUAL(index.cellIdx, 113144U);

   const moja::Point point2 = {-56.614, -25.97063};
   const auto index2 = indexer.getCellIndexFromLatLon(point2);
   BOOST_CHECK_EQUAL(index2.tileIdx, 52714U);
   BOOST_CHECK_EQUAL(index2.blockIdx, 60U);
   BOOST_CHECK_EQUAL(index2.cellIdx, 22517U);
}

BOOST_AUTO_TEST_CASE(TileBlockCellIndexer_GetLatLonFromIndexRoundTrip) {
   TileBlockCellIndexer indexer = {1.0, 1.0, 0.1, 0.1, 0.00025, 0.00025};  // 1.0 Degree Tiles

   for (auto cY = 0U; cY < 400U; cY++) {
      for (auto cX = 0U; cX < 400U; cX++) {
         CellIdx cellidx(XYIndex({1U, 0U}), XYIndex({0U, 0U}), XYIndex({cX, cY}), indexer);
         auto point = indexer.getLatLonFromIndex(cellidx);
         auto index = indexer.getCellIndexFromLatLon(point);
         // Check indexes
         BOOST_CHECK_EQUAL(index.cellX(), cX);
         BOOST_CHECK_EQUAL(index.cellY(), cY);
      }
   }
}

#if !defined(_DEBUG)
BOOST_AUTO_TEST_CASE(TileBlockCellIndexer_TestToBlockLevel) {
   TileBlockCellIndexer indexer = {1.0, 1.0, 0.1, 0.1, 0.00025, 0.00025};  // 1.0 Degree Tiles

   for (auto t = 0U; t < indexer.tileDesc.indexLimit; t++) {
      moja::datarepository::TileIdx tileIdx(t, indexer);
      auto pointT = indexer.getLatLonFromIndex(tileIdx);
      auto result_tileIdx = indexer.getTileIndexFromLatLon(pointT);

      // Check indexes
      BOOST_CHECK_EQUAL(result_tileIdx.tileX(), tileIdx.tileX());
      BOOST_CHECK_EQUAL(result_tileIdx.tileY(), tileIdx.tileY());

      for (auto b = 0U; b < indexer.blockDesc.indexLimit; b++) {
         moja::datarepository::BlockIdx blockIdx(t, b, indexer);
         auto pointTB = indexer.getLatLonFromIndex(blockIdx);
         auto result_blockIdx = indexer.getBlockIndexFromLatLon(pointTB);

         // Check indexes
         BOOST_CHECK_EQUAL(result_blockIdx.tileX(), blockIdx.tileX());
         BOOST_CHECK_EQUAL(result_blockIdx.tileY(), blockIdx.tileY());
         BOOST_CHECK_EQUAL(result_blockIdx.blockX(), blockIdx.blockX());
         BOOST_CHECK_EQUAL(result_blockIdx.blockY(), blockIdx.blockY());
      }
   }
}
#endif

#if 0
BOOST_AUTO_TEST_CASE(TileBlockCellIndexer_TestToCellLevel) {
   TileBlockCellIndexer indexer = {1.0, 1.0, 0.1, 0.1, 0.00025, 0.00025};  // 1.0 Degree Tiles

   // This section would loop over the global dimension using the indexing system and do checks that
   // conversions to and from lat/lons and indexes match
   // WARNING: Takes ages to run!
   for (auto t = 0U; t < indexer.tileDesc.indexLimit; t++) {
      moja::datarepository::TileIdx tileIdx(t, indexer);
      auto pointT = indexer.getLatLonFromIndex(tileIdx);
      auto result_tileIdx = indexer.getTileIndexFromLatLon(pointT);

      // Check indexes
      BOOST_CHECK_EQUAL(result_tileIdx.tileX(), tileIdx.tileX());
      BOOST_CHECK_EQUAL(result_tileIdx.tileY(), tileIdx.tileY());

      for (auto b = 0U; b < indexer.blockDesc.indexLimit; b++) {
         moja::datarepository::BlockIdx blockIdx(t, b, indexer);
         auto pointTB = indexer.getLatLonFromIndex(blockIdx);
         auto result_blockIdx = indexer.getBlockIndexFromLatLon(pointTB);

         // Check indexes
         BOOST_CHECK_EQUAL(result_blockIdx.tileX(), blockIdx.tileX());
         BOOST_CHECK_EQUAL(result_blockIdx.tileY(), blockIdx.tileY());
         BOOST_CHECK_EQUAL(result_blockIdx.blockX(), blockIdx.blockX());
         BOOST_CHECK_EQUAL(result_blockIdx.blockY(), blockIdx.blockY());

         for (auto c = 0U; c < indexer.cellDesc.indexLimit; c++) {
            CellIdx cellIdx(t, b, c, indexer);
            auto pointTBC = indexer.getLatLonFromIndex(cellIdx);
            auto result_cellIdx = indexer.getCellIndexFromLatLon(pointTBC);

            // Check indexes
            BOOST_CHECK_EQUAL(result_cellIdx.tileX(), cellIdx.tileX());
            BOOST_CHECK_EQUAL(result_cellIdx.tileY(), cellIdx.tileY());
            BOOST_CHECK_EQUAL(result_cellIdx.blockX(), cellIdx.blockX());
            BOOST_CHECK_EQUAL(result_cellIdx.blockY(), cellIdx.blockY());
            BOOST_CHECK_EQUAL(result_cellIdx.cellX(), cellIdx.cellX());
            BOOST_CHECK_EQUAL(result_cellIdx.cellY(), cellIdx.cellY());
         }
      }
   }
}
#endif

#if 0
BOOST_AUTO_TEST_CASE(TileBlockCellIndexer_TestToCellLevelWithXY) {
   TileBlockCellIndexer indexer = {1.0, 1.0, 0.1, 0.1, 0.00025, 0.00025};  // 1.0 Degree Tiles

   // This section would loop over the global dimension using the indexing system and do checks that
   // conversions to and from lat/lons and indexes match
   // WARNING: Takes ages to run!
   for (auto tY = 0U; tY < indexer.tileDesc.rows; tY++) {
      for (auto tX = 0U; tX < indexer.tileDesc.cols; tX++) {
         moja::datarepository::TileIdx tileIdx(XYIndex{tX, tY}, indexer);
         auto pointT = indexer.getLatLonFromIndex(tileIdx);
         auto result_tileIdx = indexer.getTileIndexFromLatLon(pointT);
         BOOST_CHECK_EQUAL(result_tileIdx.tileX() , tX);
         BOOST_CHECK_EQUAL(result_tileIdx.tileY(), tY);

         for (auto bY = 0U; bY < indexer.blockDesc.rows; bY++) {
            for (auto bX = 0U; bX < indexer.blockDesc.cols; bX++) {
               moja::datarepository::BlockIdx blockIdx(XYIndex{tX, tY}, XYIndex{bX, bY}, indexer);
               auto pointTB = indexer.getLatLonFromIndex(blockIdx);
               auto result_blockIdx = indexer.getBlockIndexFromLatLon(pointTB);
               BOOST_CHECK_EQUAL(result_blockIdx.tileX(), tX);
               BOOST_CHECK_EQUAL(result_blockIdx.tileY(), tY);
               BOOST_CHECK_EQUAL(result_blockIdx.blockX(), bX);
               BOOST_CHECK_EQUAL(result_blockIdx.blockY(), bY);

               for (auto cY = 0U; cY < indexer.cellDesc.rows; cY++) {
                  for (auto cX = 0U; cX < indexer.cellDesc.cols; cX++) {
                     // Get Lat & Lon
                     CellIdx cellIdx(XYIndex{tX, tY}, XYIndex{bX, bY}, XYIndex{cX, cY}, indexer);
                     auto pointTBC = indexer.getLatLonFromIndex(cellIdx);
                     auto result_cellIdx = indexer.getCellIndexFromLatLon(pointTBC);
                     BOOST_CHECK_EQUAL(result_cellIdx.tileX(), tX);
                     BOOST_CHECK_EQUAL(result_cellIdx.tileY(), tY);
                     BOOST_CHECK_EQUAL(result_cellIdx.blockX(), bX);
                     BOOST_CHECK_EQUAL(result_cellIdx.blockY(), bY);
                     BOOST_CHECK_EQUAL(result_cellIdx.cellX(), cX);
                     BOOST_CHECK_EQUAL(result_cellIdx.cellY(), cY);
                  }
               }
            }
         }
      }
   }
}
#endif

BOOST_AUTO_TEST_CASE(TileBlockCellIndexer_TestConvertIndexDifferentCellSizes) {
   TileBlockCellIndexer indexerS = {1.0, 1.0,     0.1,
                                    0.1, 0.00025, 0.00025};  // 1.0 Degree Tiles, .1 blocks & 0.00025 cells
   TileBlockCellIndexer indexerD = {1.0, 1.0,    0.1,
                                    0.1, 0.0005, 0.0005};  // 1.0 Degree Tiles, .1 blocks and 0.0005 cells

   for (auto cY = 0U; cY < 400U; cY++) {
      for (auto cX = 0U; cX < 400U; cX++) {
         CellIdx cellidxS(XYIndex({1U, 0U}), XYIndex({0U, 0U}), XYIndex({cX, cY}), indexerS);
         auto pointS = indexerS.getLatLonFromIndex(cellidxS);
         auto indexS = indexerS.getCellIndexFromLatLon(pointS);

         // Check indexes
         BOOST_CHECK(indexS.cellX() == cX);
         BOOST_CHECK(indexS.cellY() == cY);

         CellIdx cellidxD = indexerD.convertIndex(cellidxS);
         auto pointD = indexerD.getLatLonFromIndex(cellidxD);
         auto indexD = indexerD.getCellIndexFromLatLon(pointD);

         // Check indexes
         auto xx = indexD.cellX();
         auto xy = indexD.cellY();

         BOOST_CHECK(indexD.cellX() == (cX / 2));
         BOOST_CHECK(indexD.cellY() == (cY / 2));
      }
   }
}
#if 0
BOOST_AUTO_TEST_CASE(TileBlockCellIndexer_TestConvertIndexDifferentBlockSizes) {
   // This test will currently fail, as the convertIndex only handles different cell sizes with Tile&Blocks indexes
   // that are the same

   TileBlockCellIndexer indexerS = {1.0, 1.0,     0.1,
                                    0.1, 0.00025, 0.00025};  // 1.0 Degree Tiles, .1 blocks & 0.00025 cells
   TileBlockCellIndexer indexerD = {180.0, 360.0, 180.0, 360.0, 180.0, 360.0};
   TileBlockCellIndexer indexerD2 = {180.0, 360.0, 1.0, 1.0, 0.1, 0.1};

   for (auto cY = 0U; cY < 400U; cY++) {
      for (auto cX = 0U; cX < 400U; cX++) {
         CellIdx cellidxS(XYIndex({40U, 40U}), XYIndex({1U, 1U}), XYIndex({cX, cY}), indexerS);
         auto pointS = indexerS.getLatLonFromIndex(cellidxS);
         auto indexS = indexerS.getCellIndexFromLatLon(pointS);

         // Check indexes
         BOOST_CHECK(indexS.cellX() == cX);
         BOOST_CHECK(indexS.cellY() == cY);

         CellIdx cellidxD = indexerD.convertIndex(cellidxS);
         moja::Point pointD = indexerD.getLatLonFromIndex(cellidxD);
         CellIdx indexD = indexerD.getCellIndexFromLatLon(pointS);

         BOOST_CHECK(indexD.tileIdx == cellidxD.tileIdx);
         BOOST_CHECK(indexD.blockIdx == cellidxD.blockIdx);
         BOOST_CHECK(indexD.cellIdx == cellidxD.cellIdx);

         // Check indexes
         auto xx = indexD.cellX();
         auto xy = indexD.cellY();

         BOOST_CHECK(indexD.cellX() == 0);
         BOOST_CHECK(indexD.cellY() == 0);
      }
   }
}
#endif

BOOST_AUTO_TEST_SUITE_END()

}  // namespace flint_datarepository
