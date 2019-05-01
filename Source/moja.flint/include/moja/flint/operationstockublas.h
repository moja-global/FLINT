#ifndef MOJA_FLINT_OPERATIONSTOCKUBLAS_H_
#define MOJA_FLINT_OPERATIONSTOCKUBLAS_H_

#include "moja/flint/ioperation.h"
#include "moja/flint/matrixublas.h"

namespace moja {
namespace flint {

class OperationManagerUblas;

class StockOperationUblas : public IOperation {
public:
	StockOperationUblas(OperationManagerUblas* manager, moja_ublas_vector& pools, double timeScale, /*moja_ublas_identity& I, */const ModuleMetaData* metaData);
	StockOperationUblas(OperationManagerUblas* manager, moja_ublas_vector& pools, double timeScale, /*moja_ublas_identity& I, */const ModuleMetaData* metaData, DynamicVar& dataPackage);
	virtual ~StockOperationUblas() = default;

	StockOperationUblas* addTransfer(const IPool* source, const IPool* sink, double value) override;

	void submitOperation() override;
	std::shared_ptr<IOperationResult> computeOperation(ITiming& _timing) override;

	OperationTransferType transferType() const override;

	OperationTransferHandlingType transferHandlingType() const override { return OperationTransferHandlingType::Aggregated; };

protected:
	OperationManagerUblas* _manager;
	bool _hasChange;
	double _timeScale;
	moja_ublas_vector& _pools;
	moja_ublas_matrix _matrix;
};

}
} // moja::flint

#endif // MOJA_FLINT_OPERATIONSTOCKUBLAS_H_