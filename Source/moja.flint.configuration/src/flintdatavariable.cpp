#include "moja/flint/configuration/flintdatavariable.h"

namespace moja {
namespace flint {
namespace configuration {

FlintDataVariable::FlintDataVariable(const std::string& name, std::shared_ptr<FlintData> flintData)
	: IVariable(VariableType::FlintData, name), _flintdata(flintData) {
}

}
}
} // namespace moja::flint::configuration
