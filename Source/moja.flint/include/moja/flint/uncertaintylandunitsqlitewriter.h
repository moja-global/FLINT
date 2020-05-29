#pragma once
#include "moja/flint/modulebase.h"
#include "moja/flint/recordaccumulatorwithmutex.h"

#include <functional>
#include <string>

namespace Poco::Data {
class Session;
}  // namespace Poco::Data

namespace moja::flint {
class UncertaintySimulationUnitData;

class FLINT_API UncertaintyLandUnitSQLiteWriter : public ModuleBase {
  public:
   UncertaintyLandUnitSQLiteWriter(
       std::shared_ptr<RecordAccumulatorWithMutex2<Date2Row, Date2Record>> date_dimension,
       std::shared_ptr<RecordAccumulatorWithMutex2<PoolInfoRow, PoolInfoRecord>> pool_info_dimension,
       std::shared_ptr<RecordAccumulatorWithMutex2<ModuleInfoRow, ModuleInfoRecord>> module_info_dimension,
       std::shared_ptr<RecordAccumulatorWithMutex2<TileInfoRow, TileInfoRecord>> tile_info_dimension,
       std::shared_ptr<RecordAccumulatorWithMutex2<ClassifierSetRow, ClassifierSetRecord>> classifier_set_dimension);

   virtual ~UncertaintyLandUnitSQLiteWriter() = default;

   void configure(const DynamicObject& config) override;
   void subscribe(NotificationCenter& notificationCenter) override;

   ModuleTypes moduleType() override { return flint::ModuleTypes::Other; };

   void onSystemInit() override;
   void onSystemShutdown() override;
   void onLocalDomainInit() override;
   void onLocalDomainShutdown() override;
   void onLocalDomainProcessingUnitShutdown() override;

  private:
   void writeStock() const;
   void writeFlux() const;
   void writeErrorLog() const;
   void writeRunStats(const std::string& unit_label, DateTime& start_time, DateTime& finish_time, Int64 lu_count) const;
   void writeRunSummary(const std::string& unit_label, DateTime& start_time, DateTime& finish_time,
                        Int64 lu_count) const;

   template <typename TAccumulator>
   void load(Poco::Data::Session& session, const std::string& table, std::shared_ptr<TAccumulator> data_dimension);

   static void try_execute(Poco::Data::Session& session, std::function<void(Poco::Data::Session&)> fn);

   typedef Poco::Tuple<Int64, int, std::string, std::string, std::string> runStatDataRecord;

   // -- Shared Data
   
   std::shared_ptr<RecordAccumulatorWithMutex2<Date2Row, Date2Record>> date_dimension_;
   std::shared_ptr<RecordAccumulatorWithMutex2<PoolInfoRow, PoolInfoRecord>> pool_info_dimension_;
   std::shared_ptr<RecordAccumulatorWithMutex2<ModuleInfoRow, ModuleInfoRecord>> module_info_dimension_;
   std::shared_ptr<RecordAccumulatorWithMutex2<TileInfoRow, TileInfoRecord>> tile_info_dimension_;
   std::shared_ptr<RecordAccumulatorWithMutex2<ClassifierSetRow, ClassifierSetRecord>> classifier_set_dimension_;


   // -- flint data objects data
   std::shared_ptr<UncertaintySimulationUnitData> simulation_unit_data_;
   std::string db_name_sqlite_;
   std::string generated_db_name_sqlite_;
   bool do_run_stats_on_;
   bool do_stock_;
   bool log_errors_;
   bool block_index_on_;
   bool log_error_per_block_;
};



}  // namespace moja::flint
