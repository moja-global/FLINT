#include "moja/flint/configuration/spinup.h"

#include "moja/flint/configuration/configuration.h"
#include "moja/flint/configuration/configurationexceptions.h"
#include "moja/flint/configuration/externalvariable.h"
#include "moja/flint/configuration/flintdatavariable.h"
#include "moja/flint/configuration/localdomain.h"
#include "moja/flint/configuration/variable.h"

#include <boost/algorithm/string.hpp>

namespace moja {
namespace flint {
namespace configuration {

SpinupModule::SpinupModule(const std::string& libraryName, const std::string& name, int order, bool createNew,
                           DynamicObject settings)
    : _libraryName(libraryName), _name(name), _order(order), _createNew(createNew), _settings(settings) {
   if (libraryName.length() == 0 || all(libraryName, boost::algorithm::is_space())) {
      throw std::invalid_argument("library name cannot be empty");
   }

   if (name.length() == 0 || all(name, boost::algorithm::is_space())) {
      throw std::invalid_argument("name cannot be empty");
   }
}

void Spinup::addVariable(const std::string& name, moja::DynamicVar value) {
   auto variable = std::make_shared<Variable>(name, value);
   _variables.push_back(variable);
}

void Spinup::addExternalVariable(const std::string& name, const std::string& libraryName,
                                 const std::string transformTypeName, DynamicObject settings) {
   auto transform = std::make_shared<Transform>(libraryName, transformTypeName, settings);
   auto variable = std::make_shared<ExternalVariable>(name, transform);
   _externalVariables.push_back(variable);
}

void Spinup::addFlintDataVariable(const std::string& name, const std::string& libraryName,
                                  const std::string flintDataTypeName, DynamicObject settings) {
   auto flintData = std::make_shared<FlintData>(libraryName, flintDataTypeName, settings);
   auto variable = std::make_shared<FlintDataVariable>(name, flintData);
   _flintDataVariables.push_back(variable);
}

void Spinup::addSpinupModule(const std::string& libraryName, const std::string& name, int order, bool createNew,
                             DynamicObject settings) {
   // It is an error for two modules to have the same order - simulation
   // results could be inconsistent.
   auto sameOrder = std::find_if(_modules.begin(), _modules.end(),
                                 [order](std::shared_ptr<SpinupModule> other) { return other->order() == order; });

   if (sameOrder != _modules.end()) {
      throw SpinupModuleOrderOverlapException()
          << Order(order) << SpinupModuleNames({libraryName, name, (*sameOrder)->name()});
   }

   auto module = std::make_shared<SpinupModule>(libraryName, name, order, createNew, settings);
   _modules.push_back(module);
   std::sort(_modules.begin(), _modules.end(),
             [](std::shared_ptr<SpinupModule> lhs, std::shared_ptr<SpinupModule> rhs) {
                return lhs->order() < rhs->order();
             });
}

Spinup::Spinup(bool enabled, const std::string& sequencerLibrary, const std::string& sequencerName,
               const std::string& simulateLandUnit, const std::string& landUnitBuildSuccess, DynamicObject settings)
    : _enabled(enabled),
      _sequencerLibrary(sequencerLibrary),
      _sequencerName(sequencerName),
      _simulateLandUnit(simulateLandUnit),
      _landUnitBuildSuccess(landUnitBuildSuccess),
      _settings(settings) {}

}  // namespace configuration
}  // namespace flint
}  // namespace moja
