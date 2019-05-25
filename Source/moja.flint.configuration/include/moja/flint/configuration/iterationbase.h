#ifndef MOJA_FLINT_CONFIGURATION_ITERATIONBASE_H_
#define MOJA_FLINT_CONFIGURATION_ITERATIONBASE_H_

#include "moja/flint/configuration/_configuration_exports.h"

#include <string>

namespace moja {
namespace flint {
namespace configuration {

enum class LocalDomainIterationType {
   NotAnIteration,  // i.e Point sim
   ASpatialIndex,
   ASpatialMongoIndex,
   AreaOfInterest,
   LandscapeTiles,
   TileIndex,
   BlockIndex,
   CellIndex
};

static LocalDomainIterationType convertStrToLocalDomainIterationType(std::string iterationTypeStr) {
   if (iterationTypeStr == "LandscapeTiles")
      return LocalDomainIterationType::LandscapeTiles;
   else if (iterationTypeStr == "TileIndex")
      return LocalDomainIterationType::TileIndex;
   else if (iterationTypeStr == "BlockIndex")
      return LocalDomainIterationType::BlockIndex;
   else if (iterationTypeStr == "CellIndex")
      return LocalDomainIterationType::CellIndex;
   else if (iterationTypeStr == "AreaOfInterest")
      return LocalDomainIterationType::LandscapeTiles;
   else if (iterationTypeStr == "ASpatialIndex")
      return LocalDomainIterationType::ASpatialIndex;
   else if (iterationTypeStr == "ASpatialMongoIndex")
      return LocalDomainIterationType::ASpatialMongoIndex;
   return LocalDomainIterationType::NotAnIteration;
}

static std::string convertLocalDomainIterationTypeToStr(LocalDomainIterationType type) {
   switch (type) {
      case LocalDomainIterationType::NotAnIteration:
         return "NotAnIteration";
      case LocalDomainIterationType::ASpatialIndex:
         return "ASpatialIndex";
      case LocalDomainIterationType::ASpatialMongoIndex:
         return "ASpatialMongoIndex";
      case LocalDomainIterationType::AreaOfInterest:
         return "AreaOfInterest";
      case LocalDomainIterationType::LandscapeTiles:
         return "LandscapeTiles";
      case LocalDomainIterationType::TileIndex:
         return "TileIndex";
      case LocalDomainIterationType::BlockIndex:
         return "BlockIndex";
      case LocalDomainIterationType::CellIndex:
         return "CellIndex";
   }
   return "unknown";
}

class CONFIGURATION_API IterationBase {
  public:
   IterationBase(LocalDomainIterationType iterationType);
   virtual ~IterationBase() {}

   virtual LocalDomainIterationType iterationType() const { return _iterationType; }

  private:
   LocalDomainIterationType _iterationType;
};

}  // namespace configuration
}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_CONFIGURATION_ITERATIONBASE_H_
