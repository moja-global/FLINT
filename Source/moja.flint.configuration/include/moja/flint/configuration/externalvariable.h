#ifndef MOJA_FLINT_CONFIGURATION_EXTERNALVARIABLE_H_
#define MOJA_FLINT_CONFIGURATION_EXTERNALVARIABLE_H_

#include "moja/flint/configuration/_configuration_exports.h"
#include "moja/flint/configuration/ivariable.h"
#include "moja/flint/configuration/transform.h"

#include <string>

namespace moja {
namespace flint {
namespace configuration {

class CONFIGURATION_API ExternalVariable : public IVariable {
  public:
   explicit ExternalVariable(const std::string& name, std::shared_ptr<Transform> transform = nullptr);
   virtual ~ExternalVariable() {}

   virtual inline const Transform& transform() const { return *_transform; }

  private:
   std::shared_ptr<Transform> _transform;
};

}  // namespace configuration
}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_CONFIGURATION_EXTERNALVARIABLE_H_
