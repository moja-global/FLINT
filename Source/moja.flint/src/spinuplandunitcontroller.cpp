#include "moja/flint/spinuplandunitcontroller.h"

#include "moja/flint/flintexceptions.h"
#include "moja/flint/imodule.h"
#include "moja/flint/ioperation.h"
#include "moja/flint/ivariable.h"
#include "moja/flint/landunitcontroller.h"

#include <moja/dynamic.h>

#include <boost/algorithm/string.hpp>

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------

int SpinupLandUnitController::_currentLandUnitControllerId = 2000;

// --------------------------------------------------------------------------------------------

SpinupLandUnitController::SpinupLandUnitController(LandUnitController& landUnitController)
    : _landUnitController(landUnitController), _config(nullptr) {}

SpinupLandUnitController::~SpinupLandUnitController() {}

void SpinupLandUnitController::configure(const configuration::Configuration& config) {
   // Keep pointer to config
   _config = &config;
}

void SpinupLandUnitController::run() {}

void SpinupLandUnitController::initialiseData(bool inLandUnit) {
   if (inLandUnit) {
      initialisePools();
   }
}

// Operations
std::shared_ptr<IOperation> SpinupLandUnitController::createStockOperation(IModule& module) {
   return _landUnitController.createStockOperation(module);
}

std::shared_ptr<IOperation> SpinupLandUnitController::createStockOperation(IModule& module, DynamicVar& dataPackage) {
   return _landUnitController.createStockOperation(module, dataPackage);
}

std::shared_ptr<IOperation> SpinupLandUnitController::createProportionalOperation(IModule& module) {
   return _landUnitController.createProportionalOperation(module);
}

std::shared_ptr<IOperation> SpinupLandUnitController::createProportionalOperation(IModule& module,
                                                                                  DynamicVar& dataPackage) {
   return _landUnitController.createProportionalOperation(module, dataPackage);
}

void SpinupLandUnitController::submitOperation(std::shared_ptr<IOperation> operation) {
   _landUnitController.submitOperation(operation);
}

void SpinupLandUnitController::applyOperations() { _landUnitController.applyOperations(); }

void SpinupLandUnitController::clearLastAppliedOperationResults() {
   _landUnitController.clearLastAppliedOperationResults();
}

void SpinupLandUnitController::clearAllOperationResults() { _landUnitController.clearAllOperationResults(); }

void SpinupLandUnitController::resetVariables() {
   for (auto& variable : _variablesMap) {
      variable.second->reset_value();
   }
}

void SpinupLandUnitController::initialisePools() const { _landUnitController.initialisePools(); }

void SpinupLandUnitController::addVariable(std::string name, std::shared_ptr<IVariable> variable) {
   if (name.length() == 0 || all(name, boost::algorithm::is_space())) {
      throw std::invalid_argument("name cannot be empty");
   }

   _variablesMap.insert(std::make_pair(name, variable));
   _variables.push_back(variable);
}

IVariable* SpinupLandUnitController::getVariable(const std::string& name) {
   const auto v = _variablesMap.find(name);
   if (v == _variablesMap.end()) {
      std::runtime_error("Error in spin up landunit controller variable not found " + name);
   }
   return v->second.get();
}

const IVariable* SpinupLandUnitController::getVariable(const std::string& name) const {
   const auto v = _variablesMap.find(name);
   if (v == _variablesMap.end()) {
      std::runtime_error("Error in spin up landunit controller variable not found " + name);
   }
   return v->second.get();
}

bool SpinupLandUnitController::hasVariable(const std::string& name) const {
   return _variablesMap.find(name) != _variablesMap.end();
}

ITiming& SpinupLandUnitController::timing() { return _timing; }

IOperationManager* SpinupLandUnitController::operationManager() { return _landUnitController.operationManager(); }

IOperationManager* SpinupLandUnitController::operationManager() const {
   return _landUnitController.operationManager();
   ;
}

const ITiming& SpinupLandUnitController::timing() const { return _timing; }

}  // namespace flint
}  // namespace moja
