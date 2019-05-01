#ifndef MOJA_FLINT_IDNAMEDESCCOLLECTION_H_
#define MOJA_FLINT_IDNAMEDESCCOLLECTION_H_

#include "moja/flint/_flint_exports.h"
#include "moja/flint/iflintdata.h"

#include "moja/dynamic.h"

namespace moja {
namespace flint {

class FLINT_API IdNameDescCollection : public flint::IFlintData {
public:
	IdNameDescCollection() = default;
	virtual ~IdNameDescCollection() = default;
	IdNameDescCollection(const IdNameDescCollection&) = delete;
	IdNameDescCollection& operator= (const IdNameDescCollection&) = delete;

	void configure(DynamicObject config, const flint::ILandUnitController& landUnitController, moja::datarepository::DataRepository& dataRepository) override {

		_collectionName = config.contains("collection_name") ? config["collection_name"].extract<const std::string>() : "unnamed";
		_data.clear();
		const auto& arr = config["items"].extract<DynamicVector>();
		for (auto& item : arr) {
			_data.emplace_back(DataRecord{ item["id"], item["name"].extract<const std::string>(), item["desc"].extract<const std::string>() });
		}
	}

	DynamicObject exportObject() const override {
		DynamicObject object;
		object["collection_name"] = _collectionName;
		DynamicVector items;
		for (auto& dataItem : _data) {
			items.push_back(
				DynamicObject({ 
					{ "id", dataItem._id },
					{ "name", dataItem._name },
					{ "desc", dataItem._desc }
			}));
		}
		object["items"] = items;
		return object;
	};

	struct DataRecord {
		Int64 _id;
		std::string _name;
		std::string _desc;
	};

	std::string _collectionName;
	std::vector<DataRecord> _data;
};

}
} // namespace moja::flint

#endif // MOJA_FLINT_IDNAMEDESCCOLLECTION_H_