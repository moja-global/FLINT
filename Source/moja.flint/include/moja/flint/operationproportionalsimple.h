#pragma once

#include "moja/flint/ioperation.h"
#include "moja/flint/operationtransfersimple.h"

namespace moja::flint {

class OperationManagerSimple;

class ProportionalOperationSimple : public IOperation {
   friend class OperationManagerSimple;

  public:
   // ProportionalOperationSimple() = default;
   ProportionalOperationSimple(OperationManagerSimple* manager, const std::vector<double>& pools, double timeScale,
                               const ModuleMetaData* metaData);
   ProportionalOperationSimple(OperationManagerSimple* manager, const std::vector<double>& pools, double timeScale,
                               const ModuleMetaData* metaData, DynamicVar& dataPackage);
   virtual ~ProportionalOperationSimple() = default;

   ProportionalOperationSimple* addTransfer(const IPool* source, const IPool* sink, double value) override;

   void submitOperation() override;
   std::shared_ptr<IOperationResult> computeOperation(ITiming& timing) override;

   OperationTransferType transferType() const override;
   OperationTransferHandlingType transferHandlingType() const override {
      return OperationTransferHandlingType::Disaggregated;
   }

   void set_metaData(const ModuleMetaData* metaData) { _metaData = metaData; }

  protected:
   OperationManagerSimple* _manager;
   double _timeScale;
   std::vector<OperationTransferSimple> _transfers;
   const std::vector<double>& _pools;
};

}  // namespace moja::flint
