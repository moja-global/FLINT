#pragma once

#include "moja/flint/_flint_exports.h"
#include "moja/flint/modulebase.h"
#include "moja/flint/recordaccumulatorwithmutex.h"

#include <string>
#include <vector>

namespace moja::flint {

class UncertaintySimulationUnitData;

class FLINT_API AggregatorUncertainty : public ModuleBase {
  public:
   AggregatorUncertainty(std::shared_ptr<RecordAccumulatorWithMutex2<Date2Row, Date2Record>> date_dimension,
                         std::shared_ptr<RecordAccumulatorWithMutex2<PoolInfoRow, PoolInfoRecord>> pool_info_dimension,
       std::shared_ptr<RecordAccumulatorWithMutex2<ModuleInfoRow, ModuleInfoRecord>> module_info_dimension)
       : ModuleBase(),
         date_dimension_(date_dimension),
         pool_info_dimension_(pool_info_dimension),
         module_info_dimension_(module_info_dimension) {}

   virtual ~AggregatorUncertainty() = default;

   void configure(const DynamicObject& config) override;
   void subscribe(NotificationCenter& notificationCenter) override;

   ModuleTypes moduleType() override { return ModuleTypes::Aggregator; };

   void onSystemInit() override;
   void onSystemShutdown() override;
   void onLocalDomainInit() override;
   void onLocalDomainShutdown() override;
   void onLocalDomainProcessingUnitInit() override;
   void onLocalDomainProcessingUnitShutdown() override;
   void onTimingInit() override;
   void onTimingPostInit() override;
   void onTimingEndStep() override;
   void onTimingShutdown() override;
   void onOutputStep() override;
   void onError(std::string msg) override;
   void onPostDisturbanceEvent() override;

  private:
   void record_flux_set();
   void record_stock_set();

   // -- Dimensions
   std::shared_ptr<RecordAccumulatorWithMutex2<Date2Row, Date2Record>> date_dimension_;
   std::shared_ptr<RecordAccumulatorWithMutex2<PoolInfoRow, PoolInfoRecord>> pool_info_dimension_;
   std::shared_ptr<RecordAccumulatorWithMutex2<ModuleInfoRow, ModuleInfoRecord>> module_info_dimension_;

   std::shared_ptr<UncertaintySimulationUnitData> simulation_unit_data_;
   bool output_annual_;
   bool module_info_on_;
   bool aggregate_sink_and_source_;
   bool aggregate_stock_;

   // -- Land Unit level Collections
   // -- these collections will be kep for each land unit and put into greater aggregation on LU success.
   // -- handles the case when a LU fails during processing rather than in the build land unit phase
   struct flux_data_lu {
      Int64 date_record_id;
      Poco::Nullable<Int64> module_id;
      int src_ix;
      int dst_ix;
      double flux_value;
   };
   std::vector<flux_data_lu> fluxes_lu_;

   struct stock_data_lu {
      Int64 date_record_id;
      int pool_ix;
      double stock_value;
   };
   std::vector<stock_data_lu> stocks_lu_;
};

}  // namespace moja::flint
