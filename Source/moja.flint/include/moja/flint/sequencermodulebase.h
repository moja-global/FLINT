#ifndef MOJA_FLINT_SEQUENCERMODULEBASE_H_
#define MOJA_FLINT_SEQUENCERMODULEBASE_H_

#include "moja/flint/_flint_exports.h"
#include "moja/flint/modulebase.h"

namespace moja {
namespace flint {

class ITiming;

class FLINT_API SequencerModuleBase : public ModuleBase {
  public:
   SequencerModuleBase() : ModuleBase(){};
   virtual ~SequencerModuleBase() {}

   virtual void configure(ITiming& timing);

   virtual bool Run(NotificationCenter& _notificationCenter, ILandUnitController& luc);
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_SEQUENCERMODULEBASE_H_
