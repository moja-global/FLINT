#include "moja/flint/configuration/iterationaspatialindex.h"

namespace moja {
namespace flint {
namespace configuration {

IterationASpatialIndex::IterationASpatialIndex()
    : IterationBase(LocalDomainIterationType::ASpatialIndex), _maxTileSize(0), _tileCacheSize(0) {}

}  // namespace configuration
}  // namespace flint
}  // namespace moja
