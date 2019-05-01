#include "moja/flint/locationidxfromflintdatatransform.h"
#include "moja/flint/ivariable.h"

#include "moja/datarepository/datarepository.h"
#include "moja/datarepository/iproviderspatialrasterinterface.h"
#include "moja/flint/ilandunitcontroller.h"
#include "moja/flint/spatiallocationinfo.h"

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------

void LocationIdxFromFlintDataTransform::configure(DynamicObject config, const ILandUnitController& landUnitController, datarepository::DataRepository& dataRepository) {
	_landUnitController = &landUnitController;
	auto provider = std::static_pointer_cast<moja::datarepository::IProviderSpatialRasterInterface>(dataRepository.getProvider(config["provider"].convert<std::string>()));
	_indexer = &provider->indexer();
	_layer = provider->getLayer(config["data_id"].convert<std::string>());
	_spatialLocationInfo = std::static_pointer_cast<SpatialLocationInfo>(landUnitController.getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<flint::IFlintData>>());
}

void LocationIdxFromFlintDataTransform::controllerChanged(const ILandUnitController& landUnitController) {
	_landUnitController = &landUnitController;
	_spatialLocationInfo = std::static_pointer_cast<SpatialLocationInfo>(landUnitController.getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<flint::IFlintData>>());
};

const DynamicVar& LocationIdxFromFlintDataTransform::value() const {
	datarepository::CellIdx cell(_spatialLocationInfo->_tileIdx, _spatialLocationInfo->_blockIdx, _spatialLocationInfo->_cellIdx, *_indexer);
	_cachedValue = nullptr;
	_cachedValue = _layer->getValueByCellIndex(cell);
	return _cachedValue;
}

}
} // namespace moja::flint
