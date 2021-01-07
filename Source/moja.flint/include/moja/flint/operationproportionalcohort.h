#pragma once

#include "moja/flint/ioperation.h"
#include "moja/flint/operationtransfersimple.h"

namespace moja::flint {

class OperationManagerCohort;

class ProportionalOperationCohort : public IOperation {
   friend class OperationManagerSimple;

  public:
   // ProportionalOperationSimple() = default;
   ProportionalOperationCohort(OperationManagerCohort* manager, const std::vector<double>& pools,
                                     double timeScale, const ModuleMetaData* metaData);
   ProportionalOperationCohort(OperationManagerCohort* manager, const std::vector<double>& pools,
                                     double timeScale, const ModuleMetaData* metaData, DynamicVar& dataPackage);
   virtual ~ProportionalOperationCohort() = default;

   ProportionalOperationCohort* addTransfer(const IPool* source, const IPool* sink, double value) override;

   void submitOperation() override;
   std::shared_ptr<IOperationResult> computeOperation(ITiming& timing) override;

   OperationTransferType transferType() const override;
   OperationTransferHandlingType transferHandlingType() const override {
      return OperationTransferHandlingType::Disaggregated;
   }

   void set_metaData(const ModuleMetaData* metaData) { _metaData = metaData; }

  protected:
   OperationManagerCohort* manager_;
   double time_scale_;
   std::vector<OperationTransferSimple> transfers_;
   const std::vector<double>& pools_;
};

}  // namespace moja::flint
