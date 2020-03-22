#include "moja/flint/landunitcontroller.h"

#include "moja/flint/configuration/configuration.h"
#include "moja/flint/configuration/localdomain.h"
#include "moja/flint/flintexceptions.h"
#include "moja/flint/ioperation.h"
#include "moja/flint/ivariable.h"
#include "moja/flint/operationmanagersimple.h"
#include "moja/flint/operationmanagersimplecache.h"
#include "moja/flint/operationmanagerublas.h"
#include "moja/flint/uncertaintyvariable.h"

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

void LandUnitController::setVariable(std::string name, std::shared_ptr<IVariable> variable) {
   if (name.length() == 0 || all(name, boost::algorithm::is_space())) {
      throw std::invalid_argument("name cannot be empty");
   }
   const auto v = _variablesMap.find(name);
   if (v == _variablesMap.end()) {
      MOJA_LOG_FATAL << "Variable not found: " << name;
      BOOST_THROW_EXCEPTION(VariableNotFoundException() << VariableName(name));
   }
   _variablesMap.insert_or_assign(name, variable.get());
   std::replace_if(
       _variables.begin(), _variables.end(), [&](std::shared_ptr<IVariable> v) { return v->info().name == name; },
       variable);
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

void LandUnitController::configure_uncertainty(const configuration::Uncertainty& uncertainty) {
   if (!uncertainty.enabled()) return;
   _uncertainty.set_enabled(true);
   _uncertainty.set_iterations(uncertainty.iterations());
   for (auto& variable : uncertainty.variables()) {
      auto var = std::find_if(_variables.begin(), _variables.end(),
                              [&](std::shared_ptr<IVariable> v) { return v->info().name == variable.variable(); });
      if (var == _variables.end()) {
         MOJA_LOG_FATAL << "Variable not found: " << variable.variable();
         BOOST_THROW_EXCEPTION(VariableNotFoundException() << VariableName(variable.variable()));
      }
      auto uncertainty_var = std::make_shared<UncertaintyVariable>(*this, *var);
      uncertainty_var->set_name(variable.variable());
      for (auto& replacement : variable.replacements()) {
         auto& replacement_var = uncertainty_var->replacements().emplace_back(replacement.query());
         for (auto& field : replacement.fields()) {
            switch (field->type) {
               case configuration::UncertaintyField::FieldType::Manual: {
                  const auto config_field_manual =
                      std::static_pointer_cast<configuration::UncertaintyFieldManual>(field);
                  auto field_manual = std::make_shared<UncertaintyFieldManual>();
                  field_manual->key = config_field_manual->key;
                  field_manual->set_distribution(config_field_manual->distribution);
                  replacement_var.fields().emplace_back(field_manual);
               } break;
               case configuration::UncertaintyField::FieldType::Normal: {
                  const auto config_field_normal =
                      std::static_pointer_cast<configuration::UncertaintyFieldNormal>(field);
                  auto field_normal = std::make_shared<UncertaintyFieldNormal>();
                  field_normal->key = config_field_normal->key;
                  field_normal->mean = config_field_normal->mean;
                  field_normal->std_dev = config_field_normal->std_dev;
                  field_normal->seed = config_field_normal->seed;
                  field_normal->build_distribution(uncertainty.iterations());
                  replacement_var.fields().emplace_back(field_normal);
               } break;
               case configuration::UncertaintyField::FieldType::Triangular: {
                  const auto config_field_triangular =
                      std::static_pointer_cast<configuration::UncertaintyFieldTriangular>(field);
                  auto field_triangular = std::make_shared<UncertaintyFieldTriangular>();
                  field_triangular->key = config_field_triangular->key;
                  field_triangular->min = config_field_triangular->min;
                  field_triangular->max = config_field_triangular->max;
                  field_triangular->peak = config_field_triangular->peak;
                  field_triangular->seed = config_field_triangular->seed;
                  field_triangular->build_distribution(uncertainty.iterations());
                  replacement_var.fields().emplace_back(field_triangular);
               } break;
               default:;
            }
         }
      }
      setVariable(variable.variable(), uncertainty_var);
   }
}

const Uncertainty& LandUnitController::uncertainty() const { return _uncertainty; }
Uncertainty& LandUnitController::uncertainty() { return _uncertainty; }

const ITiming& LandUnitController::timing() const { return _timing; }

}  // namespace flint
}  // namespace moja
