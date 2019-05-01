#ifndef MOJA_FLINT_LOCATIONIDXFROMFLINTDATATRANSFORM_H_
#define MOJA_FLINT_LOCATIONIDXFROMFLINTDATATRANSFORM_H_

#include "moja/flint/itransform.h"


namespace moja {
namespace datarepository {
	class IProviderLayer;
	class TileBlockCellIndexer;
}

namespace flint {
class SpatialLocationInfo;

class FLINT_API LocationIdxFromFlintDataTransform : public ITransform {
public:
	LocationIdxFromFlintDataTransform() : _spatialLocationInfo(nullptr) {}

	void configure(DynamicObject config,
		const ILandUnitController& landUnitController,
		datarepository::DataRepository& dataRepository) override;

	void controllerChanged(const ILandUnitController& controller) override;
	const DynamicVar& value() const override;

private:
	const ILandUnitController* _landUnitController;

	mutable std::shared_ptr<const SpatialLocationInfo> _spatialLocationInfo;
	const datarepository::IProviderLayer* _layer;
	const datarepository::TileBlockCellIndexer* _indexer;

	mutable DynamicVar _cachedValue;
};

}
} // namespace moja::flint

#endif // MOJA_FLINT_LOCATIONIDXFROMFLINTDATATRANSFORM_H_