#include "moja/datarepository/landunitinfo.h"

#include "moja/datarepository/aspatialtileinfo.h"
#include "moja/datarepository/datarepositoryexceptions.h"

#include <boost/format.hpp>

using moja::datarepository::LandscapeDefinitionException;

namespace moja {
namespace datarepository {

LandUnitInfo::LandUnitInfo(const AspatialTileInfo& tile, Int64 id, double area) {
   if (id < 1) {
      BOOST_THROW_EXCEPTION(LandscapeDefinitionException() << Component("id") << Constraint("> 0"));
   }

   if (area <= 0.0) {
      BOOST_THROW_EXCEPTION(LandscapeDefinitionException() << Component("area") << Constraint("> 0"));
   }

   _tile = &tile;
   _id = id;
   _area = area;
}

std::string LandUnitInfo::toString() const { return (boost::format("Tile: %1% Cell: %2%") % _tile->id() % _id).str(); }

}  // namespace datarepository
}  // namespace moja
