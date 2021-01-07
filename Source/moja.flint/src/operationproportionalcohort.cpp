#include "moja/flint/operationproportionalcohort.h"

#include "moja/flint/ipool.h"
#include "moja/flint/operationmanagercohort.h"
#include "moja/flint/operationresultcohort.h"

namespace moja::flint {

ProportionalOperationCohort::ProportionalOperationCohort(OperationManagerCohort* manager,
                                                                     const std::vector<double>& pools, double timeScale,
                                                                     const ModuleMetaData* metaData)
    : IOperation(metaData), manager_(manager), time_scale_(timeScale), pools_(pools) {
   //_transfers.reserve(20);
}

ProportionalOperationCohort::ProportionalOperationCohort(OperationManagerCohort* manager,
                                                                     const std::vector<double>& pools, double timeScale,
                                                                     const ModuleMetaData* metaData,
                                                                     DynamicVar& dataPackage)
    : IOperation(metaData, dataPackage), manager_(manager), time_scale_(timeScale), pools_(pools) {
   //_transfers.reserve(20);
}

ProportionalOperationCohort* ProportionalOperationCohort::addTransfer(const IPool* source,
                                                                                  const IPool* sink, double value) {
   if (!manager_->allow_zero_result_transfers_ &&
       (FloatCmp::equalTo(value, 0.0) || FloatCmp::equalTo(source->value(), 0.0)))
      return this;
   transfers_.emplace_back(transferType(), source->idx(), sink->idx(), value, _metaData);
   return this;
}

std::shared_ptr<IOperationResult> ProportionalOperationCohort::computeOperation(ITiming& timing) {
   auto result = std::make_shared<OperationResultCohort>(*this);
   if (timing.isFullStep()) {
      for (auto& transfer : transfers_) {
         if (FloatCmp::equalTo(pools_[transfer.source()], 0.0)) continue;
         const auto flux = pools_[transfer.source()] * transfer.value();
         result->addFlux(transfer.source(), transfer.sink(), flux);
      }
   } else {
      for (auto& transfer : transfers_) {
         const auto flux = pools_[transfer.source()] * transfer.value() * time_scale_;
         result->addFlux(transfer.source(), transfer.sink(), flux);
      }
   }
   return std::static_pointer_cast<IOperationResult>(result);
}

OperationTransferType ProportionalOperationCohort::transferType() const {
   return OperationTransferType::Proportional;
}

void ProportionalOperationCohort::submitOperation() { manager_->submitOperation(this); }

}  // namespace moja::flint
