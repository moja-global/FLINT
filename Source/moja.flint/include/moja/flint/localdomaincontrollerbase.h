#ifndef MOJA_FLINT_LOCALDOMAINCONTROLLERBASE_H_
#define MOJA_FLINT_LOCALDOMAINCONTROLLERBASE_H_

#include "moja/flint/ilocaldomaincontroller.h"
#include "moja/flint/landunitcontroller.h"
#include "moja/flint/librarymanager.h"

#include <moja/datarepository/datarepository.h>

namespace moja {
namespace flint {

// namespace configuration {
// class Configuration;
//}

class FLINT_API LocalDomainControllerBase : public ILocalDomainController {
  public:
   LocalDomainControllerBase(void);
   explicit LocalDomainControllerBase(std::shared_ptr<FlintLibraryHandles> libraryHandles);
   virtual ~LocalDomainControllerBase(void) = default;

   status configure(const configuration::Configuration& config) override;
   status run() override;
   status startup() override;
   status shutdown() override;

   ILandUnitController& landUnitController() override { return _landUnitController; };
   virtual void initialiseData() { _landUnitController.initialiseData(false); }

   int localDomainId() override { return _localDomainId; }
   void set_localDomainId(const int value) override { _localDomainId = value; }

   std::map<ModuleMapKey, IModule*> modules() const override;
   NotificationCenter& notification_center() override { return _notificationCenter; }
   const NotificationCenter& notification_center() const override { return _notificationCenter; }

  protected:
   NotificationCenter _notificationCenter;
   std::shared_ptr<SequencerModuleBase> _sequencer;
   const IVariable* _simulateLandUnit;
   IVariable* _landUnitBuildSuccess;
   int _localDomainId;
   const configuration::Configuration* _config;
   LandUnitController _landUnitController;
   datarepository::DataRepository _dataRepository;
   LibraryManager _libraryManager;
   ModuleMap _moduleMap;
};
}  // namespace flint
}  // namespace moja
#endif  // MOJA_FLINT_LOCALDOMAINCONTROLLER_H_
