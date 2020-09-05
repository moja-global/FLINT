#include "moja/flint/locationfromflintdatatransform.h"

#include "moja/flint/ilandunitcontroller.h"
#include "moja/flint/ivariable.h"
#include "moja/flint/spatiallocationinfo.h"

#include <moja/datarepository/datarepository.h>
#include <moja/datarepository/iproviderspatialrasterinterface.h>

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------

void LocationFromFlintDataTransform::configure(DynamicObject config, const ILandUnitController& landUnitController,
                                               datarepository::DataRepository& dataRepository) {
   _landUnitController = &landUnitController;
   _dataRepository = &dataRepository;
   _provider = std::static_pointer_cast<datarepository::IProviderSpatialRasterInterface>(
       _dataRepository->getProvider(config["provider"].extract<std::string>()));
   _dataId = config["data_id"].extract<std::string>();
   _spatialLocationInfo = std::static_pointer_cast<SpatialLocationInfo>(
       landUnitController.getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<flint::IFlintData>>());
}

void LocationFromFlintDataTransform::controllerChanged(const ILandUnitController& landUnitController) {
   _landUnitController = &landUnitController;
   _spatialLocationInfo = std::static_pointer_cast<SpatialLocationInfo>(
       landUnitController.getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<flint::IFlintData>>());
}

const DynamicVar& LocationFromFlintDataTransform::value() const {
   _cachedValue = nullptr;
   _cachedValue =
       _provider->GetValue(_dataId, _spatialLocationInfo->_cellLatLon.lat, _spatialLocationInfo->_cellLatLon.lon);
   return _cachedValue;
}

}  // namespace flint
}  // namespace moja
