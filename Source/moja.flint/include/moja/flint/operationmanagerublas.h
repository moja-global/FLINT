#ifndef MOJA_FLINT_OPERATIONMANAGERUBLAS_H_
#define MOJA_FLINT_OPERATIONMANAGERUBLAS_H_

#include "moja/flint/_flint_exports.h"
#include "moja/flint/matrixublas.h"
#include "moja/flint/ioperationmanager.h"
//#include "moja/flint/poolcollection.h"
#include "moja/flint/operationresultcollection.h"

#include "moja/dynamic.h"
#include "moja/exception.h"

namespace moja {
namespace flint {
class IOperation;
class IModule;
class IPool;
class Timing;

// --------------------------------------------------------------------------------------------

class FLINT_API OperationManagerUblas : public IOperationManager {
	friend class StockOperationUblas;
	friend class ProportionalOperationUblas;
public:
	OperationManagerUblas(Timing& timing, const DynamicObject& config);
	virtual ~OperationManagerUblas() = default;

	// Operations
	std::shared_ptr<IOperation> createStockOperation(IModule& module) override;
	std::shared_ptr<IOperation> createStockOperation(IModule& module, DynamicVar& dataPackage) override;
	std::shared_ptr<IOperation> createProportionalOperation(IModule& module) override;
	std::shared_ptr<IOperation> createProportionalOperation(IModule& module, DynamicVar& dataPackage) override;
	void applyOperations() override;

	void clearAllOperationResults() override;
	void clearLastAppliedOperationResults() override;

	const OperationResultCollection& operationResultsPending() const override;
	const OperationResultCollection& operationResultsLastApplied() const override;
	const OperationResultCollection& operationResultsCommitted() const override;

	bool hasOperationResultsLastApplied() const override;

	// Pools
	PoolCollection poolCollection() override;

	void initialisePools() override;

	int poolCount() override { return _poolCount; }

    const IPool* addPool(const std::string& name, const std::string& description, const std::string& units, double scale, int order, const std::shared_ptr<ITransform> transform) override { throw moja::NotImplementedException(); };
    const IPool* addPool(const std::string& name, double initValue = 0.0) override;
	const IPool* addPool(const std::string& name, const std::string& description, const std::string& units, double scale,
	                     int order, double initValue = 0.0) override;
	const IPool* addPool(PoolMetaData& metadata, double initValue) override;

	const IPool* getPool(const std::string& name) const override;
	const IPool* getPool(int index) const override;;

	// Details of instance
	std::string name() const override { return "Ublas"; }
	std::string version() const override { return "1.0"; }
	std::string config() const override;;

protected:
	void commitPendingOperationResults();

	Timing& _timing;		// timing object for simulation

	moja_ublas_vector						_poolValues;		// pool values
	std::vector<double>						_corrections;		// used for https://en.wikipedia.org/wiki/Kahan_summation_algorithm

	std::vector<std::shared_ptr<IPool>>					_poolObjects;		// pool object vector
	std::map<std::string, std::shared_ptr<IPool>>		_poolNameObjectMap;	// pool objects indexed by name
	int										_poolCount;			// keep count of pools

	OperationResultCollection	_operationResultsPending;		// OperationResults (flux & meta-data) submitted by modules - waiting to be applied
	OperationResultCollection	_operationResultsLastApplied;	// The most recently applied OperationResults (copy)
	OperationResultCollection	_operationResultsCommitted;		// List of OperationResults that have been committed

	//moja_ublas_matrix _matrix;
	//moja_ublas_matrix _matrixProportional;

private:
	bool _useKahan;
	bool _allowZeroResultTransfers;

	// Operations
	void submitOperation(IOperation* operation) override;
};

}
} // moja::flint

#endif // MOJA_FLINT_OPERATIONMANAGERUBLAS_H_