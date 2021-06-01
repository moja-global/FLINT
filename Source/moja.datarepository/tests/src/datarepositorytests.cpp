#include <moja/datarepository/datarepositoryexceptions.h>
#include <moja/datarepository/providerspatialrastertiled.h>
#include <moja/datarepository/rasterreader.h>
#include <moja/datarepository/tileblockcellindexer.h>

#include <moja/dynamic.h>

#include <boost/test/unit_test.hpp>

#include <vector>

namespace flint_datarepository {

struct DataRepositoryTestsFixture {
   moja::DynamicObject settings;

   DataRepositoryTestsFixture() {}

   ~DataRepositoryTestsFixture() {}
};

BOOST_FIXTURE_TEST_SUITE(DataRepositoryTests, DataRepositoryTestsFixture);

BOOST_AUTO_TEST_CASE(ProviderSpatialRasterTiled_TileIteration) {
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
                                                                 {"layer_path", "./data/SLEEK/County"},
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
                                                                 {"layer_path", "./Data/SLEEK/Forests"},
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
                                                                 {"layer_path", "./Data/SLEEK/Plantations"},
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

   std::vector<uint32_t> tileIdxs = {20697, 20337, 19977, 19617, 19257, 18897, 18537, 18177, 17817, 17457,
                                     20697, 20337, 19977, 19617, 19257, 18897, 18537, 18177, 17817, 17457,
                                     20696, 20336, 19976, 19616, 19256, 18896, 18536, 18176, 17816, 17456,
                                     20695, 20335, 19975, 19615, 19255, 18895, 18535, 18175, 17815, 17455,
                                     20694, 20334, 19974, 19614, 19254, 18894, 18534, 18174, 17814, 17454};

   // iterate every cell in each tile defined above @ size 1.0 for tile size
   auto tile_idx = 0;
   for (auto& tile : tileSet) {
      moja::datarepository::Rectangle rect{tile, {1.0, 1.0}};
      for (auto& cell : provider.cells(rect)) {
         BOOST_CHECK_EQUAL(tileIdxs[tile_idx], cell.tileIdx);
         break;
      }
      tile_idx++;
   }
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace flint_datarepository
