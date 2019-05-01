#ifndef MOJA_DATAREPOSITORY_STACK_H_
#define MOJA_DATAREPOSITORY_STACK_H_

#include "moja/datarepository/cube.h"
#include "moja/datarepository/datarepositoryexceptions.h"
#include <vector>

namespace moja {
namespace datarepository {

template<class T>
class Stack {
public:
	Stack(
		std::shared_ptr<RasterReaderFactoryInterface> rasterReaderFactory,
		const std::string& path,
		const Point& origin, 
		const std::string& prefix, 
		size_t blockSize, 
		size_t nSeries, 
		const TileBlockCellIndexer& indexer,
		const DynamicObject& settings) :
			_indexer(indexer), 
			_currentBlockIdx(std::numeric_limits<UInt32>::max()), 
			_block(blockSize), 
			_nSeries(nSeries), 
			_settings(settings)
	{
		_stackRasterReader = rasterReaderFactory->getStackRasterReader(path, origin, prefix, indexer, _settings);
	}

	std::vector<T> getValueByLocation(const CellIdx& cellIdx) const;

private:
	std::unique_ptr<StackRasterReaderInterface> _stackRasterReader;
	const TileBlockCellIndexer& _indexer;
	mutable UInt32 _currentBlockIdx; //  = std::numeric_limits<UInt32>::max();
	mutable std::vector<T> _block;
	size_t _nSeries;
	DynamicObject _settings;
};

template <class T>
inline std::vector<T> Stack<T>::getValueByLocation(const CellIdx& cellIdx) const {
	if (cellIdx.blockIdx != _currentBlockIdx) {
		auto block_idx = BlockIdx(cellIdx.tileIdx, cellIdx.blockIdx, _indexer);
		_stackRasterReader->readBlockData(block_idx, int(_nSeries), &_block);
		_currentBlockIdx = cellIdx.blockIdx;
	}
	// Now get the series from the block read
	auto begin = _block.begin() + cellIdx.cellIdx * _nSeries;
	std::vector<T> series(_nSeries);
	std::copy(begin, begin + _nSeries, series.begin());
	return series;
}

}} // moja::datarepository

#endif // MOJA_DATAREPOSITORY_LAYER_H_
