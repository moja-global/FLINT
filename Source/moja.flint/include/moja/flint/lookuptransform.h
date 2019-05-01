#ifndef MOJA_FLINT_LOOKUPTRANSFORM_H_
#define MOJA_FLINT_LOOKUPTRANSFORM_H_

#include "moja/flint/_flint_exports.h"
#include "moja/flint/itransform.h"
#include "moja/dynamic.h"

namespace moja {
namespace flint {

class FLINT_API LookupTransform : public ITransform {
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
	std::string _fromVarName;
	std::string _toVarName;
	mutable DynamicVar _cachedValue;

	const DynamicVar& matchDynamicToStruct(const DynamicVar& from, const DynamicVar& to) const;
	const DynamicVar& matchDynamicToMultiStruct(const DynamicVar& from, const DynamicVar& to) const;
	const DynamicVar& matchStructToMultiStruct(const DynamicVar& from, const DynamicVar& to) const;
    
    const void setCachedValue(const DynamicVar& value) const {
        _cachedValue = nullptr;
        _cachedValue = value;
    }
};

}
} // namespace moja::flint

#endif // MOJA_FLINT_LOOKUPTRANSFORM_H_
