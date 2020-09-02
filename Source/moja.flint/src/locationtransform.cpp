#include "moja/flint/locationtransform.h"

#include "moja/flint/ilandunitcontroller.h"
#include "moja/flint/ivariable.h"

#include <moja/datarepository/datarepository.h>
#include <moja/datarepository/iproviderspatialrasterinterface.h>

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------

void LocationTransform::configure(DynamicObject config, const ILandUnitController& landUnitController,
                                  datarepository::DataRepository& dataRepository) {
   _landUnitController = &landUnitController;
   _dataRepository = &dataRepository;
   _provider = std::static_pointer_cast<moja::datarepository::IProviderSpatialRasterInterface>(
       _dataRepository->getProvider(config["provider"].extract<std::string>()));

   _dataId = config["data_id"].extract<std::string>();
   _latitude = _landUnitController->getVariable("latitude");
   _longitude = _landUnitController->getVariable("longitude");
}

void LocationTransform::controllerChanged(const ILandUnitController& controller) {
   _landUnitController = &controller;
   _latitude = _landUnitController->getVariable("latitude");
   _longitude = _landUnitController->getVariable("longitude");
};

const moja::DynamicVar& LocationTransform::value() const {
   double latitude = _latitude->value();
   double longitude = _longitude->value();

   _cachedValue = nullptr;
   _cachedValue = _provider->GetValue(_dataId, latitude, longitude);
   return _cachedValue;
}

}  // namespace flint
}  // namespace moja
