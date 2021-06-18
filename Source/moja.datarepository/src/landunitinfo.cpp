#include "moja/datarepository/landunitinfo.h"

#include "moja/datarepository/aspatialtileinfo.h"

#include <boost/format.hpp>


namespace moja {
namespace datarepository {

LandUnitInfo::LandUnitInfo(const AspatialTileInfo& tile, Int64 id, double area) {
   if (id < 1) {
      throw std::invalid_argument("Error id less that 1 " + std::to_string(id));
   }

   if (area <= 0.0) {
      throw std::invalid_argument("Error area less than or equal to 0.0 " + std::to_string(area));
   }

   _tile = &tile;
   _id = id;
   _area = area;
}

std::string LandUnitInfo::toString() const { return (boost::format("Tile: %1% Cell: %2%") % _tile->id() % _id).str(); }

}  // namespace datarepository
}  // namespace moja
