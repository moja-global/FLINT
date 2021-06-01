#pragma once

#include "moja/flint/_flint_exports.h"
#include "moja/flint/modulebase.h"
#include "moja/flint/recordaccumulatorwithmutex.h"

namespace moja::flint {

class AggregatorLandUnitSharedData;
class SimulationUnitDataBase;

/// This module is a drop in replacement for Aggregator2FluxSQLite && AggregatorSharedDataSQLite
/// Simply merging both into a single module
typedef Poco::Tuple<Int64, int, std::string, std::string, std::string> runStatDataRecord;

class FLINT_API AggregatorFileWriter : public flint::ModuleBase {
  public:
   AggregatorFileWriter(Poco::Mutex& fileMutex, AggregatorLandUnitSharedData& aggregatorLandUnitSharedData,
                        std::shared_ptr<flint::RecordAccumulatorWithMutex<DateRow>> dateDimension,
                        std::shared_ptr<flint::RecordAccumulatorWithMutex<Date2Row>> date2Dimension,
                        std::shared_ptr<flint::RecordAccumulatorWithMutex<PoolInfoRow>> poolInfoDimension,
                        std::shared_ptr<flint::RecordAccumulatorWithMutex<ModuleInfoRow>> moduleInfoDimension)
       : ModuleBase(),
         _fileMutex(fileMutex),
         _aggregatorLandUnitSharedData(aggregatorLandUnitSharedData),
         _dateDimension(dateDimension),
         _date2Dimension(date2Dimension),
         _poolInfoDimension(poolInfoDimension),
         _moduleInfoDimension(moduleInfoDimension) {}

   virtual ~AggregatorFileWriter() = default;

   void configure(const DynamicObject& config) override;
   void subscribe(NotificationCenter& notificationCenter) override;

   flint::ModuleTypes moduleType() override { return flint::ModuleTypes::Aggregator; };

   void onSystemInit() override;
   void onSystemShutdown() override;
   void onLocalDomainInit() override;
   void onLocalDomainShutdown() override;
   void onLocalDomainProcessingUnitShutdown() override;

  private:
   void writeFlux();

   // -- Shared Data
   Poco::Mutex& _fileMutex;
   AggregatorLandUnitSharedData& _aggregatorLandUnitSharedData;
   std::shared_ptr<flint::RecordAccumulatorWithMutex<DateRow>> _dateDimension;
   std::shared_ptr<flint::RecordAccumulatorWithMutex<Date2Row>> _date2Dimension;
   std::shared_ptr<flint::RecordAccumulatorWithMutex<PoolInfoRow>> _poolInfoDimension;
   std::shared_ptr<flint::RecordAccumulatorWithMutex<ModuleInfoRow>> _moduleInfoDimension;

   // -- flintdata objects data
   std::shared_ptr<SimulationUnitDataBase> _simulationUnitData;

   // Config
   std::string _fileName;
   bool _writeFileHeader;
   bool _writeToScreen;
};

}  // namespace flint
