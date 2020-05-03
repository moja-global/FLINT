#include "moja/flint/aggregatoruncertainty.h"

#include "moja/flint/ilandunitdatawrapper.h"
#include "moja/flint/ioperationresult.h"
#include "moja/flint/ioperationresultflux.h"
#include "moja/flint/ipool.h"
#include "moja/flint/itiming.h"
#include "moja/flint/ivariable.h"
#include "moja/flint/uncertaintysimulationdata.h"
#include "moja/flint/uncertaintyvariable.h"

#include <moja/logging.h>
#include <moja/notificationcenter.h>
#include <moja/signals.h>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include <thread>

namespace moja::flint {

void AggregatorUncertainty::configure(const DynamicObject& config) {
   module_info_on_ = false;
   if (config.contains("module_info_dim_enabled")) {
      module_info_on_ = config["module_info_dim_enabled"];
   }
   output_annual_ = true;
   if (config.contains("output_annual")) {
      output_annual_ = config["output_annual"];
   }
   aggregate_sink_and_source_ = true;
   if (config.contains("aggregate_sink_and_source")) {
      aggregate_sink_and_source_ = config["aggregate_sink_and_source"];
   }
   aggregate_stock_ = true;
   if (config.contains("aggregate_stock")) {
      aggregate_stock_ = config["aggregate_stock"];
   }
}

void AggregatorUncertainty::subscribe(NotificationCenter& notificationCenter) {
   notificationCenter.subscribe(signals::SystemInit, &AggregatorUncertainty::onSystemInit, *this);
   notificationCenter.subscribe(signals::SystemShutdown, &AggregatorUncertainty::onSystemShutdown, *this);
   notificationCenter.subscribe(signals::LocalDomainInit, &AggregatorUncertainty::onLocalDomainInit, *this);
   notificationCenter.subscribe(signals::LocalDomainShutdown, &AggregatorUncertainty::onLocalDomainShutdown, *this);
   notificationCenter.subscribe(signals::LocalDomainProcessingUnitInit,
                                &AggregatorUncertainty::onLocalDomainProcessingUnitInit, *this);
   notificationCenter.subscribe(signals::LocalDomainProcessingUnitShutdown,
                                &AggregatorUncertainty::onLocalDomainProcessingUnitShutdown, *this);
   notificationCenter.subscribe(signals::TimingInit, &AggregatorUncertainty::onTimingInit, *this);
   notificationCenter.subscribe(signals::TimingPostInit, &AggregatorUncertainty::onTimingPostInit, *this);
   notificationCenter.subscribe(signals::TimingEndStep, &AggregatorUncertainty::onTimingEndStep, *this);
   notificationCenter.subscribe(signals::OutputStep, &AggregatorUncertainty::onOutputStep, *this);

   notificationCenter.subscribe(signals::TimingShutdown, &AggregatorUncertainty::onTimingShutdown, *this);
   notificationCenter.subscribe(signals::PostDisturbanceEvent, &AggregatorUncertainty::onPostDisturbanceEvent, *this);
}

void AggregatorUncertainty::onSystemInit() {
   simulation_unit_data_ = std::static_pointer_cast<UncertaintySimulationUnitData>(
       _landUnitData->getVariable("uncertaintySimulationUnitData")->value().extract<std::shared_ptr<IFlintData>>());
   simulation_unit_data_->start_system_time = DateTime::now();
   try {
      // Pools and their Ids are constant for a whole simulation (across threads as well). So create them here
      for (auto& pool : _landUnitData->poolCollection()) {
         auto pool_info_record = std::make_shared<PoolInfoRecord>(pool->name(), pool->description(), pool->idx(),
                                                                  pool->order(), pool->scale(), pool->units());
         pool_info_dimension_->accumulate(pool_info_record, pool->idx());
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

void AggregatorUncertainty::onSystemShutdown() { simulation_unit_data_->end_system_time = DateTime::now(); }

void AggregatorUncertainty::onLocalDomainInit() {
   simulation_unit_data_ = std::static_pointer_cast<UncertaintySimulationUnitData>(
       _landUnitData->getVariable("uncertaintySimulationUnitData")->value().extract<std::shared_ptr<flint::IFlintData>>());

   if (_landUnitData->hasVariable("localDomainId"))
      simulation_unit_data_->local_domain_id = _landUnitData->getVariable("localDomainId")->value();
   else
      simulation_unit_data_->local_domain_id = -1;

   simulation_unit_data_->start_local_domain_time = DateTime::now();
   simulation_unit_data_->lu_count_local_domain = 0;
}

void AggregatorUncertainty::onLocalDomainShutdown() { simulation_unit_data_->end_local_domain_time = DateTime::now(); }

void AggregatorUncertainty::onLocalDomainProcessingUnitInit() {
   simulation_unit_data_->start_processing_unit_time = DateTime::now();
   simulation_unit_data_->process_unit_count++;
   simulation_unit_data_->lu_count_processing_unit = 0;
}

void AggregatorUncertainty::onLocalDomainProcessingUnitShutdown() {
   simulation_unit_data_->end_processing_unit_time = DateTime::now();
}

void AggregatorUncertainty::onTimingInit() {
   simulation_unit_data_->lu_count_processing_unit++;
   simulation_unit_data_->lu_count_local_domain++;

   if (_landUnitData->hasVariable("landUnitArea"))
      simulation_unit_data_->land_unit_area = _landUnitData->getVariable("landUnitArea")->value();
   else
      simulation_unit_data_->land_unit_area = 1.0;

   // Clear the LU data sets in prep for this LU processing
   fluxes_lu_.clear();
   stocks_lu_.clear();
}

void AggregatorUncertainty::onTimingPostInit() {
   record_stock_set();
   record_flux_set();
}

void AggregatorUncertainty::onTimingEndStep() { record_flux_set(); }

void AggregatorUncertainty::onOutputStep() { record_stock_set(); }

void AggregatorUncertainty::onTimingShutdown() {
   // Here we can assume pixel has worked without any errors - if there had been, processing would've been aborted
   // So set all LU level dimensions and then accumulate fluxes into

   const auto& uncertainty = _landUnitData->uncertainty();

   const auto i = uncertainty.iteration();
   const auto n = uncertainty.iterations();

   for (const auto& flux : fluxes_lu_) {
      // Now have the required dimensions - look for the flux record.
      std::vector<double> fluxes(n);
      fluxes[i] = flux.flux_value;

      auto flux_record = std::make_shared<UncertaintyFluxRecord>(
          aggregate_sink_and_source_, simulation_unit_data_->local_domain_id, flux.date_record_id,
          flux.module_id, 1, flux.src_ix, flux.dst_ix, fluxes);
      simulation_unit_data_->flux_results.accumulate(flux_record);
   }

   for (const auto& stock : stocks_lu_) {
      // Now have the required dimensions - look for the flux record.
      std::vector<double> stocks(n);
      stocks[i] = stock.stock_value;

      auto& stock_results = simulation_unit_data_->stock_results;
         stock_results.accumulate({short(stock.date_record_id), short(stock.pool_ix)}, {stocks});
   }
   fluxes_lu_.clear();
   stocks_lu_.clear();
}

void AggregatorUncertainty::onError(std::string /*msg*/) {
   fluxes_lu_.clear();  
   stocks_lu_.clear();
}

void AggregatorUncertainty::onPostDisturbanceEvent() { record_flux_set(); }

void AggregatorUncertainty::record_flux_set() {
   const auto timing = _landUnitData->timing();
   auto cur_step = timing->step();
   auto cur_sub_step = timing->subStep();

   // If Flux set is empty return immediately
   if (_landUnitData->getOperationLastAppliedIterator().empty()) return;

   // Find the date dimension record.
   const auto reporting_end_date = timing->curEndDate().addMilliseconds(-1);

   auto date_record = std::make_shared<Date2Record>(reporting_end_date.year());
   const auto stored_date_record = date_dimension_->accumulate(date_record, reporting_end_date.year());
   Poco::Nullable<Int64> date_record_id = stored_date_record->getId();

   for (const auto& operation_result : _landUnitData->getOperationLastAppliedIterator()) {
      const auto meta_data = operation_result->metaData();
      Poco::Nullable<Int64> module_info_id;
      if (module_info_on_ && meta_data != nullptr) {
         auto module_record =
             std::make_shared<ModuleInfoRecord>(meta_data->libraryType, meta_data->libraryInfoId, meta_data->moduleType,
                                                meta_data->moduleId, meta_data->moduleName);
         const auto stored_module_info_record = module_info_dimension_->accumulate(module_record);
         module_info_id = stored_module_info_record->getId();
      } else {
         module_info_id.clear();
      }
      for (const auto& flux : operation_result->operationResultFluxCollection()) {
         const auto src_ix = flux->source();
         const auto dst_ix = flux->sink();
         if (src_ix == dst_ix) continue;  // don't process diagonal - flux to & from same pool is ignored
         const auto flux_value = flux->value() * simulation_unit_data_->land_unit_area;
         fluxes_lu_.emplace_back(
             flux_data_lu{ date_record_id, module_info_id, src_ix, dst_ix, flux_value});
      }
   }
}

void AggregatorUncertainty::record_stock_set() {
   if (!aggregate_stock_) return;

   const auto timing = _landUnitData->timing();
   int curStep = timing->step();
   int curSubStep = timing->subStep();

   const auto reporting_end_date = timing->curEndDate().addMilliseconds(-1);
   const auto date_record = std::make_shared<Date2Record>(reporting_end_date.year());
   const auto stored_date_record = date_dimension_->accumulate(date_record, reporting_end_date.year());
   Poco::Nullable<Int64> date_record_id = stored_date_record->getId();

   // Get current pool data.
   if (reporting_end_date.month() % 12 == 0) {
      auto pools = _landUnitData->poolCollection();
      for (auto& pool : _landUnitData->poolCollection()) {
         const double pool_value = pool->value() * simulation_unit_data_->land_unit_area;
         stocks_lu_.emplace_back(stock_data_lu{date_record_id, pool->idx(), pool_value});
      }
   }
}

}  // namespace moja::flint
