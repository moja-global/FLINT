#include "moja/flint/flintdatavariable.h"

#include "moja/flint/iflintdata.h"

namespace moja {
namespace flint {
void FlintDataVariable::controllerChanged(const ILandUnitController& controller) {
   _flintdata->controllerChanged(controller);
}
}  // namespace flint
}  // namespace moja