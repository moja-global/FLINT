#ifndef MOJA_FLINT_OPERATIONMANAGERSIMPLE_H_
#define MOJA_FLINT_OPERATIONMANAGERSIMPLE_H_

#include "moja/flint/_flint_exports.h"
#include "moja/flint/ioperationmanager.h"
#include "moja/flint/operationresultcollection.h"

#include "moja/dynamic.h"

#include <sstream>


namespace moja {
namespace flint {
class IModule;
class IOperation;
class IPool;

class Timing;
class PoolMetaData;
class StockOperationSimple;
class ProportionalOperationSimple;

// --------------------------------------------------------------------------------------------

class FLINT_API OperationManagerSimple : public IOperationManager {
	friend class StockOperationSimple;
	friend class ProportionalOperationSimple;
public:
	OperationManagerSimple(Timing& timing, const DynamicObject& config);
	virtual ~OperationManagerSimple() = default;

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

	int poolCount() override { return static_cast<int>(_poolValues.size()); }

    const IPool* addPool(const std::string& name, const std::string& description, const std::string& units, double scale, int order, const std::shared_ptr<ITransform> transform) override;
	const IPool* addPool(const std::string& name, double initValue = 0.0) override;
	const IPool* addPool(const std::string& name, const std::string& description, const std::string& units, double scale,
	                     int order, double initValue = 0.0) override;
	const IPool* addPool(PoolMetaData& metadata, double initValue) override;

	const IPool* getPool(const std::string& name) const override;
	const IPool* getPool(int index) const override;;

	// Details of instance
	std::string name() const override { return "Simple"; }
	std::string version() const override { return "1.0"; }
	std::string config() const override {
		std::stringstream ss;
		ss  << "(" << "Kahan[" << (_useKahan ? "ON" : "OFF") << "], "
            << "ZeroTransfers[" << (_allowZeroResultTransfers ? "ON" : "OFF") << "], "
            << "AllowNegativeTransfers[" << (_allowNegativeTransfers ? "ON" : "OFF") << "], "
			<< "WarnNegativeTransfers[" << (_warnNegativeTransfers ? "ON" : "OFF") << "]"
			<< ")";
		return ss.str();
	};

protected:
	void commitPendingOperationResults();

	Timing& _timing;		// timing object for simulation

	std::vector<double>					_poolValues;		// pool values
	std::vector<double>					_corrections;		// used for https://en.wikipedia.org/wiki/Kahan_summation_algorithm

	std::vector<std::shared_ptr<IPool>>				_poolObjects;		// pool object vector
	std::map<std::string, std::shared_ptr<IPool>>	_poolNameObjectMap;	// pool objects indexed by name

	OperationResultCollection	_operationResultsPending;		// OperationResults (flux & meta-data) submitted by modules - waiting to be applied
	OperationResultCollection	_operationResultsLastApplied;	// The most recently applied OperationResults (copy)
	OperationResultCollection	_operationResultsCommitted;		// List of OperationResults that have been committed

private:
	bool _useKahan;
	bool _allowZeroResultTransfers;
    bool _allowNegativeTransfers = true;
	bool _warnNegativeTransfers = false;
    
    template<class TPool, typename TInitValue>
    const IPool* addPool(const std::string& name, const std::string& description, const std::string& units, double scale, int order, TInitValue initValue);

	// Operations
	void submitOperation(IOperation* operation) override;
};

#undef USE_INT_ITERATOR

}
} // moja::flint

#endif // MOJA_FLINT_OPERATIONMANAGERSIMPLE_H_
