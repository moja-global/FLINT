#ifndef MOJA_FLINT_LOOKUPRANDOMTRANSFORM_H_
#define MOJA_FLINT_LOOKUPRANDOMTRANSFORM_H_

#include "moja/flint/_flint_exports.h"
#include "moja/flint/itransform.h"
#include "moja/dynamic.h"

namespace moja {
namespace flint {
	class SpatialLocationInfo;

	class FLINT_API LookupRandomTransform : public ITransform {
public:
	void configure(
		DynamicObject config,
		const ILandUnitController& landUnitController,
		datarepository::DataRepository& dataRepository) override;

	void controllerChanged(const ILandUnitController& controller) override;
	const DynamicVar& value() const override;

private:
	const ILandUnitController* _landUnitController;
	datarepository::DataRepository* _dataRepository;
	std::string _toVarName;
	bool _reverseLookup;

	mutable DynamicVar _cachedValue;
	std::shared_ptr<SpatialLocationInfo> _spatiallocationinfo;
	const DynamicVar& matchDynamicToStruct(const DynamicVar& to) const;
	const DynamicVar& matchDynamicToMultiStruct(const DynamicVar& to) const;
	const DynamicVar& matchStructToMultiStruct(const DynamicVar& to) const;
};

}
} // namespace moja::flint

#endif // MOJA_FLINT_LOOKUPRANDOMTRANSFORM_H_