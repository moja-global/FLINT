#include "moja/datarepository/providerspatialvectorgdal.h"

#include <moja/dynamic.h>

#include <vector>

namespace moja {
namespace datarepository {

ProviderSpatialVectorGDAL::ProviderSpatialVectorGDAL() {}

DynamicVar ProviderSpatialVectorGDAL::GetValue(const std::string& name, double Latitude, double Longitude) {
   double value = 42.42;
   DynamicVar x = value;
   return x;
}

DynamicVar ProviderSpatialVectorGDAL::GetValueStack(const std::string& name, double Latitude, double Longitude) {
   std::vector<double> values = {42.42, 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9};
   DynamicVar x = values;
   return x;
}

DynamicVar ProviderSpatialVectorGDAL::GetAttribute(const std::string& name, double Latitude, double Longitude) {
   double value = 42.42;
   DynamicVar x = value;
   return x;
}

DynamicVar ProviderSpatialVectorGDAL::GetAttributeStack(const std::string& name, double Latitude, double Longitude) {
   std::vector<double> values = {42.42, 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9};
   DynamicVar x = values;
   return x;
}

}  // namespace datarepository
}  // namespace moja