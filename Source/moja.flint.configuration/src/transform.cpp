#include "moja/flint/configuration/transform.h"

#include <boost/algorithm/string.hpp>

namespace moja {
namespace flint {
namespace configuration {

Transform::Transform(const std::string& libraryName, const std::string& typeName, DynamicObject settings)
    : _libraryName(libraryName), _typeName(typeName) {
   if (libraryName.length() == 0 || all(libraryName, boost::algorithm::is_space())) {
      throw std::invalid_argument("libraryName cannot be empty");
   }
   if (typeName.length() == 0 || all(typeName, boost::algorithm::is_space())) {
      throw std::invalid_argument("typeName cannot be empty");
   }
   _settings = settings;
}

}  // namespace configuration
}  // namespace flint
}  // namespace moja
