#include "moja/flint/operationresultsimple.h"

#include "moja/flint/ioperation.h"
#include "moja/flint/operationresultfluxiteratorsimple.h"

namespace moja::flint {

OperationResultSimple::OperationResultSimple(IOperation& operation)
    : _transferType(operation.transferType()),
      _metaData(operation.metaData()),
      _dataPackage(operation.dataPackage()),
      _hasDataPackage(operation.hasDataPackage()) {
   _fluxes.reserve(40);
}

OperationResultFluxCollection OperationResultSimple::operationResultFluxCollection() {
   auto it = std::make_shared<OperationResultFluxIteratorSimple>(_fluxes);
   return OperationResultFluxCollection(it);
}

OperationTransferType OperationResultSimple::transferType() const { return _transferType; }

void OperationResultSimple::addFlux(int source, int sink, double value) {
   // MOJA_LOG_DEBUG << "Simple addFlux - src: " << source << ", snk: " << sink << ", value:" << value;
   _fluxes.emplace_back(_transferType, _metaData, source, sink, value);
}

const ModuleMetaData* OperationResultSimple::metaData() const { return _metaData; }

const DynamicVar& OperationResultSimple::dataPackage() const { return _dataPackage; }

bool OperationResultSimple::hasDataPackage() const { return _hasDataPackage; }

}  // namespace moja::flint
