#ifndef MOJA_FLINT_CONFIGURATION_OPERATIONMANAGER_H_
#define MOJA_FLINT_CONFIGURATION_OPERATIONMANAGER_H_

#include "moja/flint/configuration/_configuration_exports.h"

#include <moja/dynamic.h>

#include <string>

namespace moja {
namespace flint {
namespace configuration {

class CONFIGURATION_API OperationManager {
  public:
   explicit OperationManager();
   explicit OperationManager(const std::string& name, DynamicObject& settings);
   virtual ~OperationManager() {}

   virtual const std::string& name() const { return _name; }
   virtual DynamicObject& settings() const { return _settings; }

   virtual void set_name(const std::string& str) { _name = str; }
   virtual void set_settings(const DynamicObject& value) { _settings = value; }

  private:
   std::string _name;
   mutable DynamicObject _settings;
};

}  // namespace configuration
}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_CONFIGURATION_OPERATIONMANAGER_H_