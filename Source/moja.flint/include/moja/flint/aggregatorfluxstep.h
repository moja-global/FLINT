#pragma once

#include "moja/flint/modulebase.h"

namespace moja::flint {

class AggregatorFluxStep : public ModuleBase {
  public:
   AggregatorFluxStep() : ModuleBase() {}
   virtual ~AggregatorFluxStep() {}

   void configure(const DynamicObject& config) override;
   void subscribe(NotificationCenter& notificationCenter) override;

   void onLocalDomainInit() override;
   void onLocalDomainShutdown() override;
   void onTimingEndStep() override;

   void setOutFile(std::string filename) {}
};

}  // namespace moja::flint
