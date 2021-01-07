#include "moja/flint/operationmanagercohort.h"

#include "moja/flint/externalpoolcohort.h"
#include "moja/flint/flintexceptions.h"
#include "moja/flint/imodule.h"
#include "moja/flint/operationproportionalcohort.h"
#include "moja/flint/operationresultcohort.h"
#include "moja/flint/operationresultcollection.h"
#include "moja/flint/operationstockcohort.h"
#include "moja/flint/poolcohort.h"

#include <moja/exception.h>
#include <moja/logging.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <fmt/format.h>

namespace moja::flint {

class PoolManager::impl {
  public:
   virtual ~impl() = default;

   const IPool* add_pool(const std::string& name, const std::string& description, const std::string& units,
                         double scale, int order, double init_value) {
      pool_values_.emplace_back(0.0);
      auto pool = std::make_shared<PoolCohort>(pool_values_, name, description, units, scale, order,
                                          static_cast<int>(pool_values_.size() - 1), init_value);
      pool_pointers_.emplace_back(pool);
      pool_map_[name] = pool;

      // if (use_kahan_) {
      //   corrections_.resize(pool_values_.size());
      //   corrections_[pool_values_.size() - 1] = 0.0;
      //   std::fill(corrections_.begin(), corrections_.end(), 0.0);
      //}

      return pool.get();
   }
   const IPool* add_pool(const std::string& name, const std::string& description,
                                                                const std::string& units, double scale, int order, std::shared_ptr<ITransform> init_value) {
      pool_values_.emplace_back(0.0);
      auto pool = std::make_shared<ExternalPoolCohort>(pool_values_, name, description, units, scale, order,
                                               static_cast<int>(pool_values_.size() - 1), init_value);
      pool_pointers_.emplace_back(pool);
      pool_map_[name] = pool;

      // if (use_kahan_) {
      //   corrections_.resize(pool_values_.size());
      //   corrections_[pool_values_.size() - 1] = 0.0;
      //   std::fill(corrections_.begin(), corrections_.end(), 0.0);
      //}

      return pool.get();
   }

   impl(bool allow_negative_transfers, bool warn_negative_transfers)
       : allow_negative_transfers_(allow_negative_transfers), warn_negative_transfers_(warn_negative_transfers) {
      pool_values_.reserve(255);
      pool_pointers_.reserve(255);
   }

   std::vector<double>& pool_values() { return pool_values_; }
   std::vector<std::shared_ptr<IPool>>& pools() { return pool_pointers_; }

   [[nodiscard]] const IPool* get_pool(const std::string& name) {
      try {
         auto pool = pool_map_.at(name);
         return pool.get();
      } catch (...) {
         throw PoolNotFoundException() << PoolName(name);
      }
   }

   [[nodiscard]] const IPool* get_pool(int index) const {
      if (index >= pool_pointers_.size() || index < 0) throw PoolNotFoundException() << PoolName("Bad index");
      const auto& pool = pool_pointers_[index];
      return pool.get();
   }

   virtual void initialise() = 0;
   virtual std::string config() = 0;
   virtual void process_fluxes(const std::vector<OperationResultFluxSimple>& fluxes) = 0;

  protected:
   std::vector<double> pool_values_;                         // pool values
   std::vector<std::shared_ptr<IPool>> pool_pointers_;       // pool object vector
   std::map<std::string, std::shared_ptr<IPool>> pool_map_;  // pool objects indexed by name
   bool allow_negative_transfers_;
   bool warn_negative_transfers_;
};

// used for https://en.wikipedia.org/wiki/Kahan_summation_algorithm
class Kahan final : public PoolManager::impl {
  public:
   Kahan(bool allow_negative_transfers, bool warn_negative_transfers)
       : impl(allow_negative_transfers, warn_negative_transfers) {
      corrections_.reserve(100);
   }
   virtual ~Kahan() override = default;
   void process_fluxes(const std::vector<OperationResultFluxSimple>& fluxes) override {
      for (const auto& flux : fluxes) {
         const auto src_ix = flux.source();
         const auto dst_ix = flux.sink();
         if (src_ix == dst_ix) {
            // don't process transfers to same pool
            continue;
         }
         const auto val = flux.value();

         if (!allow_negative_transfers_ && FloatCmp::lessThan(val, 0.0)) {
            const auto amount = flux.transferType() == OperationTransferType::Proportional ? val * 100.0 : val;
            BOOST_THROW_EXCEPTION(SimulationError()
                                  << Details(fmt::format(
                                         "Negative transfer by {}: {} {} {} to {}", flux.metaData()->moduleName, amount,
                                         (flux.transferType() == OperationTransferType::Proportional ? "% of" : "from"),
                                         get_pool(src_ix)->name(), get_pool(dst_ix)->name()))
                                  << LibraryName("moja.flint") << ModuleName("OperationManagerCohort") << ErrorCode(0));
         }

         if (warn_negative_transfers_ && FloatCmp::lessThan(val, 0.0)) {
            const auto amount = flux.transferType() == OperationTransferType::Proportional ? val * 100.0 : val;
            MOJA_LOG_DEBUG << "Negative transfer by " << flux.metaData()->moduleName << ": " << amount
                           << (flux.transferType() == OperationTransferType::Proportional ? "% of " : " from ")
                           << get_pool(src_ix)->name() << " to " << get_pool(dst_ix)->name();
         }

         double y = -val - corrections_[src_ix];  // Do source
         double t = pool_values_[src_ix] + y;
         corrections_[src_ix] = (t - pool_values_[src_ix]) - y;
         pool_values_[src_ix] = t;
         y = val - corrections_[dst_ix];  // Do sink
         t = pool_values_[dst_ix] + y;
         corrections_[dst_ix] = (t - pool_values_[dst_ix]) - y;
         pool_values_[dst_ix] = t;
      }
   }

   void initialise() override {
      std::fill(corrections_.begin(), corrections_.end(), 0.0);
      for (auto& pool : pool_pointers_) {
         pool->init();
      }
   }
   std::string config() override {
      std::stringstream ss;
      ss << "("
         << "Kahan[ON], "
         << "AllowNegativeTransfers[" << (allow_negative_transfers_ ? "ON" : "OFF") << "], "
         << "WarnNegativeTransfers[" << (warn_negative_transfers_ ? "ON" : "OFF") << "]"
         << ")";
      return ss.str();
   }

  private:
   std::vector<double> corrections_;
};

class Simple final : public PoolManager::impl {
  public:
   Simple(bool allow_negative_transfers, bool warn_negative_transfers)
       : impl(allow_negative_transfers, warn_negative_transfers) {}

   virtual ~Simple() override = default;

   void process_fluxes(const std::vector<OperationResultFluxSimple>& fluxes) override {
      for (const auto& flux : fluxes) {
         const auto src_ix = flux.source();
         const auto dst_ix = flux.sink();
         if (src_ix == dst_ix) continue;  // don't process transfers to same pool
         const auto val = flux.value();

         if (!allow_negative_transfers_ && FloatCmp::lessThan(val, 0.0)) {
            const auto amount = flux.transferType() == OperationTransferType::Proportional ? val * 100.0 : val;
            BOOST_THROW_EXCEPTION(SimulationError()
                                  << Details(fmt::format(
                                         "Negative transfer by {}: {} {} {} to {}", flux.metaData()->moduleName, amount,
                                         (flux.transferType() == OperationTransferType::Proportional ? "% of" : "from"),
                                         get_pool(src_ix)->name(), get_pool(dst_ix)->name()))
                                  << LibraryName("moja.flint") << ModuleName("OperationManagerCohort") << ErrorCode(0));
         }

         if (warn_negative_transfers_ && FloatCmp::lessThan(val, 0.0)) {
            const auto amount = flux.transferType() == OperationTransferType::Proportional ? val * 100.0 : val;
            MOJA_LOG_DEBUG << "Negative transfer by " << flux.metaData()->moduleName << ": " << amount
                           << (flux.transferType() == OperationTransferType::Proportional ? "% of " : " from ")
                           << get_pool(src_ix)->name() << " to " << get_pool(dst_ix)->name();
         }

         pool_values_[src_ix] -= val;
         pool_values_[dst_ix] += val;
      }
   }

   void initialise() override {
      for (auto& pool : pool_pointers_) {
         pool->init();
      }
   }

   std::string config() override {
      std::stringstream ss;
      ss << "("
         << "Kahan[OFF], "
         << "AllowNegativeTransfers[" << (allow_negative_transfers_ ? "ON" : "OFF") << "], "
         << "WarnNegativeTransfers[" << (warn_negative_transfers_ ? "ON" : "OFF") << "]"
         << ")";
      return ss.str();
   }
};

PoolManager::PoolManager(const DynamicObject& config) {
   bool allow_negative_transfers = true;
   if (config.find("allow_negative_transfers") != config.end()) {
      allow_negative_transfers = config["allow_negative_transfers"];
   }
   bool warn_negative_transfers = false;
   if (config.find("warn_negative_transfers") != config.end()) {
      warn_negative_transfers = config["warn_negative_transfers"];
   }
   if (config.contains("use_kahan") && config["use_kahan"]) {
      impl_ = std::make_unique<Kahan>(allow_negative_transfers, warn_negative_transfers);
   } else {
      impl_ = std::make_unique<Simple>(allow_negative_transfers, warn_negative_transfers);
   }
}

PoolManager::~PoolManager() {}

std::vector<double>& PoolManager::pool_values() { return impl_->pool_values(); }

const IPool* PoolManager::add_pool(const std::string& name, const std::string& description, const std::string& units,
   double scale, int order, std::shared_ptr<ITransform> init_value) {
   return impl_->add_pool(name, description, units, scale, order, init_value);
}
const IPool* PoolManager::add_pool(const std::string& name, const std::string& description, const std::string& units,
                                   double scale, int order, double init_value) {
   return impl_->add_pool(name, description, units, scale, order, init_value);
}

void PoolManager::process_fluxes(const std::vector<OperationResultFluxSimple>& fluxes) {
   impl_->process_fluxes(fluxes);
}

void PoolManager::initialise() { impl_->initialise(); }

inline std::vector<std::shared_ptr<IPool>>& PoolManager::pools() { return impl_->pools(); }

std::string PoolManager::config() const { return impl_->config(); }

const IPool* PoolManager::get_pool(const std::string& name) const { return impl_->get_pool(name); }

const IPool* PoolManager::get_pool(int index) const { return impl_->get_pool(index); }

OperationManagerCohort::OperationManagerCohort(Timing& timing, const DynamicObject& config)
    : timing_(timing), pool_manager_(config) {
   operation_results_pending_.reserve(50);
   operation_results_last_applied_.reserve(50);
   operation_results_committed_.reserve(500);

   if (config.find("allow_zero_result_transfers") != config.end()) {
      allow_zero_result_transfers_ = config["allow_zero_result_transfers"];
   }
}

std::shared_ptr<IOperation> OperationManagerCohort::createStockOperation(IModule& module) {
   return std::make_shared<StockOperationCohort>(this, timing_.fractionOfStep(), &module.metaData());
}

std::shared_ptr<IOperation> OperationManagerCohort::createStockOperation(IModule& module, DynamicVar& dataPackage) {
   return std::make_shared<StockOperationCohort>(this, timing_.fractionOfStep(), &module.metaData(), dataPackage);
}

std::shared_ptr<IOperation> OperationManagerCohort::createProportionalOperation(IModule& module) {
   return std::make_shared<ProportionalOperationCohort>(this, pool_manager_.pool_values(), timing_.fractionOfStep(),
                                                        &module.metaData());
}

std::shared_ptr<IOperation> OperationManagerCohort::createProportionalOperation(IModule& module,
                                                                                DynamicVar& dataPackage) {
   return std::make_shared<ProportionalOperationCohort>(this, pool_manager_.pool_values(), timing_.fractionOfStep(),
                                                        &module.metaData(), dataPackage);
}

void OperationManagerCohort::applyOperations() {
   for (auto& operationResult : operation_results_pending_) {
      auto operation_result = std::static_pointer_cast<OperationResultCohort>(operationResult);
      pool_manager_.process_fluxes(operation_result->fluxes_);
   }
   commitPendingOperationResults();
}

void OperationManagerCohort::clearAllOperationResults() {
   operation_results_pending_.clear();
   operation_results_last_applied_.clear();
   operation_results_committed_.clear();
}

void OperationManagerCohort::clearLastAppliedOperationResults() { operation_results_last_applied_.clear(); }

const OperationResultCollection& OperationManagerCohort::operationResultsPending() const {
   return operation_results_pending_;
}

const OperationResultCollection& OperationManagerCohort::operationResultsLastApplied() const {
   return operation_results_last_applied_;
}

const OperationResultCollection& OperationManagerCohort::operationResultsCommitted() const {
   return operation_results_committed_;
}

PoolCollection OperationManagerCohort::poolCollection() { return PoolCollection(pool_manager_.pools()); }

void OperationManagerCohort::initialisePools() { pool_manager_.initialise(); }

int OperationManagerCohort::poolCount() { return static_cast<int>(pool_manager_.pools().size()); }

const IPool* OperationManagerCohort::addPool(const std::string& name, double initValue) {
   return addPool(name, "", "", 1.0, 0, initValue);
}

const IPool* OperationManagerCohort::addPool(const std::string& name, const std::string& description,
                                             const std::string& units, double scale, int order, double initValue) {
   if (initValue < 0.0) {
      throw std::invalid_argument("initValue cannot be less than 0.0");  // TODO: check this is true?
   }

   return pool_manager_.add_pool(name, description, units, scale, order, initValue);
}

const IPool* OperationManagerCohort::addPool(const std::string& name, const std::string& description,
                                             const std::string& units, double scale, int order,
                                             std::shared_ptr<ITransform> initValue) {
   if (name.length() == 0 || all(name, boost::algorithm::is_space())) {
      throw std::invalid_argument("name cannot be empty");
   }

   return pool_manager_.add_pool(name, description, units, scale, order, initValue);

   //pool_values_.emplace_back(0.0);
   //auto pool = std::make_shared<TPool>(pool_values_, name, description, units, scale, order,
   //                                    static_cast<int>(pool_values_.size() - 1), initValue);
   //pool_pointers_.push_back(pool);
   //pool_map_[name] = pool;

   //if (use_kahan_) {
   //   corrections_.resize(pool_values_.size());
   //   corrections_[pool_values_.size() - 1] = 0.0;
   //   std::fill(corrections_.begin(), corrections_.end(), 0.0);
   //}

   //return pool.get();
}

// --------------------------------------------------------------------------------------------

const IPool* OperationManagerCohort::addPool(PoolMetaData& metadata, double initValue) {
   // TODO: finish this
   return nullptr;
}

// --------------------------------------------------------------------------------------------

const IPool* OperationManagerCohort::getPool(const std::string& name) const { return pool_manager_.get_pool(name); }

// --------------------------------------------------------------------------------------------

const IPool* OperationManagerCohort::getPool(int index) const { return pool_manager_.get_pool(index); }

// --------------------------------------------------------------------------------------------

std::string OperationManagerCohort::config() const { return pool_manager_.config(); }

void OperationManagerCohort::commitPendingOperationResults() {
   std::copy(operation_results_pending_.begin(), operation_results_pending_.end(),
             std::back_inserter(operation_results_last_applied_));
   std::move(operation_results_pending_.begin(), operation_results_pending_.end(),
             std::back_inserter(operation_results_committed_));
   operation_results_pending_.clear();
}

// --------------------------------------------------------------------------------------------

const IPool* OperationManagerCohort::add_pool(const std::string& name, const std::string& description,
   const std::string& units, double scale, int order, std::shared_ptr<ITransform> init_value) {
   if (name.length() == 0 || all(name, boost::algorithm::is_space())) {
      throw std::invalid_argument("name cannot be empty");
   }
   return pool_manager_.add_pool(name, description, units, scale, order, init_value);
}

const IPool* OperationManagerCohort::add_pool(const std::string& name, const std::string& description,
   const std::string& units, double scale, int order, double init_value) {
   if (name.length() == 0 || all(name, boost::algorithm::is_space())) {
      throw std::invalid_argument("name cannot be empty");
   }
   return pool_manager_.add_pool(name, description, units, scale, order, init_value);
}

void OperationManagerCohort::submitOperation(IOperation* operation) {
   const auto result = operation->computeOperation(timing_);
   if (result != nullptr) operation_results_pending_.push_back(result);
}

bool OperationManagerCohort::hasOperationResultsLastApplied() const { return !operation_results_last_applied_.empty(); }
}  // namespace moja::flint
