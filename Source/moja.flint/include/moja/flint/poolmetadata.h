#ifndef MOJA_FLINT_POOLMETADATA_H_
#define MOJA_FLINT_POOLMETADATA_H_

#include "moja/flint/_flint_exports.h"

#include <string>

namespace moja {
namespace flint {

class FLINT_API PoolMetaData {
  public:
   PoolMetaData() : _name(""), _description(""), _units(""), _scale(1.0), _order(-1) {}

   PoolMetaData(const std::string& name, const std::string& description, const std::string& units, double scale,
                int order)
       : _name(name), _description(description), _units(units), _scale(scale), _order(order) {}

   virtual ~PoolMetaData() = default;

   virtual const std::string& name() const { return _name; }

   virtual const std::string& description() const { return _description; }

   virtual const std::string& units() const { return _units; }

   virtual double scale() const { return _scale; }

   virtual int order() const { return _order; }

  private:
   std::string _name;
   std::string _description;
   std::string _units;
   double _scale;
   int _order;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_POOLMETADATA_H_
