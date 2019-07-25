#include "moja/flint/landunitcontroller.h"

#include "moja/flint/flintexceptions.h"
#include "moja/flint/ioperation.h"
#include "moja/flint/ivariable.h"
#include "moja/flint/operationmanagersimple.h"
#include "moja/flint/operationmanagersimplecache.h"
#include "moja/flint/operationmanagerublas.h"

#include <moja/flint/configuration/configuration.h>
#include <moja/flint/configuration/localdomain.h>

#include <moja/dynamic.h>
#include <moja/logging.h>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------

int LandUnitController::_currentLandUnitControllerId = 1000;

// --------------------------------------------------------------------------------------------

LandUnitController::LandUnitController() : _config(nullptr) {
   _operationManager = std::make_shared<OperationManagerSimple>(_timing, DynamicObject());  // default
}

LandUnitController::~LandUnitController() {}

void LandUnitController::configure(const configuration::Configuration& config) {
   // Keep pointer to config
   _config = &config;

   auto const operationManagerConfig = config.localDomain()->operationManagerObject();

   if (operationManagerConfig->name() == "Ublas") {
      _operationManager = std::make_shared<OperationManagerUblas>(_timing, operationManagerConfig->settings());
   } else if (operationManagerConfig->name() == "Simple") {
      _operationManager = std::make_shared<OperationManagerSimple>(_timing, operationManagerConfig->settings());
   } else if (operationManagerConfig->name() == "SimpleCache") {
      _operationManager = std::make_shared<OperationManagerSimpleCache>(_timing, operationManagerConfig->settings());
   }

   _timing.setStepping(TimeStepping(config.localDomain()->timeStepping()));
}

void LandUnitController::run() {}

void LandUnitController::initialiseData(bool inLandUnit) {
   if (inLandUnit) {
      initialisePools();
   }
}

std::shared_ptr<IOperation> LandUnitController::createStockOperation(IModule& module) {
   return _operationManager->createStockOperation(module);
}

std::shared_ptr<IOperation> LandUnitController::createStockOperation(IModule& module, DynamicVar& dataPackage) {
   return _operationManager->createStockOperation(module, dataPackage);
}

std::shared_ptr<IOperation> LandUnitController::createProportionalOperation(IModule& module) {
   return _operationManager->createProportionalOperation(module);
}

std::shared_ptr<IOperation> LandUnitController::createProportionalOperation(IModule& module, DynamicVar& dataPackage) {
   return _operationManager->createProportionalOperation(module, dataPackage);
}

void LandUnitController::submitOperation(std::shared_ptr<IOperation> operation) { operation->submitOperation(); }

void LandUnitController::applyOperations() { _operationManager->applyOperations(); }

void LandUnitController::clearLastAppliedOperationResults() { _operationManager->clearLastAppliedOperationResults(); }

void LandUnitController::clearAllOperationResults() { _operationManager->clearAllOperationResults(); }

void LandUnitController::resetVariables() const {
   for (auto& variable : _variables) {
      variable->reset_value();
   }
}

void LandUnitController::initialisePools() const { _operationManager->initialisePools(); }

void LandUnitController::addVariable(std::string name, std::shared_ptr<IVariable> variable) {
   if (name.length() == 0 || all(name, boost::algorithm::is_space())) {
      throw std::invalid_argument("name cannot be empty");
   }
   _variablesMap.insert(std::make_pair(name, variable.get()));
   _variables.push_back(variable);
}

IVariable* LandUnitController::getVariable(const std::string& name) {
   const auto v = _variablesMap.find(name);
   if (v == _variablesMap.end()) {
      MOJA_LOG_FATAL << "Variable not found: " << name;
      BOOST_THROW_EXCEPTION(VariableNotFoundException() << VariableName(name));
   }
   return v->second;
}

const IVariable* LandUnitController::getVariable(const std::string& name) const {
   const auto v = _variablesMap.find(name);
   if (v == _variablesMap.end()) {
      MOJA_LOG_FATAL << "Variable not found: " << name;
      BOOST_THROW_EXCEPTION(VariableNotFoundException() << VariableName(name));
   }
   return v->second;
}

bool LandUnitController::hasVariable(const std::string& name) const {
   return _variablesMap.find(name) != _variablesMap.end();
}

ITiming& LandUnitController::timing() { return _timing; }

const ITiming& LandUnitController::timing() const { return _timing; }

}  // namespace flint
}  // namespace moja
