#include "moja/flint/operationstocksimple.h"

#include "moja/flint/ipool.h"
#include "moja/flint/operationmanagersimple.h"
#include "moja/flint/operationresultsimple.h"

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------
StockOperationSimple::StockOperationSimple(OperationManagerSimple* manager, double timeScale,
                                           const ModuleMetaData* metaData)
    : IOperation(metaData), _manager(manager), _timeScale(timeScale) {
   _transfers.reserve(20);
}

StockOperationSimple::StockOperationSimple(OperationManagerSimple* manager, double timeScale,
                                           const ModuleMetaData* metaData, DynamicVar& dataPackage)
    : IOperation(metaData, dataPackage), _manager(manager), _timeScale(timeScale) {
   _transfers.reserve(20);
}

// --------------------------------------------------------------------------------------------

StockOperationSimple* StockOperationSimple::addTransfer(const IPool* source, const IPool* sink, double value) {
   // MOJA_LOG_DEBUG << "Simple Transfer (stck) - src: " << source->idx() << ", snk: " << sink->idx() << ", value:" <<
   // value;

   if (!_manager->_allowZeroResultTransfers && FloatCmp::equalTo(value, 0.0)) return this;
   _transfers.emplace_back(transferType(), source->idx(), sink->idx(), value, _metaData);
   return this;
}

// --------------------------------------------------------------------------------------------

std::shared_ptr<IOperationResult> StockOperationSimple::computeOperation(ITiming& _timing) {
   // if (_transfers.size() == 0)
   //	return nullptr;

   auto result = std::make_shared<OperationResultSimple>(*this);

   if (_timing.isFullStep()) {
      for (auto& transfer : _transfers) {
         auto flux = transfer.value();
         result->addFlux(transfer.source(), transfer.sink(), flux);
      }
   } else {
      for (auto& transfer : _transfers) {
         auto flux = transfer.value() * _timeScale;
         result->addFlux(transfer.source(), transfer.sink(), flux);
      }
   }
   return result;
}

// --------------------------------------------------------------------------------------------

OperationTransferType StockOperationSimple::transferType() const { return OperationTransferType::Stock; }

// --------------------------------------------------------------------------------------------

void StockOperationSimple::submitOperation() { _manager->submitOperation(this); }

}  // namespace flint
}  // namespace moja
