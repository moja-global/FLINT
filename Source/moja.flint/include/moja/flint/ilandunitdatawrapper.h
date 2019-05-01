#ifndef MOJA_FLINT_ILANDUNITDATAWRAPPER_H_
#define MOJA_FLINT_ILANDUNITDATAWRAPPER_H_

#include "_flint_exports.h"
#include "moja/flint/operationresultcollection.h"
#include "moja/flint/poolcollection.h"

#include "moja/dynamic.h"

#include <string>


namespace moja {
namespace flint {
namespace configuration {
	class Configuration;
}

class IModule;
class IOperation;
class IPool;
class IVariable;
class ITiming;
class ILandUnitController;
class IOperationManager;

class FLINT_API ILandUnitDataWrapper {
public:
	virtual ~ILandUnitDataWrapper() {}

	virtual std::shared_ptr<IOperation> createStockOperation() = 0;
	virtual std::shared_ptr<IOperation> createStockOperation(DynamicVar& dataPackage) = 0;
	virtual std::shared_ptr<IOperation> createProportionalOperation() = 0;
	virtual std::shared_ptr<IOperation> createProportionalOperation(DynamicVar& dataPackage) = 0;
	virtual void submitOperation(std::shared_ptr<IOperation> iOperation) = 0;
	virtual void applyOperations() = 0;

	virtual const OperationResultCollection& getOperationPendingIterator() = 0;
	virtual const OperationResultCollection& getOperationLastAppliedIterator() = 0;
	virtual const OperationResultCollection& getOperationCommittedIterator() = 0;

	virtual bool hasLastAppliedOperationResults() const = 0;

	virtual void clearLastAppliedOperationResults() = 0;
	virtual void clearAllOperationResults() = 0;

	virtual PoolCollection poolCollection() const = 0;

	virtual int getPoolCount() const = 0;
	virtual const IPool* getPool(const std::string& name) const = 0;
	virtual const IPool* getPool(int index) const = 0;

	virtual IVariable* getVariable(const std::string& name) = 0;
	virtual const IVariable* getVariable(const std::string& name) const = 0;
	virtual std::vector<std::shared_ptr<IVariable>> variables() const = 0;
    virtual bool hasVariable(const std::string& name) const = 0;

	virtual IOperationManager* operationManager() = 0;
	virtual const IOperationManager* operationManager() const = 0;

	virtual ITiming* timing() = 0;
	virtual const ITiming* timing() const = 0;

	virtual const configuration::Configuration* config() = 0;
	//private:
	// TODO: hide this method
	virtual void setLandUnitController(ILandUnitController* landUnitController) = 0;
};

}
} // moja::flint

#endif // MOJA_FLINT_ILANDUNITDATAWRAPPER_H_
