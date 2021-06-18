#include "moja/flint/compositetransform.h"

#include "moja/flint/flintexceptions.h"
#include "moja/flint/ilandunitcontroller.h"
#include "moja/flint/ivariable.h"

#include <boost/algorithm/string.hpp>

using moja::flint::IncompleteConfigurationException;
using moja::flint::VariableNotFoundException;

namespace moja {
namespace flint {

void CompositeTransform::configure(DynamicObject config, const ILandUnitController& landUnitController,
                                   moja::datarepository::DataRepository& dataRepository) {
   _landUnitController = &landUnitController;
   _dataRepository = &dataRepository;

   if (!config.contains("vars")) {
      BOOST_THROW_EXCEPTION(IncompleteConfigurationException() << Item("vars"));
   }

   auto varNames = config["vars"];
   if (varNames.size() < 1) {
      BOOST_THROW_EXCEPTION(IncompleteConfigurationException()
                            << Item("vars") << Details("At least one variable name required"));
   }

   for (std::string varName : varNames) {
      if (std::find(_varNames.begin(), _varNames.end(), varName) != _varNames.end()) {
         BOOST_THROW_EXCEPTION(IncompleteConfigurationException()
                               << Item("vars") << Details("Duplicate variable reference"));
      }

      _varNames.push_back(varName);
   }

   _format = Format::Wide;
   if (config.contains("format")) {
      std::string format = config["format"];
      if (format == "long") {
         _format = Format::Long;
      }
   }

   if (config.contains("allow_nulls")) {
      _allowNulls = config["allow_nulls"];
   }
}

void CompositeTransform::controllerChanged(const ILandUnitController& controller) {
   _cachedValue = nullptr;
   _landUnitController = &controller;
};

const DynamicVar& CompositeTransform::value() const {
   if (_variables.empty()) {
      for (auto varName : _varNames) {
         auto var = _landUnitController->getVariable(varName);
         if (var == nullptr) {
            BOOST_THROW_EXCEPTION(VariableNotFoundException() << VariableName(varName));
         }

         _variables.push_back(var);
      }
   }

   if (_format == Format::Long) {
      return longFormatValue();
   }

   return wideFormatValue();
}

const DynamicVar& CompositeTransform::longFormatValue() const {
   std::vector<DynamicObject> result;
   for (const auto var : _variables) {
      const auto& value = var->value();
      if (value.isEmpty()) {
         continue;
      }

      if (value.isStruct()) {
         auto& valueStruct = value.extract<DynamicObject>();
         result.push_back(valueStruct);
      } else if (value.isVector()) {
         auto& valueVec = value.extract<std::vector<DynamicObject>>();
         for (auto& row : valueVec) {
            result.push_back(row);
         }
      }
   }

   if (result.empty()) {
      _cachedValue = DynamicVar();
   } else {
      _cachedValue = result;
   }

   return _cachedValue;
}

const DynamicVar& CompositeTransform::wideFormatValue() const {
   DynamicObject result;
   for (const auto var : _variables) {
      const auto& value = var->value();
      if (!_allowNulls && value.isEmpty()) {
         _cachedValue = DynamicVar();
         return _cachedValue;
      }

      if (value.isStruct()) {
         auto& valueStruct = value.extract<DynamicObject>();
         for (auto& kvp : valueStruct) {
            result.insert(kvp.first, kvp.second);
         }
      } else if (value.isVector()) {
         auto& valueVec = value.extract<std::vector<DynamicObject>>();
         for (auto& kvp : valueVec[0]) {
            result.insert(kvp.first, kvp.second);
         }
      } else {
         result.insert(var->info().name, value);
      }
   }

   _cachedValue = result;
   return _cachedValue;
}

}  // namespace flint
}  // namespace moja
