#pragma once

#include "moja/flint/ivariable.h"

namespace moja::flint {

class Variable : public IVariable {
  public:
   Variable(DynamicVar value, VariableInfo info) : _info(info), _value(value), _initValue(value) {}

   ~Variable() {}

   const VariableInfo& info() const override { return _info; }

   const DynamicVar& value() const override { return _value; }

   void set_value(DynamicVar value) override {
      _value = nullptr;
      _value = value;
   }

   void reset_value() override {
      _value = nullptr;
      _value = _initValue;
   }

   bool isExternal() const override { return false; }

   bool isFlintData() const override { return false; }

  private:
   VariableInfo _info;
   DynamicVar _value;
   DynamicVar _initValue;
};

}  // namespace moja::flint
