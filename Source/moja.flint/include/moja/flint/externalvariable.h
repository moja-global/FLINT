#pragma once

#include "moja/flint/_flint_exports.h"
#include "moja/flint/ivariable.h"

namespace moja::flint {

class ILandUnitController;
class ITransform;

class FLINT_API ExternalVariable : public IVariable {
  public:
   ExternalVariable(const std::shared_ptr<ITransform> transform, VariableInfo info)
       : _info(info), _transform(transform) {}

   ~ExternalVariable() = default;

   const VariableInfo& info() const override { return _info; }
   const DynamicVar& value() const override;
   void set_value(DynamicVar) override;
   void reset_value() override { /* TODO: FILL THIS IN IF YOU WANT - maybe throw exception here*/
   }
   bool isExternal() const override { return true; }
   bool isFlintData() const override { return false; }
   void controllerChanged(const ILandUnitController& controller) const;

  private:
   VariableInfo _info;
   const std::shared_ptr<ITransform> _transform;
};

}  // namespace moja::flint
