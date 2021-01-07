#pragma once

#include "moja/flint/ioperation.h"
#include "moja/flint/operationtransfersimple.h"

namespace moja::flint {

class OperationManagerCohort;

class StockOperationCohort : public IOperation {
   friend class OperationManagerCohort;

  public:
   StockOperationCohort(OperationManagerCohort* manager, double timeScale, const ModuleMetaData* metaData);
   StockOperationCohort(OperationManagerCohort* manager, double timeScale, const ModuleMetaData* metaData,
                              DynamicVar& dataPackage);
   virtual ~StockOperationCohort() = default;

   StockOperationCohort* addTransfer(const IPool* source, const IPool* sink, double value) override;

   void submitOperation() override;
   std::shared_ptr<IOperationResult> computeOperation(ITiming& timing) override;

   OperationTransferType transferType() const override;
   OperationTransferHandlingType transferHandlingType() const override {
      return OperationTransferHandlingType::Disaggregated;
   }

   void set_metaData(const ModuleMetaData* metaData) { _metaData = metaData; }
   void pooled_init(OperationManagerCohort* manager, double timeScale,
                    const ModuleMetaData* metaData);

protected:
   OperationManagerCohort* manager_;
   double time_scale_;
   std::vector<OperationTransferSimple> transfers_;
};

}  // namespace moja::flint
