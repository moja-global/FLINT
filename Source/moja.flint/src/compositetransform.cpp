#include "moja/flint/compositetransform.h"

#include "moja/flint/flintexceptions.h"
#include "moja/flint/ilandunitcontroller.h"
#include "moja/flint/ivariable.h"

#include <boost/algorithm/string.hpp>

namespace moja {
namespace flint {

void CompositeTransform::configure(DynamicObject config, const ILandUnitController& landUnitController,
                                   moja::datarepository::DataRepository& dataRepository) {
   _landUnitController = &landUnitController;
   _dataRepository = &dataRepository;

   if (!config.contains("vars")) {
      throw std::runtime_error("Error composite transform settings missing vars");
   }

   auto varNames = config["vars"];
   if (varNames.size() < 1) {
      throw std::runtime_error("Error composite transform vars at least one variable name required");
   }

   for (std::string varName : varNames) {
      if (std::find(_varNames.begin(), _varNames.end(), varName) != _varNames.end()) {
         throw std::runtime_error("Error composite transform vars duplicate variable reference");
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
      for (const auto& varName : _varNames) {
         const auto* var = _landUnitController->getVariable(varName);
         if (var == nullptr) {
            throw std::runtime_error("Error composite transform variable not found " + varName);
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
