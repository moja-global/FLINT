#include "moja/datarepository/datarepositoryexceptions.h"
#include "moja/datarepository/providerspatialrastertiled.h"
#include "moja/datarepository/tileblockcellindexer.h"

#include <moja/dynamic.h>
#include <moja/logging.h>

#include <boost/test/unit_test.hpp>

#include <iostream>
#include <vector>

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

BOOST_AUTO_TEST_CASE(datarepository_Sanity_On_Creation) {
   // BOOST_CHECK_THROW(TileBlockCellIndexer indexer(1.3, 1.0, 0.1, 0.1, 0.00025, 0.00025),
   // TileBlockCellIndexerInvalidParameterException); BOOST_CHECK_THROW(TileBlockCellIndexer indexer(120, 360, 10, 10,
   // 400, 400), TileBlockCellIndexerInvalidParameterException);
}

BOOST_AUTO_TEST_CASE(datarepository_Test_HELPER) {
   TileBlockCellIndexer indexer = {1.0, 1.0, 0.1, 0.1, 0.00025, 0.00025};  // 1.0 Degree Tiles

   // moja::Point point = indexer.getLatLonFromIndex(cellidx);
   moja::Point point = {-25.97063, -56.614};
   auto index = indexer.getCellIndexFromLatLon(point);
   std::cout << index.tileIdx << ", " << index.blockIdx << ", " << index.cellIdx << std::endl;

   moja::Point point2 = {-56.614, -25.97063};
   auto index2 = indexer.getCellIndexFromLatLon(point2);
   std::cout << index2.tileIdx << ", " << index2.blockIdx << ", " << index2.cellIdx << std::endl;
}

BOOST_AUTO_TEST_CASE(datarepository_Test_1) {
   TileBlockCellIndexer indexer = {1.0, 1.0, 0.1, 0.1, 0.00025, 0.00025};  // 1.0 Degree Tiles

   for (auto cY = 0U; cY < 400U; cY++) {
      for (auto cX = 0U; cX < 400U; cX++) {
         CellIdx cellidx(XYIndex({1U, 0U}), XYIndex({0U, 0U}), XYIndex({cX, cY}), indexer);
         auto point = indexer.getLatLonFromIndex(cellidx);
         auto index = indexer.getCellIndexFromLatLon(point);
         // Check indexes
         BOOST_CHECK(index.cellX() == cX);
         BOOST_CHECK(index.cellY() == cY);
      }
   }
}

BOOST_AUTO_TEST_CASE(datarepository_Test_to_block_level) {
#if 0  // Takes 1 minutes to run! need to reduce test covereage somehow
	TileBlockCellIndexer indexer = { 1.0, 1.0, 0.1, 0.1, 0.00025, 0.00025 };	// 1.0 Degree Tiles

	for (auto t = 0U; t < indexer.tileDesc.indexLimit; t++) {

		TileIdx  tileIdx(t, indexer);
		auto pointT = indexer.getLatLonFromIndex(tileIdx);
		auto result_tileIdx = indexer.getTileIndexFromLatLon(pointT);

		// Check indexes
		BOOST_CHECK(result_tileIdx.tileX() == tileIdx.tileX());
		BOOST_CHECK(result_tileIdx.tileY() == tileIdx.tileY());

		for (auto b = 0U; b < indexer.blockDesc.indexLimit; b++) {

			BlockIdx blockIdx(t, b, indexer);
			auto pointTB = indexer.getLatLonFromIndex(blockIdx);
			auto result_blockIdx = indexer.getBlockIndexFromLatLon(pointTB);

			// Check indexes
			BOOST_CHECK(result_blockIdx.tileX() == blockIdx.tileX());
			BOOST_CHECK(result_blockIdx.tileY() == blockIdx.tileY());
			BOOST_CHECK(result_blockIdx.blockX() == blockIdx.blockX());
			BOOST_CHECK(result_blockIdx.blockY() == blockIdx.blockY());
		}
	}
#endif
}

#if 0
BOOST_AUTO_TEST_CASE(datarepository_Test_to_cell_level)
{
	TileBlockCellIndexer indexer = { 1.0, 1.0, 0.1, 0.1, 0.00025, 0.00025 };	// 1.0 Degree Tiles

	// This section would loop over the global dimension using the indexing system and do checks that 
	// conversions to and from lat/lons and indexes match
	// WARNING: Takes ages to run!
	for (auto t = 0U; t < indexer.tileDesc.indexLimit; t++) {

		TileIdx  tileIdx(t, indexer);
		auto pointT = indexer.getLatLonFromIndex(tileIdx);
		auto result_tileIdx = indexer.getTileIndexFromLatLon(pointT);

		// Check indexes
		BOOST_CHECK(result_tileIdx.tileX() == tileIdx.tileX());
		BOOST_CHECK(result_tileIdx.tileY() == tileIdx.tileY());

		for (auto b = 0U; b < indexer.blockDesc.indexLimit; b++) {

			BlockIdx blockIdx(t, b, indexer);
			auto pointTB = indexer.getLatLonFromIndex(blockIdx);
			auto result_blockIdx = indexer.getBlockIndexFromLatLon(pointTB);

			// Check indexes
			BOOST_CHECK(result_blockIdx.tileX() == blockIdx.tileX());
			BOOST_CHECK(result_blockIdx.tileY() == blockIdx.tileY());
			BOOST_CHECK(result_blockIdx.blockX() == blockIdx.blockX());
			BOOST_CHECK(result_blockIdx.blockY() == blockIdx.blockY());

			for (auto c = 0U; c < indexer.cellDesc.indexLimit; c++) {
				CellIdx  cellIdx(t, b, c, indexer);
				auto pointTBC = indexer.getLatLonFromIndex(cellIdx);
				auto result_cellIdx = indexer.getCellIndexFromLatLon(pointTBC);

				// Check indexes
				BOOST_CHECK(result_cellIdx.tileX() == cellIdx.tileX());
				BOOST_CHECK(result_cellIdx.tileY() == cellIdx.tileY());
				BOOST_CHECK(result_cellIdx.blockX() == cellIdx.blockX());
				BOOST_CHECK(result_cellIdx.blockY() == cellIdx.blockY());
				BOOST_CHECK(result_cellIdx.cellX() == cellIdx.cellX());
				BOOST_CHECK(result_cellIdx.cellY() == cellIdx.cellY());
			}
		}
	}
}
#endif

#if 0
BOOST_AUTO_TEST_CASE(datarepository_Test_to_cell_level_with_x_y)
{
	TileBlockCellIndexer indexer = { 1.0, 1.0, 0.1, 0.1, 0.00025, 0.00025 };	// 1.0 Degree Tiles

	// This section would loop over the global dimension using the indexing system and do checks that 
	// conversions to and from lat/lons and indexes match
	// WARNING: Takes ages to run!
	for (auto tY = 0U; tY < indexer.tileDesc.rows; tY++) {
		for (auto tX = 0U; tX < indexer.tileDesc.cols; tX++) {
			TileIdx  tileIdx(XYIndex{ tX, tY }, indexer);
			auto pointT = indexer.getLatLonFromIndex(tileIdx);
			auto result_tileIdx = indexer.getTileIndexFromLatLon(pointT);
			BOOST_CHECK(result_tileIdx.tileX() == tX);
			BOOST_CHECK(result_tileIdx.tileY() == tY);

			for (auto bY = 0U; bY < indexer.blockDesc.rows; bY++) {
				for (auto bX = 0U; bX < indexer.blockDesc.cols; bX++) {
					BlockIdx blockIdx(XYIndex{ tX, tY }, XYIndex{ bX, bY }, indexer);
					auto pointTB = indexer.getLatLonFromIndex(blockIdx);
					auto result_blockIdx = indexer.getBlockIndexFromLatLon(pointTB);
					BOOST_CHECK(result_blockIdx.tileX() == tX);
					BOOST_CHECK(result_blockIdx.tileY() == tY);
					BOOST_CHECK(result_blockIdx.blockX() == bX);
					BOOST_CHECK(result_blockIdx.blockY() == bY);

					for (auto cY = 0U; cY < indexer.cellDesc.rows; cY++) {
						for (auto cX = 0U; cX < indexer.cellDesc.cols; cX++) {
							// Get Lat & Lon
							CellIdx  cellIdx(XYIndex{ tX, tY }, XYIndex{ bX, bY }, XYIndex{ cX, cY }, indexer);
							auto pointTBC = indexer.getLatLonFromIndex(cellIdx);
							auto result_cellIdx = indexer.getCellIndexFromLatLon(pointTBC);
							BOOST_CHECK(result_cellIdx.tileX() == tX);
							BOOST_CHECK(result_cellIdx.tileY() == tY);
							BOOST_CHECK(result_cellIdx.blockX() == bX);
							BOOST_CHECK(result_cellIdx.blockY() == bY);
							BOOST_CHECK(result_cellIdx.cellX() == cX);
							BOOST_CHECK(result_cellIdx.cellY() == cY);
						}
					}
				}
			}
		}
	}
}
#endif

BOOST_AUTO_TEST_CASE(datarepository_Test_XXXXXX) {
   std::vector<TileBlockCellIndexer> indexer = {
       {1.0, 1.0, 0.1, 0.1, 0.00025, 0.00025},  // 1.0 Degree Tiles
       {180u, 360u, 10u, 10u, 400u, 400u},      // 1.0 Degree Tiles
       {0.9, 0.9, 0.1, 0.1, 0.00025, 0.00025},  // 0.9 Degree Tiles
       {200u, 400u, 9u, 9u, 400u, 400u},        // 0.9 Degree Tiles
       {360u, 360u, 5u, 10u, 400u, 400u},
   };

   // std::cout << "DataRepository_Test_Empty_TileInfoCollection" << std::endl;
   int count = 1;
   for (auto& d : indexer) {
      // std::cout << "indexer " << count++ << std::endl;
      // std::cout << d.toString() << std::endl;
   }
}

BOOST_AUTO_TEST_CASE(datarepository_Test_convertIndex_different_cell_sizes) {
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

BOOST_AUTO_TEST_CASE(datarepository_Test_convertIndex_different_block_sizes) {
#if 0  // Need to fix this test
       // This test will currently fail, as the convertIndex only handles different cell sizes with Tile&Blocks indexes
       // that are the same

	TileBlockCellIndexer indexerS = { 1.0, 1.0, 0.1, 0.1, 0.00025, 0.00025 };	// 1.0 Degree Tiles, .1 blocks & 0.00025 cells
	TileBlockCellIndexer indexerD = { 180.0, 360.0, 180.0, 360.0, 180.0, 360.0 };
	TileBlockCellIndexer indexerD2 = { 180.0, 360.0, 1.0, 1.0, 0.1, 0.1 };

	for (auto cY = 0U; cY < 400U; cY++) {
		for (auto cX = 0U; cX < 400U; cX++) {
			CellIdx cellidxS(XYIndex({ 40U, 40U }), XYIndex({ 1U, 1U }), XYIndex({ cX, cY }), indexerS);
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

			MOJA_LOG_INFO << "datarepository_Test_convertIndex_different_block_sizes: src : " << indexS.toString();
			MOJA_LOG_INFO << "datarepository_Test_convertIndex_different_block_sizes: dest: " << indexD.toString();

			// Check indexes
			auto xx = indexD.cellX();
			auto xy = indexD.cellY();

			BOOST_CHECK(indexD.cellX() == 0);
			BOOST_CHECK(indexD.cellY() == 0);
		}
	}
#endif
}

BOOST_AUTO_TEST_SUITE_END();
