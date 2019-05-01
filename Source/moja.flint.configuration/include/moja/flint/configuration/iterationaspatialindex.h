#ifndef MOJA_FLINT_CONFIGURATION_ITERATIONASPATIALINDEX_H_
#define MOJA_FLINT_CONFIGURATION_ITERATIONASPATIALINDEX_H_

#include "moja/flint/configuration/_configuration_exports.h"
#include "moja/flint/configuration/iterationbase.h"

namespace moja {
namespace flint {
namespace configuration {

class CONFIGURATION_API IterationASpatialIndex : public IterationBase {
public:
	IterationASpatialIndex();
	virtual ~IterationASpatialIndex() {}

	virtual int maxTileSize()	const { return _maxTileSize; }
	virtual int tileCacheSize()	const { return _tileCacheSize; }

	virtual void set_maxTileSize(int value) { _maxTileSize = value; }
	virtual void set_tileCacheSize(int value) { _tileCacheSize = value; }

private:
	int _maxTileSize;		// = landscape["max_tile_size"];
	int _tileCacheSize;		// = landscape["tile_cache_size"];
};

}
}
} // namespace moja::flint::configuration

#endif // MOJA_FLINT_CONFIGURATION_ITERATIONASPATIALINDEX_H_
