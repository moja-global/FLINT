#include "moja/datarepository/datarepositoryexceptions.h"
#include "moja/datarepository/providerspatialrastertiled.h"
#include "moja/datarepository/rasterreader.h"
#include "moja/datarepository/tileblockcellindexer.h"

#include <moja/dynamic.h>

#include <boost/test/unit_test.hpp>

#include <vector>

struct DataRepositoryTestsFixture {
   moja::DynamicObject settings;

   DataRepositoryTestsFixture() {}

   ~DataRepositoryTestsFixture() {}
};

BOOST_FIXTURE_TEST_SUITE(DataRepositoryTests, DataRepositoryTestsFixture);

BOOST_AUTO_TEST_CASE(datarepository_Test_Tile_Iteration) {
   moja::datarepository::ProviderSpatialRasterTiled provider{
       std::make_shared<moja::datarepository::FlintRasterReaderFactory>(),
       moja::DynamicObject(
           {{"type", "raster_tiled"},
            {"tileLatSize", 1.0},
            {"tileLonSize", 1.0},
            {"blockLatSize", 0.1},
            {"blockLonSize", 0.1},
            {"cellLatSize", 0.00025},
            {"cellLonSize", 0.00025},
            {"layers", moja::DynamicVector({moja::DynamicObject({{"name", "county"},
                                                                 {"layer_type", "GridLayer"},
                                                                 {"layer_data", "UInt8"},
                                                                 {"layer_path", ".\\data\\SLEEK\\County"},
                                                                 {"layer_prefix", "counties_pop"},
                                                                 {"tileLatSize", 1.0},
                                                                 {"tileLonSize", 1.0},
                                                                 {"blockLatSize", 0.1},
                                                                 {"blockLonSize", 0.1},
                                                                 {"cellLatSize", 0.00025},
                                                                 {"cellLonSize", 0.00025},
                                                                 {"nodata", 0}}),
                                            moja::DynamicObject({{"name", "forests"},
                                                                 {"layer_type", "GridLayer"},
                                                                 {"layer_data", "UInt8"},
                                                                 {"layer_path", ".\\Data\\SLEEK\\Forests"},
                                                                 {"layer_prefix", "ke_forests"},
                                                                 {"tileLatSize", 1.0},
                                                                 {"tileLonSize", 1.0},
                                                                 {"blockLatSize", 0.1},
                                                                 {"blockLonSize", 0.1},
                                                                 {"cellLatSize", 0.00025},
                                                                 {"cellLonSize", 0.00025},
                                                                 {"nodata", 0}}),
                                            moja::DynamicObject({{"name", "plantations"},
                                                                 {"layer_type", "GridLayer"},
                                                                 {"layer_data", "UInt8"},
                                                                 {"layer_path", ".\\Data\\SLEEK\\Plantations"},
                                                                 {"layer_prefix", "ke_tree-plantations"},
                                                                 {"tileLatSize", 1.0},
                                                                 {"tileLonSize", 1.0},
                                                                 {"blockLatSize", 0.1},
                                                                 {"blockLonSize", 0.1},
                                                                 {"cellLatSize", 0.00025},
                                                                 {"cellLonSize", 0.00025},
                                                                 {"nodata", 0}})})}})};

   std::vector<moja::Point> tileSet = {
       {33.0, -3.0}, {34.0, -3.0}, {35.0, -3.0}, {36.0, -3.0}, {37.0, -3.0}, {38.0, -3.0}, {39.0, -3.0}, {40.0, -3.0},
       {41.0, -3.0}, {42.0, -3.0}, {33.0, -3.0}, {34.0, -3.0}, {35.0, -3.0}, {36.0, -3.0}, {37.0, -3.0}, {38.0, -3.0},
       {39.0, -3.0}, {40.0, -3.0}, {41.0, -3.0}, {42.0, -3.0}, {33.0, -4.0}, {34.0, -4.0}, {35.0, -4.0}, {36.0, -4.0},
       {37.0, -4.0}, {38.0, -4.0}, {39.0, -4.0}, {40.0, -4.0}, {41.0, -4.0}, {42.0, -4.0}, {33.0, -5.0}, {34.0, -5.0},
       {35.0, -5.0}, {36.0, -5.0}, {37.0, -5.0}, {38.0, -5.0}, {39.0, -5.0}, {40.0, -5.0}, {41.0, -5.0}, {42.0, -5.0},
       {33.0, -6.0}, {34.0, -6.0}, {35.0, -6.0}, {36.0, -6.0}, {37.0, -6.0}, {38.0, -6.0}, {39.0, -6.0}, {40.0, -6.0},
       {41.0, -6.0}, {42.0, -6.0}};

   // iterate every cell in each tile defined above @ size 1.0 for tile size
   for (auto& tile : tileSet) {
      moja::datarepository::Rectangle rect{tile, {1.0, 1.0}};
      // for (auto& cell : provider.cells(rect))
      //{

      //}
   }
}

BOOST_AUTO_TEST_CASE(datarepository_Test_TileInfoCollection) {
   // std::vector<TileSetData> tileSet = {
   //	{ 33.0, -3.0 }

   //	//{ 33.0, -3.0 }, { 34.0, -3.0 }

   //	//{ 33.0, -3.0 }, { 34.0, -3.0 }, { 35.0, -3.0 }

   //	//{ 33.0,  -3.0 }, { 34.0,  -3.0 }, { 35.0,  -3.0 }, { 36.0,  -3.0 }, { 37.0,  -3.0 }, { 38.0,  -3.0 }, { 39.0,
   //-3.0 }, { 40.0,  -3.0 }, { 41.0,  -3.0 }, { 42.0,  -3.0 }

   //	//{ 33.0,  -3.0 }, { 34.0,  -3.0 }, { 35.0,  -3.0 }, { 36.0,  -3.0 }, { 37.0,  -3.0 }, { 38.0,  -3.0 }, { 39.0,
   //-3.0 }, { 40.0,  -3.0 }, { 41.0,  -3.0 }, { 42.0,  -3.0 },
   //	//{ 33.0,  -4.0 }, { 34.0,  -4.0 }, { 35.0,  -4.0 }, { 36.0,  -4.0 }, { 37.0,  -4.0 }, { 38.0,  -4.0 }, { 39.0,
   //-4.0 }, { 40.0,  -4.0 }, { 41.0,  -4.0 }, { 42.0,  -4.0 },
   //	//{ 33.0,  -5.0 }, { 34.0,  -5.0 }, { 35.0,  -5.0 }, { 36.0,  -5.0 }, { 37.0,  -5.0 }, { 38.0,  -5.0 }, { 39.0,
   //-5.0 }, { 40.0,  -5.0 }, { 41.0,  -5.0 }, { 42.0,  -5.0 },
   //	//{ 33.0,  -6.0 }, { 34.0,  -6.0 }, { 35.0,  -6.0 }, { 36.0,  -6.0 }, { 37.0,  -6.0 }, { 38.0,  -6.0 }, { 39.0,
   //-6.0 }, { 40.0,  -6.0 }, { 41.0,  -6.0 }, { 42.0,  -6.0 }
   //};

   // std::cout << "Start -- DataRepository_Test_TileInfoCollection:" << std::endl << std::endl;

   // TileInfoCollection tileInfoCollection;
   // int id = 0;
   // for (auto tile : tileSet) {
   //	tileInfoCollection._tilesVec.emplace_back(TileInfo(tileInfoCollection, id++, tile.x, tile.y, 1.0, 1.0, 4000,
   // 4000, 400, 400));
   //}

   // LocalDomainControllerStats ldStats(tileInfoCollection);
   // moja::Int64 cellCount = 0;
   // for (const auto& cell : tileInfoCollection.cells()) {
   //	cellCount++;
   //	ldStats.setStatus(cell);
   //}
   // ldStats.endStats();
   // ldStats.outputStats();

   // std::cout << std::endl << "End -- DataRepository_Test_TileInfoCollection: cells (" << cellCount << ")" <<
   // std::endl;
}

BOOST_AUTO_TEST_SUITE_END();
