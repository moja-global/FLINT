#include "moja/datarepository/providerspatialrastergdal.h"

namespace moja {
namespace datarepository {

DynamicVar ProviderSpatialRasterGDAL::GetValue(const std::string& name, double Latitude, double Longitude) {
   double value = 42.42;
   DynamicVar x = value;
   return x;
};

DynamicVar ProviderSpatialRasterGDAL::GetValueStack(const std::string& name, double Latitude, double Longitude) {
   double value = 42.42;
   DynamicVar x = value;
   return x;
};

}  // namespace datarepository
}  // namespace moja