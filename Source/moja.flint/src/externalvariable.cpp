#include "moja/flint/externalvariable.h"

#include "moja/flint/itransform.h"

#include <moja/exception.h>

#include <boost/format.hpp>

namespace moja {
namespace flint {

const DynamicVar& ExternalVariable::value() const { return _transform->value(); }

void ExternalVariable::set_value(DynamicVar) {
   auto msg = (boost::format("Can't set External Variable: %1%.") % _info.name).str();
   throw ApplicationException(msg);
}

void ExternalVariable::controllerChanged(const ILandUnitController& controller) const {
   _transform->controllerChanged(controller);
}
}  // namespace flint
}  // namespace moja
