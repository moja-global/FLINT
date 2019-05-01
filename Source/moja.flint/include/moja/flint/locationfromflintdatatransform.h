#ifndef MOJA_FLINT_LOCATIONFROMFLINTDATATRANSFORM_H_
#define MOJA_FLINT_LOCATIONFROMFLINTDATATRANSFORM_H_

#include "moja/flint/itransform.h"
#include <string>


namespace moja {
	namespace datarepository {
		class IProviderSpatialRasterInterface;
	}

namespace flint {
class SpatialLocationInfo;

class FLINT_API LocationFromFlintDataTransform : public ITransform {
public:
	LocationFromFlintDataTransform() : _spatialLocationInfo(nullptr) {}

	void configure(DynamicObject config,
		const ILandUnitController& landUnitController,
		datarepository::DataRepository& dataRepository) override;

	void controllerChanged(const ILandUnitController& controller) override;
	const DynamicVar& value() const override;

private:
	const ILandUnitController* _landUnitController;
	datarepository::DataRepository* _dataRepository;
	std::shared_ptr<datarepository::IProviderSpatialRasterInterface> _provider;

	std::string _dataId;
	mutable std::shared_ptr<const SpatialLocationInfo> _spatialLocationInfo;
	mutable DynamicVar _cachedValue;
};

}
} // namespace moja::flint

#endif // MOJA_FLINT_LOCATIONFROMFLINTDATATRANSFORM_H_