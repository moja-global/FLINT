#include "moja/flint/landunitdatawrapper.h"

#include "moja/flint/ilandunitcontroller.h"
#include "moja/flint/ioperationmanager.h"
#include "moja/flint/ivariable.h"

namespace moja {
namespace flint {

	std::shared_ptr<IOperation> LandUnitDataWrapper::createStockOperation() {
		return (_landUnitController == nullptr ? nullptr : _landUnitController->createStockOperation(*_module));
	}

	std::shared_ptr<IOperation> LandUnitDataWrapper::createStockOperation(DynamicVar& dataPackage) {
		return (_landUnitController == nullptr ? nullptr : _landUnitController->createStockOperation(*_module, dataPackage));
	}

	std::shared_ptr<IOperation> LandUnitDataWrapper::createProportionalOperation() {
		return (_landUnitController == nullptr ? nullptr : _landUnitController->createProportionalOperation(*_module));
	}

	std::shared_ptr<IOperation> LandUnitDataWrapper::createProportionalOperation(DynamicVar& dataPackage) {
		return _landUnitController == nullptr
			? nullptr
			: _landUnitController->createProportionalOperation(*_module, dataPackage);
	}

	void LandUnitDataWrapper::submitOperation(std::shared_ptr<IOperation> operation) {
		if (_landUnitController != nullptr) _landUnitController->submitOperation(operation);
	}

	void LandUnitDataWrapper::applyOperations() {
		if (_landUnitController != nullptr) _landUnitController->applyOperations();
	}

	const OperationResultCollection& LandUnitDataWrapper::getOperationPendingIterator() {
		return _landUnitController->operationManager()->operationResultsPending();
	}

	const OperationResultCollection& LandUnitDataWrapper::getOperationLastAppliedIterator() {
		return _landUnitController->operationManager()->operationResultsLastApplied();
	}

	const OperationResultCollection& LandUnitDataWrapper::getOperationCommittedIterator() {
		return _landUnitController->operationManager()->operationResultsCommitted();
	}

	bool LandUnitDataWrapper::hasLastAppliedOperationResults() const {
		return _landUnitController->operationManager()->hasOperationResultsLastApplied();
	}

	void LandUnitDataWrapper::clearLastAppliedOperationResults() {
		if (_landUnitController != nullptr) _landUnitController->clearLastAppliedOperationResults();
	}

	void LandUnitDataWrapper::clearAllOperationResults() {
		if (_landUnitController != nullptr) _landUnitController->clearAllOperationResults();
	}

	PoolCollection LandUnitDataWrapper::poolCollection() const {
		return _landUnitController->operationManager()->poolCollection();
	}

	int LandUnitDataWrapper::getPoolCount() const {
		return (_landUnitController == nullptr ? 0 : _landUnitController->operationManager()->poolCount());
	}

	const IPool* LandUnitDataWrapper::getPool(const std::string& name) const {
		return (_landUnitController == nullptr ? nullptr : _landUnitController->operationManager()->getPool(name));
	}

	const IPool* LandUnitDataWrapper::getPool(int index) const {
		return (_landUnitController == nullptr ? nullptr : _landUnitController->operationManager()->getPool(index));
	}

	IVariable* LandUnitDataWrapper::getVariable(const std::string& name) {
		return (_landUnitController == nullptr ? nullptr : _landUnitController->getVariable(name));
	}

	const IVariable* LandUnitDataWrapper::getVariable(const std::string& name) const {
		return (_landUnitController == nullptr ? nullptr : _landUnitController->getVariable(name));
	}

	std::vector<std::shared_ptr<IVariable>> LandUnitDataWrapper::variables() const {
		return (_landUnitController == nullptr ? std::vector<std::shared_ptr<IVariable>>() : _landUnitController->variables());
	}

	bool LandUnitDataWrapper::hasVariable(const std::string& name) const {
		return _landUnitController == nullptr
			? false
			: _landUnitController->hasVariable(name);
	}

	IOperationManager* LandUnitDataWrapper::operationManager() {
		return _landUnitController->operationManager();
	}

	const IOperationManager* LandUnitDataWrapper::operationManager() const {
		return _landUnitController->operationManager();
	}

	ITiming* LandUnitDataWrapper::timing() {
		return &_landUnitController->timing();
	}

	const ITiming* LandUnitDataWrapper::timing() const {
		return &_landUnitController->timing();
	}

	const configuration::Configuration* LandUnitDataWrapper::config() {
		return _landUnitController->config();
	}

}
}
