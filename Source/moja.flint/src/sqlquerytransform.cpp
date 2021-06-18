#include "moja/flint/sqlquerytransform.h"

#include "moja/flint/flintexceptions.h"
#include "moja/flint/ilandunitcontroller.h"
#include "moja/flint/ioperationmanager.h"
#include "moja/flint/ipool.h"
#include "moja/flint/ivariable.h"

#include <moja/datarepository/datarepository.h>
#include <moja/datarepository/iproviderrelationalinterface.h>

#include <moja/filesystem.h>

#include <boost/algorithm/string/join.hpp>

#include <fstream>
#include <sstream>

using moja::datarepository::IProviderRelationalInterface;

namespace moja {
namespace flint {

SQLQueryTransform::SQLQueryTransform() : _landUnitController(nullptr), _dataRepository(nullptr) {}

void SQLQueryTransform::configure(DynamicObject config, const ILandUnitController& landUnitController,
                                  moja::datarepository::DataRepository& dataRepository) {
   _config = config;
   if (config.contains("queryString")) {
      _queryStr = config["queryString"].convert<std::string>();
   } else if (config.contains("queryFile")) {
      _queryStr = readSQLFile(config["queryFile"]);
   }

   _allowEmptyVarValues = false;
   if (config.contains("allow_empty_var_values")) {
      _allowEmptyVarValues = config["allow_empty_var_values"].extract<const bool>();
   }
   _landUnitController = &landUnitController;
   _dataRepository = &dataRepository;

   std::string providerName = config["provider"];
   _provider = std::static_pointer_cast<IProviderRelationalInterface>(_dataRepository->getProvider(providerName));

   // Search string for pool names.
   auto poolNames = extractTokens(poolMarker, _queryStr);
   for (auto name : poolNames) {
      _pools.push_back(landUnitController.operationManager()->getPool(name));
   }

   // Search string for variable names.
   auto varNames = extractTokens(varMarker, _queryStr);
   for (auto var : varNames) {
      auto propertySeparator = var.find_first_of('.');
      if (propertySeparator != std::string::npos) {
         // Reference to a multi-property variable.
         auto prop = var.substr(propertySeparator + 1, var.length());
         var = var.substr(0, propertySeparator);
         auto val = landUnitController.getVariable(var);
         _variables.push_back(std::make_tuple(val, prop));
      } else {
         // Reference to a single-value variable.
         auto val = landUnitController.getVariable(var);
         _variables.push_back(std::make_tuple(val, DynamicVar()));
      }
   }
}

std::string SQLQueryTransform::readSQLFile(const std::string& path) {
   namespace fs = moja::filesystem;
   if (!fs::exists(path)) {
      throw std::runtime_error("SQL file not found " + path);
   }

   std::ifstream file(path);
   std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
   return sql;
}

void SQLQueryTransform::controllerChanged(const ILandUnitController& controller) {
   _pools.clear();
   _variables.clear();
   _dataSet = nullptr;
   configure(_config, controller, *_dataRepository);
};

const DynamicVar& SQLQueryTransform::value() const {
   // Build string here from current variable and pool values.
   std::vector<std::string> _variablesValues;
   for (auto v : _variables) {
      const IVariable* var;
      DynamicVar property;
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

   auto query(_queryStr);
   replaceTokens(varMarker, query, _variablesValues);
   replaceTokens(poolMarker, query, _poolValues);

   DynamicVar result = _provider->GetDataSet(query);
   switch (detectResultType(result)) {
      case SQLQueryTransform::ResultType::MultiColumnMultiRow: {
         auto& results = result.extract<std::vector<DynamicObject>>();
         _dataSet = results;
         break;
      }
      case SQLQueryTransform::ResultType::MultiColumnSingleRow: {
         auto& results = result.extract<std::vector<DynamicObject>>();
         _dataSet = results[0];
         break;
      }
      case SQLQueryTransform::ResultType::SingleColumnMultiRow: {
         auto& results = result.extract<std::vector<DynamicObject>>();
         std::vector<DynamicVar> dataSet;
         for (const auto& row : results) {
            dataSet.push_back(row.begin()->second);
         }

         _dataSet = dataSet;
         break;
      }
      case SQLQueryTransform::ResultType::SingleColumnSingleRow: {
         auto& results = result.extract<std::vector<DynamicObject>>();
         _dataSet = results[0].begin()->second;
         break;
      }
      case SQLQueryTransform::ResultType::Empty:
      default:
         _dataSet = DynamicVar();
         break;
   }

   return _dataSet;
}

SQLQueryTransform::ResultType SQLQueryTransform::detectResultType(const DynamicVar& resultSet) {
   if (resultSet.isEmpty() || resultSet.size() == 0) {
      return SQLQueryTransform::ResultType::Empty;
   }

   auto& results = resultSet.extract<std::vector<DynamicObject>>();
   auto numRows = results.size();
   auto numCols = results[0].size();

   if (numCols > 1) {
      return numRows > 1 ? SQLQueryTransform::ResultType::MultiColumnMultiRow
                         : SQLQueryTransform::ResultType::MultiColumnSingleRow;
   }

   return numRows > 1 ? SQLQueryTransform::ResultType::SingleColumnMultiRow
                      : SQLQueryTransform::ResultType::SingleColumnSingleRow;
}

std::string SQLQueryTransform::formatVariableValues(const IVariable& var, DynamicVar& property) const {
   std::vector<DynamicVar> values;
   std::vector<std::string> strings;

   auto& varValue = var.value();
   if (varValue.isEmpty()) {
      if (_allowEmptyVarValues) {
         return "NULL";
      } else {
         throw std::runtime_error("SQL value NULL when value expected");
      }
   }

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
   } else {
      // Multi-value variable reference.
      std::string propertyName = property;
      if (varValue.isVector()) {
         auto varVector = varValue.extract<std::vector<DynamicObject>>();
         for (auto value : varVector) {
            values.push_back(value[propertyName]);
         }
      } else if (varValue.isStruct()) {
         values.push_back(varValue[propertyName]);
      }
   }
   for (auto value : values) {
      auto str = value.convert<std::string>();
      if (value.isString()) {
         // Enclose string values in SQL single quotes.
         str = "'" + str + "'";
      }
      strings.push_back(str);
   }
   return boost::algorithm::join(strings, ", ");
}

// Searching for tokens of the form "{<type>:name[.property]}", for example "{pool:totalCM}"
// or "{var:Fractions.clay}" and return token names.
std::vector<std::string> SQLQueryTransform::extractTokens(const std::string& tokenType, const std::string& query) {
   std::string tokenStart = "{" + tokenType + ":";
   char tokenEnd = '}';
   auto tokenStartLen = tokenStart.length();
   auto tokenStartPos = query.find(tokenStart);  // look for a token start

   std::vector<std::string> tokens;
   while (tokenStartPos != std::string::npos) {
      auto tokenValueStartPos = tokenStartPos + tokenStartLen;
      auto tokenEndPos = query.find(tokenEnd, tokenStartPos + 1);  // look for the end of the token
      if (tokenEndPos != std::string::npos && tokenEndPos > tokenValueStartPos) {
         auto tokenValueLen = tokenEndPos - tokenValueStartPos;
         tokens.push_back(query.substr(tokenValueStartPos, tokenValueLen));
         tokenStartPos = query.find(tokenStart, tokenEndPos + 1);
      } else {  // didn't find an end token marker so ignore this token
         tokenStartPos = query.find(tokenStart, tokenStartPos + 1);
      }
   }

   return tokens;
}

void SQLQueryTransform::replaceTokens(const std::string& tokenType, std::string& query,
                                      std::vector<std::string> values) {
   std::vector<std::string> tokens;
   std::string tokenStart = "{" + tokenType + ":";
   char tokenEnd = '}';

   std::size_t tokenPos = query.find(tokenStart);  // look for a token start
   for (auto value : values) {
      std::size_t tokenEndPos = query.find(tokenEnd, tokenPos + 1);  // look for the end of the token
      if (tokenEndPos != std::string::npos) {
         query.replace(tokenPos, tokenEndPos - tokenPos + 1, value);
         tokenPos = query.find(tokenStart, tokenPos + value.length());
      } else {  // didn't find an end token marker so ignore this token
         tokenPos = query.find(tokenStart, tokenPos + 1);
      }
   }
}

}  // namespace flint
}  // namespace moja
