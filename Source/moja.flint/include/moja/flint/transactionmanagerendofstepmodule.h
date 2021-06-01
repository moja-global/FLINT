#pragma once

#include "moja/flint/modulebase.h"

namespace moja::flint {

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

}  // namespace moja::flint
