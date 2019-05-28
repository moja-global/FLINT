#ifndef MOJA_FLINT_CONFIGURATION_EXTERNALPOOL_H_
#define MOJA_FLINT_CONFIGURATION_EXTERNALPOOL_H_

#include "moja/flint/configuration/_configuration_exports.h"
#include "moja/flint/configuration/transform.h"

#include <string>

namespace moja {
namespace flint {
namespace configuration {

class CONFIGURATION_API ExternalPool {
  public:
   ExternalPool(const std::string& name, std::shared_ptr<Transform> transform = nullptr);
   ExternalPool(const std::string& name, const std::string& description, const std::string& units, double scale,
                int order, std::shared_ptr<Transform> transform = nullptr);
   virtual ~ExternalPool() {}

   virtual inline const Transform& transform() const { return *_transform; }
   virtual inline const std::string& name() const { return _name; }
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
   std::shared_ptr<Transform> _transform;
};

}  // namespace configuration
}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_CONFIGURATION_EXTERNALPOOL_H_
