#pragma once

#include "moja/flint/modulebase.h"

#include <vector>

namespace moja::flint {

class AggregatorStockStep : public ModuleBase {
  public:
   AggregatorStockStep() : ModuleBase() {}
   virtual ~AggregatorStockStep() {}

   void configure(const DynamicObject& config) override;
   void subscribe(NotificationCenter& notificationCenter) override;

   void onLocalDomainInit() override;
   void onTimingInit() override;
   void onOutputStep() override;

   const std::vector<StepRecord>* results() const { return &_stockByStep; }

  private:
   std::vector<StepRecord> _stockByStep;
};

}  // namespace moja::flint
