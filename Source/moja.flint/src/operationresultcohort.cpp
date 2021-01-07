#include "moja/flint/operationresultcohort.h"

#include "moja/flint/ioperation.h"
#include "moja/flint/operationresultfluxiteratorcohort.h"

namespace moja::flint {

OperationResultCohort::OperationResultCohort(IOperation& operation)
    : transfer_type_(operation.transferType()),
      meta_data_(operation.metaData()),
      data_package_(operation.dataPackage()),
      has_data_package_(operation.hasDataPackage()) {
      fluxes_.reserve(40);
}

OperationResultFluxCollection OperationResultCohort::operationResultFluxCollection() {
   const auto it = std::make_shared<OperationResultFluxIteratorCohort>(fluxes_);
   return OperationResultFluxCollection(it);
}

void OperationResultCohort::addFlux(int source, int sink, double value) {
   fluxes_.emplace_back(transfer_type_, meta_data_, source, sink, value);
}

}  // namespace moja::flint
