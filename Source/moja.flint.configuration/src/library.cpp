#include "moja/flint/configuration/library.h"

#include <boost/algorithm/string.hpp>

namespace moja {
namespace flint {
namespace configuration {

Library::Library(const std::string& path, const std::string& library, const std::string& name, LibraryType type)
    : _path(path), _library(library), _name(name), _type(type) {
   if (path.length() == 0 || all(path, boost::algorithm::is_space())) {
      throw std::invalid_argument("path cannot be empty");
   }

   if (library.length() == 0 || all(library, boost::algorithm::is_space())) {
      throw std::invalid_argument("library cannot be empty");
   }

   if (name.length() == 0 || all(name, boost::algorithm::is_space())) {
      throw std::invalid_argument("name cannot be empty");
   }
   _hasPath = true;
}

Library::Library(const std::string& name, LibraryType type) : _path(), _name(name), _type(type) {
   if (name.length() == 0 || all(name, boost::algorithm::is_space())) {
      throw std::invalid_argument("name cannot be empty");
   }
   _hasPath = false;
}

}  // namespace configuration
}  // namespace flint
}  // namespace moja
