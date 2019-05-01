#include "moja/modules/libpq/providerrelationallibpqpostgresql.h"

#include <moja/flint/mojalibapi.h>

// Modules

// Transforms

// Flint Data

// Providers

// Other

using moja::flint::IModule;
using moja::flint::ITransform;
using moja::flint::IFlintData;
using moja::flint::ModuleRegistration;
using moja::flint::TransformRegistration;
using moja::flint::FlintDataRegistration;
using moja::flint::FlintDataFactoryRegistration;
using moja::flint::DataRepositoryProviderRegistration;

namespace moja {
namespace modules {
namespace libpq {

extern "C" {

	MOJA_LIB_API int getModuleRegistrations(ModuleRegistration* outModuleRegistrations) {
		int index = 0;
		return index;
	}

	MOJA_LIB_API int getTransformRegistrations(TransformRegistration* outTransformRegistrations) {
		int index = 0;
		return index;
	}

	MOJA_LIB_API int getFlintDataRegistrations(FlintDataRegistration* outFlintDataRegistrations) {
		auto index = 0;
		return index;
	}

	MOJA_LIB_API int getFlintDataFactoryRegistrations(FlintDataFactoryRegistration* outFlintDataFactoryRegistrations) {
		auto index = 0;
		return index;
	}

	MOJA_LIB_API int getDataRepositoryProviderRegistrations(moja::flint::DataRepositoryProviderRegistration* outDataRepositoryProviderRegistration) {
		auto index = 0;
		outDataRepositoryProviderRegistration[index++] = DataRepositoryProviderRegistration{ "LibpqPostgreSQL",	static_cast<int>(datarepository::ProviderTypes::Relational), [](const DynamicObject& settings) ->std::shared_ptr<datarepository::IProviderInterface> { return std::make_shared<ProviderRelationalLibpqPostgreSQL>(settings); } };
		return index;
	}

}

}}}
