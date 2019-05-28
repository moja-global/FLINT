#ifndef MOJA_FLINT_SQLQUERYTRANSFORM_H_
#define MOJA_FLINT_SQLQUERYTRANSFORM_H_

#include "moja/flint/itransform.h"

#include <string>
#include <vector>

namespace moja {
namespace datarepository {
class IProviderRelationalInterface;
}

namespace flint {

class IPool;
class ILandUnitController;
class IVariable;

class FLINT_API SQLQueryTransform : public ITransform {
  public:
   SQLQueryTransform();
   SQLQueryTransform(const SQLQueryTransform&) = delete;

   void configure(DynamicObject config, const ILandUnitController& landUnitController,
                  moja::datarepository::DataRepository& dataRepository) override;

   SQLQueryTransform& operator=(const SQLQueryTransform&) = delete;
   void controllerChanged(const ILandUnitController& controller) override;
   const DynamicVar& value() const override;

  private:
   enum class ResultType {
      SingleColumnSingleRow,
      SingleColumnMultiRow,
      MultiColumnSingleRow,
      MultiColumnMultiRow,
      Empty
   };

   DynamicObject _config;
   bool _allowEmptyVarValues;
   const ILandUnitController* _landUnitController;
   const std::string varMarker = "var";
   const std::string poolMarker = "pool";
   datarepository::DataRepository* _dataRepository;
   std::shared_ptr<datarepository::IProviderRelationalInterface> _provider;
   std::string _queryStr;
   std::vector<std::tuple<const IVariable*, DynamicVar>> _variables;
   std::vector<const IPool*> _pools;
   mutable DynamicVar _dataSet;

   std::string readSQLFile(const std::string& path);

   static std::vector<std::string> extractTokens(const std::string& tokenType, const std::string& query);

   static void replaceTokens(const std::string& tokenType, std::string& query, std::vector<std::string> values);

   std::string formatVariableValues(const IVariable& var, DynamicVar& property) const;

   static ResultType detectResultType(const DynamicVar&);
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_SQLQUERYTRANSFORM_H_
