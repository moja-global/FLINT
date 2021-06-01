#pragma once

#include "moja/flint/_flint_exports.h"
#include "moja/flint/iflintdata.h"

namespace moja::flint {

class FLINT_API OperationExtraData : public IFlintData {
  public:
   OperationExtraData() = default;
   OperationExtraData(std::string moduleName, std::string moduleMethod, std::string other)
       : _moduleName(moduleName), _moduleMethod(moduleMethod), _other(other) {}
   virtual ~OperationExtraData() = default;

   virtual void configure(DynamicObject config, const flint::ILandUnitController& landUnitController,
                          moja::datarepository::DataRepository& dataRepository) override {}

   std::string _moduleName;
   std::string _moduleMethod;
   std::string _other;
};

}  // namespace moja::flint
