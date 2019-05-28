#ifndef MOJA_DATAREPOSITORY_ASPATIALTILEINFO_H_
#define MOJA_DATAREPOSITORY_ASPATIALTILEINFO_H_

#include "moja/datarepository/_datarepository_exports.h"
#include "moja/datarepository/landunitinfo.h"

#include <moja/dynamic.h>

#include <map>

namespace moja {
namespace datarepository {
class AspatialTileInfoCollection;
class AspatialTileInfoIterator;

class DATAREPOSITORY_API AspatialTileInfo {
  public:
   AspatialTileInfo(const AspatialTileInfoCollection& tileInfoCollection, int id);
   virtual ~AspatialTileInfo() = default;

   virtual int id() const { return _id; }
   virtual int size() const;

   virtual std::shared_ptr<LandUnitInfo> landUnitAtIndex(int index) const;

   AspatialTileInfoIterator begin();
   AspatialTileInfoIterator end();

  private:
   const AspatialTileInfoCollection* _tileInfoCollection;

   int _id;
   mutable int _size;
   mutable std::shared_ptr<std::map<Int64, DynamicVar>> _cachedBlock;
};

class DATAREPOSITORY_API AspatialTileInfoIterator : public std::iterator<std::forward_iterator_tag, LandUnitInfo> {
  public:
   AspatialTileInfoIterator() {}

   AspatialTileInfoIterator(AspatialTileInfo& tileInfo, bool start = true) : _tileInfo(&tileInfo) {
      if (start) {
         _currentIndex = 0;
      } else {
         _currentIndex = tileInfo.size();
      }
   }

   void operator++() {
      _currentIndex++;
      _currentLU = nullptr;
   }

   bool operator==(const AspatialTileInfoIterator& other) const {
      return _currentIndex == other._currentIndex && _tileInfo->id() == other._tileInfo->id();
   }

   bool operator!=(const AspatialTileInfoIterator& other) const { return !(*this == other); }

   LandUnitInfo& operator*() {
      if (_currentLU == nullptr) {
         _currentLU = _tileInfo->landUnitAtIndex(_currentIndex);
      }

      return *_currentLU;
   }

  private:
   AspatialTileInfo* _tileInfo;
   int _currentIndex;
   std::shared_ptr<LandUnitInfo> _currentLU;
};

}  // namespace datarepository
}  // namespace moja

#endif  // MOJA_DATAREPOSITORY_ASPATIALTILEINFO_H_
