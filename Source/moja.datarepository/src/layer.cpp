#include "moja/datarepository/layer.h"

#include <moja/floatcmp.h>

namespace moja {
namespace datarepository {
Layer::Layer(const std::string& name, const std::string& path, const std::string& prefix,
             const TileBlockCellIndexer& baseIndexer, TileBlockCellIndexer indexer, const DynamicObject& settings)
    : _name(name), _path(path), _prefix(prefix), _indexer(indexer), _settings(settings) {
   if (!FloatCmp::equalTo(baseIndexer.cellDesc.latSize, indexer.cellDesc.latSize) ||
       !FloatCmp::equalTo(baseIndexer.cellDesc.lonSize, indexer.cellDesc.lonSize))
      atBaseResolution = false;
   else
      atBaseResolution = true;
}
}  // namespace datarepository
}  // namespace moja
