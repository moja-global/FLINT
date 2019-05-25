#include "moja/flint/operationmanagersimple.h"

#include "moja/flint/externalpoolsimple.h"
#include "moja/flint/flintexceptions.h"
#include "moja/flint/imodule.h"
#include "moja/flint/operationproportionalsimple.h"
#include "moja/flint/operationresultcollection.h"
#include "moja/flint/operationresultsimple.h"
#include "moja/flint/operationstocksimple.h"
#include "moja/flint/poolsimple.h"

#include <moja/exception.h>
#include <moja/logging.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>

namespace moja {
namespace flint {

#define OPERATION_MANAGER_CACHE_SIZE 100

// --------------------------------------------------------------------------------------------

OperationManagerSimple::OperationManagerSimple(Timing& timing, const DynamicObject& config) : _timing(timing) {
   _poolValues.reserve(255);
   _poolObjects.reserve(255);

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

   if (config.find("allow_negative_transfers") != config.end()) {
      _allowNegativeTransfers = config["allow_negative_transfers"];
   }

   if (config.find("warn_negative_transfers") != config.end()) {
      _warnNegativeTransfers = config["warn_negative_transfers"];
   }
}

// --------------------------------------------------------------------------------------------

std::shared_ptr<IOperation> OperationManagerSimple::createStockOperation(IModule& module) {
   return std::make_shared<StockOperationSimple>(this, _timing.fractionOfStep(), &module.metaData());
}

// --------------------------------------------------------------------------------------------

std::shared_ptr<IOperation> OperationManagerSimple::createStockOperation(IModule& module, DynamicVar& dataPackage) {
   return std::make_shared<StockOperationSimple>(this, _timing.fractionOfStep(), &module.metaData(), dataPackage);
}

// --------------------------------------------------------------------------------------------

std::shared_ptr<IOperation> OperationManagerSimple::createProportionalOperation(IModule& module) {
   return std::make_shared<ProportionalOperationSimple>(this, _poolValues, _timing.fractionOfStep(),
                                                        &module.metaData());
}

// --------------------------------------------------------------------------------------------

std::shared_ptr<IOperation> OperationManagerSimple::createProportionalOperation(IModule& module,
                                                                                DynamicVar& dataPackage) {
   return std::make_shared<ProportionalOperationSimple>(this, _poolValues, _timing.fractionOfStep(), &module.metaData(),
                                                        dataPackage);
}

// --------------------------------------------------------------------------------------------

void OperationManagerSimple::applyOperations() {
   if (_useKahan) {  // used for https://en.wikipedia.org/wiki/Kahan_summation_algorithm
      double y;
      double t;

      for (auto& operationResult : _operationResultsPending) {
         // operationResult->setTimingWhenApplied(_timing);
         auto p = std::static_pointer_cast<OperationResultSimple>(operationResult);
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
         auto p = std::static_pointer_cast<OperationResultSimple>(operationResult);
         for (auto& flux : p->_fluxes) {
            auto srcIx = flux.source();
            auto dstIx = flux.sink();
            if (srcIx == dstIx) continue;  // don't process transfers to same pool
            auto val = flux.value();

            if (!_allowNegativeTransfers && FloatCmp::lessThan(val, 0.0)) {
               auto amount = flux.transferType() == OperationTransferType::Proportional ? val * 100.0 : val;
               BOOST_THROW_EXCEPTION(SimulationError()
                                     << Details((boost::format("Negative transfer by %1%: %2% %3% %4% to %5%") %
                                                 flux.metaData()->moduleName % amount %
                                                 (flux.transferType() == OperationTransferType::Proportional ? "% of"
                                                                                                             : "from") %
                                                 getPool(srcIx)->name() % getPool(dstIx)->name())
                                                    .str())
                                     << LibraryName("moja.flint") << ModuleName("OperationManagerSimple")
                                     << ErrorCode(0));
            }

            if (_warnNegativeTransfers && FloatCmp::lessThan(val, 0.0)) {
               auto amount = flux.transferType() == OperationTransferType::Proportional ? val * 100.0 : val;
               MOJA_LOG_DEBUG << "Negative transfer by " << flux.metaData()->moduleName << ": " << amount
                              << (flux.transferType() == OperationTransferType::Proportional ? "% of " : " from ")
                              << getPool(srcIx)->name() << " to " << getPool(dstIx)->name();
            }

            // MOJA_LOG_DEBUG << "Simple applyOperations - pool src pre: " << std::setfill(' ') << std::setw(30) <<
            // std::setprecision(15) << _poolValues[srcIx] << ", pool dst pre: " << std::setfill(' ') << std::setw(30)
            // << std::setprecision(15) << _poolValues[dstIx] << ", src: " << std::setfill(' ') << std::setw(3) << srcIx
            // <<
            // ", snk: " << std::setfill(' ') << std::setw(3) << dstIx << ", value:" << val << ", ";
            _poolValues[srcIx] -= val;
            _poolValues[dstIx] += val;
            // MOJA_LOG_DEBUG << "Simple applyOperations - pool src pst: " << std::setfill(' ') << std::setw(30) <<
            // std::setprecision(15) << _poolValues[srcIx] << ", pool dst pst: " << std::setfill(' ') << std::setw(30)
            // << std::setprecision(15) << _poolValues[dstIx];
         }
      }
   }
   commitPendingOperationResults();
}

// --------------------------------------------------------------------------------------------

void OperationManagerSimple::clearAllOperationResults() {
   _operationResultsPending.clear();
   _operationResultsLastApplied.clear();
   _operationResultsCommitted.clear();
}

// --------------------------------------------------------------------------------------------

void OperationManagerSimple::clearLastAppliedOperationResults() { _operationResultsLastApplied.clear(); }

// --------------------------------------------------------------------------------------------

const OperationResultCollection& OperationManagerSimple::operationResultsPending() const {
   return _operationResultsPending;
}

// --------------------------------------------------------------------------------------------

const OperationResultCollection& OperationManagerSimple::operationResultsLastApplied() const {
   return _operationResultsLastApplied;
}

// --------------------------------------------------------------------------------------------

const OperationResultCollection& OperationManagerSimple::operationResultsCommitted() const {
   return _operationResultsCommitted;
}

// --------------------------------------------------------------------------------------------

PoolCollection OperationManagerSimple::poolCollection() { return PoolCollection(_poolObjects); }

// --------------------------------------------------------------------------------------------

void OperationManagerSimple::initialisePools() {
   if (_useKahan) std::fill(_corrections.begin(), _corrections.end(), 0.0);
   for (auto& pool : _poolObjects) {
      pool->init();
   }
}

// --------------------------------------------------------------------------------------------

const IPool* OperationManagerSimple::addPool(const std::string& name, const std::string& description,
                                             const std::string& units, double scale, int order,
                                             const std::shared_ptr<ITransform> transform) {
   return addPool<ExternalPoolSimple>(name, description, units, scale, order, transform);
}

// --------------------------------------------------------------------------------------------

const IPool* OperationManagerSimple::addPool(const std::string& name, double initValue) {
   return addPool(name, "", "", 1.0, 0, initValue);
}

// --------------------------------------------------------------------------------------------

const IPool* OperationManagerSimple::addPool(const std::string& name, const std::string& description,
                                             const std::string& units, double scale, int order, double initValue) {
   if (initValue < 0.0) {
      throw std::invalid_argument("initValue cannot be less than 0.0");  // TODO: check this is true?
   }

   return addPool<PoolSimple>(name, description, units, scale, order, initValue);
}

// --------------------------------------------------------------------------------------------

template <class TPool, typename TInitValue>
const IPool* OperationManagerSimple::addPool(const std::string& name, const std::string& description,
                                             const std::string& units, double scale, int order, TInitValue initValue) {
   if (name.length() == 0 || all(name, boost::algorithm::is_space())) {
      throw std::invalid_argument("name cannot be empty");
   }

   if (_poolValues.size() == _poolValues.capacity()) {
      throw ApplicationException(
          "maximum pool definitions exceeded. Only 255 pools allowed");  // to protect the references held by PoolSimple
                                                                         // wrappers
   }

   _poolValues.push_back(0.0);
   auto pool = std::make_shared<TPool>(_poolValues, name, description, units, scale, order,
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

// --------------------------------------------------------------------------------------------

const IPool* OperationManagerSimple::addPool(PoolMetaData& metadata, double initValue) {
   // TODO: finish this
   return nullptr;
}

// --------------------------------------------------------------------------------------------

const IPool* OperationManagerSimple::getPool(const std::string& name) const {
   try {
      auto r = _poolNameObjectMap.at(name);
      return r.get();
   } catch (...) {
      throw PoolNotFoundException() << PoolName(name);
   }
}

// --------------------------------------------------------------------------------------------

const IPool* OperationManagerSimple::getPool(int index) const {
   if (index >= _poolObjects.size() || index < 0) throw PoolNotFoundException() << PoolName("Bad index");
   auto& r = _poolObjects[index];
   return r.get();
}

// --------------------------------------------------------------------------------------------

void OperationManagerSimple::commitPendingOperationResults() {
   std::copy(_operationResultsPending.begin(), _operationResultsPending.end(),
             std::back_inserter(_operationResultsLastApplied));
   std::move(_operationResultsPending.begin(), _operationResultsPending.end(),
             std::back_inserter(_operationResultsCommitted));
   _operationResultsPending.clear();
}

// --------------------------------------------------------------------------------------------

void OperationManagerSimple::submitOperation(IOperation* operation) {
   auto result = operation->computeOperation(_timing);
   if (result != nullptr) _operationResultsPending.push_back(result);
}

bool OperationManagerSimple::hasOperationResultsLastApplied() const { return !_operationResultsLastApplied.empty(); }
}  // namespace flint
}  // namespace moja
