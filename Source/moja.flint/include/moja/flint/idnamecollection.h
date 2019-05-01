#ifndef MOJA_FLINT_IDNAMECOLLECTION_H_
#define MOJA_FLINT_IDNAMECOLLECTION_H_

#include "moja/flint/_flint_exports.h"
#include "moja/flint/iflintdata.h"
#include "moja/flint/sqlquerytransform.h"
#include "moja/flint/variableandpoolstringbuilder.h"

#include "moja/dynamic.h"

namespace moja {
namespace flint {

class FLINT_API IdNameCollection : public flint::IFlintData {
public:
	IdNameCollection() = default;
	virtual ~IdNameCollection() = default;
	IdNameCollection(const IdNameCollection&) = delete;
	IdNameCollection& operator= (const IdNameCollection&) = delete;

	void configure(DynamicObject config, const flint::ILandUnitController& landUnitController, moja::datarepository::DataRepository& dataRepository) override {
		_landUnitController = &landUnitController;
		_dataRepository = &dataRepository;

		_collectionName = config.contains("collection_name") ? config["collection_name"].extract<const std::string>() : "unnamed";
		_data.clear();
		_dataLoaded = false;

		// we are looking for either"items" or "provider" and "queryString"
		if (config.contains("items")) {
			_useDataBuilder = false;
			const auto& arr = config["items"].extract<DynamicVector>();
			for (auto& item : arr) {
				_data.emplace_back(DataRecord{ item["id"], item["name"].extract<const std::string>() });
			}
		} else if (_landUnitController != nullptr && _dataRepository != nullptr && config.contains("provider") && config.contains("queryString")) {
			_useDataBuilder = true;
			_providerName = config["provider"].extract<const std::string>();
			_queryStr = config["queryString"].extract<const std::string>();
			sQLQueryTransform.configure(config, *_landUnitController, *_dataRepository);
			_queryStringBuilder.configure(*_landUnitController, _queryStr);
		}
	}

	DynamicObject exportObject() const override {
		DynamicObject object;
		object["collection_name"] = _collectionName;
		DynamicVector items;
		for (auto& dataItem : _data) {
			items.push_back(
				DynamicObject({ { "id", dataItem._id },
								{ "name", dataItem._name }}));
		}
		object["items"] = items;
		return object;
	};

	struct DataRecord {
		Int64 _id;
		std::string _name;
	};

	DataRecord& getRecord(int index);
	std::vector<DataRecord>& records() { return _data; }

private:
	std::string _collectionName;
	std::vector<DataRecord> _data;

	bool _useDataBuilder;
	bool _dataLoaded;
	std::string _providerName;
	std::string _queryStr;
	const ILandUnitController* _landUnitController;
	moja::datarepository::DataRepository* _dataRepository;
	SQLQueryTransform sQLQueryTransform;
	VariableAndPoolStringBuilder _queryStringBuilder;

};

}
} // namespace moja::flint

#endif // MOJA_FLINT_IDNAMECOLLECTION_H_