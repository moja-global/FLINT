#ifndef MOJA_FLINT_OPERATIONRESULTUBLAS_H_
#define MOJA_FLINT_OPERATIONRESULTUBLAS_H_

#include "moja/flint/ioperationresult.h"
#include "moja/flint/matrixublas.h"
#include "moja/flint/timing.h"

namespace moja {
namespace flint {
class IOperation;

// --------------------------------------------------------------------------------------------

class OperationResultUblas : public IOperationResult {
   friend class StockOperationUblas;
   friend class ProportionalOperationUblas;

  public:
   explicit OperationResultUblas(IOperation& operation);
   virtual ~OperationResultUblas() = default;

   OperationResultFluxCollection operationResultFluxCollection() override;

   OperationTransferType transferType() const override;
   const ModuleMetaData* metaData() const override;
   const DynamicVar& dataPackage() const override;
   bool hasDataPackage() const override;

   const Timing& timingWhenApplied() const override { return _timingWhenApplied; };
   void setTimingWhenApplied(const Timing& timing) override { _timingWhenApplied = timing; };

  protected:
   OperationTransferType _transferType;
   const ModuleMetaData* _metaData;
   const DynamicVar _dataPackage;
   const bool _hasDataPackage;
   moja_ublas_matrix _fluxes;

   Timing _timingWhenApplied;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_OPERATIONRESULTUBLAS_H_
