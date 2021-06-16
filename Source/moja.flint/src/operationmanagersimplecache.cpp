#include "moja/flint/operationmanagersimplecache.h"

#include "moja/flint/flintexceptions.h"
#include "moja/flint/operationproportionalsimplecache.h"
#include "moja/flint/operationresultcollection.h"
#include "moja/flint/operationresultsimplecache.h"
#include "moja/flint/operationstocksimplecache.h"
#include "moja/flint/poolsimplecache.h"

#include <moja/logging.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace moja {
namespace flint {

#define OPERATION_MANAGER_CACHE_SIZE 10

OperationManagerSimpleCache::OperationManagerSimpleCache(Timing& timing, const DynamicObject& config)
    : _timing(timing) {
   _poolValues.reserve(100);
   _poolObjects.reserve(100);

   _operationResultsPending.reserve(50);
   _operationResultsLastApplied.reserve(50);
   _operationResultsCommitted.reserve(500);

   _useKahan = false;
   _allowZeroResultTransfers = false;

   if (config.find("use_kahan") != config.end()) {
      _useKahan = config["use_kahan"];
      if (_useKahan) _corrections.reserve(100);
   }
   if (config.find("allow_zero_result_transfers") != config.end()) {
      _allowZeroResultTransfers = config["allow_zero_result_transfers"];
   }

   _stockCache.reserve(OPERATION_MANAGER_CACHE_SIZE);
   for (int i = 0; i < OPERATION_MANAGER_CACHE_SIZE; i++) {
      _stockCache.push_back(
          std::make_shared<StockOperationSimpleCache>(this, _poolValues, _timing.fractionOfStep(), nullptr));
   }
   _stockCachePosition = _stockCache.begin();
   _proportionalCache.reserve(OPERATION_MANAGER_CACHE_SIZE);
   for (int i = 0; i < OPERATION_MANAGER_CACHE_SIZE; i++) {
      _proportionalCache.push_back(
          std::make_shared<ProportionalOperationSimpleCache>(this, _poolValues, _timing.fractionOfStep(), nullptr));
   }
   _proportionalCachePosition = _proportionalCache.begin();

   for (int i = 0; i < OPERATION_MANAGER_CACHE_SIZE; i++) {
      _operationResultCache.push_back(
          std::make_shared<OperationResultSimpleCache>(OperationTransferType::Proportional, nullptr));
   }
   _operationResultCachePosition = _operationResultCache.begin();
}

std::shared_ptr<IOperation> OperationManagerSimpleCache::createStockOperation(IModule& module) {
   if (_stockCachePosition == _stockCache.end()) _stockCachePosition = _stockCache.begin();

   auto obj = std::static_pointer_cast<StockOperationSimpleCache>(*_stockCachePosition);
   obj->set_metaData(&module.metaData());
   obj->_transfers.clear();
   ++_stockCachePosition;
   return obj;
}

std::shared_ptr<IOperation> OperationManagerSimpleCache::createStockOperation(IModule& module, DynamicVar& extra) {
   if (_stockCachePosition == _stockCache.end()) _stockCachePosition = _stockCache.begin();

   auto obj = std::static_pointer_cast<StockOperationSimpleCache>(*_stockCachePosition);
   // TODO: need to somehow set extraData here!
   obj->set_metaData(&module.metaData());
   obj->_transfers.clear();
   ++_stockCachePosition;
   return obj;
}

std::shared_ptr<IOperation> OperationManagerSimpleCache::createProportionalOperation(IModule& module) {
   if (_proportionalCachePosition == _proportionalCache.end()) _proportionalCachePosition = _proportionalCache.begin();

   auto obj = std::static_pointer_cast<ProportionalOperationSimpleCache>(*_proportionalCachePosition);
   obj->set_metaData(&module.metaData());
   obj->_transfers.clear();
   ++_proportionalCachePosition;
   return obj;
}

std::shared_ptr<IOperation> OperationManagerSimpleCache::createProportionalOperation(IModule& module,
                                                                                     DynamicVar& dataPackage) {
   if (_proportionalCachePosition == _proportionalCache.end()) _proportionalCachePosition = _proportionalCache.begin();

   auto obj = std::static_pointer_cast<ProportionalOperationSimpleCache>(*_proportionalCachePosition);
   // TODO: need to somehow set extraData here!
   obj->set_metaData(&module.metaData());
   obj->_transfers.clear();
   ++_proportionalCachePosition;
   return obj;
}

void OperationManagerSimpleCache::applyOperations() {
   if (_useKahan) {  // used for https://en.wikipedia.org/wiki/Kahan_summation_algorithm
      double y;
      double t;

      for (auto& operationResult : _operationResultsPending) {
         // operationResult->setTimingWhenApplied(_timing);
         auto p = std::static_pointer_cast<OperationResultSimpleCache>(operationResult);
         for (auto& flux : p->_fluxes) {
            auto srcIx = flux.source();
            auto dstIx = flux.sink();
            if (srcIx == dstIx) {
               // don't process transfers to same pool
               continue;
            }
            auto val = flux.value();
            y = -val - _corrections[srcIx];  // Do source
            t = _poolValues[srcIx] + y;
            _corrections[srcIx] = (t - _poolValues[srcIx]) - y;
            _poolValues[srcIx] = t;
            y = val - _corrections[dstIx];  // Do sink
            t = _poolValues[dstIx] + y;
            _corrections[dstIx] = (t - _poolValues[dstIx]) - y;
            _poolValues[dstIx] = t;
         }
      }
   } else {
      for (auto& operationResult : _operationResultsPending) {
         auto p = std::static_pointer_cast<OperationResultSimpleCache>(operationResult);
         for (auto& flux : p->_fluxes) {
            auto srcIx = flux.source();
            auto dstIx = flux.sink();
            if (srcIx == dstIx) continue;  // don't process transfers to same pool
            auto val = flux.value();
            // MOJA_LOG_DEBUG << "SimpleCache applyOperations - pool src pre: " << std::setfill(' ') << std::setw(30) <<
            // std::setprecision(15) << _poolValues[srcIx] << ", pool dst pre: " << std::setfill(' ') << std::setw(30) <<
            // std::setprecision(15) << _poolValues[dstIx] << ", src: " << std::setfill(' ') << std::setw(3) << srcIx <<
            // ", snk: " << std::setfill(' ') << std::setw(3) << dstIx << ", value:" << val << ", ";
            _poolValues[srcIx] -= val;
            _poolValues[dstIx] += val;
            // MOJA_LOG_DEBUG << "SimpleCache applyOperations - pool src pst: " << std::setfill(' ') << std::setw(30) <<
            // std::setprecision(15) << _poolValues[srcIx] << ", pool dst pst: " << std::setfill(' ') << std::setw(30) <<
            // std::setprecision(15) << _poolValues[dstIx];
         }
      }
   }
   commitPendingOperationResults();
}

void OperationManagerSimpleCache::clearAllOperationResults() {
   _operationResultsPending.clear();
   _operationResultsLastApplied.clear();
   _operationResultsCommitted.clear();
}

void OperationManagerSimpleCache::clearLastAppliedOperationResults() { _operationResultsLastApplied.clear(); }

const OperationResultCollection& OperationManagerSimpleCache::operationResultsPending() const {
   return _operationResultsPending;
}

const OperationResultCollection& OperationManagerSimpleCache::operationResultsLastApplied() const {
   return _operationResultsLastApplied;
}

const OperationResultCollection& OperationManagerSimpleCache::operationResultsCommitted() const {
   return _operationResultsCommitted;
}

bool OperationManagerSimpleCache::hasOperationResultsLastApplied() const {
   return !_operationResultsLastApplied.empty();
}

PoolCollection OperationManagerSimpleCache::poolCollection() {
   return PoolCollection(_poolObjects);
   // return _poolObjects;
}

void OperationManagerSimpleCache::initialisePools() {
   if (_useKahan) std::fill(_corrections.begin(), _corrections.end(), 0.0);
   // std::fill(_poolValues.begin(), _poolValues.end(), 0.0);
   for (auto& pool : _poolObjects) {
      pool->init();
   }
}

const IPool* OperationManagerSimpleCache::addPool(const std::string& name, double initValue) {
   return addPool(name, "", "", 1.0, 0, initValue);
}

const IPool* OperationManagerSimpleCache::addPool(const std::string& name, const std::string& description,
                                                  const std::string& units, double scale, int order, double initValue) {
   if (name.length() == 0 || all(name, boost::algorithm::is_space())) {
      throw std::invalid_argument("name cannot be empty");
   }
   if (initValue < 0.0) {
      throw std::invalid_argument("initValue cannot be less than 0.0");  // TODO: check this is true?
   }
   _poolValues.push_back(0.0);
   auto pool = std::make_shared<PoolSimpleCache>(_poolValues, name, description, units, scale, order,
                                                 static_cast<int>(_poolValues.size() - 1), initValue);
   _poolObjects.push_back(pool);
   _poolNameObjectMap[name] = pool;

   if (_useKahan) {
      _corrections.resize(_poolValues.size());
      _corrections[_poolValues.size() - 1] = 0.0;
      std::fill(_corrections.begin(), _corrections.end(), 0.0);
   }
   return pool.get();
}

const IPool* OperationManagerSimpleCache::addPool(PoolMetaData& metadata, double initValue) {
   // TODO: finish this
   return nullptr;
}

const IPool* OperationManagerSimpleCache::getPool(const std::string& name) const {
   try {
      auto& r = _poolNameObjectMap.at(name);
      return r.get();
   } catch (...) {
      throw std::invalid_argument("Error pool not found " + name);
   }
}

const IPool* OperationManagerSimpleCache::getPool(int index) const {
   if (index >= _poolObjects.size() || index < 0) throw std::invalid_argument("Error in get pool index out of range");
   auto& r = _poolObjects[index];
   return r.get();
}

std::string OperationManagerSimpleCache::config() const {
   std::stringstream ss;
   ss << "("
      << "Kahan[" << (_useKahan ? "ON" : "OFF") << "], "
      << "ZeroTransfers[" << (_allowZeroResultTransfers ? "ON" : "OFF") << "]"
      << ")";
   return ss.str();
}

void OperationManagerSimpleCache::commitPendingOperationResults() {
   std::copy(_operationResultsPending.begin(), _operationResultsPending.end(),
             std::back_inserter(_operationResultsLastApplied));
   std::move(_operationResultsPending.begin(), _operationResultsPending.end(),
             std::back_inserter(_operationResultsCommitted));
   _operationResultsPending.clear();
}

void OperationManagerSimpleCache::submitOperation(IOperation* operation) {
   auto result = operation->computeOperation(_timing);
   if (result != nullptr) _operationResultsPending.push_back(result);
}

}  // namespace flint
}  // namespace moja
