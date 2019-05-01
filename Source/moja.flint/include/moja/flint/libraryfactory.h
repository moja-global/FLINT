#ifndef MOJA_FLINT_LIBRARYFACTORY_H_
#define MOJA_FLINT_LIBRARYFACTORY_H_

#include "moja/flint/librarymanager.h"

namespace moja {
namespace flint {

extern "C" int getFlintModuleRegistrations				(moja::flint::ModuleRegistration*					outModuleRegistrations);
extern "C" int getFlintTransformRegistrations			(moja::flint::TransformRegistration*				outTransformRegistrations);
extern "C" int getFlintFlintDataRegistrations			(moja::flint::FlintDataRegistration*				outFlintDataRegistrations);
extern "C" int getFlintFlintDataFactoryRegistrations	(moja::flint::FlintDataFactoryRegistration*			outFlintDataFactoryRegistrations);
extern "C" int getDataRepositoryProviderRegistrations	(moja::flint::DataRepositoryProviderRegistration*	outDataRepositoryProviderRegistration);

}
}

#endif	// MOJA_FLINT_LIBRARYFACTORY_H_
