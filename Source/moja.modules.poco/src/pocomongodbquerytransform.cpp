#include "moja/modules/poco/pocomongodbquerytransform.h"

#include "moja/modules/poco/providernosqlpocomongodb.h"

#include <moja/flint/flintexceptions.h>
#include <moja/flint/ilandunitcontroller.h>
#include <moja/flint/ioperationmanager.h>
#include <moja/flint/ipool.h>
#include <moja/flint/ivariable.h>

#include <moja/datarepository/datarepository.h>

#include <moja/dynamic.h>

#include <Poco/JSON/Parser.h>

#include <boost/algorithm/string/join.hpp>

namespace moja {
namespace modules {
namespace poco {

PocoMongoDBQueryTransform::PocoMongoDBQueryTransform() : _landUnitController(nullptr), _dataRepository(nullptr) {}

void PocoMongoDBQueryTransform::configure(DynamicObject config, const flint::ILandUnitController& landUnitController,
                                          moja::datarepository::DataRepository& dataRepository) {
   _config = config;
   _queryStr = config["queryString"].convert<std::string>();
   _filterStr = config["filterString"].convert<std::string>();
   _landUnitController = &landUnitController;
   _dataRepository = &dataRepository;

   const std::string providerName = config["provider"];
   _provider = std::static_pointer_cast<ProviderNoSQLPocoMongoDB>(_dataRepository->getProvider(providerName));

   // Search string for pool names.
   _pools.clear();
   auto poolNames = extractTokens(poolMarker, _queryStr);
   for (auto name : poolNames) {
      _pools.push_back(landUnitController.operationManager()->getPool(name));
   }

   // Search string for variable names.
   _variables.clear();
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

void PocoMongoDBQueryTransform::controllerChanged(const flint::ILandUnitController& controller) {
   configure(_config, controller, *_dataRepository);
};

const DynamicVar& PocoMongoDBQueryTransform::value() const {
   // Build string here from current variable and pool values.
   std::vector<std::string> _variablesValues;
   for (auto v : _variables) {
      const flint::IVariable* var;
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

   auto itemCount = _provider->Count();
   auto result = _provider->SendQueryRequest(query, _filterStr, itemCount);
   switch (detectResultType(result)) {
      case PocoMongoDBQueryTransform::ResultType::MultiColumnMultiRow: {
         _dataSet = result;
         break;
      }
      case PocoMongoDBQueryTransform::ResultType::MultiColumnSingleRow: {
         _dataSet = result[0];
         break;
      }
#if 0  // TODO: Fix this
	case PocoMongoDBQueryTransform::ResultType::SingleColumnMultiRow: {
		auto results = result.extract<DynamicVector>();
		std::vector<DynamicVar> dataSet;
		for (const auto& row : results) {
			dataSet.push_back(row.begin()->second);
		}
		_dataSet = dataSet;
		break;
	}
	case PocoMongoDBQueryTransform::ResultType::SingleColumnSingleRow: {
		auto results = result.extract<DynamicVector>();
		_dataSet = results[0].begin()->second;
		break;
	}
#endif
      default:
      case PocoMongoDBQueryTransform::ResultType::Empty: {
         _dataSet = DynamicVar();
         break;
      }
   }

   return _dataSet;
}

PocoMongoDBQueryTransform::ResultType PocoMongoDBQueryTransform::detectResultType(const DynamicVector& results) {
   if (results.size() == 0) {
      return PocoMongoDBQueryTransform::ResultType::Empty;
   }

   auto numRows = results.size();
   auto numCols = results[0].size();

   if (numCols > 1) {
      return numRows > 1 ? PocoMongoDBQueryTransform::ResultType::MultiColumnMultiRow
                         : PocoMongoDBQueryTransform::ResultType::MultiColumnSingleRow;
   }
   return numRows > 1 ? PocoMongoDBQueryTransform::ResultType::SingleColumnMultiRow
                      : PocoMongoDBQueryTransform::ResultType::SingleColumnSingleRow;
}

std::string PocoMongoDBQueryTransform::formatVariableValues(const flint::IVariable& var, DynamicVar& property) {
   auto& varValue = var.value();
   std::vector<DynamicVar> values;
   if (property.isEmpty()) {
      // Single-value variable reference.
      if (varValue.isVector()) {
         auto varVector = varValue.extract<std::vector<DynamicObject>>();
         for (auto value : varVector) {
            values.push_back(value);
         }
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
      } else {
         if (varValue.isStruct()) {
            values.push_back(varValue[propertyName]);
         }
      }
   }

   std::vector<std::string> strings;
   for (auto value : values) {
      auto str = value.convert<std::string>();
      if (value.isString()) {
         // Enclose string values in SQL single quotes.
         str = "\"" + str + "\"";
      }
      strings.push_back(str);
   }
   return boost::algorithm::join(strings, ", ");
}

// Searching for tokens of the form "{<type>:name[.property]}", for example "{pool:totalCM}"
// or "{var:Fractions.clay}" and return token names.
std::vector<std::string> PocoMongoDBQueryTransform::extractTokens(const std::string& tokenType,
                                                                  const std::string& query) {
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

void PocoMongoDBQueryTransform::replaceTokens(const std::string& tokenType, std::string& query,
                                              std::vector<std::string> values) {
   std::vector<std::string> tokens;
   std::string tokenStart = "{" + tokenType + ":";
   auto tokenEnd = '}';

   auto tokenPos = query.find(tokenStart);  // look for a token start
   for (auto value : values) {
      auto tokenEndPos = query.find(tokenEnd, tokenPos + 1);  // look for the end of the token
      if (tokenEndPos != std::string::npos) {
         query.replace(tokenPos, tokenEndPos - tokenPos + 1, value);
         tokenPos = query.find(tokenStart, tokenPos + value.length());
      } else {  // didn't find an end token marker so ignore this token
         tokenPos = query.find(tokenStart, tokenPos + 1);
      }
   }
}

}  // namespace poco
}  // namespace modules
}  // namespace moja
