#ifndef MOJA_FLINT_OPERATIONSTOCKSIMPLE_H_
#define MOJA_FLINT_OPERATIONSTOCKSIMPLE_H_

#include "moja/flint/ioperation.h"
#include "moja/flint/operationtransfersimple.h"

namespace moja {
namespace flint {

class OperationManagerSimple;

class StockOperationSimple : public IOperation {
   friend class OperationManagerSimple;

  public:
   // StockOperationSimple() = default;
   StockOperationSimple(OperationManagerSimple* manager, double timeScale, const ModuleMetaData* metaData);
   StockOperationSimple(OperationManagerSimple* manager, double timeScale, const ModuleMetaData* metaData,
                        DynamicVar& dataPackage);
   virtual ~StockOperationSimple() = default;

   StockOperationSimple* addTransfer(const IPool* source, const IPool* sink, double value) override;

   void submitOperation() override;
   std::shared_ptr<IOperationResult> computeOperation(ITiming& _timing) override;

   OperationTransferType transferType() const override;
   OperationTransferHandlingType transferHandlingType() const override {
      return OperationTransferHandlingType::Disaggregated;
   };

   void set_metaData(const ModuleMetaData* metaData) { _metaData = metaData; }

  protected:
   OperationManagerSimple* _manager;
   double _timeScale;
   std::vector<OperationTransferSimple> _transfers;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_OPERATIONSTOCKSIMPLE_H_