#include "moja/flint/configuration/operationmanager.h"

#include <boost/algorithm/string.hpp>

namespace moja {
namespace flint {
namespace configuration {

OperationManager::OperationManager() : _name("Simple") {}

OperationManager::OperationManager(const std::string& name, DynamicObject& settings)
    : _name(name), _settings(settings) {
   if (name.length() == 0 || all(name, boost::algorithm::is_space())) {
      _name = "Simple";
   }
}

}  // namespace configuration
}  // namespace flint
}  // namespace moja
