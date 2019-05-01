#ifndef MOJA_DATAREPOSITORY_PROVIDERSPATIALRASTERTILED_H_
#define MOJA_DATAREPOSITORY_PROVIDERSPATIALRASTERTILED_H_

#include "moja/datarepository/iproviderspatialrasterinterface.h"
#include <moja/dynamic.h>
#include <string>
#include <unordered_map>
#include "rasterreaderinterface.h"

namespace moja {
namespace datarepository {

class Layer;

// --------------------------------------------------------------------------------------------
// Moja Implmentation of a Spatial Raster data provider derived from IProviderSpatialRasterInterface.

// --------------------------------------------------------------------------------------------

class DATAREPOSITORY_API ProviderSpatialRasterTiled final : public IProviderSpatialRasterInterface {
public:
	explicit ProviderSpatialRasterTiled(
		std::shared_ptr<RasterReaderFactoryInterface> rasterReaderFactory,
		DynamicObject settings);
	virtual ~ProviderSpatialRasterTiled() = default;
	ProviderSpatialRasterTiled(ProviderSpatialRasterTiled const&) = delete;
	void operator=(ProviderSpatialRasterTiled const&) = delete;

	const IProviderLayer* getLayer(const std::string& name) const override;

	DynamicVar GetValue(const std::string& name, double Latitude, double Longitude) override;
	DynamicVar GetValueStack(const std::string& name, double Latitude, double Longitude) override;

	DynamicVar GetValue(const std::string& name, const CellIdx& cell) override;
	DynamicVar GetValueStack(const std::string& name, const CellIdx& cell) override;

	CellIdx getCellIdx(double lat, double lon) override;
	BlockIdx getBlockIdx(double lat, double lon) override;
	TileIdx getTileIdx(double lat, double lon) override;

	SpatialCellsCollection cells() const override { return SpatialCellsCollection(*(_indexer.get())); };
	SpatialCellsCollection cells(Rectangle areaOfInterest) const override { return SpatialCellsCollection(areaOfInterest, *(_indexer.get())); };
	SpatialCellsCollection cells(const TileIdx& tileIdx) const override { return SpatialCellsCollection(tileIdx, *(_indexer.get())); };
	SpatialCellsCollection cells(const BlockIdx& blockIdx) const override { return SpatialCellsCollection(blockIdx, *(_indexer.get())); };

	SpatialBlocksCollection blocks() const override { return SpatialBlocksCollection(*(_indexer.get())); };
	SpatialBlocksCollection blocks(Rectangle areaOfInterest) const override { return SpatialBlocksCollection(areaOfInterest, *(_indexer.get())); };
	SpatialBlocksCollection blocks(const TileIdx& tileIdx) const override { return SpatialBlocksCollection(tileIdx, *(_indexer.get())); };

	SpatialTilesCollection tiles() const override { return SpatialTilesCollection(*(_indexer.get())); };
	SpatialTilesCollection tiles(Rectangle areaOfInterest) const override { return SpatialTilesCollection(areaOfInterest, *(_indexer.get())); };

	const TileBlockCellIndexer& indexer() override;
	const TileBlockCellIndexer& indexer(const std::string& layerName) override;

	CellIdx getLayerCellIdx(const std::string& name, UInt32 tile_idx, UInt32 block_idx, UInt32 cell_idx) const override;
	CellIdx getLayerCellIdx(const std::string& name, double lat, double lon) const override;

	double area(const CellIdx& cell) const override;
	double area(const Point& point) const override;

private:
	const double Pi = 3.141592653590;
	const double RadiusOfEarthInMeters = 6378137.0;
	const double DiameterOfEarthInMetersPerDeg = 2.0 * Pi * RadiusOfEarthInMeters / 360.0;
	const double DegToRadMultiplier = Pi / 180.0;

	std::unique_ptr<TileBlockCellIndexer> _indexer;
	std::unordered_map<std::string, std::shared_ptr<Layer>> _layers;
};

inline const TileBlockCellIndexer& ProviderSpatialRasterTiled::indexer() {
	return *(_indexer.get());
}

inline double ProviderSpatialRasterTiled::area(const CellIdx& cell) const {
	auto lat = _indexer->getLatFromIndex(cell);
	auto x = _indexer->cellDesc.latSize * DiameterOfEarthInMetersPerDeg;
	return x * x * cos(lat * DegToRadMultiplier) * 0.0001;
}

inline double ProviderSpatialRasterTiled::area(const Point& point) const {
	auto x = _indexer->cellDesc.latSize * DiameterOfEarthInMetersPerDeg;
	return x * x * cos(point.lat * DegToRadMultiplier) * 0.0001;
}

}} // moja::datarepository

#endif // MOJA_DATAREPOSITORY_PROVIDERSPATIALRASTERTILED_H_
