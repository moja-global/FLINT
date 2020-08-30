#include "moja/flint/locationidxtransform.h"

#include "moja/flint/ilandunitcontroller.h"
#include "moja/flint/ivariable.h"

#include <moja/datarepository/datarepository.h>
#include <moja/datarepository/iproviderspatialrasterinterface.h>

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------

void LocationIdxTransform::configure(DynamicObject config, const ILandUnitController& landUnitController,
                                     datarepository::DataRepository& dataRepository) {
   _landUnitController = &landUnitController;
   auto provider = std::static_pointer_cast<moja::datarepository::IProviderSpatialRasterInterface>(
       dataRepository.getProvider(config["provider"].extract<std::string>()));
   _indexer = &provider->indexer();
   _layer = provider->getLayer(config["data_id"].extract<std::string>());

   _tileIdx = _landUnitController->getVariable("tileIndex");
   _blockIdx = _landUnitController->getVariable("blockIndex");
   _cellIdx = _landUnitController->getVariable("cellIndex");
}

void LocationIdxTransform::controllerChanged(const ILandUnitController& controller) {
   _landUnitController = &controller;
   _tileIdx = _landUnitController->getVariable("tileIndex");
   _blockIdx = _landUnitController->getVariable("blockIndex");
   _cellIdx = _landUnitController->getVariable("cellIndex");
}

const DynamicVar& LocationIdxTransform::value() const {
   const UInt32 tileIdx = _tileIdx->value();
   const UInt32 blockIdx = _blockIdx->value();
   const UInt32 cellIdx = _cellIdx->value();

   const datarepository::CellIdx cell(tileIdx, blockIdx, cellIdx, *_indexer);
   _cachedValue = nullptr;
   _cachedValue = _layer->getValueByCellIndex(cell);

   return _cachedValue;
}

}  // namespace flint
}  // namespace moja
