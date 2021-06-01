#pragma once

#include "moja/flint/localdomaincontrollerbase.h"
#include "moja/flint/spinuplandunitcontroller.h"

namespace moja::datarepository {
class LandUnitInfo;
class AspatialTileInfo;
class AspatialTileInfoCollection;
}  // namespace moja::datarepository

namespace moja::flint {

class FLINT_API AspatialLocalDomainController final : public LocalDomainControllerBase {
  public:
   AspatialLocalDomainController() : _spinupLandUnitController(_landUnitController), _runSpinUp(false) {}
   ~AspatialLocalDomainController() = default;

   void configure(const configuration::Configuration& config) override;
   void run() override;
   void startup() override;
   void shutdown() override;

   void run(moja::datarepository::AspatialTileInfo& tile);

  private:
   std::unique_ptr<moja::datarepository::AspatialTileInfoCollection> _landscape;
   SpinupLandUnitController _spinupLandUnitController;
   NotificationCenter _spinupNotificationCenter;
   bool _runSpinUp;
   std::shared_ptr<SequencerModuleBase> _spinupSequencer;
   LocalDomainControllerBase::ModuleMap _spinupModules;
   IVariable* _luId;

   void run(const moja::datarepository::LandUnitInfo& lu);
};

}  // namespace moja::flint
