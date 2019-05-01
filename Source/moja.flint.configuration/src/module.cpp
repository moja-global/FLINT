#include "moja/flint/configuration/module.h"

#include <boost/algorithm/string.hpp>

namespace moja {
namespace flint {
namespace configuration {

	Module::Module(const std::string& libraryName, const std::string& name, int order, bool isProxy, DynamicObject settings)
		: _libraryName(libraryName), _name(name), _order(order), _isProxy(isProxy), _settings(settings) {

		if (libraryName.length() == 0 || all(libraryName, boost::algorithm::is_space())) {
			throw std::invalid_argument("library name cannot be empty");
		}

		if (name.length() == 0 || all(name, boost::algorithm::is_space())) {
			throw std::invalid_argument("name cannot be empty");
		}
	}

}}} // namespace moja::flint::configuration
