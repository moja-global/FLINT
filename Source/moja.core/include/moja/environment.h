#ifndef MOJA_CORE_ENVIRONMENT_H_
#define MOJA_CORE_ENVIRONMENT_H_

#include "moja/_core_exports.h"

namespace moja {

/**
 * <summary>
 * This class provides access to environment variables and some general system information.
 * </summary>
 */
class CORE_API Environment {  ///: public Poco::Environment {
  public:
   /**
    * <summary>    Gets the folder the executable resides in. </summary>
    */
   static std::string startProcessFolder();
};

}  // namespace moja

#endif  // MOJA_CORE_ENVIRONMENT_H_
