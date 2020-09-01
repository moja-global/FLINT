#include "moja/flint/configuration/localdomain.h"

#include <boost/algorithm/string.hpp>
#include "moja/instrumentor.h"

namespace moja {
namespace flint {
namespace configuration {

LocalDomain::LocalDomain(LocalDomainType type, LocalDomainIterationType iterationType, bool doLogging, int numThreads,
                         const std::string& sequencerLibrary, const std::string& sequencer,
                         const std::string& simulateLandUnit, const std::string& landUnitBuildSuccess,
                         DynamicObject settings, TimeStepping timeStepping)
    : _type(type),
      _iterationType(iterationType),
      _doLogging(doLogging),
      _numThreads(numThreads),
      _sequencerLibrary(sequencerLibrary),
      _sequencer(sequencer),
      _simulateLandUnit(simulateLandUnit),
      _landUnitBuildSuccess(landUnitBuildSuccess),
      _timeStepping(timeStepping) {
   if (sequencer.length() == 0 || all(sequencer, boost::algorithm::is_space())) {
      throw std::invalid_argument("sequencer cannot be empty");
   }
   if (sequencerLibrary.length() == 0 || all(sequencerLibrary, boost::algorithm::is_space())) {
      throw std::invalid_argument("sequencerLibrary cannot be empty");
   }
   if (landUnitBuildSuccess.length() == 0 || all(landUnitBuildSuccess, boost::algorithm::is_space())) {
      throw std::invalid_argument("landUnitBuildSuccess cannot be empty");
   }
   if (simulateLandUnit.length() == 0 || all(simulateLandUnit, boost::algorithm::is_space())) {
      throw std::invalid_argument("simulateLandUnit cannot be empty");
   }
   _settings = settings;
}

void LocalDomain::setLandscapeObject(const std::string& providerName, LocalDomainIterationType iterationType) {
     MOJA_PROFILE_FUNCTION();

    _landscapeObject = std::make_shared<Landscape>(providerName, iterationType);
   _landscapeObject->setIteration(iterationType);
}

std::string LocalDomain::localDomainTypeToStr(LocalDomainType type) {
   if (type == LocalDomainType::SpatialTiled)
      // return "SpatialTiled";
      return "spatial_tiled";
   if (type == LocalDomainType::SpatiallyReferencedSQL)
      // return "SpatiallyReferencedSQL";
      return "spatially_referenced_sql";
   if (type == LocalDomainType::SpatiallyReferencedNoSQL)
      // return "SpatiallyReferencedNoSQL";
      return "spatially_referenced_nosql";
   if (type == LocalDomainType::ThreadedSpatiallyReferencedNoSQL)
      // return "ThreadedSpatiallyReferencedNoSQL";
      return "threaded_spatially_referenced_nosql";
   if (type == LocalDomainType::Point)
      // return "Point";
      return "point";

   return "Unknown";
}

}  // namespace configuration
}  // namespace flint
}  // namespace moja
