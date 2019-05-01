#include "moja/flint/idnamecollection.h"
#include "moja/datarepository/datarepository.h"
#include "moja/datarepository/iproviderrelationalinterface.h"

namespace moja {
namespace flint {
	IdNameCollection::DataRecord& IdNameCollection::getRecord(int index) {
		if (_useDataBuilder && !_dataLoaded) {
			auto provider = std::static_pointer_cast<datarepository::IProviderRelationalInterface>(_dataRepository->getProvider(_providerName));
			auto dataSet = provider->GetDataSet(_queryStr);

			// Expect return to be std::vector<DynamicObject> -> of  (id, name)
			bool first = true;
			if (!dataSet.isEmpty())
			{
				auto arr = dataSet.extract<const std::vector<DynamicObject>>();
				for (auto& item : arr)
				{
					int id = item["id"];
					if (first && id != 0)
						_data.emplace_back(DataRecord{ 0, "Unknown" });
					first = false;
					_data.emplace_back(DataRecord{ item["id"], item["name"].extract<const std::string>() });
				}
			}
			_dataLoaded = true;
		}
		return _data[index];
	}
}
}
