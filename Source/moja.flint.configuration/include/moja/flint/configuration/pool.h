#ifndef MOJA_FLINT_CONFIGURATION_POOL_H_
#define MOJA_FLINT_CONFIGURATION_POOL_H_

#include "moja/flint/configuration/_configuration_exports.h"

#include <string>

namespace moja {
namespace flint {
namespace configuration {

class CONFIGURATION_API Pool {
  public:
   Pool(const std::string& name, double initValue = 0.0);
   Pool(const std::string& name, const std::string& description, const std::string& units, double scale, int order,
        double initValue = 0.0);
   virtual ~Pool() {}

   virtual const std::string& name() const { return _name; }
   virtual const std::string& description() const { return _description; }
   virtual const std::string& units() const { return _units; }
   virtual double scale() const { return _scale; }
   virtual int order() const { return _order; }
   virtual double initValue() const { return _initValue; }

  private:
   std::string _name;
   std::string _description;
   std::string _units;
   double _scale;
   int _order;
   double _initValue;
};

}  // namespace configuration
}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_CONFIGURATION_POOL_H_
