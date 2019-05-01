#include "moja/flint/operationresultublas.h"

#include "moja/flint/operationresultfluxiteratorublas.h"

namespace moja {
namespace flint {

OperationResultUblas::OperationResultUblas(IOperation& operation)
	: _transferType(operation.transferType()), _metaData(operation.metaData()), _dataPackage(operation.dataPackage()), _hasDataPackage(operation.hasDataPackage()) {}

OperationResultFluxCollection OperationResultUblas::operationResultFluxCollection() {
	auto it = std::make_shared<OperationResultFluxIteratorUblas>(_transferType, _metaData, _fluxes);
	return OperationResultFluxCollection(it);
}

OperationTransferType OperationResultUblas::transferType() const {
	return _transferType;
}

const ModuleMetaData* OperationResultUblas::metaData() const {
	return _metaData;
}

const DynamicVar& OperationResultUblas::dataPackage() const {
	return _dataPackage;
}

bool OperationResultUblas::hasDataPackage() const {
	return _hasDataPackage;
}

}
} // moja::flint
