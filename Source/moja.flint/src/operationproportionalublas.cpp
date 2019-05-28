#include "moja/flint/operationproportionalublas.h"

#include "moja/flint/ipool.h"
#include "moja/flint/operationmanagerublas.h"
#include "moja/flint/operationresultublas.h"

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------

ProportionalOperationUblas::ProportionalOperationUblas(OperationManagerUblas* manager, moja_ublas_vector& pools,
                                                       double timeScale,
                                                       /*moja_ublas_identity& I, */ const ModuleMetaData* metaData)
    : IOperation(metaData),
      _manager(manager),
      _hasChange(false),
      _timeScale(timeScale),
      _pools(pools),
      /*_I(I), */ _matrix(manager->_poolCount, manager->_poolCount, manager->_poolCount) {
   //_matrix
   _hasChange = false;
}

ProportionalOperationUblas::ProportionalOperationUblas(OperationManagerUblas* manager, moja_ublas_vector& pools,
                                                       double timeScale, const ModuleMetaData* metaData,
                                                       DynamicVar& dataPackage)
    : IOperation(metaData, dataPackage),
      _manager(manager),
      _hasChange(false),
      _timeScale(timeScale),
      _pools(pools),
      /*_I(I), */ _matrix(manager->_poolCount, manager->_poolCount, manager->_poolCount) {
   //_matrix
   _hasChange = false;
}

// --------------------------------------------------------------------------------------------

ProportionalOperationUblas* ProportionalOperationUblas::addTransfer(const IPool* source, const IPool* sink,
                                                                    double value) {
   //// Ublas allows a zero value to be inserted in a sparse matrix, after Prod of it and Diag of Pools the Zero value
   /// will be left out
   // if (FloatCmp::equalTo(value, 0.0) || FloatCmp::equalTo(source->value(), 0.0))
   //	return this;

   _matrix(source->idx(), sink->idx()) += value;
   _hasChange = true;
   return this;
}

// --------------------------------------------------------------------------------------------

std::shared_ptr<IOperationResult> ProportionalOperationUblas::computeOperation(ITiming& _timing) {
   // if (!_hasChange)
   //	return nullptr;

   auto result = std::make_shared<OperationResultUblas>(*this);

   if (!_hasChange) return result;

   const moja_ublas_diagonal diag(_pools.size(), _pools.data());
   if (_timing.isFullStep()) {
      result->_fluxes = MOJA_UBLAS_MAT_PROD(diag, _matrix);
   } else {
      result->_fluxes = MOJA_UBLAS_MAT_PROD(diag, _matrix) * _timeScale;
   }
   return result;
}

// --------------------------------------------------------------------------------------------

OperationTransferType ProportionalOperationUblas::transferType() const { return OperationTransferType::Proportional; }

// --------------------------------------------------------------------------------------------
void ProportionalOperationUblas::submitOperation() { _manager->submitOperation(this); }

}  // namespace flint
}  // namespace moja
