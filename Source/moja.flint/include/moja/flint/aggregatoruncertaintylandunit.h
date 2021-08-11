#pragma once

#include "moja/flint/_flint_exports.h"
#include "moja/flint/modulebase.h"
#include "moja/flint/recordaccumulatorwithmutex.h"

#include <string>
#include <vector>

namespace moja::flint {
class UncertaintySimulationUnitData;
class SpatialLocationInfo;

class FLINT_API AggregatorUncertaintyLandUnitSharedData {
  public:
   AggregatorUncertaintyLandUnitSharedData() : module_info_on(true), do_run_stats_on(false) {}

   virtual ~AggregatorUncertaintyLandUnitSharedData() = default;
   AggregatorUncertaintyLandUnitSharedData(const AggregatorUncertaintyLandUnitSharedData& src) = delete;
   AggregatorUncertaintyLandUnitSharedData& operator=(const AggregatorUncertaintyLandUnitSharedData&) = delete;

   // Run flags for Modules
   bool module_info_on;
   bool do_run_stats_on;
   bool tile_index_on;
   bool block_index_on;
   bool cell_index_on;
   bool do_stock;
   bool output_month_12_only;
   bool clear_fluxes_after_recording;
};

typedef Poco::Tuple<Int64, int, std::string, std::string, std::string> runStatDataRecord;
class FLINT_API AggregatorUncertaintyLandUnit : public ModuleBase {
  public:
   AggregatorUncertaintyLandUnit(
       AggregatorUncertaintyLandUnitSharedData& aggregatorLandUnitSharedData,
       std::shared_ptr<RecordAccumulatorWithMutex2<Date2Row, Date2Record>> dateDimension,
       std::shared_ptr<RecordAccumulatorWithMutex2<PoolInfoRow, PoolInfoRecord>> poolInfoDimension,
       std::shared_ptr<RecordAccumulatorWithMutex2<ModuleInfoRow, ModuleInfoRecord>> moduleInfoDimension,
       std::shared_ptr<RecordAccumulatorWithMutex2<TileInfoRow, TileInfoRecord>> tile_info_dimension,
       std::shared_ptr<RecordAccumulatorWithMutex2<ClassifierSetRow, ClassifierSetRecord>> classifier_set_dimension,
       std::shared_ptr<std::vector<std::string>> classifier_names)
       : ModuleBase(),
         aggregator_land_unit_shared_data_(aggregatorLandUnitSharedData),
         date_dimension_(dateDimension),
         pool_info_dimension_(poolInfoDimension),
         module_info_dimension_(moduleInfoDimension),
         tile_info_dimension_(tile_info_dimension),
         classifier_set_dimension_(classifier_set_dimension),
         classifier_names_(classifier_names)
   {}

   virtual ~AggregatorUncertaintyLandUnit() = default;

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
   void recordClassifierNames(const DynamicObject& classifierSet);
   void recordStockSet();

private:
   void recordFluxSet();

   // -- Shared Data
   AggregatorUncertaintyLandUnitSharedData& aggregator_land_unit_shared_data_;
   std::shared_ptr<RecordAccumulatorWithMutex2<Date2Row, Date2Record>> date_dimension_;
   std::shared_ptr<RecordAccumulatorWithMutex2<PoolInfoRow, PoolInfoRecord>> pool_info_dimension_;
   std::shared_ptr<RecordAccumulatorWithMutex2<ModuleInfoRow, ModuleInfoRecord>> module_info_dimension_;
   std::shared_ptr<RecordAccumulatorWithMutex2<TileInfoRow, TileInfoRecord>> tile_info_dimension_;
   std::shared_ptr<RecordAccumulatorWithMutex2<ClassifierSetRow, ClassifierSetRecord>> classifier_set_dimension_;
   std::shared_ptr<std::vector<std::string>> classifier_names_;

   // -- flint data objects data
   std::shared_ptr<UncertaintySimulationUnitData> simulation_unit_data_;
   std::shared_ptr<const SpatialLocationInfo> spatial_location_info_;
   std::string classifier_set_var_name_;

   // Variable handles
   IVariable* classifier_set_var_;
   IVariable* build_worked_var_;

   // -- Land Unit level Collections
   // -- these collections will be kept for each land unit and put into greater aggregation on LU success.
   // -- handles the case when a LU fails during processing rather than in the build land unit phase
   struct fluxDataLU {
      Int64 date_record_id;
      Poco::Nullable<Int64> module_info_id;
      int src_idx;
      int dst_idx;
      double flux_value;
   };
   std::vector<fluxDataLU> fluxes_lu_;

   struct stockDataLU {
      Int64 date_record_id;
      int pool_idx;
      double value;
   };
   std::vector<stockDataLU> stocks_lu_;

   // -- Data
   //Poco::Nullable<Int64> date_record_id_;

   Poco::Nullable<UInt32> tile_idx_;
   Poco::Nullable<UInt32> block_idx_;
   Poco::Nullable<UInt32> cell_idx_;
   Poco::Nullable<double> tile_lat_;
   Poco::Nullable<double> tile_lon_;
   Poco::Nullable<double> block_lat_;
   Poco::Nullable<double> block_lon_;
   Poco::Nullable<double> cell_lat_;
   Poco::Nullable<double> cell_lon_;
   Poco::Nullable<UInt32> global_random_seed_;
   Poco::Nullable<UInt32> tile_random_seed_;
   Poco::Nullable<UInt32> block_random_seed_;
   Poco::Nullable<UInt32> cell_random_seed_;

   Int64 constructed_tile_id_;
};

}  // namespace moja::flint
