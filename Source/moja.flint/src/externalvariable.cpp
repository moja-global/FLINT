#include "moja/flint/externalvariable.h"

#include "moja/flint/itransform.h"

namespace moja {
namespace flint {

const DynamicVar& ExternalVariable::value() const { return _transform->value(); }

void ExternalVariable::set_value(DynamicVar) {
   throw std::runtime_error("Can't set External Variable: "+ _info.name);
}

void ExternalVariable::controllerChanged(const ILandUnitController& controller) const {
   _transform->controllerChanged(controller);
}
}  // namespace flint
}  // namespace moja
