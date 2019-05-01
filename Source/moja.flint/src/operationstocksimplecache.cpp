#include "moja/flint/operationstocksimplecache.h"

#include "moja/flint/operationmanagersimplecache.h"
#include "moja/flint/operationresultsimplecache.h"
#include "moja/flint/ipool.h"

#include <moja/logging.h>

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------
StockOperationSimpleCache::StockOperationSimpleCache(OperationManagerSimpleCache* manager, std::vector<double>& pools, double timeScale, const ModuleMetaData* metaData)
	: IOperation(metaData), _manager(manager), _timeScale(timeScale), _pools(pools) {
	_transfers.reserve(20);
}

StockOperationSimpleCache::StockOperationSimpleCache(OperationManagerSimpleCache* manager, std::vector<double>& pools, double timeScale, const ModuleMetaData* metaData, DynamicVar& dataPackage)
	: IOperation(metaData, dataPackage), _manager(manager), _timeScale(timeScale), _pools(pools) {
	_transfers.reserve(20);
}

// --------------------------------------------------------------------------------------------

StockOperationSimpleCache* StockOperationSimpleCache::addTransfer(const IPool* source, const IPool* sink, double value) {
	//MOJA_LOG_DEBUG << "SimpleCache Transfer (stck) - src: " << source->idx() << ", snk: " << sink->idx() << ", value:" << value;

	if (!_manager->_allowZeroResultTransfers && FloatCmp::equalTo(value, 0.0))
		return this;
	_transfers.emplace_back(transferType(), source->idx(), sink->idx(), value, _metaData);
	return this;
}

// --------------------------------------------------------------------------------------------

std::shared_ptr<IOperationResult> StockOperationSimpleCache::computeOperation(ITiming& _timing) {
	if (_manager->_operationResultCachePosition == _manager->_operationResultCache.end())
		_manager->_operationResultCachePosition = _manager->_operationResultCache.begin();
	auto result = std::static_pointer_cast<OperationResultSimpleCache>(*(_manager->_operationResultCachePosition));
	++_manager->_operationResultCachePosition;
	result->_fluxes.clear();	// called because we are reusing them
	result->_transferType = OperationTransferType::Stock;
	result->_metaData = _metaData;

	if (_timing.isFullStep()) {
		for (auto& transfer : _transfers) {
			auto flux = transfer.value();
			result->addFlux(transfer.source(), transfer.sink(), flux);
		}
	}
	else {
		for (auto& transfer : _transfers) {
			auto flux = transfer.value() * _timeScale;
			result->addFlux(transfer.source(), transfer.sink(), flux);
		}
	}
	return result;
}

// --------------------------------------------------------------------------------------------

OperationTransferType StockOperationSimpleCache::transferType() const {
	return OperationTransferType::Stock;
}

// --------------------------------------------------------------------------------------------

void StockOperationSimpleCache::submitOperation() {
	_manager->submitOperation(this);
}

}
} // moja::flint

