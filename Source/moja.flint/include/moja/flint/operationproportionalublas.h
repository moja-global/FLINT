#ifndef MOJA_FLINT_OPERATIONPROPORTIONALUBLAS_H_
#define MOJA_FLINT_OPERATIONPROPORTIONALUBLAS_H_

#include "moja/flint/ioperation.h"
#include "moja/flint/matrixublas.h"

namespace moja {
namespace flint {

class OperationManagerUblas;

class ProportionalOperationUblas : public IOperation {
  public:
   ProportionalOperationUblas(OperationManagerUblas* manager, moja_ublas_vector& pools, double timeScale,
                              /*moja_ublas_identity& I, */ const ModuleMetaData* metaData);
   ProportionalOperationUblas(OperationManagerUblas* manager, moja_ublas_vector& pools, double timeScale,
                              /*moja_ublas_identity& I, */ const ModuleMetaData* metaData, DynamicVar& dataPackage);
   virtual ~ProportionalOperationUblas() = default;

   ProportionalOperationUblas* addTransfer(const IPool* source, const IPool* sink, double value) override;

   void submitOperation() override;
   std::shared_ptr<IOperationResult> computeOperation(ITiming& _timing) override;

   OperationTransferType transferType() const override;

   OperationTransferHandlingType transferHandlingType() const override {
      return OperationTransferHandlingType::Aggregated;
   };

  protected:
   OperationManagerUblas* _manager;
   bool _hasChange;
   double _timeScale;
   moja_ublas_vector& _pools;
   moja_ublas_matrix _matrix;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_OPERATIONPROPORTIONALUBLAS_H_