#ifndef MOJA_TESTMODULE_EXTERNALMODEL_H_
#define MOJA_TESTMODULE_EXTERNALMODEL_H_

#include <string>

namespace moja {
namespace externalmodel {
class ExternalModelTest {
  public:
   ExternalModelTest() = default;
   ~ExternalModelTest() = default;

   std::string type() const;
};
}  // namespace externalmodel
}  // namespace moja

#endif  // OJA_TESTMODULE_EXTERNALMODEL_H_
