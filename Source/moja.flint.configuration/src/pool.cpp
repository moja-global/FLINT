#include "moja/flint/configuration/pool.h"

#include <boost/algorithm/string.hpp>

namespace moja {
namespace flint {
namespace configuration {

	Pool::Pool(const std::string& name, double initValue)
		: _name(name), _description(""), _units(""), _scale(1.0), _order(0), _initValue(initValue) {

		if (name.length() == 0 || all(name, boost::algorithm::is_space())) {
			throw std::invalid_argument("name cannot be empty");
		}
	}

	Pool::Pool(const std::string& name, const std::string& description, const std::string& units, double scale, int order, double initValue)
		: _name(name), _description(description), _units(units), _scale(scale), _order(order), _initValue(initValue) {

		if (name.length() == 0 || all(name, boost::algorithm::is_space())) {
			throw std::invalid_argument("name cannot be empty");
		}
	}


}
}
} // namespace moja::flint::configuration
