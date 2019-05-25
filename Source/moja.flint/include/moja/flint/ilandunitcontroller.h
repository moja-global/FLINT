#ifndef MOJA_FLINT_ILANDUNITCONTROLLER_H_
#define MOJA_FLINT_ILANDUNITCONTROLLER_H_

#include "moja/flint/_flint_exports.h"

#include <moja/dynamic.h>

#include <string>
#include <vector>

namespace moja {
namespace flint {

namespace configuration {
class Configuration;
}

class IModule;
class IOperation;
class IVariable;
class ITiming;
class IOperationManager;

class FLINT_API ILandUnitController {
  public:
   virtual ~ILandUnitController(void) = default;

   // configure the simulation
   // virtual void configure(const std::string& config) = 0;
   virtual void configure(const configuration::Configuration& config) = 0;

   // execute the simulation
   virtual void run() = 0;

   // Unique Id for each controller
   virtual int id() = 0;

   // Operations
   virtual std::shared_ptr<IOperation> createStockOperation(IModule& module) = 0;
   virtual std::shared_ptr<IOperation> createStockOperation(IModule& module, DynamicVar& dataPackage) = 0;
   virtual std::shared_ptr<IOperation> createProportionalOperation(IModule& module) = 0;
   virtual std::shared_ptr<IOperation> createProportionalOperation(IModule& module, DynamicVar& dataPackage) = 0;
   virtual void submitOperation(std::shared_ptr<IOperation> operation) = 0;
   virtual void applyOperations() = 0;
   virtual void clearLastAppliedOperationResults() = 0;
   virtual void clearAllOperationResults() = 0;

   // Operation Manager
   virtual IOperationManager* operationManager() = 0;
   virtual const IOperationManager* operationManager() const = 0;

   // Data-Set accessors
   virtual IVariable* getVariable(const std::string& name) = 0;
   virtual const IVariable* getVariable(const std::string& name) const = 0;
   virtual std::vector<std::shared_ptr<IVariable>> variables() const = 0;
   virtual bool hasVariable(const std::string& name) const = 0;

   virtual const ITiming& timing() const = 0;
   virtual ITiming& timing() = 0;

   virtual const configuration::Configuration* config() = 0;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_ILANDUNITCONTROLLER_H_
