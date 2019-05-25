#ifndef MOJA_DATAREPOSITORY_IPROVIDERSPATIALRASTERSTACKINTERFACE_H_
#define MOJA_DATAREPOSITORY_IPROVIDERSPATIALRASTERSTACKINTERFACE_H_

#include "moja/datarepository/iproviderinterface.h"
#include "moja/datarepository/tileblockcellindexer.h"

#include <moja/dynamic.h>

namespace moja {
namespace datarepository {
struct CellIdx;

class IProviderRasterStack {
  public:
   virtual ~IProviderRasterStack() = default;
   virtual DynamicVar getValueByCellIndex(const CellIdx& cell) const = 0;
   virtual const TileBlockCellIndexer& indexer() const = 0;
};

// --------------------------------------------------------------------------------------------
/**
 * Interface class that all provider implementations for Spatial Raster stack data should derive from.
 */
class IProviderSpatialRasterStackInterface : public IProviderInterface {
  public:
   /**
    * required because providers are deleted via a pointer to this interface
    */
   IProviderSpatialRasterStackInterface() = default;
   virtual ~IProviderSpatialRasterStackInterface() = default;

   virtual const IProviderRasterStack* getRasterStack(const std::string& name) const = 0;

   virtual DynamicVar GetValue(const std::string& name, double latitude, double longitude) = 0;
   virtual DynamicVar GetValue(const std::string& name, const CellIdx& cell) = 0;

   ProviderTypes providerType() override { return ProviderTypes::RasterStack; }

   virtual const TileBlockCellIndexer& indexer() = 0;
};

}  // namespace datarepository
}  // namespace moja

#endif  // MOJA_DATAREPOSITORY_IPROVIDERSPATIALRASTERSTACKINTERFACE_H_