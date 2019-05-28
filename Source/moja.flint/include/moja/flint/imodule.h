#ifndef MOJA_FLINT_IMODULE_H_
#define MOJA_FLINT_IMODULE_H_

#include "moja/flint/_flint_exports.h"

#include <moja/datetime.h>
#include <moja/dynamic.h>

#include <string>

namespace moja {
class NotificationCenter;
}

namespace moja {
namespace flint {
class ILandUnitController;

struct StepRecord {
   DateTime date;
   int step;
   double fracOfStep;
   double lengthOfStepInYears;
   std::vector<double> stockValues;
};

struct ModuleMetaData {
   void setDefaults() {
      libraryType = -1;
      libraryInfoId = -1;
      moduleType = -1;
      moduleId = 0;
      moduleName = "unknown";
   }

   void copy(const ModuleMetaData& src) {
      libraryType = src.libraryType;
      libraryInfoId = src.libraryInfoId;
      moduleType = src.moduleType;
      moduleId = src.moduleId;
      moduleName = src.moduleName;
   }

   int libraryType;
   int libraryInfoId;
   int moduleType;
   int moduleId;
   std::string moduleName;
};

enum class ModuleTypes : int {
   Model,             ///
   DisturbanceEvent,  ///
   Aggregator,        ///
   Proxy,             ///
   System,            ///
   Other              ///
};

/**
 * Interface class that all module implementations should derive from. This
 * is used to initialize a module after it's been loaded, and also to clean
 * it up before the module is unloaded.
 */
class FLINT_API IModule {
  public:
   /** Required because modules are deleted via a pointer to this interface. */
   virtual ~IModule() {}

   virtual ModuleMetaData& metaData() = 0;
   virtual void configure(const DynamicObject& config) = 0;
   virtual void subscribe(NotificationCenter& notificationCenter) = 0;
   virtual void setLandUnitController(ILandUnitController& landUnitController) = 0;

   /**
    * Called right after the module DLL has been loaded and the module object has been created.
    */
   virtual void StartupModule(const ModuleMetaData& metaData) = 0;

   /**
    * Called before the module is unloaded, right before the module object is destroyed.
    */
   virtual void ShutdownModule() = 0;

   virtual ModuleTypes moduleType() { return ModuleTypes::Other; };

   /**
    * Event messages.
    */
   virtual void onSystemInit() = 0;
   virtual void onSystemShutdown() = 0;
   virtual void onLocalDomainInit() = 0;
   virtual void onLocalDomainShutdown() = 0;
   virtual void onLocalDomainProcessingUnitInit() = 0;
   virtual void onLocalDomainProcessingUnitShutdown() = 0;
   virtual void onPreTimingSequence() = 0;
   virtual void onTimingInit() = 0;
   virtual void onTimingPrePostInit() = 0;
   virtual void onTimingPostInit() = 0;
   virtual void onTimingPostInit2() = 0;
   virtual void onTimingShutdown() = 0;
   virtual void onTimingStep() = 0;
   virtual void onTimingPreEndStep() = 0;
   virtual void onTimingEndStep() = 0;
   virtual void onTimingPostStep() = 0;
   virtual void onOutputStep() = 0;
   virtual void onError(std::string msg) = 0;
   virtual void onDisturbanceEvent(DynamicVar) = 0;
   virtual void onPrePostDisturbanceEvent() = 0;
   virtual void onPostDisturbanceEvent() = 0;
   virtual void onPostDisturbanceEvent2() = 0;
   virtual void onPostNotification(short preMessageSignal) = 0;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_IMODULE_H_
