#ifndef MOJA_FLINT_IOPERATIONRESULTFLUXITERATOR_H_
#define MOJA_FLINT_IOPERATIONRESULTFLUXITERATOR_H_

#include "moja/flint/_flint_exports.h"

namespace moja {
namespace flint {

class IOperationResultFlux;
// --------------------------------------------------------------------------------------------

class IOperationResultFluxIterator {
  public:
   explicit IOperationResultFluxIterator() {}
   virtual ~IOperationResultFluxIterator(){};

   virtual std::shared_ptr<IOperationResultFluxIterator> createIterator(bool atEnd = false) = 0;

   virtual IOperationResultFlux* dereference() const = 0;
   virtual void increment() = 0;
   virtual void decrement() = 0;

   virtual bool isEqual(IOperationResultFluxIterator& it) const = 0;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_IOPERATIONRESULTFLUXITERATOR_H_