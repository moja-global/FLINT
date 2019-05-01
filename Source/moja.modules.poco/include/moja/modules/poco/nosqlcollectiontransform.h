#ifndef MOJA_MODULES_POCO_NOSQLCOLLECTIONTRANSFORM_H_
#define MOJA_MODULES_POCO_NOSQLCOLLECTIONTRANSFORM_H_

#include "moja/modules/poco/_modules.poco_exports.h"
#include "moja/flint/itransform.h"

#include <string>
#include <unordered_map>

namespace moja {
namespace datarepository {
	class IProviderNoSQLInterface;
}
namespace flint {
	class IVariable;
}

namespace modules {
namespace poco {

/// This transform will cache an entire nosql collection and allow the use to get a document based on a specified key value

class POCO_API NoSQLCollectionTransform : public flint::ITransform {
public:
	NoSQLCollectionTransform();
	NoSQLCollectionTransform(const NoSQLCollectionTransform&) = delete;

	void configure(DynamicObject config, const flint::ILandUnitController& landUnitController, moja::datarepository::DataRepository& dataRepository) override;

	NoSQLCollectionTransform& operator=(const NoSQLCollectionTransform&) = delete;
	void controllerChanged(const flint::ILandUnitController& controller) override;
	const DynamicVar& value() const override;

private:
	DynamicObject _config;
	const flint::ILandUnitController* _landUnitController;
	moja::datarepository::DataRepository* _dataRepository;
	std::shared_ptr<datarepository::IProviderNoSQLInterface> _provider;

	std::string	_keyName;
	std::string	_keyValueVariable;
	const flint::IVariable* _keyVariable;
	std::unordered_map<int, DynamicVar> _collection;

	mutable DynamicVar _dataSet;
};

}}}

#endif // MOJA_MODULES_POCO_NOSQLCOLLECTIONTRANSFORM_H_