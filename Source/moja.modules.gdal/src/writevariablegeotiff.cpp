#include "moja/modules/gdal/writevariablegeotiff.h"

#include <moja/flint/iflintdata.h>
#include <moja/flint/ipool.h>
#include <moja/flint/itiming.h>
#include <moja/flint/ivariable.h>
#include <moja/flint/spatiallocationinfo.h>
#include <moja/flint/variableandpoolstringbuilder.h>

#include <moja/notificationcenter.h>
#include <moja/signals.h>
#include <moja/filesystem.h>

#include <fmt/format.h>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/format/group.hpp>

#include <cmath>
#include <gdal_priv.h>
#include <limits>
#include <string>

namespace fs = moja::filesystem;

namespace moja {
namespace modules {
namespace gdal {

void WriteVariableGeotiff::configure(const DynamicObject& config) {
   _globalOutputPath = config.contains("output_path") ? config["output_path"].convert<std::string>() : "";

   _useIndexesForFolderName = false;
   if (config.contains("use_indexes_for_folder_name")) {
      _useIndexesForFolderName = config["use_indexes_for_folder_name"];
   }
   _forceVariableFolderName = true;
   if (config.contains("force_variable_folder_name")) {
      _forceVariableFolderName = config["force_variable_folder_name"];
   }
   _applyAreaAdjustment = false;
   if (config.contains("apply_area_adjustment")) {
      _applyAreaAdjustment = config["apply_area_adjustment"];
   }

   const auto& items = config["items"].extract<DynamicVector>();

   for (auto& item : items) {
      const auto& itemConfig = item.extract<const DynamicObject>();
      auto enabled = true;
      if (itemConfig.contains("enabled")) enabled = itemConfig["enabled"];
      if (enabled) {
         const auto variableDataType = itemConfig["variable_data_type"].convert<std::string>();

         if (variableDataType == "UInt8") {
            _dataVecT.emplace_back(std::make_unique<DataSettingsT<UInt8>>(_fileHandlingMutex, data_type::byte));
         } else if (variableDataType == "UInt16") {
            _dataVecT.emplace_back(std::make_unique<DataSettingsT<UInt16>>(_fileHandlingMutex, data_type::u_int16));
         } else if (variableDataType == "Int16") {
            _dataVecT.emplace_back(std::make_unique<DataSettingsT<Int16>>(_fileHandlingMutex, data_type::int16));
         } else if (variableDataType == "Int32") {
            _dataVecT.emplace_back(std::make_unique<DataSettingsT<Int32>>(_fileHandlingMutex, data_type::int32));
         } else if (variableDataType == "UInt32") {
            _dataVecT.emplace_back(std::make_unique<DataSettingsT<UInt32>>(_fileHandlingMutex, data_type::u_int32));
         } else if (variableDataType == "Int64") {
            _dataVecT.emplace_back(std::make_unique<DataSettingsT<Int32>>(_fileHandlingMutex, data_type::int32));
         } else if (variableDataType == "UInt64") {
            _dataVecT.emplace_back(std::make_unique<DataSettingsT<UInt32>>(_fileHandlingMutex, data_type::u_int32));
         } else if (variableDataType == "float") {
            _dataVecT.emplace_back(std::make_unique<DataSettingsT<float>>(_fileHandlingMutex, data_type::float32));
         } else if (variableDataType == "double") {
            _dataVecT.emplace_back(std::make_unique<DataSettingsT<double>>(_fileHandlingMutex, data_type::float64));
         } else {
            _dataVecT.emplace_back(std::make_unique<DataSettingsT<int>>(_fileHandlingMutex, data_type::int16));
         }

         _dataVecT.back()->configure(_globalOutputPath, _useIndexesForFolderName, _forceVariableFolderName,
                                     _applyAreaAdjustment, itemConfig);
      }
   }
}

// --------------------------------------------------------------------------------------------

void WriteVariableGeotiff::subscribe(NotificationCenter& notificationCenter) {
   notificationCenter.subscribe(signals::SystemInit, &WriteVariableGeotiff::onSystemInit, *this);
   notificationCenter.subscribe(signals::LocalDomainInit, &WriteVariableGeotiff::onLocalDomainInit, *this);
   notificationCenter.subscribe(signals::LocalDomainProcessingUnitInit,
                                &WriteVariableGeotiff::onLocalDomainProcessingUnitInit, *this);
   notificationCenter.subscribe(signals::OutputStep, &WriteVariableGeotiff::onOutputStep, *this);
   notificationCenter.subscribe(signals::LocalDomainProcessingUnitShutdown,
                                &WriteVariableGeotiff::onLocalDomainProcessingUnitShutdown, *this);
   notificationCenter.subscribe(signals::PreTimingSequence, &WriteVariableGeotiff::onPreTimingSequence, *this);
   notificationCenter.subscribe(signals::TimingInit, &WriteVariableGeotiff::onTimingInit, *this);
   notificationCenter.subscribe(signals::TimingShutdown, &WriteVariableGeotiff::onTimingShutdown, *this);
   notificationCenter.subscribe(signals::Error, &WriteVariableGeotiff::onError, *this);
}

// --------------------------------------------------------------------------------------------

void WriteVariableGeotiff::onSystemInit() {
   // build the filename using pools and variable values
   flint::VariableAndPoolStringBuilder databaseNameBuilder(_landUnitData.get(), _globalOutputPath);
   _globalOutputPath = databaseNameBuilder.result();

   for (auto& item : _dataVecT) {
      item->doSystemInit(_landUnitData.get());
   }
}

// --------------------------------------------------------------------------------------------

void WriteVariableGeotiff::onLocalDomainInit() {
   _spatialLocationInfo = std::static_pointer_cast<flint::SpatialLocationInfo>(
       _landUnitData->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<flint::IFlintData>>());

   for (auto& item : _dataVecT) {
      item->doLocalDomainInit(_landUnitData.get());
   }
}

// --------------------------------------------------------------------------------------------

void WriteVariableGeotiff::onLocalDomainProcessingUnitInit() {
   for (auto& item : _dataVecT) {
      item->doLocalDomainProcessingUnitInit(_spatialLocationInfo);
   }
}

// --------------------------------------------------------------------------------------------

void WriteVariableGeotiff::onLocalDomainProcessingUnitShutdown() {
   for (auto& item : _dataVecT) {
      item->doLocalDomainProcessingUnitShutdown(_spatialLocationInfo);
   }
}

// --------------------------------------------------------------------------------------------

void WriteVariableGeotiff::onPreTimingSequence() {
   for (auto& item : _dataVecT) {
      if (item->notificationType == DataSettingsB::OnNotificationType::TimingInit ||
          item->notificationType == DataSettingsB::OnNotificationType::TimingShutdown ||
          item->notificationType == DataSettingsB::OnNotificationType::Error) {
         item->initData(_spatialLocationInfo, 0);
      }
   }
}

// --------------------------------------------------------------------------------------------

void WriteVariableGeotiff::onTimingInit() {
   for (auto& item : _dataVecT) {
      if (item->notificationType == DataSettingsB::OnNotificationType::TimingInit)
         /// if we have timeStep here it's treated like a timeseries of data, which increase data volume massively
         // Best to force timestep to 0 for now
         item->setLUValue(_spatialLocationInfo, 0);  //  getTimestep());
   }
}

// --------------------------------------------------------------------------------------------

void WriteVariableGeotiff::onTimingShutdown() {
   for (auto& item : _dataVecT) {
      if (item->notificationType == DataSettingsB::OnNotificationType::TimingShutdown)
         /// if we have timeStep here it's treated like a timeseries of data, which increase data volume massively
         // Best to force timestep to 0 for now
         item->setLUValue(_spatialLocationInfo, 0);  //  getTimestep());
   }
}

// --------------------------------------------------------------------------------------------

void WriteVariableGeotiff::onOutputStep() {
   for (auto& item : _dataVecT) {
      if (item->notificationType == DataSettingsB::OnNotificationType::OutputStep) {
         const auto timing = _landUnitData->timing();
         if (!item->_outputAnnually || timing->curStartDate().month() == 12)
            item->setLUValue(_spatialLocationInfo, getTimestep());
      }
   }
}

// --------------------------------------------------------------------------------------------

void WriteVariableGeotiff::onError(std::string msg) {
   for (auto& item : _dataVecT) {
      if (item->notificationType == DataSettingsB::OnNotificationType::Error)
         /// if we have timeStep here it's treated like a timeseries of data, which increase data volume massively
         // Best to force timestep to 0 for now
         item->setLUValue(_spatialLocationInfo, 0);  //  getTimestep());
   }
}

// --------------------------------------------------------------------------------------------

int WriteVariableGeotiff::getTimestep() const {
   const auto timing = _landUnitData->timing();
   return timing->step();
}

// --------------------------------------------------------------------------------------------

template <typename T>
void WriteVariableGeotiff::DataSettingsT<T>::configure(std::string& globalOutputPath, bool useIndexesForFolderName,
                                                       bool forceVariableFolderName, bool applyAreaAdjustment,
                                                       const DynamicObject& config) {
   _name = config["data_name"].convert<std::string>();

   _useIndexesForFolderName = config.contains("use_indexes_for_folder_name")
                                  ? config["use_indexes_for_folder_name"].convert<bool>()
                                  : useIndexesForFolderName;
   _forceVariableFolderName = config.contains("force_variable_folder_name")
                                  ? config["force_variable_folder_name"].convert<bool>()
                                  : forceVariableFolderName;
   _applyAreaAdjustment =
       config.contains("apply_area_adjustment") ? config["apply_area_adjustment"].convert<bool>() : applyAreaAdjustment;
   _subtractPrevValue =
       config.contains("subtract_previous_value") ? config["subtract_previous_value"].convert<bool>() : false;
   _outputPath = config.contains("output_path") ? config["output_path"].convert<std::string>() : globalOutputPath;
   _propertyName = config.contains("property_name") ? config["property_name"].convert<std::string>() : "";
   _variableName = config.contains("variable_name") ? config["variable_name"].convert<std::string>() : "";
   _variableDataType = config["variable_data_type"].convert<std::string>();
   _onNotification =
       config.contains("on_notification") ? config["on_notification"].convert<std::string>() : "TimingInit";
   _isArray = config.contains("is_array") ? config["is_array"].convert<bool>() : false;
   _arrayIndex = config.contains("array_index") ? config["array_index"].convert<int>() : 0;
   _outputInterval = config.contains("output_interval") ? config["output_interval"].convert<int>() : 1;
   _nodataValue = config.contains("nodata_value")
                      ? config["nodata_value"].convert<T>()
                      : std::numeric_limits<T>::is_integer ? std::numeric_limits<T>::lowest()
                                                           : T(-pow(10.0, std::numeric_limits<T>::max_exponent10));

   if (config.contains("pool_name")) {
      if (config["pool_name"].isVector()) {
         for (const auto& s : config["pool_name"]) {
            _poolName.push_back(s);
         }
      } else {
         _poolName.push_back(config["pool_name"].convert<std::string>());
      }
   }

   if (config.contains("flux")) {
      if (config["flux"].isVector()) {
         for (const auto& fluxGroup : config["flux"]) {
            addFlux(fluxGroup);
         }
      } else {
         addFlux(config["flux"]);
      }
   }

   if (_onNotification == "TimingInit")
      notificationType = OnNotificationType::TimingInit;
   else if (_onNotification == "TimingShutdown")
      notificationType = OnNotificationType::TimingShutdown;
   else if (_onNotification == "Error")
      notificationType = OnNotificationType::Error;
   else if (_onNotification == "OutputStep") {
      _outputAnnually = config.contains("output_annual_only") ? config["output_annual_only"].convert<bool>() : false;
      notificationType = OnNotificationType::OutputStep;
   }
}

// --------------------------------------------------------------------------------------------

template <typename T>
void WriteVariableGeotiff::DataSettingsT<T>::addFlux(const DynamicVar& fluxConfig) {
   const auto& fluxGroup = fluxConfig.extract<const DynamicObject>();

   std::vector<std::string> sourcePoolNames;
   for (const auto& poolName : fluxGroup["from"]) {
      sourcePoolNames.push_back(poolName);
   }

   std::vector<std::string> destPoolNames;
   for (const auto& poolName : fluxGroup["to"]) {
      destPoolNames.push_back(poolName);
   }

   auto fluxSource = flint::FluxSource::COMBINED;
   if (fluxGroup.contains("flux_source")) {
      auto src = fluxGroup["flux_source"].convert<std::string>();
      fluxSource = boost::iequals(src, "disturbance")
                       ? flint::FluxSource::DISTURBANCE
                       : boost::iequals(src, "annual_process") ? flint::FluxSource::ANNUAL_PROCESS : fluxSource;
   }

   bool subtract = false;
   if (fluxGroup.contains("subtract")) {
      subtract = fluxGroup["subtract"];
   }

   _flux.push_back(flint::Flux(sourcePoolNames, destPoolNames, fluxSource, subtract));
}

// --------------------------------------------------------------------------------------------

template <typename T>
void WriteVariableGeotiff::DataSettingsT<T>::doSystemInit(flint::ILandUnitDataWrapper* _landUnitData) {
   flint::VariableAndPoolStringBuilder databaseNameBuilder(_landUnitData, _outputPath);
   _outputPath = databaseNameBuilder.result();

   fs::path spatialOutputFolderPath(_outputPath);
   if (_forceVariableFolderName) {
      spatialOutputFolderPath /= _name;
   }
   fs::create_directories(spatialOutputFolderPath);
}

// --------------------------------------------------------------------------------------------

template <typename T>
void WriteVariableGeotiff::DataSettingsT<T>::doLocalDomainInit(flint::ILandUnitDataWrapper* _landUnitData) {
   this->_landUnitData = _landUnitData;
   if (!_poolName.empty()) {
      for (std::string s : _poolName) {
         _pool.push_back(_landUnitData->getPool(s));
      }
   } else if (!_flux.empty()) {
      for (auto& flux : _flux) {
         flux.init(_landUnitData);
      }
   } else {
      _variable = _landUnitData->getVariable(_variableName);
   }
}

// --------------------------------------------------------------------------------------------

template <typename T>
void WriteVariableGeotiff::DataSettingsT<T>::doLocalDomainProcessingUnitInit(
    std::shared_ptr<const flint::SpatialLocationInfo> spatialLocationInfo) {
   fs::path workingFolder(_outputPath);

   if (_forceVariableFolderName) {
      workingFolder /= _name;
   }

   fs::path tileFolder;
   if (_useIndexesForFolderName) {
      tileFolder = workingFolder / fmt::format("{:06}", spatialLocationInfo->_tileIdx);
   } else {
      tileFolder = workingFolder / fmt::format("{}{:03}_{}{:03}", spatialLocationInfo->_tileLatLon.lon < 0 ? "-" : "",
                                               std::abs(spatialLocationInfo->_tileLatLon.lon),
                                               spatialLocationInfo->_tileLatLon.lat < 0 ? "-" : "",
                                               std::abs(spatialLocationInfo->_tileLatLon.lat));
   }

   std::unique_lock lock(_fileHandlingMutex);
   fs::create_directories(tileFolder);
}

// --------------------------------------------------------------------------------------------

template <typename T>
void WriteVariableGeotiff::DataSettingsT<T>::initializeData(
    std::shared_ptr<const flint::SpatialLocationInfo> spatialLocationInfo, std::vector<T>& data) {
   int numCells = spatialLocationInfo->_cellRows * spatialLocationInfo->_cellCols;
   data.resize(numCells, _nodataValue);
}

// --------------------------------------------------------------------------------------------
GDALDataType gdal_type(WriteVariableGeotiff::data_type type) {
   switch (type) {
      case WriteVariableGeotiff::data_type::int32:
         return GDALDataType::GDT_Int32;
      case WriteVariableGeotiff::data_type::unknown:
         return GDALDataType::GDT_Unknown;
      case WriteVariableGeotiff::data_type::byte:
         return GDALDataType::GDT_Byte;
      case WriteVariableGeotiff::data_type::u_int16:
         return GDALDataType::GDT_UInt16;
      case WriteVariableGeotiff::data_type::int16:
         return GDALDataType::GDT_Int16;
      case WriteVariableGeotiff::data_type::u_int32:
         return GDALDataType::GDT_UInt32;
      case WriteVariableGeotiff::data_type::float32:
         return GDALDataType::GDT_Float32;
      case WriteVariableGeotiff::data_type::float64:
         return GDALDataType::GDT_Float64;
      default:
         return GDALDataType::GDT_Unknown;
   }
}

struct dataset_closer {
   dataset_closer(GDALDataset* dataset) : gdal_dataset_(dataset) {}

   void operator()(GDALRasterBand* band) const {
      auto gdal_dataset = band ? band->GetDataset() : nullptr;
      assert(gdal_dataset == gdal_dataset_);
      if (gdal_dataset) {
         if (band && band->GetAccess() == GA_Update) {
            double min, max, mean, std_dev;
            auto try_statistics = band->GetStatistics(FALSE, FALSE, &min, &max, &mean, &std_dev);
            if (try_statistics != CE_Warning) {
               band->ComputeStatistics(FALSE, &min, &max, &mean, &std_dev, nullptr, nullptr);
               band->SetStatistics(min, max, mean, std_dev);
            }
         }
         GDALFlushCache(gdal_dataset);
         GDALClose(gdal_dataset);
      }
   }
   GDALDataset* gdal_dataset_;
};

static std::shared_ptr<GDALRasterBand> create_gdalraster(const fs::path& path, int rows, int cols,
                                                         GDALDataType datatype, double* transform) {
   if (GDALGetDriverCount() == 0) {
      GDALAllRegister();
   }

   GDALDriver* driver = GetGDALDriverManager()->GetDriverByName("GTiff");

   char** options = nullptr;
   options = CSLSetNameValue(options, "TILED", "YES");
   options = CSLSetNameValue(options, "COMPRESS", "DEFLATE");

   auto dataset = driver->Create(path.string().c_str(), cols, rows, 1, datatype, options);
   if (dataset == nullptr) {
      throw std::runtime_error("Could not create raster file: " + path.string());
   }
   dataset->SetGeoTransform(transform);
   OGRSpatialReference srs;
   srs.SetWellKnownGeogCS("EPSG:4326");
   char* srs_wkt = nullptr;
   srs.exportToWkt(&srs_wkt);
   dataset->SetProjection(srs_wkt);
   CPLFree(srs_wkt);
   auto band = dataset->GetRasterBand(1);
   return std::shared_ptr<GDALRasterBand>(band, dataset_closer(dataset));
}

template <typename T>
void WriteVariableGeotiff::DataSettingsT<T>::doLocalDomainProcessingUnitShutdown(
    std::shared_ptr<const flint::SpatialLocationInfo> spatialLocationInfo) {
   std::unique_lock lock(_fileHandlingMutex);

   fs::path tileFolder(_tileFolderPath);
   std::string folderLocStr;
   if (_useIndexesForFolderName) {
      folderLocStr = fmt::format("{:06}_{:02}", spatialLocationInfo->_tileIdx, spatialLocationInfo->_blockIdx);
   } else {
      folderLocStr = fmt::format("{}{:03}_{}{:03}_{:02}", spatialLocationInfo->_tileLatLon.lon < 0 ? "-" : "",
                                 std::abs(spatialLocationInfo->_tileLatLon.lon),
                                 spatialLocationInfo->_tileLatLon.lat < 0 ? "-" : "",
                                 std::abs(spatialLocationInfo->_tileLatLon.lat), spatialLocationInfo->_blockIdx);
   }

   int cellRows = spatialLocationInfo->_cellRows;
   int cellCols = spatialLocationInfo->_cellCols;
   double adfGeoTransform[6] = {spatialLocationInfo->_blockLatLon.lon,
                                1.0 / spatialLocationInfo->_blockCols / cellCols,
                                0,
                                spatialLocationInfo->_blockLatLon.lat,
                                0,
                                -1.0 / spatialLocationInfo->_blockRows / cellRows};

   typename std::unordered_map<int, std::vector<T>>::iterator itPrev;
   for (auto it = _data.begin(); it != _data.end(); ++it) {
      const auto& [step, data] = (*it);

      auto block_path = tileFolder /= fmt::format("{}_{}_{}.tif", _name, folderLocStr, step);
      if (fs::exists(block_path)) {
         fs::remove(block_path);  // delete existing file
      }

      auto band = create_gdalraster(block_path, cellRows, cellCols, gdal_type(_dataType), adfGeoTransform);
      auto err = band->SetNoDataValue(_nodataValue);
      if (err != CE_None) {
         std::ostringstream oss;
         oss << "Could not set raster file nodata: " << block_path.string() << " (" << _nodataValue << ")" << std::endl;
         throw std::runtime_error(oss.str());
      }

      if (_subtractPrevValue) {
         if (it == _data.begin()) {  // prev value is 0
            auto err = band->RasterIO(GF_Write, 0, 0, cellCols, cellRows, (void*)data.data(), cellCols,
                                      cellRows, gdal_type(_dataType), 0, 0);
            if (err != CE_None) {
               std::ostringstream oss;
               oss << "Could not write to raster file: " << block_path.string() << std::endl;
               throw std::runtime_error(oss.str());
            }
         } else {
            // I know i have a prevIt
            const auto& [step_prev, data_prev] = (*itPrev);
            std::vector<T> newData;

            auto it1 = data.begin();
            auto it2 = data_prev.begin();
            for (; it1 != data.end() && it2 != data_prev.end(); ++it1, ++it2) {
               newData.push_back(*it1 - *it2);
            }

            auto err = band->RasterIO(GF_Write, 0, 0, cellCols, cellRows, newData.data(), cellCols, cellRows,
                                      gdal_type(_dataType), 0, 0);
            if (err != CE_None) {
               std::ostringstream oss;
               oss << "Could not write to raster file: " << block_path.string() << std::endl;
               throw std::runtime_error(oss.str());
            }
         }
      } else {
         auto err = band->RasterIO(GF_Write, 0, 0, cellCols, cellRows, (void*)data.data(), cellCols,
                                   cellRows, gdal_type(_dataType), 0, 0);
         if (err != CE_None) {
            std::ostringstream oss;
            oss << "Could not write to raster file: " << block_path.string() << std::endl;
            throw std::runtime_error(oss.str());
         }
      }
      itPrev = it;
   }

   _data.clear();
}  // namespace gdal

// --------------------------------------------------------------------------------------------

template <typename T>
T WriteVariableGeotiff::DataSettingsT<T>::applyValueAdjustment(
    std::shared_ptr<const flint::SpatialLocationInfo> spatialLocationInfo, int timestep, const T val) {
   if (_applyAreaAdjustment) {
      return T(val * spatialLocationInfo->_landUnitArea);
   }
   return val;
}

// --------------------------------------------------------------------------------------------

template <typename T>
void WriteVariableGeotiff::DataSettingsT<T>::setLUValue(
    std::shared_ptr<const flint::SpatialLocationInfo> spatialLocationInfo, int timestep) {
    if ((timestep - 1) % _outputInterval != 0) {
        return;
    }

    initData(spatialLocationInfo, timestep);
    if (_variable != nullptr) {
        setLUVariableValue(spatialLocationInfo, timestep);
    } else if (!_pool.empty()) {
        setLUPoolValue(spatialLocationInfo, timestep);
    } else if (!_flux.empty()) {
        setLUFluxValue(spatialLocationInfo, timestep);
    }
}

// --------------------------------------------------------------------------------------------

template <typename T>
void WriteVariableGeotiff::DataSettingsT<T>::setLUVariableValue(
    std::shared_ptr<const flint::SpatialLocationInfo> spatialLocationInfo, int timestep) {
    if (_propertyName != "") {
        auto variableValue = _variable->value();
        if (variableValue.isStruct()) {
            const auto& structVal = variableValue.extract<DynamicObject>();
            _data[timestep][spatialLocationInfo->_cellIdx] =
                applyValueAdjustment(spatialLocationInfo, timestep, structVal[_propertyName].convert<T>());
        } else {
            auto flintDataVariable = _variable->value().extract<std::shared_ptr<flint::IFlintData>>();
            if (!_isArray) {
                auto variablePropertyValue = flintDataVariable->getProperty(_propertyName);
                _data[timestep][spatialLocationInfo->_cellIdx] =
                    applyValueAdjustment(spatialLocationInfo, timestep, variablePropertyValue.convert<T>());
            }
        }
    } else {
        auto variableValue = _variable->value();
        if (_isArray) {
            auto val = variableValue.extract<std::vector<boost::optional<T>>>()[_arrayIndex];
            if (val.is_initialized()) {
                _data[timestep][spatialLocationInfo->_cellIdx] =
                    applyValueAdjustment(spatialLocationInfo, timestep, val.value());
            }
        } else {
            if (!variableValue.isEmpty()) {
                _data[timestep][spatialLocationInfo->_cellIdx] =
                    applyValueAdjustment(spatialLocationInfo, timestep, variableValue.convert<T>());
            }
        }
    }
}

// --------------------------------------------------------------------------------------------

template <typename T>
void WriteVariableGeotiff::DataSettingsT<T>::setLUPoolValue(
    std::shared_ptr<const flint::SpatialLocationInfo> spatialLocationInfo, int timestep) {
   /// TODO: check this. Pool values are always doubles so shouldn't be defined by T
   double sum = 0.0;
   for (auto p : _pool) {
      sum += p->value();
   }
   _data[timestep][spatialLocationInfo->_cellIdx] = applyValueAdjustment(spatialLocationInfo, timestep, T(sum));
}

// --------------------------------------------------------------------------------------------

template <typename T>
void WriteVariableGeotiff::DataSettingsT<T>::setLUFluxValue(
    std::shared_ptr<const flint::SpatialLocationInfo> spatialLocationInfo, int timestep) {
   double sum = 0.0;
   for (auto operationResult : _landUnitData->getOperationLastAppliedIterator()) {
      for (auto& flux : _flux) {
         sum += flux.calculate(operationResult);
      }
   }
   _data[timestep][spatialLocationInfo->_cellIdx] = applyValueAdjustment(spatialLocationInfo, timestep, T(sum));
}

// --------------------------------------------------------------------------------------------

template <typename T>
void WriteVariableGeotiff::DataSettingsT<T>::initData(
    std::shared_ptr<const flint::SpatialLocationInfo> spatialLocationInfo, int timestep) {
   if (_data.find(timestep) == _data.end()) {
      _data[timestep] = std::vector<T>();
      initializeData(spatialLocationInfo, _data[timestep]);
   }
}

}  // namespace gdal
}  // namespace modules
}  // namespace moja
