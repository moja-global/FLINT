#ifndef MOJA_FLINT_COMPOSITETRANSFORM_H_
#define MOJA_FLINT_COMPOSITETRANSFORM_H_

#include "moja/flint/_flint_exports.h"
#include "moja/flint/itransform.h"

#include <moja/dynamic.h>

namespace moja {
namespace flint {
class IVariable;

class FLINT_API CompositeTransform : public ITransform {
  public:
   enum Format { Wide, Long };

   void configure(DynamicObject config, const ILandUnitController& landUnitController,
                  moja::datarepository::DataRepository& dataRepository) override;

   void controllerChanged(const ILandUnitController& controller) override;
   const DynamicVar& value() const override;

  private:
   const ILandUnitController* _landUnitController;
   datarepository::DataRepository* _dataRepository;
   std::vector<std::string> _varNames;
   Format _format;
   mutable std::vector<const IVariable*> _variables;
   mutable DynamicVar _cachedValue;
   bool _allowNulls = false;

   const DynamicVar& longFormatValue() const;
   const DynamicVar& wideFormatValue() const;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_COMPOSITETRANSFORM_H_
