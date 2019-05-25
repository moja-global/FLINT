#ifndef MOJA_FLINT_OPERATIONSTOCKSIMPLECACHE_H_
#define MOJA_FLINT_OPERATIONSTOCKSIMPLECACHE_H_

#include "moja/flint/ioperation.h"
#include "moja/flint/operationtransfersimplecache.h"

namespace moja {
namespace flint {

class OperationManagerSimpleCache;

class StockOperationSimpleCache : public IOperation {
   friend class OperationManagerSimpleCache;

  public:
   // StockOperationSimpleCache() = default;
   StockOperationSimpleCache(OperationManagerSimpleCache* manager, std::vector<double>& pools, double timeScale,
                             const ModuleMetaData* metaData);
   StockOperationSimpleCache(OperationManagerSimpleCache* manager, std::vector<double>& pools, double timeScale,
                             const ModuleMetaData* metaData, DynamicVar& dataPackage);
   virtual ~StockOperationSimpleCache() = default;

   StockOperationSimpleCache* addTransfer(const IPool* source, const IPool* sink, double value) override;

   void submitOperation() override;
   std::shared_ptr<IOperationResult> computeOperation(ITiming& _timing) override;

   OperationTransferType transferType() const override;
   OperationTransferHandlingType transferHandlingType() const override {
      return OperationTransferHandlingType::Disaggregated;
   };

   void set_metaData(const ModuleMetaData* metaData) { _metaData = metaData; }

  protected:
   OperationManagerSimpleCache* _manager;
   double _timeScale;
   std::vector<OperationTransferSimpleCache> _transfers;
   std::vector<double>& _pools;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_OPERATIONSTOCKSIMPLECACHE_H_