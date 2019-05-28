#ifndef MOJA_DATAREPOSITORY_IPROVIDERSPATIALRASTERINTERFACE_H_
#define MOJA_DATAREPOSITORY_IPROVIDERSPATIALRASTERINTERFACE_H_

#include "moja/datarepository/iproviderinterface.h"
#include "moja/datarepository/spatialblockscollection.h"
#include "moja/datarepository/spatialcellscollection.h"
#include "moja/datarepository/spatialtilescollection.h"

#include <moja/dynamic.h>

namespace moja {
namespace datarepository {

// --------------------------------------------------------------------------------------------

class IProviderLayer {
  public:
   virtual ~IProviderLayer() = default;
   virtual DynamicVar getValueByCellIndex(const CellIdx& cell) const = 0;
   virtual const TileBlockCellIndexer& indexer() const = 0;
};

// --------------------------------------------------------------------------------------------
/**
 * Interface class that all provider implementations for Spatial Raster data should derive from.
 */

class IProviderSpatialRasterInterface : public IProviderInterface {
  public:
   /**
    * required because providers are deleted via a pointer to this interface
    */
   IProviderSpatialRasterInterface() = default;
   virtual ~IProviderSpatialRasterInterface() = default;

   virtual const IProviderLayer* getLayer(const std::string& name) const = 0;

   virtual DynamicVar GetValue(const std::string& name, double Latitude, double Longitude) = 0;
   virtual DynamicVar GetValueStack(const std::string& name, double Latitude, double Longitude) = 0;

   virtual DynamicVar GetValue(const std::string& name, const CellIdx& cell) = 0;
   virtual DynamicVar GetValueStack(const std::string& name, const CellIdx& cell) = 0;

   virtual CellIdx getCellIdx(double lat, double lon) = 0;
   virtual BlockIdx getBlockIdx(double lat, double lon) = 0;
   virtual TileIdx getTileIdx(double lat, double lon) = 0;

   virtual ProviderTypes providerType() override { return ProviderTypes::Raster; }
   virtual double area(const CellIdx& cell) const = 0;
   virtual double area(const Point& point) const = 0;

   virtual SpatialCellsCollection cells() const = 0;
   virtual SpatialCellsCollection cells(Rectangle areaOfInterest) const = 0;
   virtual SpatialCellsCollection cells(const TileIdx& tileIdx) const = 0;
   virtual SpatialCellsCollection cells(const BlockIdx& blockIdx) const = 0;

   virtual SpatialBlocksCollection blocks() const = 0;
   virtual SpatialBlocksCollection blocks(Rectangle areaOfInterest) const = 0;
   virtual SpatialBlocksCollection blocks(const TileIdx& tileIdx) const = 0;

   virtual SpatialTilesCollection tiles() const = 0;
   virtual SpatialTilesCollection tiles(Rectangle areaOfInterest) const = 0;

   virtual const TileBlockCellIndexer& indexer() = 0;
   virtual const TileBlockCellIndexer& indexer(const std::string& layerName) = 0;

   virtual CellIdx getLayerCellIdx(const std::string& name, UInt32 tile_idx, UInt32 block_idx,
                                   UInt32 cell_idx) const = 0;
   virtual CellIdx getLayerCellIdx(const std::string& name, double lat, double lon) const = 0;
};

}  // namespace datarepository
}  // namespace moja

#endif  // MOJA_DATAREPOSITORY_IPROVIDERSPATIALRASTERINTERFACE_H_
