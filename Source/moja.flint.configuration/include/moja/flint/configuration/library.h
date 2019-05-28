#ifndef MOJA_FLINT_CONFIGURATION_LIBRARY_H_
#define MOJA_FLINT_CONFIGURATION_LIBRARY_H_

#include "moja/flint/configuration/_configuration_exports.h"

#include <string>

namespace moja {
namespace flint {
namespace configuration {

enum class LibraryType { Internal, External, Managed, Python };

inline std::string libraryTypeToStr(configuration::LibraryType type) {
   if (type == configuration::LibraryType::Internal) return "internal";
   if (type == configuration::LibraryType::External) return "external";
   if (type == configuration::LibraryType::Managed) return "managed";
   if (type == configuration::LibraryType::Python) return "python";
   return "unknown";
}

class CONFIGURATION_API Library {
  public:
   Library(const std::string& path, const std::string& library, const std::string& name, LibraryType type);
   Library(const std::string& name, LibraryType type);
   virtual ~Library() {}

   virtual inline bool hasPath() const { return _hasPath; }
   virtual inline const std::string& path() const { return _path; }
   virtual inline const std::string& library() const { return _library; }
   virtual inline const std::string& name() const { return _name; }
   virtual inline LibraryType type() const { return _type; }

  private:
   bool _hasPath;
   std::string _path;
   std::string _library;
   std::string _name;
   LibraryType _type;
};

}  // namespace configuration
}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_CONFIGURATION_LIBRARY_H_
