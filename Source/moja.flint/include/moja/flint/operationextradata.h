#ifndef MOJA_FLINT_OPERATIONEXTRADATA_H_
#define MOJA_FLINT_OPERATIONEXTRADATA_H_

#include "moja/flint/_flint_exports.h"
#include "moja/flint/iflintdata.h"

namespace moja {
namespace flint {

class FLINT_API OperationExtraData : public IFlintData {
public:
	OperationExtraData() = default;
	OperationExtraData(std::string moduleName, std::string moduleMethod, std::string other) :_moduleName(moduleName), _moduleMethod(moduleMethod), _other(other) {}
	virtual ~OperationExtraData() = default;

	virtual void configure(DynamicObject config, const flint::ILandUnitController& landUnitController, moja::datarepository::DataRepository& dataRepository) override {}

	std::string _moduleName;
	std::string _moduleMethod;
	std::string _other;
};

} // flint
} // namespace moja

#endif // MOJA_FLINT_OPERATIONEXTRADATA_H_