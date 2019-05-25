#ifndef MOJA_MODULES_LIBPQ_LIBRARYFACTORY_H_
#define MOJA_MODULES_LIBPQ_LIBRARYFACTORY_H_

#include <moja/flint/librarymanager.h>

namespace moja {
namespace modules {
namespace libpq {

extern "C" MOJA_LIB_API int getModuleRegistrations(moja::flint::ModuleRegistration* outModuleRegistrations);
extern "C" MOJA_LIB_API int getTransformRegistrations(moja::flint::TransformRegistration* outTransformRegistrations);
extern "C" MOJA_LIB_API int getFlintDataRegistrations(moja::flint::FlintDataRegistration* outFlintDataRegistrations);
extern "C" MOJA_LIB_API int getFlintDataFactoryRegistrations(
    moja::flint::FlintDataFactoryRegistration* outFlintDataFactoryRegistrations);
extern "C" MOJA_LIB_API int getDataRepositoryProviderRegistrations(
    moja::flint::DataRepositoryProviderRegistration* outDataRepositoryProviderRegistration);

}  // namespace libpq
}  // namespace modules
}  // namespace moja

#endif  // MOJA_MODULES_LIBPQ_LIBRARYFACTORY_H_