#ifndef MOJA_FLINT_AGGREGATORLANDUNIT_H_
#define MOJA_FLINT_AGGREGATORLANDUNIT_H_

#include "moja/flint/_flint_exports.h"
#include "moja/flint/modulebase.h"
#include "moja/flint/recordaccumulatorwithmutex.h"

#include <string>
#include <vector>

namespace moja {
namespace flint {
class SimulationUnitDataBase;

// --------------------------------------------------------------------------------------------
class FLINT_API AggregatorLandUnitSharedData {
  public:
   AggregatorLandUnitSharedData()
       : _moduleInfoOn(true), _doRunStatsOn(false), _outputMonth12Only(true), _aggregateSinkAndSource(true) {}

   virtual ~AggregatorLandUnitSharedData() = default;
   AggregatorLandUnitSharedData(const AggregatorLandUnitSharedData& src) = delete;
   AggregatorLandUnitSharedData& operator=(const AggregatorLandUnitSharedData&) = delete;

   // Run flags for Modules
   bool _moduleInfoOn;
   bool _doRunStatsOn;
   bool _outputMonth12Only;
   bool _aggregateSinkAndSource;
};

// --------------------------------------------------------------------------------------------

/// This module is a drop in replacement for Aggregator2FluxSQLite && AggregatorSharedDataSQLite
/// Simply merging both into a single module
typedef Poco::Tuple<Int64, int, std::string, std::string, std::string> runStatDataRecord;

class FLINT_API AggregatorLandUnit : public flint::ModuleBase {
  public:
   AggregatorLandUnit(AggregatorLandUnitSharedData& aggregatorLandUnitSharedData,
                      std::shared_ptr<RecordAccumulatorWithMutex<DateRow>> dateDimension,
                      std::shared_ptr<RecordAccumulatorWithMutex<Date2Row>> date2Dimension,
                      std::shared_ptr<RecordAccumulatorWithMutex<PoolInfoRow>> poolInfoDimension,
                      std::shared_ptr<RecordAccumulatorWithMutex<ModuleInfoRow>> moduleInfoDimension)
       : ModuleBase(),
         _aggregatorLandUnitSharedData(aggregatorLandUnitSharedData),
         _dateDimension(dateDimension),
         _date2Dimension(date2Dimension),
         _poolInfoDimension(poolInfoDimension),
         _moduleInfoDimension(moduleInfoDimension) {}

   virtual ~AggregatorLandUnit() = default;

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
   void onError(std::string msg) override;
   void onPostDisturbanceEvent() override;

  private:
   void recordFluxSet();

   // -- Shared Data
   AggregatorLandUnitSharedData& _aggregatorLandUnitSharedData;
   std::shared_ptr<RecordAccumulatorWithMutex<DateRow>> _dateDimension;
   std::shared_ptr<RecordAccumulatorWithMutex<Date2Row>> _date2Dimension;
   std::shared_ptr<RecordAccumulatorWithMutex<PoolInfoRow>> _poolInfoDimension;
   std::shared_ptr<RecordAccumulatorWithMutex<ModuleInfoRow>> _moduleInfoDimension;

   // -- flintdata objects data
   std::shared_ptr<SimulationUnitDataBase> _simulationUnitData;

   // -- Land Unit level Collections
   // -- these collections will be kep for each land unit and put into greater aggregation on LU success.
   // -- handles the case when a LU fails during processing rather than in the build land unit phase
   struct fluxDataLU {
      Int64 _dateRecordId;
      Poco::Nullable<Int64> _moduleId;
      int _srcIx;
      int _dstIx;
      double _fluxValue;
   };
   std::vector<fluxDataLU> _fluxesLU;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_AGGREGATORLANDUNIT_H_