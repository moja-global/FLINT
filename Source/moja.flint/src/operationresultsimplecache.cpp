#include "moja/flint/operationresultsimplecache.h"

#include "moja/flint/operationresultfluxiteratorsimplecache.h"
#include <moja/logging.h>

namespace moja {
namespace flint {

OperationResultSimpleCache::OperationResultSimpleCache(OperationTransferType transferType, ModuleMetaData* metaData)
	: _transferType(transferType), _metaData(metaData), _dataPackage(), _hasDataPackage(false) {
	_fluxes.reserve(40);
}

OperationResultSimpleCache::OperationResultSimpleCache(IOperation& operation)
	: _transferType(operation.transferType()), _metaData(operation.metaData()), _dataPackage(operation.dataPackage()), _hasDataPackage(operation.hasDataPackage()) {
	_fluxes.reserve(40);
}

OperationResultFluxCollection OperationResultSimpleCache::operationResultFluxCollection() {
	auto it = std::make_shared<OperationResultFluxIteratorSimpleCache>(_fluxes);
	return OperationResultFluxCollection(it);
}

OperationTransferType OperationResultSimpleCache::transferType() const {
	return _transferType;
}

void OperationResultSimpleCache::addFlux(int source, int sink, double value) {
	//MOJA_LOG_DEBUG << "SimpleCache addFlux - src: " << source << ", snk: " << sink << ", value:" << value;
	_fluxes.emplace_back(_transferType, _metaData, source, sink, value);
}

const ModuleMetaData* OperationResultSimpleCache::metaData() const {
	return _metaData;
}

const DynamicVar& OperationResultSimpleCache::dataPackage() const {
	return _dataPackage;
}

bool OperationResultSimpleCache::hasDataPackage() const {
	return _hasDataPackage;
}

}
} // moja::flint
