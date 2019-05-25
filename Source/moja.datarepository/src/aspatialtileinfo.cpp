#include "moja/datarepository/aspatialtileinfo.h"

#include "moja/datarepository/aspatialtileinfocollection.h"
#include "moja/datarepository/datarepositoryexceptions.h"

using moja::datarepository::LandscapeDefinitionException;

namespace moja {
namespace datarepository {

AspatialTileInfo::AspatialTileInfo(const AspatialTileInfoCollection& tileInfoCollection, int id)
    : _tileInfoCollection(&tileInfoCollection) {
   if (id < 1) {
      BOOST_THROW_EXCEPTION(LandscapeDefinitionException() << Component("id") << Constraint("> 0"));
   }

   _id = id;
   _size = -1;
}

int AspatialTileInfo::size() const {
   if (_size == -1) {
      _size = _tileInfoCollection->luCount(_id);
   }

   return _size;
}

std::shared_ptr<LandUnitInfo> AspatialTileInfo::landUnitAtIndex(int index) const {
   Int64 luid = _id + (index * _tileInfoCollection->tileCount());

   if (_cachedBlock == nullptr) {
      _cachedBlock = _tileInfoCollection->fetchLUBlock(luid);
   }

   auto luData = _cachedBlock->find(luid);
   if (luData == _cachedBlock->end()) {
      _cachedBlock = _tileInfoCollection->fetchLUBlock(luid);
      luData = _cachedBlock->find(luid);
   }

   return std::make_shared<LandUnitInfo>(*this, luid, luData->second["area"]);
}

AspatialTileInfoIterator AspatialTileInfo::begin() { return AspatialTileInfoIterator(*this); }

AspatialTileInfoIterator AspatialTileInfo::end() { return AspatialTileInfoIterator(*this, false); }

}  // namespace datarepository
}  // namespace moja
