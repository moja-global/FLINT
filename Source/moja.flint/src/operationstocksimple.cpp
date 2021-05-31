#include "moja/flint/operationstocksimple.h"

#include "moja/flint/ipool.h"
#include "moja/flint/operationmanagersimple.h"
#include "moja/flint/operationresultsimple.h"

#include <fmt/format.h>

namespace moja::flint {

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

StockOperationSimple* StockOperationSimple::addTransfer(const IPool* source, const IPool* sink, double value) {
   // MOJA_LOG_DEBUG << "Simple Transfer (stck) - src: " << source->idx() << ", snk: " << sink->idx() << ", value:" <<
   // value;

   if (!source->children().empty()) {
      throw std::invalid_argument(fmt::format("source pool[{}] can not have children", source->name()));
   }

   if (!sink->children().empty()) {
      throw std::invalid_argument(fmt::format("sink pool[{}] can not have children", sink->name()));
   }

   if (!_manager->_allowZeroResultTransfers && FloatCmp::equalTo(value, 0.0)) return this;
   _transfers.emplace_back(transferType(), source->idx(), sink->idx(), value, _metaData);
   return this;
}

std::shared_ptr<IOperationResult> StockOperationSimple::computeOperation(ITiming& timing) {
   auto result = std::make_shared<OperationResultSimple>(*this);

   if (timing.isFullStep()) {
      for (auto& transfer : _transfers) {
         const auto flux = transfer.value();
         result->addFlux(transfer.source(), transfer.sink(), flux);
      }
   } else {
      for (auto& transfer : _transfers) {
         const auto flux = transfer.value() * _timeScale;
         result->addFlux(transfer.source(), transfer.sink(), flux);
      }
   }
   return result;
}

OperationTransferType StockOperationSimple::transferType() const { return OperationTransferType::Stock; }

void StockOperationSimple::submitOperation() { _manager->submitOperation(this); }

}  // namespace moja::flint
