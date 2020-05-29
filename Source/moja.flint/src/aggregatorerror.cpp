#include "moja/flint/aggregatorerror.h"

#include "moja/flint/flintexceptions.h"
#include "moja/flint/ivariable.h"
#include "moja/flint/simulationunitdatabase.h"
#include "moja/flint/spatiallocationinfo.h"

#include <moja/notificationcenter.h>
#include <moja/signals.h>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

namespace moja::flint {

void AggregatorError::configure(const DynamicObject& config) {
   log_errors_ = true;
   if (config.contains("log_errors")) {
      log_errors_ = config["log_errors"];
   }
   log_error_max_ = 0;
   if (config.contains("log_errors_max_number")) {
      log_error_max_ = config["log_errors_max_number"];
   }
   log_error_per_block_ = false;
   if (config.contains("log_errors_per_block")) {
      log_error_per_block_ = config["log_errors_per_block"];
   }
}

void AggregatorError::subscribe(NotificationCenter& notificationCenter) {
   notificationCenter.subscribe(signals::LocalDomainInit, &AggregatorError::onLocalDomainInit, *this);
   notificationCenter.subscribe(signals::LocalDomainProcessingUnitInit,
                                &AggregatorError::onLocalDomainProcessingUnitInit, *this);
   notificationCenter.subscribe(signals::Error, &AggregatorError::onError, *this);
}

void AggregatorError::onLocalDomainInit() {
   try {
      simulation_unit_data_ = std::static_pointer_cast<SimulationUnitDataBase>(
          _landUnitData->getVariable("simulationUnitData")->value().extract<std::shared_ptr<IFlintData>>());
      spatial_location_info_ = std::static_pointer_cast<SpatialLocationInfo>(
          _landUnitData->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<IFlintData>>());
      simulation_unit_data_->_logErrorCount = 0;
   } catch (VariableNotFoundException& exc) {
      auto str = ((boost::format("Variable not found: %1%") % *(boost::get_error_info<VariableName>(exc))).str());
      BOOST_THROW_EXCEPTION(flint::LocalDomainError()
                            << flint::Details(str) << flint::LibraryName("internal.flint")
                            << flint::ModuleName(BOOST_CURRENT_FUNCTION) << flint::ErrorCode(1));
   } catch (...) {
      if (_landUnitData->hasVariable("spatialLocationInfo")) {
         auto spatial_location_info = std::static_pointer_cast<SpatialLocationInfo>(
             _landUnitData->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<IFlintData>>());
         spatial_location_info->_library = "internal.flint";
         spatial_location_info->_module = BOOST_CURRENT_FUNCTION;
         spatial_location_info->_errorCode = 1;
      }
      throw;
   }
}
void AggregatorError::onLocalDomainProcessingUnitInit() {
   if (log_error_per_block_) simulation_unit_data_->_logErrorCount = 0;
}
void AggregatorError::onError(std::string msg) {
   if (log_errors_ && (log_error_max_ == 0 || simulation_unit_data_->_logErrorCount < log_error_max_)) {
      // Db's don't handle the newlines very well
      boost::replace_all(msg, "\n", " ");
      simulation_unit_data_->_errorLog.accumulate(
          {0,  // old _localDomainId
           spatial_location_info_->_tileIdx, spatial_location_info_->_blockIdx, spatial_location_info_->_cellIdx,
           spatial_location_info_->_randomSeedGlobal, spatial_location_info_->_randomSeedBlock,
           spatial_location_info_->_randomSeedCell, spatial_location_info_->_tileLatLon.lat,
           spatial_location_info_->_tileLatLon.lon, spatial_location_info_->_blockLatLon.lat,
           spatial_location_info_->_blockLatLon.lon, spatial_location_info_->_cellLatLon.lat,
           spatial_location_info_->_cellLatLon.lon,
           spatial_location_info_->_errorCode, spatial_location_info_->_library, spatial_location_info_->_module, msg});
   }
   simulation_unit_data_->_logErrorCount++;
}

}  // namespace moja::flint
