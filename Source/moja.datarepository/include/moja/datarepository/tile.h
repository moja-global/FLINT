#ifndef MOJA_DATAREPOSITORY_TILE_H_
#define MOJA_DATAREPOSITORY_TILE_H_

#include "moja/datarepository/rasterreaderinterface.h"

#include <vector>

namespace moja {
namespace datarepository {

template<class T>
class Tile {
public:
	Tile(
		std::shared_ptr<RasterReaderFactoryInterface> rasterReaderFactory,
		const std::string& path,
		const Point& origin, const std::string& prefix, const TileBlockCellIndexer& indexer,
		const DynamicObject& settings) :
		_indexer(indexer), 
		_settings(settings),
		_currentBlockIdx(std::numeric_limits<UInt32>::max()),
		_block(indexer.cellDesc.indexLimit)
	{
		_tileRasterReader = rasterReaderFactory->getTileRasterReader(path, origin, prefix, indexer, settings);
	}

	T getValueByLocation(const CellIdx& cell_idx) const;

private:
	std::unique_ptr<TileRasterReaderInterface> _tileRasterReader;
	const TileBlockCellIndexer& _indexer;
	mutable UInt32 _currentBlockIdx;
	mutable std::vector<T> _block;
	DynamicObject _settings;
};


template<class T>
T Tile<T>::getValueByLocation(const CellIdx& cellIdx) const {
	if (cellIdx.blockIdx != _currentBlockIdx) {
		auto block_idx = BlockIdx(cellIdx.tileIdx, cellIdx.blockIdx, _indexer);
		_tileRasterReader->readBlockData(block_idx, &_block);
		_currentBlockIdx = cellIdx.blockIdx;
	}
	return _block[cellIdx.cellIdx];
}

}} // moja::datarepository

#endif // MOJA_DATAREPOSITORY_TILE_H_
