#include "moja/datarepository/providernosqlpocojson.h"

#include <sstream>
#include <vector>

namespace moja {
namespace datarepository {

ProviderNoSQLPocoJSON::ProviderNoSQLPocoJSON() {}

Dynamic ProviderNoSQLPocoJSON::GetObject(int id) {
   provider_object object1 = std::make_tuple("test1", 1, 42.42, true);
   Dynamic x = object1;
   return x;
}

Dynamic ProviderNoSQLPocoJSON::GetObjectStack(const std::string& query) {
   provider_object object1 = std::make_tuple("object1", 1, 42.42, true);
   provider_object object2 = std::make_tuple("object2", 1, 11.11, false);

   std::vector<provider_object> values;
   values.push_back(object1);
   values.push_back(object2);
   Dynamic x = values;
   return x;
}

}  // namespace datarepository
}  // namespace moja