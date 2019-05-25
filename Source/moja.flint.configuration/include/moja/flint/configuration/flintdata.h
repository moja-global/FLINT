#ifndef MOJA_FLINT_CONFIGURATION_FLINTDATA_H_
#define MOJA_FLINT_CONFIGURATION_FLINTDATA_H_

#include "moja/flint/configuration/_configuration_exports.h"

#include <moja/dynamic.h>

namespace moja {
namespace flint {
namespace configuration {

class CONFIGURATION_API FlintData {
  public:
   FlintData(const std::string& libraryName, const std::string& typeName, DynamicObject& settings);
   virtual ~FlintData() {}

   virtual inline const std::string& libraryName() const { return _libraryName; }
   virtual inline const std::string& typeName() const { return _typeName; }
   virtual inline const DynamicObject& settings() const { return _settings; }

  private:
   std::string _libraryName;
   std::string _typeName;
   DynamicObject _settings;
};

}  // namespace configuration
}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_CONFIGURATION_FLINTDATA_H_
