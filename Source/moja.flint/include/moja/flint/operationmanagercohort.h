#pragma once

#include "moja/flint/_flint_exports.h"
#include "moja/flint/ioperationmanager.h"
#include "moja/flint/operationresultcohort.h"
#include "moja/flint/operationresultcollection.h"
#include "moja/flint/operationstockcohort.h"

#include <moja/dynamic.h>

namespace moja::flint {

class IModule;
class IOperation;
class IPool;

class Timing;
class PoolMetaData;
class StockOperationCohort;
class ProportionalOperationCohort;

class PoolManager {
  public:
   PoolManager(const DynamicObject& config);

   ~PoolManager();

   void process_fluxes(const std::vector<OperationResultFluxSimple>& fluxes);
   void initialise();
   std::string config() const;
   const IPool* get_pool(const std::string& name) const;
   const IPool* get_pool(int index) const;
   std::vector<std::shared_ptr<IPool>>& pools();
   std::vector<double>& pool_values();

   const IPool* add_pool(const std::string& name, const std::string& description, const std::string& units,
                         double scale, int order, double init_value);
   const IPool* add_pool(const std::string& name, const std::string& description, const std::string& units,
                         double scale, int order, std::shared_ptr<ITransform> init_value);

   class impl;

  private:
   std::unique_ptr<impl> impl_;
};

class FLINT_API OperationManagerCohort : public IOperationManager {
   friend class StockOperationCohort;
   friend class ProportionalOperationCohort;

  public:
   OperationManagerCohort(Timing& timing, const DynamicObject& config);
   virtual ~OperationManagerCohort() = default;

   // Operations
   std::shared_ptr<IOperation> createStockOperation(IModule& module) override;
   std::shared_ptr<IOperation> createStockOperation(IModule& module, DynamicVar& dataPackage) override;
   std::shared_ptr<IOperation> createProportionalOperation(IModule& module) override;
   std::shared_ptr<IOperation> createProportionalOperation(IModule& module, DynamicVar& dataPackage) override;
   void applyOperations() override;

   void clearAllOperationResults() override;
   void clearLastAppliedOperationResults() override;

   [[nodiscard]] const OperationResultCollection& operationResultsPending() const override;
   [[nodiscard]] const OperationResultCollection& operationResultsLastApplied() const override;
   [[nodiscard]] const OperationResultCollection& operationResultsCommitted() const override;

   [[nodiscard]] bool hasOperationResultsLastApplied() const override;

   // Pools
   PoolCollection poolCollection() override;

   void initialisePools() override;

   int poolCount() override;

   const IPool* addPool(const std::string& name, const std::string& description, const std::string& units, double scale,
                        int order, const std::shared_ptr<ITransform> transform) override;
   const IPool* addPool(const std::string& name, double initValue = 0.0) override;
   const IPool* addPool(const std::string& name, const std::string& description, const std::string& units, double scale,
                        int order, double initValue = 0.0) override;
   const IPool* addPool(PoolMetaData& metadata, double initValue) override;

   [[nodiscard]] const IPool* getPool(const std::string& name) const override;
   [[nodiscard]] const IPool* getPool(int index) const override;

   // Details of instance
   [[nodiscard]] std::string name() const override { return "Cohort"; }
   [[nodiscard]] std::string version() const override { return "1.0"; }
   [[nodiscard]] std::string config() const override;

  protected:
   void commitPendingOperationResults();

   Timing& timing_;  // timing object for simulation

   PoolManager pool_manager_;

   //   std::vector<double> pool_values_;  // pool values
   //   std::vector<double> corrections_;  // used for https://en.wikipedia.org/wiki/Kahan_summation_algorithm

   //   std::vector<std::shared_ptr<IPool>> pool_pointers_;       // pool object vector
   //   std::map<std::string, std::shared_ptr<IPool>> pool_map_;  // pool objects indexed by name

   // OperationResults (flux & meta-data) submitted by modules - waiting to be applied
   OperationResultCollection operation_results_pending_;
   // The most recently applied OperationResults (copy)
   OperationResultCollection operation_results_last_applied_;
   // List of OperationResults that have been committed
   OperationResultCollection operation_results_committed_;
   bool allow_zero_result_transfers_;

  private:
   //   bool use_kahan_;
   //   bool allow_negative_transfers_ = true;
   //   bool warn_negative_transfers_ = false;

   const IPool* add_pool(const std::string& name, const std::string& description, const std::string& units,
                         double scale, int order, std::shared_ptr<ITransform> init_value);
   const IPool* add_pool(const std::string& name, const std::string& description, const std::string& units,
                         double scale, int order, double init_value);

   // Operations
   void submitOperation(IOperation* operation) override;
};

}  // namespace moja::flint
