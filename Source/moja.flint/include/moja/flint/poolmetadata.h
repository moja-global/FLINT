#pragma once

#include "moja/flint/_flint_exports.h"

#include <optional>
#include <string>

namespace moja::flint {

class FLINT_API PoolMetaData {
  public:
   PoolMetaData() : _parent({}), _scale(1.0), _order(-1) {}

   PoolMetaData(const std::string& name, const std::string& description, const std::string& units, double scale,
                int order, const std::optional<std::string>& parent={})
       : _name(name), _description(description), _units(units), _parent(parent), _scale(scale), _order(order) {}

   virtual ~PoolMetaData() = default;

   virtual const std::string& name() const { return _name; }

   virtual const std::string& description() const { return _description; }

   virtual const std::string& units() const { return _units; }

   virtual double scale() const { return _scale; }

   virtual int order() const { return _order; }

   virtual const std::optional<std::string>& parent() const { return _parent; }

  private:
   std::string _name;
   std::string _description;
   std::string _units;
   std::optional<std::string> _parent;
   double _scale;
   int _order;
};

}  // namespace moja::flint
