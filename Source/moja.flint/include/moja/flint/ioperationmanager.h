#ifndef MOJA_FLINT_IOPERATIONMANAGER_H_
#define MOJA_FLINT_IOPERATIONMANAGER_H_

#include "moja/flint/_flint_exports.h"
#include "moja/flint/itransform.h"
#include "moja/flint/poolcollection.h"

#include <moja/dynamic.h>

namespace moja {
namespace flint {

class IModule;
class IOperation;
class IPool;
class PoolMetaData;
class OperationResultCollection;

// --------------------------------------------------------------------------------------------

enum class OperationTransferType { Stock = 0, Proportional = 1 };

inline std::string OperationTransferTypeToString(OperationTransferType t) {
   switch (t) {
      case OperationTransferType::Stock:
         return "Stock";
      case OperationTransferType::Proportional:
         return "Proportional";
   }
   return "unknown";
}

// --------------------------------------------------------------------------------------------

class FLINT_API IOperationManager {
  public:
   virtual ~IOperationManager() = default;

   // Operations
   virtual std::shared_ptr<IOperation> createStockOperation(IModule& module) = 0;
   virtual std::shared_ptr<IOperation> createStockOperation(IModule& module, DynamicVar& dataPackage) = 0;
   virtual std::shared_ptr<IOperation> createProportionalOperation(IModule& module) = 0;
   virtual std::shared_ptr<IOperation> createProportionalOperation(IModule& module, DynamicVar& dataPackage) = 0;
   virtual void applyOperations() = 0;

   virtual void clearAllOperationResults() = 0;
   virtual void clearLastAppliedOperationResults() = 0;

   virtual const OperationResultCollection& operationResultsPending() const = 0;
   virtual const OperationResultCollection& operationResultsLastApplied() const = 0;
   virtual const OperationResultCollection& operationResultsCommitted() const = 0;

   virtual bool hasOperationResultsLastApplied() const = 0;

   // Pools
   virtual PoolCollection poolCollection() = 0;

   virtual void initialisePools() = 0;
   virtual int poolCount() = 0;

   virtual const IPool* addPool(const std::string& name, const std::string& description, const std::string& units,
                                double scale, int order, const std::shared_ptr<ITransform> transform) = 0;
   virtual const IPool* addPool(const std::string& name, double initValue = 0.0) = 0;
   virtual const IPool* addPool(const std::string& name, const std::string& description, const std::string& units,
                                double scale, int order, double initValue = 0.0) = 0;
   virtual const IPool* addPool(PoolMetaData& metadata, double initValue) = 0;

   virtual const IPool* getPool(const std::string& name) const = 0;
   virtual const IPool* getPool(int index) const = 0;

   // Details of instance
   virtual std::string name() const = 0;
   virtual std::string version() const = 0;
   virtual std::string config() const = 0;

  private:
   // Operations
   virtual void submitOperation(IOperation* operation) = 0;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_IOPERATIONMANAGER_H_
