#include "moja/flint/variableandpoolstringbuilder.h"
#include "moja/flint/sqlquerytransform.h"
#include "moja/flint/iflintdata.h"
#include "moja/flint/ioperationmanager.h"
#include "moja/flint/ivariable.h"
#include "moja/flint/ipool.h"

#include <moja/datetime.h>

#include <boost/format.hpp>

// stringstreams
#include <string> 
#include <sstream> 


namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------

VariableAndPoolStringBuilder::VariableAndPoolStringBuilder(
	const flint::ILandUnitController& landUnitController,
	const std::string& workingStr) : _landUnitController(nullptr), _landUnitDataWrapper(nullptr) {
	configure(landUnitController, workingStr);
}

VariableAndPoolStringBuilder::VariableAndPoolStringBuilder(
	const flint::ILandUnitDataWrapper* landUnitWrapper, 
	const std::string&  workingStr) : _landUnitController(nullptr), _landUnitDataWrapper(nullptr) {
	configure(landUnitWrapper, workingStr);
}

void VariableAndPoolStringBuilder::configure(
	const flint::ILandUnitController& landUnitController,
	const std::string&  workingStr) {

	_landUnitController = &landUnitController;
	_workingStr = workingStr;

	// Search string for pool names.
	auto poolNames = extractTokens(poolMarker, _workingStr);
	for (auto name : poolNames) {
		_pools.push_back(landUnitController.operationManager()->getPool(name));
	}

	// Search string for variable names.
	auto varNames = extractTokens(varMarker, _workingStr);
	for (auto var : varNames) {
		auto propertySeparator = var.find_first_of('.');
		if (propertySeparator != std::string::npos) {
			// Reference to a multi-property variable.
			auto prop = var.substr(propertySeparator + 1, var.length());
			var = var.substr(0, propertySeparator);
			if (landUnitController.hasVariable(var)) {
				auto val = landUnitController.getVariable(var);
				_variables.push_back(std::make_tuple(val, prop));
			} else {
				_variables.push_back(std::make_tuple(nullptr, prop));
			}
		}
		else {
			if (landUnitController.hasVariable(var)) {
				// Reference to a single-value variable.
				auto val = landUnitController.getVariable(var);
				_variables.push_back(std::make_tuple(val, ""));
			} else {
				_variables.push_back(std::make_tuple(nullptr, ""));
			}
		}
	}
}

void VariableAndPoolStringBuilder::configure(
	const flint::ILandUnitDataWrapper* landUnitWrapper,
	const std::string&  workingStr) {

	_landUnitDataWrapper = landUnitWrapper;
	_workingStr = workingStr;

	// Search string for pool names.
	auto poolNames = extractTokens(poolMarker, _workingStr);
	for (auto name : poolNames) {
		_pools.push_back(landUnitWrapper->operationManager()->getPool(name));
	}

	// Search string for variable names.
	auto varNames = extractTokens(varMarker, _workingStr);
	for (auto var : varNames) {
		auto propertySeparator = var.find_first_of('.');
		if (propertySeparator != std::string::npos) {
			// Reference to a multi-property variable.
			auto prop = var.substr(propertySeparator + 1, var.length());
			var = var.substr(0, propertySeparator);
			if (landUnitWrapper->hasVariable(var)) {
				auto val = landUnitWrapper->getVariable(var);
				_variables.push_back(std::make_tuple(val, prop));
			} else {
				_variables.push_back(std::make_tuple(nullptr, prop));
			}
		}
		else {
			// Reference to a single-value variable.
			if (landUnitWrapper->hasVariable(var)) {
				auto val = landUnitWrapper->getVariable(var);
				_variables.push_back(std::make_tuple(val, ""));
			} else {
				_variables.push_back(std::make_tuple(nullptr, ""));
			}
		}
	}
}


// Searching for tokens of the form "{<type>:name[.property]}", for example "{pool:totalCM}"
// or "{var:Fractions.clay}" and return token names.
std::vector<std::string> VariableAndPoolStringBuilder::extractTokens(const std::string& tokenType, const std::string& query) {
	std::string tokenStart = "{" + tokenType + ":";
	char tokenEnd = '}';
	auto tokenStartLen = tokenStart.length();
	auto tokenStartPos = query.find(tokenStart); // look for a token start

	std::vector<std::string> tokens;
	while (tokenStartPos != std::string::npos) {
		auto tokenValueStartPos = tokenStartPos + tokenStartLen;
		auto tokenEndPos = query.find(tokenEnd, tokenStartPos + 1); // look for the end of the token
		if (tokenEndPos != std::string::npos && tokenEndPos > tokenValueStartPos) {
			auto tokenValueLen = tokenEndPos - tokenValueStartPos;
			tokens.push_back(query.substr(tokenValueStartPos, tokenValueLen));
			tokenStartPos = query.find(tokenStart, tokenEndPos + 1);
		}
		else { // didn't find an end token marker so ignore this token
			tokenStartPos = query.find(tokenStart, tokenStartPos + 1);
		}
	}

	return tokens;
}

// --------------------------------------------------------------------------------------------

std::string escape_json2_XX(const std::string &s) {
	std::ostringstream o;
	for (auto c = s.cbegin(); c != s.cend(); ++c) {
		if (*c == '"' || *c == '\\' || ('\x00' <= *c && *c <= '\x1f'))
			//o << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(*c);
			o << "\\";
		o << *c;
	}
	return o.str();
}

// --------------------------------------------------------------------------------------------

void outputDynamicToStream_XX(std::ostream& fout, const DynamicVar& object) {
	if (object.isStruct()) {
		fout << "\"[struct]\"";
	}
	else if (object.isVector()) {
		fout << "\"[vector]\"";
	}
	else if (object.isBoolean()) {
		bool val = object;
		fout << (val ? "true" : "false");
	}
	else if (object.isEmpty()) {
		fout << "\"[null]\"";
	}
	else if (object.isString()) {
		//TODO: will not doing this kill anything?
		//fout << "\"" << escape_json2_XX(object.extract<const std::string>()) << "\"";
		fout << object.extract<const std::string>();
	}
	else if (object.isInteger()) {
		int val = object;
		fout << val;
	}
	else if (object.isNumeric()) {
		auto val = object.extract<double>();
		fout << val;
	}
	else if (object.isSigned()) {
		fout << "\"[Signed]\"";
	}
	else {
		if (object.type() == typeid(DateTime)) {
			DateTime dt = object.extract<DateTime>();
			const std::string simpleDateStr = (boost::format("\"%1%/%2%/%3%\"") % dt.year() % dt.month() % dt.day()).str();
			fout << simpleDateStr;
		}
		else if (object.type() == typeid(Int16)) {
			fout << object.extract<const Int16>();
		}
		else if (object.type() == typeid(std::shared_ptr<IFlintData>)) {
			fout << "\"[IFlintData\"]";
		}
		else
			fout << "\"[Unknown\"]";
	}
}

// --------------------------------------------------------------------------------------------

void VariableAndPoolStringBuilder::replaceTokens(const std::string& tokenType, std::string& query, std::vector<std::string> values) {
	std::vector<std::string> tokens;
	std::string tokenStart = "{" + tokenType + ":";
	char tokenEnd = '}';

	std::size_t tokenPos = query.find(tokenStart); // look for a token start
	for (auto value : values) {
		std::size_t tokenEndPos = query.find(tokenEnd, tokenPos + 1); // look for the end of the token
		if (tokenEndPos != std::string::npos) {
			query.replace(tokenPos, tokenEndPos - tokenPos + 1, value);
			tokenPos = query.find(tokenStart, tokenPos + value.length());
		}
		else { // didn't find an end token marker so ignore this token
			tokenPos = query.find(tokenStart, tokenPos + 1);
		}
	}
}

std::string VariableAndPoolStringBuilder::formatVariableValues(const flint::IVariable& var, const std::string& property) {

#if 1
	std::ostringstream stream;
	auto& varValue = var.value();

	if (property != "") {
		if (varValue.type() == typeid(std::shared_ptr<IFlintData>)) {
			auto flintDataVariable = varValue.extract<std::shared_ptr<IFlintData>>();
			auto propValue = flintDataVariable->getProperty(property);
			outputDynamicToStream_XX(stream, propValue);
		}
		else {
			if (varValue.isStruct()) {
				auto varStruct = varValue.extract<const DynamicObject>();
				auto varStructProp = varStruct.contains(property) ? varStruct[property] : DynamicVar();
				outputDynamicToStream_XX(stream, varStructProp);
			}
			else
				outputDynamicToStream_XX(stream, varValue);
		}
	} else {
		outputDynamicToStream_XX(stream, varValue);
	}
	return stream.str();

#else
	auto& varValue = var.value();
	std::vector<DynamicVar> values;
	if (property.isEmpty()) {
		// Single-value variable reference.
		if (varValue.isVector()) {
			auto varVector = varValue.extract<std::vector<DynamicObject>>();
			for (auto value : varVector) {
				values.push_back(value);
			}
		} else if (varValue.isInteger()) {
			int val = varValue;
			values.push_back(val);
		} else if (varValue.isNumeric()) {
			double val = varValue;
			values.push_back(val);
		} else {
			values.push_back(varValue);
		}
	}
	else {
		// Multi-value variable reference.
		std::string propertyName = property;

		if (varValue.isVector()) {
			auto varVector = varValue.extract<std::vector<DynamicObject>>();
			for (auto value : varVector) {
				values.push_back(value[propertyName]);
			}
		} else {
			if (varValue.isStruct()) {
				values.push_back(varValue[propertyName]);
			} else if (varValue.type() == typeid(std::shared_ptr<flint::IFlintData>)) {
				auto flintDataVariable = varValue.extract<std::shared_ptr<flint::IFlintData>>();
				values.push_back(flintDataVariable->getProperty(propertyName));
			}
		}
	}
	std::vector<std::string> strings;
	for (auto value : values) {
		auto str = value.convert<std::string>();
		if (value.isString()) {
			// Enclose string values in SQL single quotes.
			str = "'" + str + "'";
		}
		strings.push_back(str);
	}
	return boost::algorithm::join(strings, ", ");
#endif
}

std::string VariableAndPoolStringBuilder::result() const {
	// Build string here from current variable and pool values.
	std::vector<std::string> _variablesValues;
	for (auto v : _variables) {
		const flint::IVariable* var;
		std::string property;
		std::tie(var, property) = v;
		auto values = formatVariableValues(*var, property);
		_variablesValues.push_back(values);
	}
	std::vector<std::string> _poolValues;
	for (auto p : _pools) {
		std::stringstream ss;
		ss << std::setprecision(15) << p->value();
		_poolValues.push_back(ss.str());
	}

	auto query(_workingStr);
	replaceTokens(varMarker, query, _variablesValues);
	replaceTokens(poolMarker, query, _poolValues);

	return query;
}

}
} // namespace moja::flint
