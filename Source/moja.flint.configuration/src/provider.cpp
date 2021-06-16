#include "moja/flint/configuration/provider.h"

#include <boost/algorithm/string.hpp>

namespace moja {
namespace flint {
namespace configuration {

Provider::Provider(const std::string& name, const std::string& library, const std::string& providerType,
                   const DynamicObject& settings)
    : _name(name), _library(library), _providerType(providerType), _settings(settings) {
   if (name.length() == 0 || all(name, boost::algorithm::is_space())) {
      throw std::invalid_argument("Error provider name is empty");
   }
   if (library.length() == 0 || all(library, boost::algorithm::is_space())) {
      throw std::invalid_argument("Error provider library is empty");
   }
   if (providerType.length() == 0 || all(providerType, boost::algorithm::is_space())) {
      throw std::invalid_argument("Error provider type is empty");
   }
   if (settings.empty() || !settings.contains("type")) {
      throw std::invalid_argument("Error provider settings is empty or invalid");
   }
}

}  // namespace configuration
}  // namespace flint
}  // namespace moja
