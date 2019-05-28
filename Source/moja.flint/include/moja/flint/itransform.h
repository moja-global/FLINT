#ifndef MOJA_FLINT_ITRANSFORM_H_
#define MOJA_FLINT_ITRANSFORM_H_

#include "moja/flint/_flint_exports.h"

#include <moja/dynamic.h>

namespace moja {
namespace datarepository {
class DataRepository;
}
namespace flint {
class ILandUnitController;

class FLINT_API ITransform {
  public:
   virtual ~ITransform() = default;

   virtual void configure(DynamicObject config, const ILandUnitController& landUnitController,
                          datarepository::DataRepository& dataRepository) = 0;
   virtual void controllerChanged(const ILandUnitController& controller) = 0;
   virtual const DynamicVar& value() const = 0;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_ITRANSFORM_H_
