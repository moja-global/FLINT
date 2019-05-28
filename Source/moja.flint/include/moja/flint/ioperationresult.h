#ifndef MOJA_FLINT_IOPERATIONRESULT_H_
#define MOJA_FLINT_IOPERATIONRESULT_H_

#include "moja/flint/operationresultfluxcollection.h"

#include <moja/dynamic.h>

namespace moja {
namespace flint {

class Timing;
struct ModuleMetaData;
enum class OperationTransferType;

class IOperationResult {
  public:
   virtual ~IOperationResult() = default;

   // IOperationResult doesn't guarantee that transfers from the same source and to the same sink will be combined.
   // This means that checking the count won't be consistent across tests that have multiple transfers added to
   // same source/sink combinations.
   virtual OperationResultFluxCollection operationResultFluxCollection() = 0;

   virtual OperationTransferType transferType() const = 0;
   virtual const ModuleMetaData* metaData() const = 0;
   virtual const DynamicVar& dataPackage() const = 0;
   virtual bool hasDataPackage() const = 0;

   virtual const Timing& timingWhenApplied() const = 0;
   virtual void setTimingWhenApplied(const Timing& time) = 0;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_IOPERATIONRESULT_H_
