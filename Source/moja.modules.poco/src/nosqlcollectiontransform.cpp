#include "moja/modules/poco/nosqlcollectiontransform.h"

#include "moja/modules/poco/providernosqlpocomongodb.h"

#include <moja/flint/flintexceptions.h>
#include <moja/flint/ilandunitcontroller.h>
#include <moja/flint/ivariable.h>

#include <Poco/JSON/Parser.h>

using moja::datarepository::IProviderNoSQLInterface;

namespace moja {
namespace modules {
namespace poco {

NoSQLCollectionTransform::NoSQLCollectionTransform()
    : _landUnitController(nullptr), _dataRepository(nullptr), _keyVariable(nullptr) {}

void NoSQLCollectionTransform::configure(DynamicObject config, const flint::ILandUnitController& landUnitController,
                                         moja::datarepository::DataRepository& dataRepository) {
   _config = config;
   _keyName = config["key_name"].convert<std::string>();
   _keyValueVariable = config["key_value_name"].convert<std::string>();
   _landUnitController = &landUnitController;
   _dataRepository = &dataRepository;

   _keyVariable = _landUnitController->getVariable(_keyValueVariable);

   auto mongodbSettings = _config["mogo_settings"].extract<const DynamicObject>();
   _provider = std::make_shared<ProviderNoSQLPocoMongoDB>(mongodbSettings);
   auto arr = _provider->GetDataSet("{}");
   for (auto& item : arr) {
      int keyValue = item[_keyName];
      _collection[keyValue] = item;
   }
   _dataSet = DynamicObject();
}

void NoSQLCollectionTransform::controllerChanged(const flint::ILandUnitController& controller) {
   configure(_config, controller, *_dataRepository);
};

const DynamicVar& NoSQLCollectionTransform::value() const {
   int lookupVal = _keyVariable->value();
   auto it = _collection.find(lookupVal);
   if (it != _collection.end()) {
      return it->second;
   }
   return _dataSet;
}

}  // namespace poco
}  // namespace modules
}  // namespace moja
