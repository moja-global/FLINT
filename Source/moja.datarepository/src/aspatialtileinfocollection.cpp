#include "moja/datarepository/aspatialtileinfocollection.h"

#include "moja/datarepository/aspatialtileinfo.h"
#include "moja/datarepository/iproviderrelationalinterface.h"

#include <moja/dynamic.h>

#include <boost/format.hpp>

#include <cmath>

namespace moja {
namespace datarepository {

AspatialTileInfoCollection::AspatialTileInfoCollection(const IProviderRelationalInterface& provider, int maxTileSize,
                                                       int tileCacheSize) {
   if (maxTileSize < 1) {
      throw std::invalid_argument("Error maxTileSize less that 1 " + std::to_string(maxTileSize));
   }

   if (tileCacheSize < 1) {
      throw std::invalid_argument("Error tileCacheSize less that 1 " + std::to_string(tileCacheSize));
   }

   _provider = &provider;
   _maxTileSize = maxTileSize;
   _tileCacheSize = tileCacheSize;
}

int AspatialTileInfoCollection::tileCount() const {
   if (_tileCount == -1) {
      _tileCount = int(std::ceil(getTotalLUCount() / double(_maxTileSize)));
   }

   return _tileCount;
}

std::vector<AspatialTileInfo> AspatialTileInfoCollection::getTiles() const {
   std::vector<AspatialTileInfo> tiles;
   for (int i = 1; i <= tileCount(); i++) {
      tiles.push_back(AspatialTileInfo(*this, i));
   }

   return tiles;
}

int AspatialTileInfoCollection::luCount(int tileId) const {
   auto sql = (boost::format("SELECT COUNT(1) AS lu_count FROM inventory WHERE (id + (%1% - %2%)) %% %1% = 0") %
               tileCount() % tileId)
                  .str();

   auto result = _provider->GetDataSet(sql).extract<std::vector<DynamicObject>>();
   return result[0]["lu_count"];
}

std::shared_ptr<std::map<Int64, DynamicVar>> AspatialTileInfoCollection::fetchLUBlock(Int64 id) const {
   auto block = std::make_shared<std::map<Int64, DynamicVar>>();
   int tileId = id % tileCount();
   auto sql =
       (boost::format("SELECT id, area FROM inventory WHERE id >= %1% AND (id + (%2% - %3%)) %% %2% = 0 LIMIT %4%") %
        id % tileCount() % tileId % _tileCacheSize)
           .str();

   auto result = _provider->GetDataSet(sql).extract<std::vector<DynamicObject>>();
   for (const auto row : result) {
      (*block)[row["id"]] = row;
   }

   return block;
}

Int64 AspatialTileInfoCollection::getTotalLUCount() const {
   std::string sql = "SELECT COUNT(1) AS lu_count FROM inventory";
   auto result = _provider->GetDataSet(sql).extract<std::vector<DynamicObject>>();
   return result[0]["lu_count"];
}

}  // namespace datarepository
}  // namespace moja
