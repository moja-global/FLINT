#ifndef MOJA_FLINT_CONFIGURATION_VARIABLE_H_
#define MOJA_FLINT_CONFIGURATION_VARIABLE_H_

#include "moja/flint/configuration/_configuration_exports.h"
#include "moja/flint/configuration/ivariable.h"

#include <moja/dynamic.h>

#include <string>

namespace moja {
namespace flint {
namespace configuration {

class CONFIGURATION_API Variable : public IVariable {
  public:
   Variable(const std::string& name, DynamicVar value = DynamicVar());
   virtual ~Variable() {}

   virtual const DynamicVar& value() const { return _value; }

  private:
   DynamicVar _value;
};

}  // namespace configuration
}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_CONFIGURATION_VARIABLE_H_
