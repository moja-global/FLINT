#ifndef MOJA_FLINT_AGGREGATORSTOCKSTEP_H_
#define MOJA_FLINT_AGGREGATORSTOCKSTEP_H_

#include "moja/flint/modulebase.h"

#include <vector>

namespace moja {
namespace flint {

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

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_AGGREGATORSTOCKSTEP_H_