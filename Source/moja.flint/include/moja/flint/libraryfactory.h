#pragma once

#include "moja/flint/librarymanager.h"

namespace moja::flint {

extern "C" int getFlintModuleRegistrations(ModuleRegistration* outModuleRegistrations);
extern "C" int getFlintTransformRegistrations(TransformRegistration* outTransformRegistrations);
extern "C" int getFlintFlintDataRegistrations(FlintDataRegistration* outFlintDataRegistrations);
extern "C" int getFlintFlintDataFactoryRegistrations(FlintDataFactoryRegistration* outFlintDataFactoryRegistrations);
extern "C" int getProviderRegistrations(DataRepositoryProviderRegistration* outDataRepositoryProviderRegistration);

}  // namespace moja::flint
