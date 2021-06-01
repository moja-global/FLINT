#pragma once

#include "moja/flint/modulebase.h"

namespace moja::flint {

class TransactionManagerAfterSubmitModule : public ModuleBase {
  public:
   TransactionManagerAfterSubmitModule() : ModuleBase() {}
   virtual ~TransactionManagerAfterSubmitModule() = default;

   void configure(const DynamicObject& config) override;
   void subscribe(NotificationCenter& notificationCenter) override;

   void onTimingPostStep() override;
   void onPostNotification(short preMessageSignal) override;
};

}  // namespace moja::flint
