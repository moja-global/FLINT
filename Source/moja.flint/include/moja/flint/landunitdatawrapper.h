#pragma once

#include "moja/flint/_flint_exports.h"
#include "moja/flint/ilandunitcontroller.h"
#include "moja/flint/ilandunitdatawrapper.h"

#include <string>

namespace moja::flint {

class IModule;
class IOperation;
class IPool;
class IlandUnitController;

class LandUnitDataWrapper : public ILandUnitDataWrapper {
  public:
   explicit LandUnitDataWrapper(IModule* module) : _module(module), _landUnitController(nullptr) {}
   LandUnitDataWrapper(ILandUnitController* landUnitController, IModule* module)
       : _module(module), _landUnitController(landUnitController) {}

   virtual ~LandUnitDataWrapper() = default;

   std::shared_ptr<IOperation> createStockOperation() override;
   std::shared_ptr<IOperation> createStockOperation(DynamicVar& dataPackage) override;
   std::shared_ptr<IOperation> createProportionalOperation() override;
   std::shared_ptr<IOperation> createProportionalOperation(DynamicVar& dataPackage) override;
   void submitOperation(std::shared_ptr<IOperation> operation) override;
   void applyOperations() override;

   const OperationResultCollection& getOperationPendingIterator() override;
   const OperationResultCollection& getOperationLastAppliedIterator() override;
   const OperationResultCollection& getOperationCommittedIterator() override;

   bool hasLastAppliedOperationResults() const override;

   void clearLastAppliedOperationResults() override;

   void clearAllOperationResults() override;

   PoolCollection poolCollection() const override;

   int getPoolCount() const override;
   const IPool* getPool(const std::string& name) const override;
   const IPool* getPool(int index) const override;
   const IPool* addPool(const std::string& name, const std::string& description, const std::string& units, double scale,
                        int order, double initValue, IPool* parent) override;

   IVariable* getVariable(const std::string& name) override;
   const IVariable* getVariable(const std::string& name) const override;
   std::vector<std::shared_ptr<IVariable>> variables() const override;
   bool hasVariable(const std::string& name) const override;

   IOperationManager* operationManager() override;
   const IOperationManager* operationManager() const override;

   ITiming* timing() override;
   const ITiming* timing() const override;

   const configuration::Configuration* config() override;

   // private:
   // TODO: hide this method
   void setLandUnitController(ILandUnitController* landUnitController) override {
      _landUnitController = landUnitController;
   }

  private:
   IModule* _module;
   ILandUnitController* _landUnitController;
};

}  // namespace moja::flint
