#ifndef MOJA_DATAREPOSITORY_STACKLAYER_H_
#define MOJA_DATAREPOSITORY_STACKLAYER_H_

#include "moja/datarepository/iproviderspatialrasterinterface.h"
#include "moja/datarepository/layer.h"
#include "moja/datarepository/stack.h"

#include <string>

namespace moja {
namespace datarepository {

template <class T>
class StackLayer : public Layer {
  public:
   StackLayer(std::shared_ptr<RasterReaderFactoryInterface> rasterReaderFactory, const std::string& name,
              const std::string& path, const std::string& prefix, size_t nLayers,
              const TileBlockCellIndexer& baseIndexer, TileBlockCellIndexer indexer, T nodata,
              const DynamicObject& settings)
       : Layer(name, path, prefix, baseIndexer, indexer, settings),
         _nLayers(nLayers),
         _rasterReaderFactory(rasterReaderFactory),
         _nodata(nodata),
         _settings(settings) {}
   ~StackLayer() = default;
   DynamicVar getValueByCellIndex(const CellIdx& cell) const override;

  private:
   std::shared_ptr<RasterReaderFactoryInterface> _rasterReaderFactory;
   // std::shared_ptr<StackRasterReaderFactoryInterface> _stackRasterReaderFactory;
   size_t _nLayers;
   mutable UInt32 _currentTileIdx = std::numeric_limits<UInt32>::max();
   mutable std::shared_ptr<Stack<T>> _stack;
   T _nodata;
   DynamicObject _settings;
};

template <class T>
inline DynamicVar StackLayer<T>::getValueByCellIndex(const CellIdx& cellIdx) const {
   if (cellIdx.tileIdx != _currentTileIdx) {
      const size_t cells = _indexer.cellDesc.indexLimit * _nLayers;
      auto point = _indexer.getLatLonFromIndex({cellIdx.tileIdx, _indexer});
      _stack =
          std::make_shared<Stack<T>>(_rasterReaderFactory, _path, point, _prefix, cells, _nLayers, _indexer, _settings);
      _currentTileIdx = cellIdx.tileIdx;
   }
   auto values = _stack->getValueByLocation(_indexer.convertIndex(cellIdx));
   std::vector<boost::optional<T>> raw;
   raw.reserve(values.size());
   for (auto& value : values) {
      raw.emplace_back(value != _nodata, value);
   }
   return raw;
}

}  // namespace datarepository
}  // namespace moja

#endif  // MOJA_DATAREPOSITORY_STACKLAYER_H_
