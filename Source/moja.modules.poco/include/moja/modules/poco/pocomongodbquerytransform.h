#ifndef MOJA_MODULES_POCO_POCOMONGODBQUERYTRANSFORM_H_
#define MOJA_MODULES_POCO_POCOMONGODBQUERYTRANSFORM_H_

#include "moja/modules/poco/_modules.poco_exports.h"

#include <moja/dynamic.h>
#include <moja/flint/itransform.h>

#include <string>
#include <vector>

namespace moja {
namespace datarepository {
class DataRepository;
}
namespace flint {
class IPool;
class IVariable;
class ILandUnitController;
}  // namespace flint

namespace modules {
namespace poco {

class ProviderNoSQLPocoMongoDB;

class POCO_API PocoMongoDBQueryTransform : public flint::ITransform {
  public:
   PocoMongoDBQueryTransform();
   PocoMongoDBQueryTransform(const PocoMongoDBQueryTransform&) = delete;

   void configure(DynamicObject config, const flint::ILandUnitController& landUnitController,
                  moja::datarepository::DataRepository& dataRepository) override;

   PocoMongoDBQueryTransform& operator=(const PocoMongoDBQueryTransform&) = delete;
   void controllerChanged(const flint::ILandUnitController& controller) override;
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
   const flint::ILandUnitController* _landUnitController;
   const std::string varMarker = "var";
   const std::string poolMarker = "pool";
   datarepository::DataRepository* _dataRepository;
   std::shared_ptr<ProviderNoSQLPocoMongoDB> _provider;

   std::string _queryStr;
   std::string _filterStr;

   std::vector<std::tuple<const flint::IVariable*, DynamicVar>> _variables;
   std::vector<const flint::IPool*> _pools;

   mutable DynamicVar _dataSet;

   static std::vector<std::string> extractTokens(const std::string& tokenType, const std::string& query);
   static void replaceTokens(const std::string& tokenType, std::string& query, std::vector<std::string> values);
   static std::string formatVariableValues(const flint::IVariable& var, DynamicVar& property);
   static ResultType detectResultType(const DynamicVector&);
};

}  // namespace poco
}  // namespace modules
}  // namespace moja

#endif  // MOJA_MODULES_POCO_POCOMONGODBQUERYTRANSFORM_H_