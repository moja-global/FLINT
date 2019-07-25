#include "moja/flint/configuration/landscape.h"

#include <boost/algorithm/string.hpp>

namespace moja {
namespace flint {
namespace configuration {

Landscape::Landscape(const std::string& providerName, LocalDomainIterationType iterationType)
    : _providerName(providerName), _iterationType(iterationType) {
   if (providerName.length() == 0 || all(providerName, boost::algorithm::is_space())) {
      throw std::invalid_argument("providerName cannot be empty");
   }
}

void Landscape::setIteration(LocalDomainIterationType iterationType) {
   // else if (iterationType == LocalDomainIterationType::NotAnIteration)
   //	setIterationXXXXX();
   if (iterationType == LocalDomainIterationType::ASpatialIndex) setIterationASpatialIndex();
   if (iterationType == LocalDomainIterationType::ASpatialMongoIndex)
      setIterationASpatialMongoIndex();
   else if (iterationType == LocalDomainIterationType::AreaOfInterest)
      setIterationAreaOfInterest();
   else if (iterationType == LocalDomainIterationType::LandscapeTiles)
      setIterationLandscapeTiles();
   else if (iterationType == LocalDomainIterationType::TileIndex)
      setIterationTileIndex();
   else if (iterationType == LocalDomainIterationType::BlockIndex)
      setIterationBlockIndex();
   else if (iterationType == LocalDomainIterationType::CellIndex)
      setIterationCellIndex();
}

void Landscape::setIterationAreaOfInterest() { _iterationAreaOfInterest = std::make_shared<IterationAreaOfInterest>(); }

void Landscape::setIterationASpatialIndex() { _iterationASpatialIndex = std::make_shared<IterationASpatialIndex>(); }

void Landscape::setIterationASpatialMongoIndex() {
   _iterationASpatialMongoIndex = std::make_shared<IterationASpatialMongoIndex>();
}

void Landscape::setIterationLandscapeTiles() { _iterationLandscapeTiles = std::make_shared<IterationLandscapeTiles>(); }

void Landscape::setIterationTileIndex() { _iterationTileIndex = std::make_shared<IterationTileIndex>(); }

void Landscape::setIterationBlockIndex() { _iterationBlockIndex = std::make_shared<IterationBlockIndex>(); }

void Landscape::setIterationCellIndex() { _iterationCellIndex = std::make_shared<IterationCellIndex>(); }

}  // namespace configuration
}  // namespace flint
}  // namespace moja
