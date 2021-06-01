#pragma once

#include "moja/flint/ioperation.h"
#include "moja/flint/operationtransfersimple.h"

namespace moja::flint {

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
   std::shared_ptr<IOperationResult> computeOperation(ITiming& timing) override;

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

}  // namespace moja::flint
