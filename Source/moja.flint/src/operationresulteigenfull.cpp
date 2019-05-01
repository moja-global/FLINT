#include "moja/flint/operationresulteigenfull.h"

#include "moja/flint/operationresultfluxiteratoreigenfull.h"
#include "moja/flint/ioperation.h"

namespace moja {
namespace flint {

OperationResultEigenFull::OperationResultEigenFull(IOperation& operation, int poolCount)
	: _transferType(operation.transferType()), _metaData(operation.metaData()), _fluxes(poolCount, poolCount), _dataPackage(operation.dataPackage()), _hasDataPackage(operation.hasDataPackage()) { _fluxes.setZero(); }

OperationResultEigenFull::OperationResultEigenFull(IOperation& operation, Int64 poolCount)
	: _transferType(operation.transferType()), _metaData(operation.metaData()), _fluxes(poolCount, poolCount), _dataPackage(operation.dataPackage()), _hasDataPackage(operation.hasDataPackage()) { _fluxes.setZero(); }

OperationResultFluxCollection OperationResultEigenFull::operationResultFluxCollection() {
	auto it = std::make_shared<OperationResultFluxIteratorEigenFull>(_transferType, _metaData, _fluxes);
	return OperationResultFluxCollection(it);
}

OperationTransferType OperationResultEigenFull::transferType() const {
	return _transferType;
}

const ModuleMetaData* OperationResultEigenFull::metaData() const {
	return _metaData;
}

const DynamicVar& OperationResultEigenFull::dataPackage() const {
	return _dataPackage;
}

bool OperationResultEigenFull::hasDataPackage() const {
	return _hasDataPackage;
}

}
} // moja::flint
