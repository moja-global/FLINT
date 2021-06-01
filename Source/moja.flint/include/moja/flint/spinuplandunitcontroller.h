#pragma once

#include "moja/flint/_flint_exports.h"
#include "moja/flint/ilandunitcontroller.h"
#include "moja/flint/timing.h"

#include <string>
#include <vector>

namespace moja::flint {

class IOperation;
class IModule;
class LandUnitController;
class IVariable;

// --------------------------------------------------------------------------------------------
// Purpose of this land unit controller is to do the same as the LandUnitController put to use the same _poolVector of
// another controller Given on construction. Not sure this is the best approach. We could for example use 2 normal LUC
// and copy the pool values across when required An example of how CBM might implement it's Spinup procedure Another
// solution could be to write a Special LUC for Spunup which handles both the spinup and normal run, providing extra
// methods for the spinUp parts (modules etc).

class FLINT_API SpinupLandUnitController : public ILandUnitController {
  public:
   explicit SpinupLandUnitController(LandUnitController& landUnitController);
   ~SpinupLandUnitController();

   // void configure(const std::string& config) override;
   void configure(const configuration::Configuration& config) override;
   void initialiseData(bool inLandUnit);
   void initialisePools() const;

   // execute the simulation
   void run() override;

   // Unique Id for each controller
   int id() override { return _currentLandUnitControllerId; }

   // Operations
   std::shared_ptr<IOperation> createStockOperation(IModule& module) override;
   std::shared_ptr<IOperation> createStockOperation(IModule& module, DynamicVar& dataPackage) override;
   std::shared_ptr<IOperation> createProportionalOperation(IModule& module) override;
   std::shared_ptr<IOperation> createProportionalOperation(IModule& module, DynamicVar& dataPackage) override;

   void submitOperation(std::shared_ptr<IOperation> operation) override;
   void applyOperations() override;
   void clearLastAppliedOperationResults() override;
   void clearAllOperationResults() override;

   // Can be used to reset Variables back to original values
   void resetVariables();

   // Data-Set accessors

   std::vector<std::shared_ptr<IVariable>> variables() const override { return _variables; }

   void addVariable(std::string name, std::shared_ptr<IVariable> variable);

   IVariable* getVariable(const std::string& name) override;
   const IVariable* getVariable(const std::string& name) const override;
   bool hasVariable(const std::string& name) const override;

   const ITiming& timing() const override;
   ITiming& timing() override;

   const configuration::Configuration* config() override { return _config; };

   IOperationManager* operationManager() override;
   IOperationManager* operationManager() const override;

  private:
   static int _currentLandUnitControllerId;
   const int _landUnitControllerId = _currentLandUnitControllerId++;

   Timing _timing;  // timing object for simulation
   std::map<std::string, std::shared_ptr<IVariable>> _variablesMap;
   std::vector<std::shared_ptr<IVariable>> _variables;
   LandUnitController& _landUnitController;

   const configuration::Configuration* _config;
};

}  // namespace moja::flint
