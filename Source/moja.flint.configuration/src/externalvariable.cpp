#include "moja/flint/configuration/externalvariable.h"

#include <boost/algorithm/string.hpp>

namespace moja {
namespace flint {
namespace configuration {

ExternalVariable::ExternalVariable(const std::string& name, std::shared_ptr<Transform> transform)
    : IVariable(VariableType::External, name), _transform(transform) {}

}  // namespace configuration
}  // namespace flint
}  // namespace moja
