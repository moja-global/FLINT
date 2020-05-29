#pragma once

#include "moja/flint/modulebase.h"

namespace moja::flint {
class SpatialLocationInfo;
class SimulationUnitDataBase;

class AggregatorError : public ModuleBase {
  public:
   AggregatorError() : ModuleBase() {}
   virtual ~AggregatorError() {}

   void configure(const DynamicObject& config) override;
   void subscribe(NotificationCenter& notificationCenter) override;
   ModuleTypes moduleType() override { return ModuleTypes::Aggregator; }

   void onLocalDomainInit() override;
   void onLocalDomainProcessingUnitInit() override;
   void onError(std::string msg) override;

  private:
   // -- flint data objects data
   std::shared_ptr<SimulationUnitDataBase> simulation_unit_data_;
   std::shared_ptr<SpatialLocationInfo> spatial_location_info_;
   bool log_errors_;
   int log_error_max_;
   bool log_error_per_block_;
};

}  // namespace moja::flint
