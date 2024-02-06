#ifndef MOJA_MODULES_GDAL_WRITEVARIABLEMULTIBANDGEOTIFF_H_
#define MOJA_MODULES_GDAL_WRITEVARIABLEMULTIBANDGEOTIFF_H_

#include "moja/modules/gdal/_modules.gdal_exports.h"

#include <moja/flint/flux.h>
#include <moja/flint/ioperationresult.h>
#include <moja/flint/ipool.h>
#include <moja/flint/modulebase.h>

#include <unordered_map>
#include <Poco/Mutex.h>

namespace moja {

namespace flint {
class SpatialLocationInfo;
}  // namespace flint

namespace modules {
namespace gdal {

class GDAL_API WriteVariableMultibandGeotiff : public flint::ModuleBase {
  public:
   enum class data_type
   {
      unknown = 0,
      byte = 1,
      u_int16 = 2,
      int16 = 3,
      u_int32 = 4,
      int32 = 5,
      float32 = 6,
      float64 = 7,
   };

   explicit WriteVariableMultibandGeotiff(Poco::Mutex& fileHandlingMutex)
       : _fileHandlingMutex(fileHandlingMutex),
         _useIndexesForFolderName(false),
         _forceVariableFolderName(true),
         _applyAreaAdjustment(false),
         _deleteExisting(true) {}
   virtual ~WriteVariableMultibandGeotiff() = default;

   void configure(const DynamicObject& config) override;
   void subscribe(NotificationCenter& notificationCenter) override;

   // Notification handlers
   void onSystemInit() override;
   void onLocalDomainInit() override;
   void onLocalDomainProcessingUnitInit() override;
   void onLocalDomainProcessingUnitShutdown() override;
   void onPreTimingSequence() override;
   void onTimingInit() override;
   void onTimingShutdown() override;
   void onOutputStep() override;
   void onError(std::string msg) override;

   // --- Base class for data layer
   class DataSettingsB {
     public:
      DataSettingsB(Poco::Mutex& fileHandlingMutex, data_type dataType)
          : notificationType(OnNotificationType::TimingInit),
            _useIndexesForFolderName(false),
            _forceVariableFolderName(true),
            _applyAreaAdjustment(false),
            _subtractPrevValue(false),
            _isArray(false),
            _arrayIndex(0),
            _outputAnnually(false),
            _dataType(dataType),
            _variable(nullptr),
            _deleteExisting(true),
            _fileHandlingMutex(fileHandlingMutex) {}

      virtual ~DataSettingsB() = default;

      virtual void configure(std::string& globalOutputPath, bool useIndexesForFolderName, bool forceVariableFolderName,
                             bool applyAreaAdjustment, bool deleteExisting, const DynamicObject& config) = 0;
      virtual void doSystemInit(flint::ILandUnitDataWrapper* _landUnitData) = 0;
      virtual void doLocalDomainInit(flint::ILandUnitDataWrapper* _landUnitData) = 0;
      virtual void doLocalDomainProcessingUnitInit(
          std::shared_ptr<const flint::SpatialLocationInfo> spatialLocationInfo) = 0;
      virtual void doLocalDomainProcessingUnitShutdown(
          std::shared_ptr<const flint::SpatialLocationInfo> spatialLocationInfo) = 0;
      virtual void setLUValue(std::shared_ptr<const flint::SpatialLocationInfo> spatialLocationInfo, int timestep) = 0;

      virtual void initData(std::shared_ptr<const flint::SpatialLocationInfo> spatialLocationInfo, int timeStep) = 0;

      enum class OnNotificationType { TimingInit, OutputStep, TimingShutdown, Error };

      // Set by onNotification string: only
      OnNotificationType notificationType;

     protected:
      friend class WriteVariableMultibandGeotiff;

      // Config settings
      std::string _name;  // "data_name"
      bool _useIndexesForFolderName;
      bool _forceVariableFolderName;
      bool _applyAreaAdjustment;
      bool _subtractPrevValue;
      bool _deleteExisting;
      std::string _outputPath;             // "output_path"
      std::string _variableName;           // "variable_name"
      std::string _propertyName;           // "property_name"
      std::vector<std::string> _poolName;  // "pool_name"
      std::vector<flint::Flux> _flux;      // "flux" - flux groups to aggregate
      std::string _variableDataType;       // "variable_data_type"
      std::string _onNotification;  // when to capture the variable value (which notification method) - TimingInit
                                    // [default], TimingShutdown, Error
      bool _isArray;                //	"is_array"
      int _arrayIndex;              //	"array_index"
      bool _outputAnnually;         // Output last step of a year only
      int _outputInterval = 1;      // output every nth timestep (default: every timestep)

      data_type _dataType;

      // Other
      const flint::IVariable* _variable;
      std::vector<const flint::IPool*> _pool;
      std::string _tileFolderPath;
      Poco::Mutex& _fileHandlingMutex;
   };

   // --- Templated version of Base class for data layer types
   template <typename T>
   class DataSettingsT : public DataSettingsB {
     public:
      DataSettingsT(Poco::Mutex& fileHandlingMutex, data_type dataType)
          : DataSettingsB(fileHandlingMutex, dataType){};
      ~DataSettingsT() = default;

      void configure(std::string& globalOutputPath, bool useIndexesForFolderName, bool forceVariableFolderName,
                             bool applyAreaAdjustment, bool deleteExisting, const DynamicObject& config) override;

      void doSystemInit(flint::ILandUnitDataWrapper* _landUnitData) override;
      void doLocalDomainInit(flint::ILandUnitDataWrapper* _landUnitData) override;
      void doLocalDomainProcessingUnitInit(
          std::shared_ptr<const flint::SpatialLocationInfo> spatialLocationInfo) override;
      void doLocalDomainProcessingUnitShutdown(
          std::shared_ptr<const flint::SpatialLocationInfo> spatialLocationInfo) override;
      T applyValueAdjustment(std::shared_ptr<const flint::SpatialLocationInfo> spatialLocationInfo, int timestep,
                             const T val);
      void setLUValue(std::shared_ptr<const flint::SpatialLocationInfo> spatialLocationInfo, int timestep) override;
      void initData(std::shared_ptr<const flint::SpatialLocationInfo> spatialLocationInfo, int timeStep) override;

     private:
      // templated data
      T _nodataValue;                                 // "nodata_value"
      std::unordered_map<int, std::vector<T>> _data;  // array of data
      flint::ILandUnitDataWrapper* _landUnitData;

      void initializeData(std::shared_ptr<const flint::SpatialLocationInfo> spatialLocationInfo, std::vector<T>& data);
      void setLUVariableValue(std::shared_ptr<const flint::SpatialLocationInfo> spatialLocationInfo, int timestep);
      void setLUPoolValue(std::shared_ptr<const flint::SpatialLocationInfo> spatialLocationInfo, int timestep);
      void setLUFluxValue(std::shared_ptr<const flint::SpatialLocationInfo> spatialLocationInfo, int timestep);
      void addFlux(const DynamicVar& fluxGroupConfig);
   };

  private:
   // Mutexes
   Poco::Mutex& _fileHandlingMutex;

   // FlintData
   std::shared_ptr<const flint::SpatialLocationInfo> _spatialLocationInfo;

   // Other
   bool _useIndexesForFolderName;
   bool _forceVariableFolderName;
   bool _applyAreaAdjustment;
   bool _deleteExisting;
   std::string _globalOutputPath;                          // global "output_path"
   std::vector<std::unique_ptr<DataSettingsB>> _dataVecT;  // Spatial Output Data Vector

   int getTimestep() const;
};

}  // namespace gdal
}  // namespace modules
}  // namespace moja

#endif  // MOJA_MODULES_GDAL_WRITEVARIABLEMULTIBANDGEOTIFF_H_