#include "moja/flint/configuration/externalpool.h"

#include <boost/algorithm/string.hpp>

#include <utility>

namespace moja::flint::configuration {

ExternalPool::ExternalPool(const std::string& name, std::shared_ptr<Transform> transform,
                           std::optional<std::string> parent)
    : _name(name), _parent(std::move(parent)), _scale(1.0), _order(0), _initValue_transform(std::move(transform)) {
   if (name.empty() || all(name, boost::algorithm::is_space())) {
      throw std::invalid_argument("name cannot be empty");
   }
}

ExternalPool::ExternalPool(const std::string& name, const std::string& description, const std::string& units,
                           double scale, int order, std::shared_ptr<Transform> transform,
                           std::optional<std::string> parent)
    : _name(name),
      _description(description),
      _units(units),
      _parent(std::move(parent)),
      _scale(scale),
      _order(order),
      _initValue_transform(std::move(transform)) {
   if (name.empty() || all(name, boost::algorithm::is_space())) {
      throw std::invalid_argument("name cannot be empty");
   }
}

}  // namespace moja::flint::configuration
