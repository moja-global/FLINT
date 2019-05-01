#include "moja/flint/flintdatavariable.h"
#include "moja/flint/iflintdata.h"

namespace moja {
	namespace flint {
		void FlintDataVariable::controllerChanged(const ILandUnitController& controller) const {
	_flintdata->controllerChanged(controller);
}
}
} // namespace moja::flint