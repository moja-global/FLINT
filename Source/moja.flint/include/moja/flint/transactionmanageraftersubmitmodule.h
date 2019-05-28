#ifndef MOJA_FLINT_TRANSACTIONMANAGERAFTERSUBMITMODULE_H_
#define MOJA_FLINT_TRANSACTIONMANAGERAFTERSUBMITMODULE_H_

#include "moja/flint/modulebase.h"

namespace moja {
namespace flint {

class TransactionManagerAfterSubmitModule : public ModuleBase {
  public:
   TransactionManagerAfterSubmitModule() : ModuleBase() {}
   virtual ~TransactionManagerAfterSubmitModule() = default;

   void configure(const DynamicObject& config) override;
   void subscribe(NotificationCenter& notificationCenter) override;

   void onTimingPostStep() override;
   void onPostNotification(short preMessageSignal) override;
};

}  // namespace flint
}  // namespace moja
#endif  // MOJA_FLINT_TRANSACTIONMANAGERAFTERSUBMITMODULE_H_