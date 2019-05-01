#include "moja/flint/lookuptransform.h"

#include "moja/flint/flintexceptions.h"
#include "moja/flint/ilandunitcontroller.h"
#include "moja/flint/ivariable.h"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

using moja::flint::IncompleteConfigurationException;
using moja::flint::VariableNotFoundException;

namespace moja {
namespace flint {

void LookupTransform::configure(
	DynamicObject config,
	const ILandUnitController& landUnitController,
	moja::datarepository::DataRepository& dataRepository) {

	for (auto key : { "from", "to" }) {
		bool validConfiguration = false;
		if (config.contains(key)) {
			std::string value = config[key];
			if (value.length() > 0 && !all(value, boost::algorithm::is_space())) {
				validConfiguration = true;
			}
		}

		if (!validConfiguration) {
			throw IncompleteConfigurationException()
				<< Item(key)
				<< Details("Expected configuration item not found");
		}
	}

	_fromVarName = config["from"].convert<std::string>();
	_toVarName = config["to"].convert<std::string>();
	_landUnitController = &landUnitController;
	_dataRepository = &dataRepository;
}

void LookupTransform::controllerChanged(const ILandUnitController& controller) {
    _cachedValue = nullptr;
	_landUnitController = &controller;
};

const DynamicVar& LookupTransform::value() const {
	auto from = _landUnitController->getVariable(_fromVarName);
	if (from == nullptr) {
		throw VariableNotFoundException() << VariableName(_fromVarName);
	}

	auto to = _landUnitController->getVariable(_toVarName);
	if (to == nullptr) {
		throw VariableNotFoundException() << VariableName(_toVarName);
	}

	auto& fromValue = from->value();
	auto& toValue = to->value();
	if (fromValue.isEmpty() || toValue.isEmpty()) {
		setCachedValue(DynamicVar());
		return _cachedValue;
	}

	if (!fromValue.isStruct()) {
		if (toValue.isStruct()) {
			return matchDynamicToStruct(fromValue, toValue);
		}

		return matchDynamicToMultiStruct(fromValue, toValue);
	}

	return matchStructToMultiStruct(fromValue, toValue);
}

const DynamicVar& LookupTransform::matchDynamicToStruct(const DynamicVar& from, const DynamicVar& to) const {
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
		setCachedValue(toValue[key]);
    }
    else {
        setCachedValue(DynamicVar());
    }

	return _cachedValue;
}

const DynamicVar& LookupTransform::matchDynamicToMultiStruct(const DynamicVar& from, const DynamicVar& to) const {
	setCachedValue(DynamicVar());
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
				setCachedValue(result);
			}
			else {
				setCachedValue(result.begin()->second);
			}

			break;
		}
	}

	return _cachedValue;
}

const DynamicVar& LookupTransform::matchStructToMultiStruct(const DynamicVar& from, const DynamicVar& to) const {
	setCachedValue(DynamicVar());
	auto fromValue = from.extract<DynamicObject>();
	for (const auto& row : to) {
        const auto& toRow = row.extract<DynamicObject>();
		bool match = true;
		for (auto kvp : fromValue) {
            auto toKvp = toRow.find(kvp.first);
            if (toKvp == toRow.end()) {
                continue;
            }

            if (kvp.second != toKvp->second) {
				match = false;
				break;
			}
		}

		if (match) {
			DynamicObject result;
            for (const auto& item : toRow) {
				if (fromValue.contains(item.first)) {
					continue;
				}

				result[item.first] = item.second;
			}

			if (result.size() > 1) {
				setCachedValue(result);
			} else {
				setCachedValue(result.begin()->second);
			}

			break;
		}
	}

	return _cachedValue;
}

}
} // namespace moja::flint
