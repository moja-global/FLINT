#pragma once

#include "moja/flint/_flint_exports.h"
#include "moja/flint/ivariable.h"

namespace moja::flint {

class ILandUnitController;
class IFlintData;

class FlintDataVariable : public IVariable {
  public:
   FlintDataVariable(std::shared_ptr<IFlintData> flintData, std::string libraryName, std::string variableName,
                     VariableInfo info)
       : _libraryName(libraryName),
         _variableName(variableName),
         _info(info),
         _value(flintData),
         _flintdata(flintData) {}
   ~FlintDataVariable() = default;

   std::string libraryName() const { return _libraryName; }
   std::string variableName() const { return _variableName; }
   const VariableInfo& info() const override { return _info; }
   const DynamicVar& value() const override { return _value; }
   void set_value(DynamicVar value) override { _value = value; }
   void reset_value() override { /* TODO: FILL THIS IN IF YOU WANT - maybe throw exception here*/
   }
   bool isExternal() const override { return false; }
   bool isFlintData() const override { return true; }

   std::shared_ptr<const IFlintData> flintdata() const { return _flintdata; }

   std::shared_ptr<IFlintData> flintdata() { return _flintdata; }

   void controllerChanged(const ILandUnitController& controller) const;

  private:
   std::string _libraryName;
   std::string _variableName;
   VariableInfo _info;
   DynamicVar _value;
   std::shared_ptr<IFlintData> _flintdata;
};

}  // namespace moja::flint
