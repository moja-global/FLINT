#ifndef MOJA_FLINT_ILOCALDOMAINCONTROLLER_H_
#define MOJA_FLINT_ILOCALDOMAINCONTROLLER_H_

#include "moja/notificationcenter.h"

namespace moja {
namespace flint {
class IVariable;
class IModule;
class ILandUnitController;
namespace configuration {
class Configuration;
}
class SequencerModuleBase;

class ILocalDomainController {
  public:
   typedef std::pair<std::string, std::string> ModuleMapKey;
   typedef std::map<ModuleMapKey, std::shared_ptr<IModule>> ModuleMap;

   virtual ~ILocalDomainController(void) = default;

   // configure the simulation
   virtual void configure(const configuration::Configuration& config) = 0;

   // execute the simulation
   virtual void run() = 0;
   virtual void startup() = 0;
   virtual void shutdown() = 0;

   virtual int localDomainId() = 0;
   virtual void set_localDomainId(const int value) = 0;

   virtual ILandUnitController& landUnitController() = 0;

   virtual std::map<ModuleMapKey, IModule*> modules() const = 0;

   // every simulation needs a notification center
   NotificationCenter _notificationCenter;

   // every simulation needs a sequencer
   std::shared_ptr<SequencerModuleBase> _sequencer;

   // Variable used to check if landUnit should be simulated
   const flint::IVariable* _simulateLandUnit;
   flint::IVariable* _landUnitBuildSuccess;
};

}  // namespace flint
}  // namespace moja
#endif  // MOJA_FLINT_ILOCALDOMAINCONTROLLER_H_
