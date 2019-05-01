#include "moja/flint/configuration/variable.h"

namespace moja {
namespace flint {
namespace configuration {

	Variable::Variable(const std::string& name, moja::DynamicVar value)
		: IVariable(VariableType::Internal, name), _value(value) {}

}}} // namespace moja::flint::configuration
