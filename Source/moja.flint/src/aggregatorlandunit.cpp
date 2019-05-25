#include "moja/flint/aggregatorlandunit.h"

#include "moja/flint/ilandunitdatawrapper.h"
#include "moja/flint/ioperationresult.h"
#include "moja/flint/ioperationresultflux.h"
#include "moja/flint/ipool.h"
#include "moja/flint/itiming.h"
#include "moja/flint/ivariable.h"
#include "moja/flint/simulationunitdatabase.h"

#include <moja/logging.h>
#include <moja/notificationcenter.h>
#include <moja/signals.h>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include <thread>

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------

void AggregatorLandUnit::configure(const DynamicObject& config) {
   _aggregatorLandUnitSharedData._moduleInfoOn = false;
   if (config.contains("moduleinfo_dim_enabled")) {
      _aggregatorLandUnitSharedData._moduleInfoOn = config["moduleinfo_dim_enabled"];
   }
   _aggregatorLandUnitSharedData._doRunStatsOn = false;
   if (config.contains("do_run_statistics")) {
      _aggregatorLandUnitSharedData._doRunStatsOn = config["do_run_statistics"];
   }
   _aggregatorLandUnitSharedData._outputMonth12Only = true;
   if (config.contains("output_month_12_only")) {
      _aggregatorLandUnitSharedData._outputMonth12Only = config["output_month_12_only"];
   }

   _aggregatorLandUnitSharedData._aggregateSinkAndSource = true;
   if (config.contains("aggregate_sink_and_source")) {
      _aggregatorLandUnitSharedData._aggregateSinkAndSource = config["aggregate_sink_and_source"];
   }
}

// --------------------------------------------------------------------------------------------

void AggregatorLandUnit::subscribe(NotificationCenter& notificationCenter) {
   notificationCenter.subscribe(signals::SystemInit, &AggregatorLandUnit::onSystemInit, *this);
   notificationCenter.subscribe(signals::SystemShutdown, &AggregatorLandUnit::onSystemShutdown, *this);
   notificationCenter.subscribe(signals::LocalDomainInit, &AggregatorLandUnit::onLocalDomainInit, *this);
   notificationCenter.subscribe(signals::LocalDomainShutdown, &AggregatorLandUnit::onLocalDomainShutdown, *this);
   notificationCenter.subscribe(signals::LocalDomainProcessingUnitInit,
                                &AggregatorLandUnit::onLocalDomainProcessingUnitInit, *this);
   notificationCenter.subscribe(signals::LocalDomainProcessingUnitShutdown,
                                &AggregatorLandUnit::onLocalDomainProcessingUnitShutdown, *this);
   notificationCenter.subscribe(signals::TimingInit, &AggregatorLandUnit::onTimingInit, *this);
   notificationCenter.subscribe(signals::TimingPostInit, &AggregatorLandUnit::onTimingPostInit, *this);
   notificationCenter.subscribe(signals::TimingEndStep, &AggregatorLandUnit::onTimingEndStep, *this);
   notificationCenter.subscribe(signals::TimingShutdown, &AggregatorLandUnit::onTimingShutdown, *this);
   notificationCenter.subscribe(signals::PostDisturbanceEvent, &AggregatorLandUnit::onPostDisturbanceEvent, *this);
}

// --------------------------------------------------------------------------------------------

void AggregatorLandUnit::onSystemInit() {
   _simulationUnitData = std::static_pointer_cast<SimulationUnitDataBase>(
       _landUnitData->getVariable("simulationUnitDataBase")->value().extract<std::shared_ptr<flint::IFlintData>>());
   _simulationUnitData->_startSystemTime = DateTime::now();
   try {
      // Pools and their Ids are constant for a whole simulation (across threads as well). So create them here
      for (auto& pool : _landUnitData->poolCollection()) {
         auto poolInfoRecord = std::make_shared<PoolInfoRecord>(pool->name(), pool->description(), pool->idx(),
                                                                pool->order(), pool->scale(), pool->units());
         _poolInfoDimension->accumulate(poolInfoRecord, pool->idx());
      }
   } catch (Poco::AssertionViolationException& exc) {
      MOJA_LOG_DEBUG << /*_localDomainId*/ -1 << ":AssertionViolationException - " << exc.displayText();
      throw;
   } catch (const std::exception& e) {
      MOJA_LOG_DEBUG << /*_localDomainId*/ -1 << ":Unknown exception: - " << e.what();
      throw;
   } catch (...) {
      MOJA_LOG_DEBUG << /*_localDomainId*/ -1 << ":Unknown exception";
      throw;
   }
}

// --------------------------------------------------------------------------------------------

void AggregatorLandUnit::onSystemShutdown() { _simulationUnitData->_endSystemTime = DateTime::now(); }

// --------------------------------------------------------------------------------------------

void AggregatorLandUnit::onLocalDomainInit() {
   _simulationUnitData = std::static_pointer_cast<SimulationUnitDataBase>(
       _landUnitData->getVariable("simulationUnitDataBase")->value().extract<std::shared_ptr<flint::IFlintData>>());

   if (_landUnitData->hasVariable("localDomainId"))
      _simulationUnitData->_localDomainId = _landUnitData->getVariable("localDomainId")->value();
   else
      _simulationUnitData->_localDomainId = -1;

   _simulationUnitData->_startLocalDomainTime = DateTime::now();
   _simulationUnitData->_luCountLocalDomain = 0;
}

// --------------------------------------------------------------------------------------------

void AggregatorLandUnit::onLocalDomainShutdown() { _simulationUnitData->_endLocalDomainTime = DateTime::now(); }

// --------------------------------------------------------------------------------------------

void AggregatorLandUnit::onLocalDomainProcessingUnitInit() {
   _simulationUnitData->_startProcessingUnitTime = DateTime::now();
   _simulationUnitData->_processUnitCount++;
   _simulationUnitData->_luCountProcessingUnit = 0;
}

// --------------------------------------------------------------------------------------------

void AggregatorLandUnit::onLocalDomainProcessingUnitShutdown() {
   _simulationUnitData->_endProcessingUnitTime = DateTime::now();
}

// --------------------------------------------------------------------------------------------

void AggregatorLandUnit::onTimingInit() {
   _simulationUnitData->_luCountProcessingUnit++;
   _simulationUnitData->_luCountLocalDomain++;

   if (_landUnitData->hasVariable("landUnitArea"))
      _simulationUnitData->_landUnitArea = _landUnitData->getVariable("landUnitArea")->value();
   else
      _simulationUnitData->_landUnitArea = 1.0;  //  _spatiallocationinfo->_landUnitArea;

   // Clear the LU dataset in prep for this LU processing
   _fluxesLU.clear();
}

// --------------------------------------------------------------------------------------------

void AggregatorLandUnit::onTimingPostInit() {
   recordFluxSet();
   //_landUnitData->clearLastAppliedOperationResults();
}

// --------------------------------------------------------------------------------------------

void AggregatorLandUnit::onTimingEndStep() {
   recordFluxSet();
   //_landUnitData->clearLastAppliedOperationResults();
}

// --------------------------------------------------------------------------------------------

void AggregatorLandUnit::onTimingShutdown() {
   // Here we can assume pixel has worked	without any errors - if there had been processing would've been aborted
   // So set all LU level dimensions and then accumulate fluxes into

   for (auto& flux : _fluxesLU) {
      // Now have the required dimensions - look for the flux record.
      auto fluxRecord = std::make_shared<FluxRecord>(_aggregatorLandUnitSharedData._aggregateSinkAndSource,
                                                     _simulationUnitData->_localDomainId, flux._dateRecordId,
                                                     flux._moduleId, 1, flux._srcIx, flux._dstIx, flux._fluxValue);
      _simulationUnitData->_fluxResults.accumulate(fluxRecord);
   }
   _fluxesLU.clear();
}

// --------------------------------------------------------------------------------------------

void AggregatorLandUnit::onError(std::string msg) {
   _fluxesLU.clear();  // dump any fluxex stored
                       // leave this up to the transaction Manager
                       //_landUnitData->clearAllOperationResults();
}

// --------------------------------------------------------------------------------------------

void AggregatorLandUnit::onPostDisturbanceEvent() {
   recordFluxSet();
   //_landUnitData->clearLastAppliedOperationResults();
}

// --------------------------------------------------------------------------------------------

void AggregatorLandUnit::recordFluxSet() {
   const auto timing = _landUnitData->timing();
   int curStep = timing->step();
   int curSubStep = timing->subStep();

   // If Flux set is empty return immediately
   if (_landUnitData->getOperationLastAppliedIterator().empty()) return;

   // Find the date dimension record.
   // Int64 dateRecordId;
   auto reportingEndDate = timing->curEndDate();
   reportingEndDate.addMilliseconds(-1);

   Poco::Nullable<Int64> dateRecordId;
   dateRecordId.clear();
   if (_aggregatorLandUnitSharedData._outputMonth12Only) {
      auto dateRecord = std::make_shared<Date2Record>(reportingEndDate.year());
      auto storedDateRecord = _date2Dimension->accumulate(dateRecord, reportingEndDate.year());
      dateRecordId = storedDateRecord->getId();
   } else {
      auto dateRecord =
          std::make_shared<DateRecord>(curStep, curSubStep, reportingEndDate.year(), reportingEndDate.month(),
                                       reportingEndDate.day(), timing->fractionOfStep(), timing->stepLengthInYears());
      auto storedDateRecord = _dateDimension->accumulate(dateRecord);
      dateRecordId = storedDateRecord->getId();
   }

   for (auto operationResult : _landUnitData->getOperationLastAppliedIterator()) {
      auto metaData = operationResult->metaData();
      Poco::Nullable<Int64> moduleInfoId;
      if (_aggregatorLandUnitSharedData._moduleInfoOn && metaData != nullptr) {
         auto moduleRecord =
             std::make_shared<ModuleInfoRecord>(metaData->libraryType, metaData->libraryInfoId, metaData->moduleType,
                                                metaData->moduleId, metaData->moduleName);
         auto storedModuleInfoRecord = _moduleInfoDimension->accumulate(moduleRecord);
         moduleInfoId = storedModuleInfoRecord->getId();
      } else {
         moduleInfoId.clear();
      }

      for (const auto& flux : operationResult->operationResultFluxCollection()) {
         const auto srcIx = flux->source();
         const auto dstIx = flux->sink();
         if (srcIx == dstIx) continue;  // don't process diagonal - flux to & from same pool is ignored
         const auto fluxValue = flux->value() * _simulationUnitData->_landUnitArea;
         _fluxesLU.emplace_back(fluxDataLU{dateRecordId, moduleInfoId, srcIx, dstIx, fluxValue});
      }
   }
}

}  // namespace flint
}  // namespace moja
