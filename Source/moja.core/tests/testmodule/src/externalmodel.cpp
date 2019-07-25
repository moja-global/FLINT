#include "externalmodel.h"

#include <memory>
#include <typeinfo>

#if defined(_WIN32)
#define MOJA_LIB_API __declspec(dllexport)
#endif
#if !defined(MOJA_LIB_API)
#if !defined(FULLCAM_NO_GCC_API_ATTRIBUTE) && defined(GNUC) && (GNUC >= 4)
#define MOJA_LIB_API attribute((visibility("default")))
#else
#define MOJA_LIB_API
#endif
#endif

#if !defined(MOJA_LIB_API)
#if !defined(FULLCAM_NO_GCC_API_ATTRIBUTE) && defined(__GNUC__) && (__GNUC__ >= 4)
#define MOJA_LIB_API __attribute__((visibility("default")))
#else
#define MOJA_LIB_API
#endif
#endif

#define IMPLEMENT_MODULE(ModuleImplClass, ModuleName) \
   extern "C" MOJA_LIB_API void* initializeModule() { return new ModuleImplClass(); }

namespace moja {
namespace externalmodel {

std::string ExternalModelTest::type() const { return typeid(*this).name(); }

IMPLEMENT_MODULE(ExternalModelTest, MyExternalModel)
}  // namespace externalmodel
}  // namespace moja
