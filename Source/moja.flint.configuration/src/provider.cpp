#include "moja/flint/configuration/provider.h"
#include "moja/flint/configuration/configurationexceptions.h"

#include <boost/algorithm/string.hpp>

namespace moja {
namespace flint {
namespace configuration {

	//Provider::Provider(const std::string& name, const std::string& providerType, const DynamicObject& settings) 
	//	: _name(name), _library("internal.flint"), _providerType(providerType), _settings(settings) {
	//	if (name.length() == 0 || all(name, boost::algorithm::is_space())) {
	//		BOOST_THROW_EXCEPTION(ProviderInvalidNameTypeException() << ProviderName(name) << ProviderLibrary(_library) << ProviderType(providerType));
	//	}
	//	if (providerType.length() == 0 || all(providerType, boost::algorithm::is_space())) {
	//		BOOST_THROW_EXCEPTION(ProviderInvalidNameTypeException() << ProviderName(name) << ProviderLibrary(_library) << ProviderType(providerType));
	//	}
	//	if(settings.empty() || !settings.contains("type")) {
	//		BOOST_THROW_EXCEPTION(ProviderSettingsException() << ProviderName(name) << ProviderLibrary(_library));
	//	}
	//}

	Provider::Provider(const std::string& name, const std::string& library, const std::string& providerType, const DynamicObject& settings)
		: _name(name), _library(library), _providerType(providerType), _settings(settings) {
		if (name.length() == 0 || all(name, boost::algorithm::is_space())) {
			BOOST_THROW_EXCEPTION(ProviderInvalidNameTypeException() << ProviderName(name) << ProviderLibrary(library) << ProviderType(providerType));
		}
		if (library.length() == 0 || all(library, boost::algorithm::is_space())) {
			BOOST_THROW_EXCEPTION(ProviderInvalidNameTypeException() << ProviderName(name) << ProviderLibrary(library) << ProviderType(providerType));
		}
		if (providerType.length() == 0 || all(providerType, boost::algorithm::is_space())) {
			BOOST_THROW_EXCEPTION(ProviderInvalidNameTypeException() << ProviderName(name) << ProviderLibrary(library) << ProviderType(providerType));
		}
		if (settings.empty() || !settings.contains("type")) {
			BOOST_THROW_EXCEPTION(ProviderSettingsException() << ProviderName(name) << ProviderLibrary(library));
		}
	}


}
}
} // namespace moja::flint::configuration
