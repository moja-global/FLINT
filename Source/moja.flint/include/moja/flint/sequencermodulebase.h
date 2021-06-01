#pragma once

#include "moja/flint/_flint_exports.h"
#include "moja/flint/modulebase.h"

namespace moja::flint {

class ITiming;

class FLINT_API SequencerModuleBase : public ModuleBase {
  public:
   SequencerModuleBase() : ModuleBase(){};
   virtual ~SequencerModuleBase() {}

   virtual void configure(ITiming& timing);

   virtual bool Run(NotificationCenter& _notificationCenter, ILandUnitController& luc);
};

}  // namespace moja::flint
