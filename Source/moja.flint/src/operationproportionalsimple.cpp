#include "moja/flint/operationproportionalsimple.h"

#include "moja/flint/ipool.h"
#include "moja/flint/operationmanagersimple.h"
#include "moja/flint/operationresultsimple.h"

#include <moja/logging.h>

#include <fmt/format.h>

namespace moja::flint {

ProportionalOperationSimple::ProportionalOperationSimple(OperationManagerSimple* manager,
                                                         const std::vector<double>& pools, double timeScale,
                                                         const ModuleMetaData* metaData)
    : IOperation(metaData), _manager(manager), _timeScale(timeScale), _pools(pools) {
   _transfers.reserve(20);
}

ProportionalOperationSimple::ProportionalOperationSimple(OperationManagerSimple* manager,
                                                         const std::vector<double>& pools, double timeScale,
                                                         const ModuleMetaData* metaData, DynamicVar& dataPackage)
    : IOperation(metaData, dataPackage), _manager(manager), _timeScale(timeScale), _pools(pools) {
   _transfers.reserve(20);
}

ProportionalOperationSimple* ProportionalOperationSimple::addTransfer(const IPool* source, const IPool* sink,
                                                                      double value) {
   // MOJA_LOG_DEBUG << "Simple Transfer (prop) - src: " << source->idx() << ", snk: " << sink->idx() << ", value:" <<
   // value;

   if (!source->children().empty()) {
      throw std::invalid_argument(fmt::format("source pool[{}] can not have children", source->name()));
   }

   if (!sink->children().empty()) {
      throw std::invalid_argument(fmt::format("sink pool[{}] can not have children", sink->name()));
   }

   if (!_manager->_allowZeroResultTransfers &&
       (FloatCmp::equalTo(value, 0.0) || FloatCmp::equalTo(source->value(), 0.0)))
      return this;
   _transfers.emplace_back(transferType(), source->idx(), sink->idx(), value, _metaData);
   return this;
}

std::shared_ptr<IOperationResult> ProportionalOperationSimple::computeOperation(ITiming& timing) {
   auto result = std::make_shared<OperationResultSimple>(*this);

   // MOJA_LOG_DEBUG << "Simple computeOperation (prop) - pools: " << boost::join(_pools |
   // boost::adaptors::transformed(static_cast<std::string(*)(double)>(std::to_string)), ", ");

   if (timing.isFullStep()) {
      for (auto& transfer : _transfers) {
         if (FloatCmp::equalTo(_pools[transfer.source()], 0.0)) continue;
         const auto flux = _pools[transfer.source()] * transfer.value();
         result->addFlux(transfer.source(), transfer.sink(), flux);
      }
   } else {
      for (auto& transfer : _transfers) {
         const auto flux = _pools[transfer.source()] * transfer.value() * _timeScale;
         result->addFlux(transfer.source(), transfer.sink(), flux);
      }
   }
   return result;
}

OperationTransferType ProportionalOperationSimple::transferType() const { return OperationTransferType::Proportional; }

void ProportionalOperationSimple::submitOperation() { _manager->submitOperation(this); }

}  // namespace moja::flint
