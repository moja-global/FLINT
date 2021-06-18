#pragma once

#include <moja/notificationcenter.h>
#include <moja/status.h>

namespace moja::flint {
class IVariable;
class IModule;
class ILandUnitController;
namespace configuration {
class Configuration;
}
class SequencerModuleBase;

class ILocalDomainController {
  public:
   using ModuleMapKey = std::pair<std::string, std::string>;
   using ModuleMap = std::map<ModuleMapKey, std::shared_ptr<IModule>>;

   virtual ~ILocalDomainController() = default;

   // configure the simulation
   virtual status configure(const configuration::Configuration& config) = 0;

   // execute the simulation
   virtual status run() = 0;
   virtual status startup() = 0;
   virtual status shutdown() = 0;

   virtual int localDomainId() = 0;
   virtual void set_localDomainId(const int value) = 0;

   virtual ILandUnitController& landUnitController() = 0;

   virtual std::map<ModuleMapKey, IModule*> modules() const = 0;

   virtual NotificationCenter& notification_center() = 0;
   virtual const NotificationCenter& notification_center() const = 0;

};

}  // namespace moja::flint
