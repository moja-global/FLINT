#ifndef MOJA_DATAREPOSITORY_PROVIDERSPATIALRASTERGDAL_H_
#define MOJA_DATAREPOSITORY_PROVIDERSPATIALRASTERGDAL_H_

#include "moja/datarepository/iproviderspatialrasterinterface.h"

#include <moja/dynamic.h>

#include <string>

namespace moja {
namespace datarepository {

// --------------------------------------------------------------------------------------------
// Moja Implmentation of a Spatial Raster data provider derived from IProviderSpatialRasterInterface.

class DATAREPOSITORY_API ProviderSpatialRasterGDAL : public IProviderSpatialRasterInterface {
  public:
   explicit ProviderSpatialRasterGDAL(DynamicObject settings) {}
   ProviderSpatialRasterGDAL(ProviderSpatialRasterGDAL const&) = delete;
   void operator=(ProviderSpatialRasterGDAL const&) = delete;
   virtual ~ProviderSpatialRasterGDAL() = default;

   DynamicVar GetValue(const std::string& name, double Latitude, double Longitude) override;
   DynamicVar GetValueStack(const std::string& name, double Latitude, double Longitude) override;
};

}  // namespace datarepository
}  // namespace moja

#endif  // MOJA_DATAREPOSITORY_PROVIDERSPATIALRASTERGDAL_H_
