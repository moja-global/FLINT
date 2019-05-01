#include "moja/flint/operationmanagerublas.h"

#include "moja/flint/operationstockublas.h"
#include "moja/flint/operationproportionalublas.h"
#include "moja/flint/operationresultcollection.h"
#include "moja/flint/poolublas.h"
#include "moja/flint/flintexceptions.h"
#include "moja/flint/timing.h"
#include "moja/flint/imodule.h"
#include "moja/flint/ioperationresult.h"
#include "moja/flint/ioperationresultflux.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace moja {
namespace flint {

OperationManagerUblas::OperationManagerUblas(Timing& timing, const DynamicObject& config) : _timing(timing), _poolCount(0) {
	_poolValues = moja_ublas_vector(100);

	_useKahan = false;
	_allowZeroResultTransfers = false;

	if (config.find("use_kahan") != config.end()) {
		_useKahan = config["use_kahan"];
		if (_useKahan)
			_corrections.reserve(100);
	}
	if (config.find("allow_zero_result_transfers") != config.end()) {
		_allowZeroResultTransfers = config["allow_zero_result_transfers"];
	}
}

std::shared_ptr<IOperation> OperationManagerUblas::createStockOperation(IModule& module) {
	return std::make_shared<StockOperationUblas>(this, _poolValues, _timing.fractionOfStep(), /*_I, */&module.metaData());
}

std::shared_ptr<IOperation> OperationManagerUblas::createStockOperation(IModule& module, DynamicVar& dataPackage) {
	return std::make_shared<StockOperationUblas>(this, _poolValues, _timing.fractionOfStep(), /*_I, */&module.metaData(), dataPackage);
}

std::shared_ptr<IOperation> OperationManagerUblas::createProportionalOperation(IModule& module) {
	return std::make_shared<ProportionalOperationUblas>(this, _poolValues, _timing.fractionOfStep(), /*_I, */&module.metaData());
}

std::shared_ptr<IOperation> OperationManagerUblas::createProportionalOperation(IModule& module, DynamicVar& dataPackage) {
	return std::make_shared<ProportionalOperationUblas>(this, _poolValues, _timing.fractionOfStep(), /*_I, */&module.metaData(), dataPackage);
}

void OperationManagerUblas::applyOperations() {
	if (_useKahan) { // used for https://en.wikipedia.org/wiki/Kahan_summation_algorithm
		double y;
		double t;

		for (auto& operationResult : _operationResultsPending) {
			auto fluxes = operationResult->operationResultFluxCollection();
			for (auto flux : fluxes) {
				auto srcIx = flux->source();
				auto dstIx = flux->sink();
				if (srcIx == dstIx) {
					// don't process transfers to same pool
					continue;
				}
				auto val = flux->value();

				// Do source
				y = -val - _corrections[srcIx];
				t = _poolValues[srcIx] + y;
				_corrections[srcIx] = (t - _poolValues[srcIx]) - y;
				_poolValues[srcIx] = t;

				// Do sink
				y = val - _corrections[dstIx];
				t = _poolValues[dstIx] + y;
				_corrections[dstIx] = (t - _poolValues[dstIx]) - y;
				_poolValues[dstIx] = t;
			}
		}
	} 
	else {
		for (auto& operationResult : _operationResultsPending) {
			auto fluxes = operationResult->operationResultFluxCollection();
			for (auto flux : fluxes) {
				auto srcIx = flux->source();
				auto dstIx = flux->sink();
				if (srcIx == dstIx)
					continue; // don't process transfers to same pool
				auto val = flux->value();
				_poolValues[srcIx] -= val;
				_poolValues[dstIx] += val;
			}
		}
	}
	commitPendingOperationResults();
}

void OperationManagerUblas::clearAllOperationResults() {
	_operationResultsPending.clear();
	_operationResultsLastApplied.clear();
	_operationResultsCommitted.clear();
}

void OperationManagerUblas::clearLastAppliedOperationResults() {
	_operationResultsLastApplied.clear();
}

const OperationResultCollection& OperationManagerUblas::operationResultsPending() const {
	return _operationResultsPending;
}

const OperationResultCollection& OperationManagerUblas::operationResultsLastApplied() const {
	return _operationResultsLastApplied;
}

const OperationResultCollection& OperationManagerUblas::operationResultsCommitted() const {
	return _operationResultsCommitted;
}

PoolCollection OperationManagerUblas::poolCollection() {
	return PoolCollection(_poolObjects);
	//return _poolObjects;
}

void OperationManagerUblas::initialisePools() {
	if (_useKahan)
		std::fill(_corrections.begin(), _corrections.end(), 0.0);

	for (auto& pool : _poolObjects) {
		pool->init();
	}
}

const IPool* OperationManagerUblas::addPool(const std::string& name, double initValue) {
	return addPool(name, "", "", 1.0, 0, initValue);
}

const IPool* OperationManagerUblas::addPool(const std::string& name, const std::string& description, const std::string& units, double scale, int order, double initValue) {
	if (name.length() == 0 || all(name, boost::algorithm::is_space())) {
		throw std::invalid_argument("name cannot be empty");
	}
	if (initValue < 0.0) {
		throw std::invalid_argument("initValue cannot be less than 0.0");	// TODO: check this is true?
	}
	_poolValues.resize(++_poolCount);

	if (_useKahan) {
		_corrections.resize(_poolValues.size());
		_corrections[_poolValues.size() - 1] = 0.0;
	}

	auto pool = std::make_shared<PoolUblas>(_poolValues, name, description, units, scale, order, _poolCount - 1, initValue);
	_poolObjects.push_back(pool);
	_poolNameObjectMap[name] = pool;

	return pool.get();
}

const IPool* OperationManagerUblas::addPool(PoolMetaData& metadata, double initValue) {
	// TODO: finish this
	return nullptr;
}

const IPool* OperationManagerUblas::getPool(const std::string& name) const {
	try {
		auto r = _poolNameObjectMap.at(name);
		return r.get();
	}
	catch (...) {
		throw PoolNotFoundException() << PoolName(name);
	}
}

const IPool* OperationManagerUblas::getPool(int index) const {
	if (index >= _poolObjects.size() || index < 0)
		throw PoolNotFoundException() << PoolName("Bad index");
	auto& r = _poolObjects[index];
	return r.get();
}

std::string OperationManagerUblas::config() const{
	std::stringstream ss;
	ss << "(" << "Kahan[" << (_useKahan ? "ON" : "OFF") << "], " << "ZeroTransfers[" << (
		_allowZeroResultTransfers ? "ON" : "OFF") << "]" << ")";
	return ss.str();
}

void OperationManagerUblas::commitPendingOperationResults() {
	std::copy(_operationResultsPending.begin(), _operationResultsPending.end(), std::back_inserter(_operationResultsLastApplied));
	std::move(_operationResultsPending.begin(), _operationResultsPending.end(), std::back_inserter(_operationResultsCommitted));
	_operationResultsPending.clear();
}

void OperationManagerUblas::submitOperation(IOperation* operation) {
	auto result = operation->computeOperation(_timing);
	if (result != nullptr)
		_operationResultsPending.push_back(result);
}

bool OperationManagerUblas::hasOperationResultsLastApplied() const {
	return !_operationResultsLastApplied.empty();
}
}
} // moja::flint

