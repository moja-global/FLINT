#ifndef MOJA_FLINT_LOCATIONTRANSFORM_H_
#define MOJA_FLINT_LOCATIONTRANSFORM_H_

#include "moja/flint/itransform.h"

#include <string>

namespace moja {
namespace datarepository {
	class IProviderSpatialRasterInterface;
}

namespace flint {
class IVariable;

class FLINT_API LocationTransform : public ITransform {
public:
	LocationTransform() {};

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
	const IVariable* _latitude;
	const IVariable* _longitude;
	mutable DynamicVar _cachedValue;
};

}
} // namespace moja::flint

#endif // MOJA_FLINT_LOCATIONTRANSFORM_H_
