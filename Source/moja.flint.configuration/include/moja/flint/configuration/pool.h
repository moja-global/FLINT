#pragma once

#include "moja/flint/configuration/_configuration_exports.h"

#include <optional>
#include <string>

namespace moja::flint::configuration {

class CONFIGURATION_API Pool {
  public:
   Pool(const std::string& name, double initValue = 0.0, std::optional<std::string> parent = {});
   Pool(const std::string& name, const std::string& description, const std::string& units, double scale, int order,
        double initValue = 0.0, std::optional<std::string> parent = {});
   ~Pool() = default;

   [[nodiscard]] const std::string& name() const { return _name; }
   [[nodiscard]] const std::string& description() const { return _description; }
   [[nodiscard]] const std::string& units() const { return _units; }
   [[nodiscard]] double scale() const { return _scale; }
   [[nodiscard]] int order() const { return _order; }
   [[nodiscard]] double initValue() const { return _initValue; }
   [[nodiscard]] const std::optional<std::string>& parent() const { return _parent; }

  private:
   std::string _name;
   std::string _description;
   std::string _units;
   std::optional<std::string> _parent;
   double _scale;
   int _order;
   double _initValue;
};

}  // namespace moja::flint::configuration
