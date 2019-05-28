#ifndef MOJA_FLINT_LIBRARYFACTORY_H_
#define MOJA_FLINT_LIBRARYFACTORY_H_

#include "moja/flint/librarymanager.h"

namespace moja {
namespace flint {

extern "C" int getFlintModuleRegistrations(ModuleRegistration* outModuleRegistrations);
extern "C" int getFlintTransformRegistrations(TransformRegistration* outTransformRegistrations);
extern "C" int getFlintFlintDataRegistrations(FlintDataRegistration* outFlintDataRegistrations);
extern "C" int getFlintFlintDataFactoryRegistrations(FlintDataFactoryRegistration* outFlintDataFactoryRegistrations);
extern "C" int getDataRepositoryProviderRegistrations(
    DataRepositoryProviderRegistration* outDataRepositoryProviderRegistration);

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_LIBRARYFACTORY_H_
