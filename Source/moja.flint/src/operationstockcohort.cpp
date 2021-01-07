#include "moja/flint/operationstockcohort.h"

#include "moja/flint/ipool.h"
#include "moja/flint/operationmanagercohort.h"
#include "moja/flint/operationresultcohort.h"

namespace moja::flint {

StockOperationCohort::StockOperationCohort(OperationManagerCohort* manager, double timeScale,
                                           const ModuleMetaData* metaData)
    : IOperation(metaData), manager_(manager), time_scale_(timeScale) {
   transfers_.reserve(20);
}

StockOperationCohort::StockOperationCohort(OperationManagerCohort* manager, double timeScale,
                                           const ModuleMetaData* metaData, DynamicVar& dataPackage)
    : IOperation(metaData, dataPackage), manager_(manager), time_scale_(timeScale) {
   transfers_.reserve(20);
}

StockOperationCohort* StockOperationCohort::addTransfer(const IPool* source, const IPool* sink, double value) {
   if (!manager_->allow_zero_result_transfers_ && FloatCmp::equalTo(value, 0.0)) return this;
   transfers_.emplace_back(transferType(), source->idx(), sink->idx(), value, _metaData);
   return this;
}

std::shared_ptr<IOperationResult> StockOperationCohort::computeOperation(ITiming& timing) {
   auto result = std::make_shared<OperationResultCohort>(*this);
   if (timing.isFullStep()) {
      for (const auto& transfer : transfers_) {
         const auto flux = transfer.value();
         result->addFlux(transfer.source(), transfer.sink(), flux);
      }
   } else {
      for (const auto& transfer : transfers_) {
         const auto flux = transfer.value() * time_scale_;
         result->addFlux(transfer.source(), transfer.sink(), flux);
      }
   }
   return std::static_pointer_cast<IOperationResult>(result);
}

OperationTransferType StockOperationCohort::transferType() const { return OperationTransferType::Stock; }

void StockOperationCohort::pooled_init(OperationManagerCohort* manager, double timeScale,
                                       const ModuleMetaData* metaData) {
   set_metaData(metaData);
   manager_ = manager;
   time_scale_ = timeScale;
   transfers_.clear();
}

void StockOperationCohort::submitOperation() { manager_->submitOperation(this); }

}  // namespace moja::flint
