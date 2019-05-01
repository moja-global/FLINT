#ifndef MOJA_FLINT_WRITESPATIALDATAGRID_H_
#define MOJA_FLINT_WRITESPATIALDATAGRID_H_

#include "moja/flint/modulebase.h"
#include "moja/flint/ioperationresult.h"
#include "moja/flint/ioperationresultflux.h"
#include "moja/flint/ipool.h"
#include "moja/flint/flux.h"

#include <boost/algorithm/string.hpp>

#include <algorithm>
#include <unordered_map>

namespace moja {
namespace flint {

class SpatialLocationInfo;

class FLINT_API WriteVariableGrid : public ModuleBase {
public:
	explicit WriteVariableGrid(Poco::Mutex& fileHandlingMutex) : ModuleBase(), _fileHandlingMutex(fileHandlingMutex), _useIndexesForFolderName(false), _forceVariableFolderName(true), _applyAreaAdjustment(false){}
	virtual ~WriteVariableGrid() = default;

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

	// --- RAII class for file handle
	class FileHandle {
		typedef FILE *ptr;
	public:
		explicit FileHandle(std::string const& name, std::string const& mode = std::string("r")) :
			_wrapped_file(fopen(name.c_str(), mode.c_str())) {}
		~FileHandle() { if (_wrapped_file) fclose(_wrapped_file); }
		operator ptr() const { return _wrapped_file; }
	private:
		ptr _wrapped_file;
	};

	// --- Base classs for data layer
	class DataSettingsB {
	public:
        DataSettingsB(Poco::Mutex& fileHandlingMutex, int hdrDataType) :
			notificationType(OnNotificationType::TimingInit),
			_useIndexesForFolderName(false),
			_forceVariableFolderName(true),
			_applyAreaAdjustment(false),
			_subtractPrevValue(false),
			_isArray(false),
			_arrayIndex(0), 
			_outputAnnually(false),
			_hdrDataType(hdrDataType),
			_variable(nullptr),
            _pool(),
			_fileHandlingMutex(fileHandlingMutex) {}

		virtual ~DataSettingsB() = default;

		virtual void configure(std::string& globalOutputPath, bool useIndexesForFolderName, bool forceVariableFolderName, bool applyAreaAdjustment, const DynamicObject& config) = 0;
		virtual void doSystemInit(ILandUnitDataWrapper* _landUnitData) = 0;
		virtual void doLocalDomainInit(ILandUnitDataWrapper* _landUnitData) = 0;
		virtual void doLocalDomainProcessingUnitInit(std::shared_ptr<const SpatialLocationInfo> spatialLocationInfo) = 0;
		virtual void doLocalDomainProcessingUnitShutdown(std::shared_ptr<const SpatialLocationInfo> spatialLocationInfo) = 0;
		virtual void setLUValue(std::shared_ptr<const SpatialLocationInfo> spatialLocationInfo, int timestep) = 0;

		virtual void initData(std::shared_ptr<const SpatialLocationInfo> spatialLocationInfo, int timeStep) = 0;

		enum class OnNotificationType {
			TimingInit,
            OutputStep,
			TimingShutdown,
			Error
		};

		// Set by onNotification string: only 
		OnNotificationType notificationType;

	protected:
		friend class WriteVariableGrid;

		// Config settings
		std::string _name;				// "data_name"
		bool		_useIndexesForFolderName;
		bool		_forceVariableFolderName;
		bool		_applyAreaAdjustment;
		bool		_subtractPrevValue;
		std::string _outputPath;		// "output_path"
		std::string _variableName;		// "variable_name"
        std::string _propertyName;		// "property_name"
        std::vector<std::string> _poolName;   // "pool_name"
        std::vector<Flux> _flux;              // "flux" - flux groups to aggregate
        std::string _variableDataType;	// "variable_data_type"
		std::string _onNotification;	// when to capture the variable value (which notification method) - TimingInit [default], TimingShutdown, Error
		bool		_isArray;			//	"is_array"
		int			_arrayIndex;		//	"array_index"
		bool		_outputAnnually;			// Output last step of a year only
        int         _outputInterval = 1;    // output every nth timestep (default: every timestep)

		int _hdrDataType;				// ENVI Data type (from: https://opticks.org/docs/help/4.9.1/Help/Opticks/Content/Importers_Exporters/ENVI_Header_Format.htm)

		// Other
		const flint::IVariable* _variable;
        std::vector<const flint::IPool*> _pool;
        std::string _tileFolderPath;
        Poco::Mutex& _fileHandlingMutex;
	};

	// --- Templated version of Base classs for data layer types
	template <typename T>
	class DataSettingsT : public DataSettingsB {
	public:
        DataSettingsT(Poco::Mutex& fileHandlingMutex, int hdrDataType) : DataSettingsB(fileHandlingMutex, hdrDataType) {};
		~DataSettingsT() = default;

		virtual void configure(std::string& globalOutputPath, bool useIndexesForFolderName, bool forceVariableFolderName, bool applyAreaAdjustment, const DynamicObject& config) override;

		void doSystemInit(flint::ILandUnitDataWrapper* _landUnitData) override;
		void doLocalDomainInit(flint::ILandUnitDataWrapper* _landUnitData) override;
		void doLocalDomainProcessingUnitInit(std::shared_ptr<const SpatialLocationInfo> spatialLocationInfo) override;
		void doLocalDomainProcessingUnitShutdown(std::shared_ptr<const SpatialLocationInfo> spatialLocationInfo) override;
		T applyValueAdjustment(std::shared_ptr<const SpatialLocationInfo> spatialLocationInfo, int timestep, const T val);
		void setLUValue(std::shared_ptr<const SpatialLocationInfo> spatialLocationInfo, int timestep) override;
		void initData(std::shared_ptr<const SpatialLocationInfo> spatialLocationInfo, int timeStep) override;
	private:
		// templated data
		T _nodataValue;			// "nodata_value"
		std::unordered_map<int, std::vector<T>> _data;	// array of data
        ILandUnitDataWrapper* _landUnitData;

        void initializeData(std::shared_ptr<const SpatialLocationInfo> spatialLocationInfo, std::vector<T>& data);
        void setLUVariableValue(std::shared_ptr<const SpatialLocationInfo> spatialLocationInfo, int timestep);
        void setLUPoolValue(std::shared_ptr<const SpatialLocationInfo> spatialLocationInfo, int timestep);
        void setLUFluxValue(std::shared_ptr<const SpatialLocationInfo> spatialLocationInfo, int timestep);
        void addFlux(const DynamicVar& fluxGroupConfig);
    };

private:
	// Mutexes
    Poco::Mutex& _fileHandlingMutex;

	// FlintData
	std::shared_ptr<const SpatialLocationInfo> _spatialLocationInfo;

	// Other
	bool _useIndexesForFolderName;
	bool _forceVariableFolderName;
	bool _applyAreaAdjustment;
	std::string _globalOutputPath;			// global "output_path"
	std::vector<std::unique_ptr<DataSettingsB>> _dataVecT;	// Spatial Output Data Vector

    int getTimestep() const;
};

}
} // namespace moja::flint

#endif // MOJA_FLINT_WRITESPATIALDATAGRID_H_