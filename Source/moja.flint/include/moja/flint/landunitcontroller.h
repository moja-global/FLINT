#ifndef MOJA_FLINT_LANDUNITCONTROLLER_H_
#define MOJA_FLINT_LANDUNITCONTROLLER_H_

#include "moja/flint/_flint_exports.h"
#include "moja/flint/ilandunitcontroller.h"
#include "moja/flint/timing.h"
#include "moja/flint/uncertaintyvariable.h"

#include <string>
#include <vector>

namespace moja {
namespace flint {

namespace configuration {
class Uncertainty;
class Configuration;
}

class IVariable;
class IOperation;
class IOperationManager;
class IModule;
class ITiming;

// --------------------------------------------------------------------------------------------

class FLINT_API LandUnitController : public ILandUnitController {
  public:
   LandUnitController();
   ~LandUnitController();

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

   void submitOperation(std::shared_ptr<IOperation> iOperation) override;
   void applyOperations() override;
   void clearLastAppliedOperationResults() override;
   void clearAllOperationResults() override;

   // Can be used to reset Variables back to original values
   void resetVariables() const;

   // Variables
   std::vector<std::shared_ptr<IVariable>> variables() const override { return _variables; }

   void addVariable(std::string name, std::shared_ptr<IVariable> variable);
   void setVariable(std::string name, std::shared_ptr<IVariable> variable);
   IVariable* getVariable(const std::string& name) override;
   const IVariable* getVariable(const std::string& name) const override;
   bool hasVariable(const std::string& name) const override;

   const ITiming& timing() const override;
   ITiming& timing() override;

   IOperationManager* operationManager() override { return _operationManager.get(); }
   const IOperationManager* operationManager() const override { return _operationManager.get(); }

   const configuration::Configuration* config() override { return _config; }
   void configure_uncertainty(const configuration::Uncertainty& uncertainty);
   const Uncertainty& uncertainty() const override;
   Uncertainty& uncertainty() override;

  private:
   static int _currentLandUnitControllerId;
   const int _landUnitControllerId = _currentLandUnitControllerId++;

   Timing _timing;
   std::map<std::string, IVariable*> _variablesMap;
   std::vector<std::shared_ptr<IVariable>> _variables;
   std::shared_ptr<IOperationManager> _operationManager;

   const configuration::Configuration* _config;
   Uncertainty _uncertainty;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_LANDUNITCONTROLLER_H_
