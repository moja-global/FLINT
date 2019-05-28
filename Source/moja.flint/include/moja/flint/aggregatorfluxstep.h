#ifndef MOJA_FLINT_AGGREGATORFLUXSTEP_H_
#define MOJA_FLINT_AGGREGATORFLUXSTEP_H_

#include "moja/flint/modulebase.h"

namespace moja {
namespace flint {

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

  private:
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_AGGREGATORFLUXSTEP_H_