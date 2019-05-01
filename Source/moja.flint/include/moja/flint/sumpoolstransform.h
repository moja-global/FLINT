#ifndef MOJA_FLINT_SUMPOOLSTRANSFORM_H_
#define MOJA_FLINT_SUMPOOLSTRANSFORM_H_

//#include "moja/flint/ilandunitcontroller.h"
#include "moja/flint/itransform.h"

#include <string>

namespace moja {
namespace flint {
class IPool;
class ILandUnitController;

class FLINT_API SumPoolsTransform : public ITransform {
public:
	SumPoolsTransform() {};

	void configure(DynamicObject config,
		const ILandUnitController& landUnitController,
		moja::datarepository::DataRepository& dataRepository) override;

	void controllerChanged(const ILandUnitController& controller) override;
	const DynamicVar& value() const override;

private:
	const ILandUnitController* _landUnitController;
	datarepository::DataRepository* _dataRepository;

	std::vector<std::string> poolNames;
	std::vector<const IPool*> poolHandles;

	mutable DynamicVar _result;
};

}
} // namespace moja::flint

#endif // MOJA_FLINT_SUMPOOLSTRANSFORM_H_
