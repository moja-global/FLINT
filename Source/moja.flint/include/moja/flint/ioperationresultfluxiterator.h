#pragma once

#include "moja/flint/_flint_exports.h"

namespace moja::flint {

class IOperationResultFlux;

class IOperationResultFluxIterator {
  public:
   explicit IOperationResultFluxIterator() {}
   virtual ~IOperationResultFluxIterator() {}

   virtual std::shared_ptr<IOperationResultFluxIterator> createIterator(bool atEnd = false) = 0;

   virtual IOperationResultFlux* dereference() const = 0;
   virtual void increment() = 0;
   virtual void decrement() = 0;

   virtual bool isEqual(IOperationResultFluxIterator& it) const = 0;
};

}  // namespace moja::flint
