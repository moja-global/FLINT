#include "moja/flint/operationproportionalsimplecache.h"

#include "moja/flint/operationmanagersimplecache.h"
#include "moja/flint/operationresultsimplecache.h"
#include "moja/flint/ipool.h"

#include "moja/logging.h"

#include <boost/range/adaptor/transformed.hpp>
#include <boost/format.hpp>

using boost::adaptors::transformed;

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------

ProportionalOperationSimpleCache::ProportionalOperationSimpleCache(OperationManagerSimpleCache* manager, std::vector<double>& pools, double timeScale, const ModuleMetaData* metaData)
	: IOperation(metaData), _manager(manager), _timeScale(timeScale), _pools(pools) {
	_transfers.reserve(20);
}

ProportionalOperationSimpleCache::ProportionalOperationSimpleCache(OperationManagerSimpleCache* manager, std::vector<double>& pools, double timeScale, const ModuleMetaData* metaData, DynamicVar& dataPackage)
	: IOperation(metaData, dataPackage), _manager(manager), _timeScale(timeScale), _pools(pools) {
	_transfers.reserve(20);
}

// --------------------------------------------------------------------------------------------

ProportionalOperationSimpleCache* ProportionalOperationSimpleCache::addTransfer(const IPool* source, const IPool* sink, double value) {
	//MOJA_LOG_DEBUG << "SimpleCache Transfer (prop) - src: " << source->idx() << ", snk: " << sink->idx() << ", value:" << value;

	if (!_manager->_allowZeroResultTransfers && (FloatCmp::equalTo(value, 0.0) || FloatCmp::equalTo(source->value(), 0.0)))
		return this;
	_transfers.emplace_back(transferType(), source->idx(), sink->idx(), value, _metaData);
	return this;
}

// --------------------------------------------------------------------------------------------

std::shared_ptr<IOperationResult> ProportionalOperationSimpleCache::computeOperation(ITiming& _timing) {
	//if (_transfers.size() == 0)
	//	return nullptr;

	//auto result = std::make_shared<OperationResultSimpleCache>(*this);
	if (_manager->_operationResultCachePosition == _manager->_operationResultCache.end())
		_manager->_operationResultCachePosition = _manager->_operationResultCache.begin();
	auto result = std::static_pointer_cast<OperationResultSimpleCache>(*(_manager->_operationResultCachePosition));
	++_manager->_operationResultCachePosition;
	result->_fluxes.clear();	// called because we are reusing them
	result->_transferType = OperationTransferType::Proportional;
	result->_metaData = _metaData;

	//MOJA_LOG_DEBUG << "SimpleCache computeOperation (prop) - pools: " << boost::join(_pools | transformed(static_cast<std::string(*)(double)>(std::to_string)), ", ");

	if (_timing.isFullStep()) {
		for (auto& transfer : _transfers) {
			if (FloatCmp::equalTo(_pools[transfer.source()], 0.0))
				continue;

			auto flux = _pools[transfer.source()] * transfer.value();
			result->addFlux(transfer.source(), transfer.sink(), flux);
		}
	}
	else {
		for (auto& transfer : _transfers) {
			auto flux = _pools[transfer.source()] * transfer.value() * _timeScale;
			result->addFlux(transfer.source(), transfer.sink(), flux);
		}
	}
	return result;
}

// --------------------------------------------------------------------------------------------

OperationTransferType ProportionalOperationSimpleCache::transferType() const {
	return OperationTransferType::Proportional;
}

// --------------------------------------------------------------------------------------------
void ProportionalOperationSimpleCache::submitOperation() {
	_manager->submitOperation(this);
}

}
} // moja::flint

