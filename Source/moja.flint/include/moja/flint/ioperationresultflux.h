#pragma once

namespace moja::flint {

struct ModuleMetaData;
enum class OperationTransferType;

class IOperationResultFlux {
  public:
   virtual ~IOperationResultFlux() = default;

   virtual int source() const = 0;
   virtual int sink() const = 0;
   virtual double value() const = 0;

   virtual OperationTransferType transferType() const = 0;
   virtual const ModuleMetaData* metaData() const = 0;
};

}  // namespace moja::flint
