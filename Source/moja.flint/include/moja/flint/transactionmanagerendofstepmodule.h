#ifndef MOJA_FLINT_TRANSACTIONMANAGERENDOFSTEPMODULE_H_
#define MOJA_FLINT_TRANSACTIONMANAGERENDOFSTEPMODULE_H_

#include "moja/flint/modulebase.h"

namespace moja {
namespace flint {

class TransactionManagerEndOfStepModule : public ModuleBase {
  public:
   TransactionManagerEndOfStepModule() : ModuleBase() {}
   virtual ~TransactionManagerEndOfStepModule() = default;

   void configure(const DynamicObject& config) override;
   void subscribe(NotificationCenter& notificationCenter) override;

   void onTimingPrePostInit() override;
   void onTimingPostInit2() override;
   void onTimingPreEndStep() override;
   void onTimingPostStep() override;
   void onPrePostDisturbanceEvent() override;
   void onPostDisturbanceEvent2() override;
   void onError(std::string msg) override;
};

}  // namespace flint
}  // namespace moja
#endif  // MOJA_FLINT_TRANSACTIONMANAGERENDOFSTEPMODULE_H_