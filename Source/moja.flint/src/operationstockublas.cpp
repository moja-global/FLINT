#include "moja/flint/operationstockublas.h"

#include "moja/flint/ipool.h"
#include "moja/flint/operationmanagerublas.h"
#include "moja/flint/operationresultublas.h"

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------
StockOperationUblas::StockOperationUblas(OperationManagerUblas* manager, moja_ublas_vector& pools, double timeScale,
                                         /*moja_ublas_identity& I, */ const ModuleMetaData* metaData)
    : IOperation(metaData),
      _manager(manager),
      _timeScale(timeScale),
      _pools(pools),
      /*_I(I), */ _matrix(manager->_poolCount, manager->_poolCount, manager->_poolCount) /*, _matrix(I)*/ {
   _hasChange = false;
}

StockOperationUblas::StockOperationUblas(OperationManagerUblas* manager, moja_ublas_vector& pools, double timeScale,
                                         const ModuleMetaData* metaData, DynamicVar& dataPackage)
    : IOperation(metaData, dataPackage),
      _manager(manager),
      _timeScale(timeScale),
      _pools(pools),
      /*_I(I), */ _matrix(manager->_poolCount, manager->_poolCount, manager->_poolCount) /*, _matrix(I)*/ {
   _hasChange = false;
}

// --------------------------------------------------------------------------------------------

StockOperationUblas* StockOperationUblas::addTransfer(const IPool* source, const IPool* sink, double value) {
   // Ublas allows a zero value to be inserted in a sparse matrix
   if (FloatCmp::equalTo(value, 0.0)) return this;

   _matrix(source->idx(), sink->idx()) += value;
   _hasChange = true;
   return this;
}

// --------------------------------------------------------------------------------------------

std::shared_ptr<IOperationResult> StockOperationUblas::computeOperation(ITiming& _timing) {
   // if (!_hasChange)
   //	return nullptr;

   auto result = std::make_shared<OperationResultUblas>(*this);

   if (!_hasChange) return result;

   if (_timing.isFullStep()) {
      result->_fluxes = _matrix;
   } else {
      result->_fluxes = _matrix * _timeScale;
   }

   return result;
}

// --------------------------------------------------------------------------------------------

OperationTransferType StockOperationUblas::transferType() const { return OperationTransferType::Stock; }

// --------------------------------------------------------------------------------------------

void StockOperationUblas::submitOperation() { _manager->submitOperation(this); }

}  // namespace flint
}  // namespace moja
