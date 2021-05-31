#pragma once

#include "moja/flint/configuration/_configuration_exports.h"

#include <optional>
#include <string>

namespace moja::flint::configuration {
class Transform;

class CONFIGURATION_API ExternalPool {
  public:
   ExternalPool(const std::string& name, std::shared_ptr<Transform> transform = nullptr,
                std::optional<std::string> parent = {});
   ExternalPool(const std::string& name, const std::string& description, const std::string& units, double scale,
                int order, std::shared_ptr<Transform> transform = nullptr, std::optional<std::string> parent = {});
   virtual ~ExternalPool() {}

   [[nodiscard]] virtual const Transform& transform() const { return *_initValue_transform; }
   [[nodiscard]] virtual const std::string& name() const { return _name; }
   [[nodiscard]] virtual const std::string& description() const { return _description; }
   [[nodiscard]] virtual const std::string& units() const { return _units; }
   [[nodiscard]] virtual double scale() const { return _scale; }
   [[nodiscard]] virtual int order() const { return _order; }
   [[nodiscard]] const std::optional<std::string>& parent() const { return _parent; }

  private:
   std::string _name;
   std::string _description;
   std::string _units;
   std::optional<std::string> _parent;
   double _scale;
   int _order;
   std::shared_ptr<Transform> _initValue_transform;
};

}  // namespace moja::flint::configuration
