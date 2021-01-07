#pragma once

#include "moja/flint/ioperationresult.h"
#include "moja/flint/ioperationresultfluxiterator.h"
#include "moja/flint/operationresultfluxcohort.h"

namespace moja::flint {

#define USE_INT_ITERATOR

class OperationResultFluxIteratorCohort : public IOperationResultFluxIterator {
  public:
   explicit OperationResultFluxIteratorCohort(std::vector<OperationResultFluxSimple>& results, bool atEnd = false)
       : results_(results) {
#ifdef USE_INT_ITERATOR
      if (atEnd || results.empty())
         current_index_ = -1;
      else
         current_index_ = 0;
#else
      if (atEnd)
         _it = _results.end();
      else
         _it = _results.begin();
#endif
   }

   virtual ~OperationResultFluxIteratorCohort() {}

   virtual std::shared_ptr<IOperationResultFluxIterator> createIterator(bool atEnd = false) override {
      return std::make_shared<OperationResultFluxIteratorCohort>(results_, atEnd);
   }

   virtual IOperationResultFlux* dereference() const override {
#ifdef USE_INT_ITERATOR
      if (current_index_ < 0) return nullptr;
      return &results_[current_index_];
#else
      if (_it != _results.end())
         return (*_it).get();
      else
         return nullptr;
#endif
   }

   virtual void increment() override {
#ifdef USE_INT_ITERATOR
      current_index_++;
      if (current_index_ >= results_.size()) current_index_ = -1;
#else
      ++_it;
#endif
   }

   virtual void decrement() override {
#ifdef USE_INT_ITERATOR
      current_index_--;
      if (current_index_ < 0) current_index_ = -1;
#else
      --_it;
#endif
   }

   virtual bool isEqual(IOperationResultFluxIterator& it) const override {
      auto* p = static_cast<OperationResultFluxIteratorCohort*>(&it);
#ifdef USE_INT_ITERATOR
      return (current_index_ == p->current_index_);
#else
      return _it == p->_it;
#endif
   }

  protected:
   std::vector<OperationResultFluxSimple>& results_;
#ifdef USE_INT_ITERATOR
   int current_index_;
#else
   std::vector<std::shared_ptr<IOperationResultFlux>>::iterator _it;
#endif
};

#undef USE_INT_ITERATOR

}  // namespace moja::flint
