#include "moja/flint/configuration/externalpool.h"

#include <boost/algorithm/string.hpp>

namespace moja {
namespace flint {
namespace configuration {

ExternalPool::ExternalPool(const std::string& name, std::shared_ptr<Transform> transform)
	: _name(name), _transform(transform), _description(""), _units(""), _scale(1.0), _order(0) { }

ExternalPool::ExternalPool(const std::string& name, const std::string& description, const std::string& units, double scale, int order, std::shared_ptr<Transform> transform)
    : _name(name), _description(description), _units(units), _scale(scale), _order(order), _transform(transform) {

    if (name.length() == 0 || all(name, boost::algorithm::is_space())) {
        throw std::invalid_argument("name cannot be empty");
    }
}

}}} // namespace moja::flint::configuration
