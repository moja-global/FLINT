#ifndef MOJA_FLINT_LOCATIONIDXTRANSFORM_H_
#define MOJA_FLINT_LOCATIONIDXTRANSFORM_H_

#include "moja/flint/itransform.h"

namespace moja {
namespace datarepository {
	class IProviderLayer;
	class TileBlockCellIndexer;
}

namespace flint {
class IVariable;

class FLINT_API LocationIdxTransform : public ITransform {
public:
	LocationIdxTransform() {};

	void configure(DynamicObject config,
		const ILandUnitController& landUnitController,
		datarepository::DataRepository& dataRepository) override;

	void controllerChanged(const ILandUnitController& controller) override;
	const DynamicVar& value() const override;

private:
	const ILandUnitController* _landUnitController;

	const IVariable* _tileIdx;
	const IVariable* _blockIdx;
	const IVariable* _cellIdx;
	const datarepository::IProviderLayer* _layer;
	const datarepository::TileBlockCellIndexer* _indexer;

	mutable DynamicVar _cachedValue;
};

}
} // namespace moja::flint

#endif // MOJA_FLINT_LOCATIONIDXTRANSFORM_H_