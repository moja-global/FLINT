#include "moja/flint/modulebase.h"

#include "moja/flint/landunitdatawrapper.h"

namespace moja {
namespace flint {

ModuleBase::~ModuleBase() {}

void ModuleBase::setLandUnitController(ILandUnitController& landUnitController) {
   if (_landUnitData != nullptr) {
      _landUnitData->setLandUnitController(&landUnitController);
   }
}

void ModuleBase::StartupModule(const ModuleMetaData& metaData) {
   _landUnitData = std::make_unique<LandUnitDataWrapper>(this);
   _metaData.copy(metaData);
}

}  // namespace flint
}  // namespace moja