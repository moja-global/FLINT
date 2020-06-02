#include <moja/flint/libraryfactory.h>

// For Internal Library Factory

#include <moja/logging.h>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

// Modules
#include "moja/flint/aggregatorfilewriter.h"
#include "moja/flint/aggregatorfluxstep.h"
#include "moja/flint/aggregatorlandunit.h"
#include "moja/flint/aggregatorstockstep.h"
#include "moja/flint/aggregatoruncertainty.h"
#include "moja/flint/calendarandeventflintdatasequencer.h"
#include "moja/flint/calendarandeventsequencer.h"
#include "moja/flint/calendarsequencer.h"
#include "moja/flint/debugnotificationmodule.h"
#include "moja/flint/errorscreenwriter.h"
#include "moja/flint/outputerstream.h"
#include "moja/flint/outputerstreamflux.h"
#include "moja/flint/proxymodule.h"
#include "moja/flint/testmodule.h"
#include "moja/flint/transactionmanageraftersubmitmodule.h"
#include "moja/flint/transactionmanagerendofstepmodule.h"
#include "moja/flint/uncertaintyfilewriter.h"
#include "moja/flint/writesystemconfig.h"
#include "moja/flint/writevariablegrid.h"

// Transforms
#include "moja/flint/compositetransform.h"
#include "moja/flint/locationfromflintdatatransform.h"
#include "moja/flint/locationidxfromflintdatatransform.h"
#include "moja/flint/locationidxtransform.h"
#include "moja/flint/locationtransform.h"
#include "moja/flint/lookuprandomtransform.h"
#include "moja/flint/lookuptransform.h"
#include "moja/flint/sqlquerytransform.h"
#include "moja/flint/sumpoolstransform.h"

// FlintData
#include "moja/flint/eventqueue.h"
#include "moja/flint/idnamecollection.h"
#include "moja/flint/idnamedesccollection.h"
#include "moja/flint/localdomaininformation.h"
#include "moja/flint/operationextradata.h"
#include "moja/flint/simulationunitdatabase.h"
#include "moja/flint/spatiallocationinfo.h"
#include "moja/flint/systeminformation.h"
#include "moja/flint/uncertaintysimulationdata.h"

#include <moja/datarepository/providerrelationalsqlite.h>
#include <moja/datarepository/providerspatialrastertiled.h>
#include <moja/datarepository/rasterreader.h>

#include "moja/flint/aggregatorerror.h"
#include "moja/flint/aggregatoruncertaintylandunit.h"
#include "moja/flint/uncertaintylandunitsqlitewriter.h"


namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------
// This data will be shared across threads

struct FLINTAggregationSharedDataObject {
   FLINTAggregationSharedDataObject() {
      dateDimension = std::make_shared<flint::RecordAccumulatorWithMutex<DateRow>>();
      date2Dimension = std::make_shared<flint::RecordAccumulatorWithMutex<Date2Row>>();
      poolInfoDimension = std::make_shared<flint::RecordAccumulatorWithMutex<PoolInfoRow>>();
      moduleInfoDimension = std::make_shared<flint::RecordAccumulatorWithMutex<ModuleInfoRow>>();
   }

   // Shared Data
   AggregatorLandUnitSharedData aggregatorLandUnitSharedData;

   // Shared Collections
   std::shared_ptr<flint::RecordAccumulatorWithMutex<DateRow>> dateDimension;
   std::shared_ptr<flint::RecordAccumulatorWithMutex<Date2Row>> date2Dimension;
   std::shared_ptr<flint::RecordAccumulatorWithMutex<PoolInfoRow>> poolInfoDimension;
   std::shared_ptr<flint::RecordAccumulatorWithMutex<ModuleInfoRow>> moduleInfoDimension;
};

struct FLINTUncertaintySharedDataObject {
   FLINTUncertaintySharedDataObject() {
      date_dimension = std::make_shared<RecordAccumulatorWithMutex2<Date2Row, Date2Record>>();
      pool_info_dimension = std::make_shared<RecordAccumulatorWithMutex2<PoolInfoRow, PoolInfoRecord>>();
      module_info_dimension = std::make_shared<RecordAccumulatorWithMutex2<ModuleInfoRow, ModuleInfoRecord>>();
      tile_info_dimension = std::make_shared<RecordAccumulatorWithMutex2<TileInfoRow, TileInfoRecord>>();
      classifier_set_dimension = std::make_shared<RecordAccumulatorWithMutex2<ClassifierSetRow, ClassifierSetRecord>>();
      classifier_names = std::make_shared<std::vector<std::string>>();
   }

   // Shared Data
   AggregatorUncertaintyLandUnitSharedData aggregator_land_unit_shared_data;

   // Shared Collections
   std::shared_ptr<RecordAccumulatorWithMutex2<Date2Row, Date2Record>> date_dimension;
   std::shared_ptr<RecordAccumulatorWithMutex2<PoolInfoRow, PoolInfoRecord>> pool_info_dimension;
   std::shared_ptr<RecordAccumulatorWithMutex2<ModuleInfoRow, ModuleInfoRecord>> module_info_dimension;
   std::shared_ptr<RecordAccumulatorWithMutex2<TileInfoRow, TileInfoRecord>> tile_info_dimension;
   std::shared_ptr<RecordAccumulatorWithMutex2<ClassifierSetRow, ClassifierSetRecord>> classifier_set_dimension;
   std::shared_ptr<std::vector<std::string>> classifier_names;
};

// --------------------------------------------------------------------------------------------
// Shared data instances

FLINTAggregationSharedDataObject flint_aggregation_shared_data;
FLINTUncertaintySharedDataObject flint_uncertainty_shared_data;

Poco::Mutex _fileHandlingMutexVarGridWriter;
Poco::Mutex _fileHandlingMutexConfigWriter;
Poco::Mutex _fileHandlingMutexAggregationFileWriter;
Poco::Mutex _fileHandlingMutexUncertaintyFileWriter;

// Flint Data Factory
std::shared_ptr<IFlintData> createEventQueueFactory(const std::string& eventTypeStr, int id, const std::string& name,
                                                    const DynamicObject& other) {
   std::shared_ptr<EventBase> newEvent = nullptr;
   if (eventTypeStr == "internal.flint.DebugModuleWithAnythingInIt")
      newEvent = std::make_shared<DebugModuleWithAnythingInItEvent>(id, name);
   return newEvent;
}

//
// NOTE: From Max Fellows: Max Fellows [4/10/2017 05:24]
// found a way to shorten the libraryfactory a bit while looking into the python stuff - you can pass lambdas instead of
// declaring all those create module methods:
//
// outModuleRegistrations[index++] = flint::ModuleRegistration{ "CBMDecayModule", []() -> flint::IModule* { return new
// cbm::CBMDecayModule(); } };
//

extern "C" {

int getFlintModuleRegistrations(moja::flint::ModuleRegistration* outModuleRegistrations) {
   MOJA_LOG_DEBUG << (boost::format("getFlintModuleRegistrations: %s") % "entered").str();

   auto index = 0;
   outModuleRegistrations[index++] = ModuleRegistration{
       "AggregatorLandUnit", []() -> flint::IModule* {
          return new AggregatorLandUnit(
              flint_aggregation_shared_data.aggregatorLandUnitSharedData, flint_aggregation_shared_data.dateDimension,
              flint_aggregation_shared_data.date2Dimension, flint_aggregation_shared_data.poolInfoDimension,
              flint_aggregation_shared_data.moduleInfoDimension);
       }};

    outModuleRegistrations[index++] = ModuleRegistration{
       "AggregatorUncertaintyLandUnit", []() -> flint::IModule* {
          return new AggregatorUncertaintyLandUnit(
              flint_uncertainty_shared_data.aggregator_land_unit_shared_data,
              flint_uncertainty_shared_data.date_dimension, flint_uncertainty_shared_data.pool_info_dimension,
              flint_uncertainty_shared_data.module_info_dimension, flint_uncertainty_shared_data.tile_info_dimension,
              flint_uncertainty_shared_data.classifier_set_dimension,
              flint_uncertainty_shared_data.classifier_names);
       }};

   outModuleRegistrations[index++] = ModuleRegistration{
        "UncertaintyLandUnitSQLiteWriter", []() -> flint::IModule* {
           return new UncertaintyLandUnitSQLiteWriter(
               flint_uncertainty_shared_data.date_dimension, flint_uncertainty_shared_data.pool_info_dimension,
               flint_uncertainty_shared_data.module_info_dimension, flint_uncertainty_shared_data.tile_info_dimension,
               flint_uncertainty_shared_data.classifier_set_dimension);
        }};

   outModuleRegistrations[index++] =
       ModuleRegistration{"AggregatorError", []() -> flint::IModule* { return new AggregatorError(); }};
   outModuleRegistrations[index++] = ModuleRegistration{
       "AggregatorFileWriter", []() -> flint::IModule* {
          return new AggregatorFileWriter(
              _fileHandlingMutexAggregationFileWriter, flint_aggregation_shared_data.aggregatorLandUnitSharedData,
              flint_aggregation_shared_data.dateDimension, flint_aggregation_shared_data.date2Dimension,
              flint_aggregation_shared_data.poolInfoDimension, flint_aggregation_shared_data.moduleInfoDimension);
       }};
   outModuleRegistrations[index++] = ModuleRegistration{
       "AggregatorUncertainty", []() -> flint::IModule* {
                             return new AggregatorUncertainty(flint_uncertainty_shared_data.date_dimension,
                                                              flint_uncertainty_shared_data.pool_info_dimension,
                                                              flint_uncertainty_shared_data.module_info_dimension);
       }};
   outModuleRegistrations[index++] = ModuleRegistration{
       "UncertaintyFileWriter", []() -> flint::IModule* {
          return new UncertaintyFileWriter(
              _fileHandlingMutexUncertaintyFileWriter, flint_uncertainty_shared_data.date_dimension,
              flint_uncertainty_shared_data.pool_info_dimension, flint_uncertainty_shared_data.module_info_dimension);
       }};
   outModuleRegistrations[index++] =
       ModuleRegistration{"AggregatorFluxStep", []() -> flint::IModule* { return new AggregatorFluxStep(); }};
   outModuleRegistrations[index++] =
       ModuleRegistration{"AggregatorStockStep", []() -> flint::IModule* { return new AggregatorStockStep(); }};
   outModuleRegistrations[index++] = ModuleRegistration{
       "CalendarAndEventSequencer", []() -> flint::IModule* { return new CalendarAndEventSequencer(); }};
   outModuleRegistrations[index++] = ModuleRegistration{"CalendarAndEventFlintDataSequencer", []() -> flint::IModule* {
                                                           return new CalendarAndEventFlintDataSequencer();
                                                        }};
   outModuleRegistrations[index++] =
       ModuleRegistration{"CalendarSequencer", []() -> flint::IModule* { return new CalendarSequencer(); }};
   outModuleRegistrations[index++] =
       ModuleRegistration{"DebugNotificationModule", []() -> flint::IModule* { return new DebugNotificationModule(); }};
   outModuleRegistrations[index++] =
       ModuleRegistration{"ErrorScreenWriter", []() -> flint::IModule* { return new ErrorScreenWriter(); }};
   outModuleRegistrations[index++] =
       ModuleRegistration{"OutputerStream", []() -> flint::IModule* { return new OutputerStream(); }};
   outModuleRegistrations[index++] =
       ModuleRegistration{"OutputerStreamFlux", []() -> flint::IModule* { return new OutputerStreamFlux(); }};
   outModuleRegistrations[index++] =
       ModuleRegistration{"ProxyModule", []() -> flint::IModule* { return new ProxyModule(); }};
   outModuleRegistrations[index++] =
       ModuleRegistration{"SequencerModuleBase", []() -> flint::IModule* { return new SequencerModuleBase(); }};
   outModuleRegistrations[index++] = ModuleRegistration{"TransactionManagerEndOfStepModule", []() -> flint::IModule* {
                                                           return new TransactionManagerEndOfStepModule();
                                                        }};
   outModuleRegistrations[index++] = ModuleRegistration{"TransactionManagerAfterSubmitModule", []() -> flint::IModule* {
                                                           return new TransactionManagerAfterSubmitModule();
                                                        }};
   outModuleRegistrations[index++] = ModuleRegistration{
       "WriteVariableGrid", []() -> flint::IModule* { return new WriteVariableGrid(_fileHandlingMutexVarGridWriter); }};
   outModuleRegistrations[index++] = ModuleRegistration{
       "WriteSystemConfig", []() -> flint::IModule* { return new WriteSystemConfig(_fileHandlingMutexConfigWriter); }};

   outModuleRegistrations[index++] =
       ModuleRegistration{"TestModule1", []() -> flint::IModule* { return new TestModule(); }};
   outModuleRegistrations[index++] =
       ModuleRegistration{"TestModule2", []() -> flint::IModule* { return new TestModule(); }};
   outModuleRegistrations[index++] =
       ModuleRegistration{"TestModule3", []() -> flint::IModule* { return new TestModule(); }};


   MOJA_LOG_DEBUG << (boost::format("getFlintModuleRegistrations: %s - %d") % "exit" % index).str();
   return index;
}

int getFlintTransformRegistrations(moja::flint::TransformRegistration* outTransformRegistrations) {
   MOJA_LOG_DEBUG << (boost::format("getFlintTransformRegistrations: %s") % "entered").str();

   auto index = 0;
   outTransformRegistrations[index++] =
       TransformRegistration{"SQLQueryTransform", []() -> flint::ITransform* { return new SQLQueryTransform(); }};
   outTransformRegistrations[index++] =
       TransformRegistration{"LocationTransform", []() -> flint::ITransform* { return new LocationTransform(); }};
   outTransformRegistrations[index++] =
       TransformRegistration{"LocationIdxTransform", []() -> flint::ITransform* { return new LocationIdxTransform(); }};
   outTransformRegistrations[index++] = TransformRegistration{
       "LocationFromFlintDataTransform", []() -> flint::ITransform* { return new LocationFromFlintDataTransform(); }};
   outTransformRegistrations[index++] =
       TransformRegistration{"LocationIdxFromFlintDataTransform",
                             []() -> flint::ITransform* { return new LocationIdxFromFlintDataTransform(); }};
   outTransformRegistrations[index++] =
       TransformRegistration{"LookupTransform", []() -> flint::ITransform* { return new LookupTransform(); }};
   outTransformRegistrations[index++] = TransformRegistration{
       "LookupRandomTransform", []() -> flint::ITransform* { return new LookupRandomTransform(); }};
   outTransformRegistrations[index++] =
       TransformRegistration{"CompositeTransform", []() -> flint::ITransform* { return new CompositeTransform(); }};
   outTransformRegistrations[index++] =
       TransformRegistration{"SumPoolsTransform", []() -> flint::ITransform* { return new SumPoolsTransform(); }};

   MOJA_LOG_DEBUG << (boost::format("getFlintTransformRegistrations: %s - %d") % "exit" % index).str();
   return index;
}

int getFlintFlintDataRegistrations(moja::flint::FlintDataRegistration* outFlintDataRegistrations) {
   MOJA_LOG_DEBUG << (boost::format("getFlintFlintDataRegistrations: %s") % "entered").str();

   auto index = 0;
   outFlintDataRegistrations[index++] =
       FlintDataRegistration{"IdNameCollection", []() -> flint::IFlintData* { return new IdNameCollection(); }};
   outFlintDataRegistrations[index++] =
       FlintDataRegistration{"IdNameDescCollection", []() -> flint::IFlintData* { return new IdNameDescCollection(); }};
   outFlintDataRegistrations[index++] =
       FlintDataRegistration{"OperationExtraData", []() -> flint::IFlintData* { return new OperationExtraData(); }};
   outFlintDataRegistrations[index++] =
       FlintDataRegistration{"SpatialLocationInfo", []() -> flint::IFlintData* { return new SpatialLocationInfo(); }};
   outFlintDataRegistrations[index++] =
       FlintDataRegistration{"SystemInformation", []() -> flint::IFlintData* { return new SystemInformation(); }};
   outFlintDataRegistrations[index++] = FlintDataRegistration{
       "LocalDomainInformation", []() -> flint::IFlintData* { return new LocalDomainInformation(); }};
   outFlintDataRegistrations[index++] =
       FlintDataRegistration{"EventQueue", []() -> flint::IFlintData* { return new EventQueue(); }};
   outFlintDataRegistrations[index++] = FlintDataRegistration{
       "SimulationUnitDataBase", []() -> flint::IFlintData* { return new SimulationUnitDataBase(); }};
   outFlintDataRegistrations[index++] = FlintDataRegistration{
       "UncertaintySimulationUnitData", []() -> flint::IFlintData* { return new UncertaintySimulationUnitData(); }};

    MOJA_LOG_DEBUG << (boost::format("getFlintFlintDataRegistrations: %s - %d") % "exit" % index).str();
   return index;
}

int getFlintFlintDataFactoryRegistrations(FlintDataFactoryRegistration* outFlintDataFactoryRegistrations) {
   MOJA_LOG_DEBUG << (boost::format("getFlintFlintDataFactoryRegistrations: %s") % "entered").str();

   auto index = 0;
   outFlintDataFactoryRegistrations[index++] =
       FlintDataFactoryRegistration{"internal.flint", "EventQueue", &createEventQueueFactory};

    MOJA_LOG_DEBUG << (boost::format("getFlintFlintDataFactoryRegistrations: %s - %d") % "exit" % index).str();
    return index;
}

int getProviderRegistrations(DataRepositoryProviderRegistration* outDataRepositoryProviderRegistration) {
   MOJA_LOG_DEBUG << (boost::format("getProviderRegistrations: %s") % "entered").str();

   auto index = 0;
   outDataRepositoryProviderRegistration[index++] = DataRepositoryProviderRegistration{
       "RasterTiled", static_cast<int>(datarepository::ProviderTypes::Raster),
       [](const DynamicObject& settings) -> std::shared_ptr<datarepository::IProviderInterface> {
          return std::make_shared<datarepository::ProviderSpatialRasterTiled>(
              std::make_shared<datarepository::FlintRasterReaderFactory>(), settings);
       }};
   outDataRepositoryProviderRegistration[index++] = DataRepositoryProviderRegistration{
       "SQLite", static_cast<int>(datarepository::ProviderTypes::Relational),
       [](const DynamicObject& settings) -> std::shared_ptr<datarepository::IProviderInterface> {
          return std::make_shared<datarepository::ProviderRelationalSQLite>(settings);
       }};

   MOJA_LOG_DEBUG << (boost::format("getProviderRegistrations: %s - %d") % "exit" % index).str();
   return index;
}

}

}  // namespace flint
}  // namespace moja
