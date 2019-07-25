#ifndef MOJA_FLINT_MODULEBASE_H_
#define MOJA_FLINT_MODULEBASE_H_

#include "moja/flint/ilandunitdatawrapper.h"
#include "moja/flint/imodule.h"

namespace moja {
namespace flint {

class FLINT_API ModuleBase : public IModule {
  public:
   virtual ~ModuleBase();

   ModuleMetaData& metaData() override { return _metaData; }

   void setLandUnitController(ILandUnitController& landUnitController) override;

   void configure(const DynamicObject&) override{};
   void subscribe(NotificationCenter& notificationCenter) override{};

   /**
    * Called right after the module DLL has been loaded and the module object has been created
    * TODO: I'm not sure we can do a move here? This would be moving a pointer accross the ABI
    * and eventually trying to destruct it inside the possible DLL module
    */
   void StartupModule(const ModuleMetaData& metaData) override;

   /** Called before the module is unloaded, right before the module object is destroyed. */
   void ShutdownModule() override {}

   ModuleTypes moduleType() override { return ModuleTypes::Other; };

   void onSystemInit() override {}
   void onSystemShutdown() override {}
   void onLocalDomainInit() override {}
   void onLocalDomainShutdown() override {}
   void onLocalDomainProcessingUnitInit() override {}
   void onLocalDomainProcessingUnitShutdown() override {}
   void onPreTimingSequence() override {}
   void onTimingInit() override {}
   void onTimingPrePostInit() override {}
   void onTimingPostInit() override {}
   void onTimingPostInit2() override {}
   void onTimingShutdown() override {}
   void onTimingStep() override {}
   void onTimingPreEndStep() override {}
   void onTimingEndStep() override {}
   void onTimingPostStep() override {}
   void onOutputStep() override {}
   void onError(std::string msg) override {}
   void onDisturbanceEvent(DynamicVar) override {}
   void onPrePostDisturbanceEvent() override {}
   void onPostDisturbanceEvent() override {}
   void onPostDisturbanceEvent2() override {}
   void onPostNotification(short preMessageSignal) override {}

  protected:
   std::unique_ptr<ILandUnitDataWrapper> _landUnitData;

  private:
   ModuleMetaData _metaData;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_MODULEBASE_H_
