#ifndef MOJA_DATAREPOSITORY_PROVIDERSPATIALVECTORGDAL_H_
#define MOJA_DATAREPOSITORY_PROVIDERSPATIALVECTORGDAL_H_

#include "moja/datarepository/_datarepository_exports.h"
#include "moja/datarepository/iproviderspatialvectorinterface.h"

#include <string>

namespace moja {
namespace datarepository {

// --------------------------------------------------------------------------------------------
/**
 * Moja Implmentation of a Spatial Vector data provider derived from IProviderSpatialVectorInterface.
 */
class DATAREPOSITORY_API ProviderSpatialVectorGDAL : public IProviderSpatialVectorInterface {
  public:
   ProviderSpatialVectorGDAL();
   virtual ~ProviderSpatialVectorGDAL() {}

   DynamicVar GetValue(const std::string& name, double Latitude, double Longitude) override;
   DynamicVar GetValueStack(const std::string& name, double Latitude, double Longitude) override;

   DynamicVar GetAttribute(const std::string& name, double Latitude, double Longitude) override;
   DynamicVar GetAttributeStack(const std::string& name, double Latitude, double Longitude) override;
};

}  // namespace datarepository
}  // namespace moja

#endif  // MOJA_DATAREPOSITORY_PROVIDERSPATIALVECTORGDAL_H_
