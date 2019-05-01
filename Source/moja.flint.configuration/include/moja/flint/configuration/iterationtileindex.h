#ifndef MOJA_FLINT_CONFIGURATION_ITERATIONTILEINDEX_H_
#define MOJA_FLINT_CONFIGURATION_ITERATIONTILEINDEX_H_

#include "moja/flint/configuration/_configuration_exports.h"
#include "moja/flint/configuration/iterationbase.h"

#include <vector>

namespace moja {
namespace flint {
namespace configuration {

class ConfigTileIdx {
public:
	explicit ConfigTileIdx(UInt32 tileIdx) : _tileIdx(tileIdx) {}
	UInt32 tileIdx() const { return _tileIdx; }
private:
	UInt32 _tileIdx;
};


class CONFIGURATION_API IterationTileIndex : public IterationBase {
public:
	IterationTileIndex();
	virtual ~IterationTileIndex() {}

	virtual const std::vector<ConfigTileIdx>& tileIndexList() const { return _tileIndexList; }
	virtual std::vector<ConfigTileIdx>& tileIndexList() { return _tileIndexList; }

	virtual void addTileIndex(ConfigTileIdx index) { _tileIndexList.push_back(index); }

private:
	std::vector<ConfigTileIdx> _tileIndexList;
};

}
}
} // namespace moja::flint::configuration

#endif // MOJA_FLINT_CONFIGURATION_ITERATIONTILEINDEX_H_
