#include "moja/flint/aggregatoruncertaintylandunit.h"

#include "moja/flint/flintexceptions.h"
#include "moja/flint/ilandunitdatawrapper.h"
#include "moja/flint/ioperationresult.h"
#include "moja/flint/ioperationresultflux.h"
#include "moja/flint/ipool.h"
#include "moja/flint/itiming.h"
#include "moja/flint/ivariable.h"
#include "moja/flint/simulationunitdatabase.h"
#include "moja/flint/spatiallocationinfo.h"
#include "moja/flint/uncertaintyvariable.h"

#include <moja/logging.h>
#include <moja/notificationcenter.h>
#include <moja/signals.h>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include <thread>

#include "moja/flint/uncertaintysimulationdata.h"

namespace moja::flint {

void AggregatorUncertaintyLandUnit::configure(const DynamicObject& config) {
   aggregator_land_unit_shared_data_.tile_index_on = true;
   if (config.contains("tile_index_dim_enabled")) {
      aggregator_land_unit_shared_data_.tile_index_on = config["tile_index_dim_enabled"];
   }
   aggregator_land_unit_shared_data_.block_index_on = false;
   if (aggregator_land_unit_shared_data_.tile_index_on &&
       config.contains("block_index_dim_enabled")) {  // can't have block without tile
      aggregator_land_unit_shared_data_.block_index_on = config["block_index_dim_enabled"];
   }
   aggregator_land_unit_shared_data_.cell_index_on = false;
   if (aggregator_land_unit_shared_data_.block_index_on &&
       config.contains("cell_index_dim_enabled")) {  // can't have cell without block
      aggregator_land_unit_shared_data_.cell_index_on = config["cell_index_dim_enabled"];
   }
   aggregator_land_unit_shared_data_.module_info_on = false;
   if (config.contains("module_info_dim_enabled")) {
      aggregator_land_unit_shared_data_.module_info_on = config["module_info_dim_enabled"];
   }
   aggregator_land_unit_shared_data_.do_run_stats_on = false;
   if (config.contains("do_run_statistics")) {
      aggregator_land_unit_shared_data_.do_run_stats_on = config["do_run_statistics"];
   }
   aggregator_land_unit_shared_data_.do_stock = false;
   if (config.contains("do_stock")) {
      aggregator_land_unit_shared_data_.do_stock = config["do_stock"];
   }
   aggregator_land_unit_shared_data_.output_month_12_only = true;
   if (config.contains("output_month_12_only")) {
      aggregator_land_unit_shared_data_.output_month_12_only = config["output_month_12_only"];
   }
   if (config.contains("reporting_classifier_set")) {
      classifier_set_var_name_ = config["reporting_classifier_set"].extract<std::string>();
   } else {
      classifier_set_var_name_ = "classifier_set";
   }
   aggregator_land_unit_shared_data_.clear_fluxes_after_recording = false;
   if (config.contains("clear_fluxes_after_recording")) {
       aggregator_land_unit_shared_data_.clear_fluxes_after_recording = config["clear_fluxes_after_recording"];
   }
}

void AggregatorUncertaintyLandUnit::subscribe(NotificationCenter& notificationCenter) {
   notificationCenter.subscribe(signals::SystemInit, &AggregatorUncertaintyLandUnit::onSystemInit, *this);
   notificationCenter.subscribe(signals::SystemShutdown, &AggregatorUncertaintyLandUnit::onSystemShutdown, *this);
   notificationCenter.subscribe(signals::LocalDomainInit, &AggregatorUncertaintyLandUnit::onLocalDomainInit, *this);
   notificationCenter.subscribe(signals::LocalDomainShutdown, &AggregatorUncertaintyLandUnit::onLocalDomainShutdown,
                                *this);
   notificationCenter.subscribe(signals::LocalDomainProcessingUnitInit,
                                &AggregatorUncertaintyLandUnit::onLocalDomainProcessingUnitInit, *this);
   notificationCenter.subscribe(signals::LocalDomainProcessingUnitShutdown,
                                &AggregatorUncertaintyLandUnit::onLocalDomainProcessingUnitShutdown, *this);
   notificationCenter.subscribe(signals::TimingInit, &AggregatorUncertaintyLandUnit::onTimingInit, *this);
   notificationCenter.subscribe(signals::TimingPostInit, &AggregatorUncertaintyLandUnit::onTimingPostInit, *this);
   notificationCenter.subscribe(signals::TimingEndStep, &AggregatorUncertaintyLandUnit::onTimingEndStep, *this);
   notificationCenter.subscribe(signals::TimingShutdown, &AggregatorUncertaintyLandUnit::onTimingShutdown, *this);
   notificationCenter.subscribe(signals::Error, &AggregatorUncertaintyLandUnit::onError, *this);
   notificationCenter.subscribe(signals::PostDisturbanceEvent, &AggregatorUncertaintyLandUnit::onPostDisturbanceEvent,
                                *this);
}

void AggregatorUncertaintyLandUnit::onSystemInit() {
   if (!_landUnitData->uncertainty().enabled()) {
      BOOST_THROW_EXCEPTION(flint::LocalDomainError()
                            << flint::Details("Uncertainty not enabled") << flint::LibraryName("flint.internal")
                            << flint::ModuleName(BOOST_CURRENT_FUNCTION) << flint::ErrorCode(1));
   }
   try {
      simulation_unit_data_ = std::static_pointer_cast<UncertaintySimulationUnitData>(
          _landUnitData->getVariable("uncertaintySimulationUnitData")->value().extract<std::shared_ptr<flint::IFlintData>>());
      simulation_unit_data_->start_system_time = DateTime::now();
      // Pools and their Ids are constant for a whole simulation (across threads as well). So create them here
      for (auto& pool : _landUnitData->poolCollection()) {
         pool_info_dimension_->accumulate(PoolInfoRecord{pool->name(), pool->description(), pool->idx(), pool->order(),
                                                         pool->scale(), pool->units()},
             pool->idx());
      }
   } catch (VariableNotFoundException& exc) {
      const auto str = (boost::format("Variable not found: %1%") % *(boost::get_error_info<VariableName>(exc))).str();
      BOOST_THROW_EXCEPTION(flint::LocalDomainError()
                            << flint::Details(str) << flint::LibraryName("flint.internal")
                            << flint::ModuleName(BOOST_CURRENT_FUNCTION) << flint::ErrorCode(1));
   } catch (...) {
      if (_landUnitData->hasVariable("spatialLocationInfo")) {
         auto spatial_location_info = std::static_pointer_cast<SpatialLocationInfo>(
             _landUnitData->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<flint::IFlintData>>());
         spatial_location_info->_library = "flint.internal";
         spatial_location_info->_module = BOOST_CURRENT_FUNCTION;
         spatial_location_info->_errorCode = 1;
      }
      throw;
   }
}

void AggregatorUncertaintyLandUnit::onSystemShutdown() { simulation_unit_data_->end_system_time = DateTime::now(); }

void AggregatorUncertaintyLandUnit::onLocalDomainInit() {
   simulation_unit_data_ = std::static_pointer_cast<UncertaintySimulationUnitData>(
       _landUnitData->getVariable("uncertaintySimulationUnitData")->value().extract<std::shared_ptr<IFlintData>>());
   spatial_location_info_ = std::static_pointer_cast<flint::SpatialLocationInfo>(
       _landUnitData->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<IFlintData>>());
   build_worked_var_ = _landUnitData->getVariable("landUnitBuildSuccess");
   classifier_set_var_ = _landUnitData->getVariable(classifier_set_var_name_);

   simulation_unit_data_->start_local_domain_time = DateTime::now();

   // date_record_id_.clear();
   global_random_seed_.clear();
   tile_random_seed_.clear();
   block_random_seed_.clear();
   cell_random_seed_.clear();

   if (!aggregator_land_unit_shared_data_.tile_index_on) {
      tile_idx_.clear();
      block_idx_.clear();
      cell_idx_.clear();
      tile_lat_.clear();
      tile_lon_.clear();
      block_lat_.clear();
      block_lon_.clear();
      cell_lat_.clear();
      cell_lon_.clear();
   } else if (!aggregator_land_unit_shared_data_.block_index_on) {
      block_idx_.clear();
      cell_idx_.clear();
      block_lat_.clear();
      block_lon_.clear();
      cell_lat_.clear();
      cell_lon_.clear();
   } else if (!aggregator_land_unit_shared_data_.cell_index_on) {
      cell_idx_.clear();
      cell_lat_.clear();
      cell_lon_.clear();
   }

   simulation_unit_data_->lu_count_local_domain = 0;
}

void AggregatorUncertaintyLandUnit::onLocalDomainShutdown() {
   simulation_unit_data_->end_local_domain_time = DateTime::now();
}

void AggregatorUncertaintyLandUnit::onLocalDomainProcessingUnitInit() {
   simulation_unit_data_->start_processing_unit_time = DateTime::now();
   simulation_unit_data_->process_unit_count++;
   simulation_unit_data_->lu_count_processing_unit = 0;
}

void AggregatorUncertaintyLandUnit::onLocalDomainProcessingUnitShutdown() {
   simulation_unit_data_->end_processing_unit_time = DateTime::now();
}

void AggregatorUncertaintyLandUnit::onTimingInit() {
   simulation_unit_data_->lu_count_processing_unit++;
   simulation_unit_data_->lu_count_local_domain++;

   if (_landUnitData->hasVariable("spatialLocationInfo")) {
       auto spatialLocationInfo = std::static_pointer_cast<SpatialLocationInfo>(
           _landUnitData->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<flint::IFlintData>>());
       simulation_unit_data_->land_unit_area = spatialLocationInfo->getProperty("landUnitArea");
   } else if (_landUnitData->hasVariable("landUnitArea")) {
       simulation_unit_data_->land_unit_area = _landUnitData->getVariable("landUnitArea")->value();
   } else {
       simulation_unit_data_->land_unit_area = 1.0;
   }

   constructed_tile_id_ = 0;  // TileIdx [24 bits], blockIdx [12 bits], cellIdx [28 bits]

   global_random_seed_ = spatial_location_info_->_randomSeedGlobal;

   if (aggregator_land_unit_shared_data_.tile_index_on) {
      tile_idx_ = spatial_location_info_->_tileIdx;
      tile_lat_ = spatial_location_info_->_tileLatLon.lat;
      tile_lon_ = spatial_location_info_->_tileLatLon.lon;
      tile_random_seed_ = spatial_location_info_->_randomSeedTile;

      constructed_tile_id_ += (static_cast<Int64>(spatial_location_info_->_tileIdx) << 40);

      if (aggregator_land_unit_shared_data_.block_index_on) {
         block_idx_ = spatial_location_info_->_blockIdx;
         block_lat_ = spatial_location_info_->_blockLatLon.lat;
         block_lon_ = spatial_location_info_->_blockLatLon.lon;
         block_random_seed_ = spatial_location_info_->_randomSeedBlock;

         constructed_tile_id_ += (static_cast<Int64>(spatial_location_info_->_blockIdx) << 28);

         if (aggregator_land_unit_shared_data_.cell_index_on) {
            cell_idx_ = spatial_location_info_->_cellIdx;
            cell_lat_ = spatial_location_info_->_cellLatLon.lat;
            cell_lon_ = spatial_location_info_->_cellLatLon.lon;
            cell_random_seed_ = spatial_location_info_->_randomSeedCell;

            constructed_tile_id_ += spatial_location_info_->_cellIdx;
         }
      }
   }

   // Clear the LU data sets in prep for this LU processing
   fluxes_lu_.clear();
   stocks_lu_.clear();
}

void AggregatorUncertaintyLandUnit::onTimingPostInit() {
   recordStockSet();
   recordFluxSet();
}

void AggregatorUncertaintyLandUnit::onTimingEndStep() { recordFluxSet(); }

void AggregatorUncertaintyLandUnit::onTimingShutdown() {

   const auto stored_tile_info = tile_info_dimension_->accumulate(
       {tile_idx_, block_idx_, cell_idx_, tile_lat_, tile_lon_, block_lat_, block_lon_, cell_lat_, cell_lon_,
        global_random_seed_, tile_random_seed_, block_random_seed_, cell_random_seed_},
       constructed_tile_id_);
   const auto tile_id = stored_tile_info->getId();

   // Classifier set information.
   const auto& land_unit_classifier_set = classifier_set_var_->value().extract<DynamicObject>();
   std::vector<Poco::Nullable<std::string>> classifier_set;
   if (classifier_names_->empty()) {
      recordClassifierNames(land_unit_classifier_set);
   }

   for (const auto& classifier : land_unit_classifier_set) {
      Poco::Nullable<std::string> classifier_value;
      if (!classifier.second.isEmpty()) {
         if (classifier.second.type() == typeid(UInt8)) {
            const auto val = classifier.second.convert<int>();
            classifier_value = std::to_string(val);
         } else {
            classifier_value = classifier.second.convert<std::string>();
         }
      }
      classifier_set.push_back(classifier_value);
   }

   const auto stored_classifier_set = classifier_set_dimension_->accumulate(ClassifierSetRecord{classifier_set});
   const auto classifier_set_id = stored_classifier_set->getId();

   // Flux aggregation
   const auto i = _landUnitData->uncertainty().iteration();
   const auto n = _landUnitData->uncertainty().iterations();
   auto& flux_results = simulation_unit_data_->land_unit_flux_results;
   for (auto& flux : fluxes_lu_) {
      std::vector<double> fluxes(n);
      fluxes[i] = flux.flux_value;

      flux_results.accumulate({simulation_unit_data_->local_domain_id, flux.date_record_id, tile_id, classifier_set_id,
                               flux.module_info_id, flux.src_idx, flux.dst_idx},
                              {fluxes});
   }
   fluxes_lu_.clear();

   // Handle Stock aggregation
   auto& stock_results = simulation_unit_data_->land_unit_stock_results;
   for (const auto& stock : stocks_lu_) {
      std::vector<double> stocks(n);
      stocks[i] = stock.value;
      stock_results.accumulate(
          {simulation_unit_data_->local_domain_id, stock.date_record_id,tile_id, classifier_set_id,
                                stock.pool_idx},
                               {stocks});
   }
   stocks_lu_.clear();
}

void AggregatorUncertaintyLandUnit::onOutputStep() {
    recordStockSet();
}

void AggregatorUncertaintyLandUnit::onError(std::string msg) {
   fluxes_lu_.clear();  
   stocks_lu_.clear();
}

void AggregatorUncertaintyLandUnit::onPostDisturbanceEvent() {
   recordFluxSet();
}

void AggregatorUncertaintyLandUnit::recordClassifierNames(const DynamicObject& classifierSet) {
   if (!classifier_names_->empty()) {
      return;
   }
   for (const auto& classifier : classifierSet) {
      auto name = classifier.first;
      std::replace(name.begin(), name.end(), '.', '_');
      std::replace(name.begin(), name.end(), ' ', '_');
      classifier_names_->push_back(name);
   }
}

void AggregatorUncertaintyLandUnit::recordStockSet() {
   if (!aggregator_land_unit_shared_data_.do_stock) return;

   const auto timing = _landUnitData->timing();

   auto reporting_end_date = timing->curEndDate();
   reporting_end_date.addMilliseconds(-1);

   const auto stored_date_record = date_dimension_->accumulate({reporting_end_date.year()}, reporting_end_date.year());
   Poco::Nullable<Int64> date_record_id = stored_date_record->getId();

   // Get current pool data.
   auto pools = _landUnitData->poolCollection();
   for (auto& pool : _landUnitData->poolCollection()) {
      const auto pool_value = pool->value() * spatial_location_info_->_landUnitArea;
      stocks_lu_.emplace_back(stockDataLU{date_record_id, pool->idx(), pool_value});
   }
}

void AggregatorUncertaintyLandUnit::recordFluxSet() {
   const auto timing = _landUnitData->timing();

   // If Flux set is empty return immediately
   if (_landUnitData->getOperationLastAppliedIterator().empty()) return;

   // Find the date dimension record.
   auto reporting_end_date = timing->curEndDate();
   reporting_end_date.addMilliseconds(-1);

   const auto stored_date_record = date_dimension_->accumulate({reporting_end_date.year()}, reporting_end_date.year());
   const auto date_record_id = stored_date_record->getId();

   for (const auto& operation_result : _landUnitData->getOperationLastAppliedIterator()) {
      const auto meta_data = operation_result->metaData();
      Poco::Nullable<Int64> module_info_id;
      if (aggregator_land_unit_shared_data_.module_info_on && meta_data != nullptr) {
         auto module_record =
             std::make_shared<ModuleInfoRecord>(meta_data->libraryType, meta_data->libraryInfoId, meta_data->moduleType,
                                                meta_data->moduleId, meta_data->moduleName);
         const auto stored_module_info_record =
             module_info_dimension_->accumulate({meta_data->libraryType, meta_data->libraryInfoId,
                                                 meta_data->moduleType, meta_data->moduleId, meta_data->moduleName});
         module_info_id = stored_module_info_record->getId();
      }

      for (const auto& flux : operation_result->operationResultFluxCollection()) {
         const auto srcIx = flux->source();
         const auto dstIx = flux->sink();
         if (srcIx == dstIx) continue;  // don't process diagonal - flux to & from same pool is ignored
         const auto fluxValue = flux->value() * simulation_unit_data_->land_unit_area;
         fluxes_lu_.emplace_back(fluxDataLU{date_record_id, module_info_id, srcIx, dstIx, fluxValue});
      }
   }

   if (aggregator_land_unit_shared_data_.clear_fluxes_after_recording) {
       _landUnitData->clearLastAppliedOperationResults();
   }
}

}  // namespace moja::flint
