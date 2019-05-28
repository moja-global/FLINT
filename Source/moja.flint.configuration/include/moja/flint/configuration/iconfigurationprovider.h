#ifndef MOJA_FLINT_CONFIGURATION_ICONFIGURATIONPROVIDER_H_
#define MOJA_FLINT_CONFIGURATION_ICONFIGURATIONPROVIDER_H_

#include "moja/flint/configuration/_configuration_exports.h"
#include "moja/flint/configuration/configuration.h"

namespace moja {
namespace flint {
namespace configuration {

class CONFIGURATION_API IConfigurationProvider {
  public:
   virtual ~IConfigurationProvider() {}

   virtual std::shared_ptr<Configuration> createConfiguration() = 0;
};

}  // namespace configuration
}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_CONFIGURATION_ICONFIGURATIONPROVIDER_H_
