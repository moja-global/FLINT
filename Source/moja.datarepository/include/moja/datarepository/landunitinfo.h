#ifndef MOJA_DATAREPOSITORY_LANDUNITINFO_H_
#define MOJA_DATAREPOSITORY_LANDUNITINFO_H_

#include "moja/datarepository/_datarepository_exports.h"

#include <string>

namespace moja {
namespace datarepository {

class AspatialTileInfo;

class DATAREPOSITORY_API LandUnitInfo final {
  public:
   LandUnitInfo(const AspatialTileInfo& tile, Int64 id, double area);

   inline Int64 id() const { return _id; }
   inline double area() const { return _area; }

   std::string toString() const;

  private:
   const AspatialTileInfo* _tile;

   Int64 _id;
   double _area;
};

}  // namespace datarepository
}  // namespace moja

#endif  // MOJA_DATAREPOSITORY_LANDUNITINFO_H_
