
#include "moja/flint/lookuprandomtransform.h"

#include "moja/flint/flintexceptions.h"
#include "moja/flint/ilandunitcontroller.h"
#include "moja/flint/ivariable.h"
#include "moja/flint/spatiallocationinfo.h"

#include <boost/algorithm/string.hpp>

#include <random>

using moja::flint::IncompleteConfigurationException;
using moja::flint::VariableNotFoundException;

namespace moja {
namespace flint {

void LookupRandomTransform::configure(DynamicObject config, const ILandUnitController& landUnitController,
                                      moja::datarepository::DataRepository& dataRepository) {
   // for (auto key : { "from", "to" }) {
   for (auto key : {"to"}) {
      auto validConfiguration = false;
      if (config.contains(key)) {
         std::string value = config[key];
         if (value.length() > 0 && !all(value, boost::algorithm::is_space())) {
            validConfiguration = true;
         }
      }
      if (!validConfiguration) {
         throw IncompleteConfigurationException() << Item(key) << Details("Expected configuration item not found");
      }
   }
   //_fromVarName = config["from"].convert<std::string>();
   _toVarName = config["to"].convert<std::string>();
   _reverseLookup = config.contains("reverse") ? bool(config["reverse"]) : false;
   _landUnitController = &landUnitController;
   _dataRepository = &dataRepository;
   _spatiallocationinfo = std::static_pointer_cast<flint::SpatialLocationInfo>(
       _landUnitController->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<flint::IFlintData>>());
}

void LookupRandomTransform::controllerChanged(const ILandUnitController& controller) {
   _landUnitController = &controller;
   _spatiallocationinfo = std::static_pointer_cast<flint::SpatialLocationInfo>(
       _landUnitController->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<flint::IFlintData>>());
}

const DynamicVar& LookupRandomTransform::value() const {
   // auto from = _landUnitController->getVariable(_fromVarName);
   // if (from == nullptr) {
   //	throw VariableNotFoundException() << VariableName(_fromVarName);
   //}

   auto to = _landUnitController->getVariable(_toVarName);
   if (to == nullptr) {
      throw VariableNotFoundException() << VariableName(_toVarName);
   }

   // auto& fromValue = from->value();
   auto& toValue = to->value();
   if (/*fromValue.isEmpty() || */ toValue.isEmpty()) {
      _cachedValue = DynamicVar();
      return _cachedValue;
   }

   // if (!fromValue.isStruct()) {
   if (toValue.isStruct()) {
      return matchDynamicToStruct(toValue);
   }

   return matchDynamicToMultiStruct(toValue);
   //}

   return matchStructToMultiStruct(toValue);
}

const DynamicVar& LookupRandomTransform::matchDynamicToStruct(const DynamicVar& to) const {
   auto toValue = to.extract<DynamicObject>();
   std::uniform_int_distribution<> distr(0, static_cast<int>(toValue.size() - 1));  // define the range
   int returnIdx = distr(_spatiallocationinfo->_engCell);

   _cachedValue = DynamicVar();

   int count = 0;
   for (auto& item : toValue) {
      if (count == returnIdx) {
         _cachedValue = _reverseLookup ? DynamicVar(item.first) : item.second;
         break;
      }
      count++;
   }
#if 0
	_cachedValue = Dynamic();
	std::string key = "%1%";
	if (from.isInteger()) {
		int intkey = from.convert<int>();
		key = (boost::format(key) % intkey).str();
	}
	else {
		key = from.convert<std::string>();
	}
	auto toValue = to.extract<DynamicObject>();
	if (toValue.contains(key)) {
		_cachedValue = toValue[key];
	}
#endif
   return _cachedValue;
}

const DynamicVar& LookupRandomTransform::matchDynamicToMultiStruct(const DynamicVar& to) const {
   _cachedValue = DynamicVar();
#if 1
   std::uniform_int_distribution<> distr(0, static_cast<int>(to.size() - 1));  // define the range
   int returnIdx = distr(_spatiallocationinfo->_engCell);
   auto& row = to[returnIdx];
   _cachedValue = DynamicVar();
   DynamicObject result;
   for (const auto& item : row.extract<DynamicObject>()) {
      result[item.first] = item.second;
   }
   if (result.size() > 1) {
      _cachedValue = result;
   } else {
      _cachedValue = result.begin()->second;
   }
#endif

#if 0
	for (const auto& row : to) {
		if (row[_fromVarName] == from) {
			DynamicObject result;
			for (const auto& item : row.extract<DynamicObject>()) {
				if (item.first == _fromVarName) {
					continue;
				}
				result[item.first] = item.second;
			}

			if (result.size() > 1) {
				_cachedValue = result;
			}
			else {
				_cachedValue = result.begin()->second;
			}
			break;
		}
	}
#endif
   return _cachedValue;
}

const DynamicVar& LookupRandomTransform::matchStructToMultiStruct(const DynamicVar& to) const {
   _cachedValue = DynamicVar();
#if 0
	auto fromValue = from.extract<DynamicObject>();
	for (const auto& row : to) {
		bool match = true;
		for (auto kvp : fromValue) {
			if (row[kvp.first] != kvp.second) {
				match = false;
				break;
			}
		}

		if (match) {
			DynamicObject result;
			for (const auto& item : row.extract<DynamicObject>()) {
				if (fromValue.contains(item.first)) {
					continue;
				}
				result[item.first] = item.second;
			}
			if (result.size() > 1) {
				_cachedValue = result;
			}
			else {
				_cachedValue = result.begin()->second;
			}
			break;
		}
	}
#endif
   return _cachedValue;
}

}  // namespace flint
}  // namespace moja
