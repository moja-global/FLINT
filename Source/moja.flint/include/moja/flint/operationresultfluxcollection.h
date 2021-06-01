#pragma once

#include "moja/flint/_flint_exports.h"
#include "moja/flint/ioperationresultfluxiterator.h"

#include <string>

namespace moja::flint {

class FLINT_API OperationResultFluxCollection {
  public:
   template <bool is_const_iterator = true>
   class _iterator : public std::iterator<std::forward_iterator_tag, IOperationResultFlux*> {
     public:
      typedef _iterator self_type;
      typedef typename std::conditional<is_const_iterator, const value_type&, value_type&>::type ValueReferenceType;
      typedef typename std::conditional<is_const_iterator, const value_type, value_type>::type ValueType;

      _iterator() = delete;
      explicit _iterator(std::shared_ptr<IOperationResultFluxIterator> it, bool atEnd = false);

      self_type& operator++();
      self_type operator++(int);

      ValueType operator*();

      bool operator==(const self_type& rhs);
      bool operator!=(const self_type& rhs);

      friend class _iterator<true>;

     private:
      void increment() const;
      std::shared_ptr<IOperationResultFluxIterator> _iteratorBase;
   };

   typedef _iterator<false> iterator;
   typedef _iterator<true> const_iterator;

   explicit OperationResultFluxCollection(std::shared_ptr<IOperationResultFluxIterator> it)
       : _collectionBaseIterator(it) {}

   iterator begin();
   iterator end();
   const_iterator begin() const;
   const_iterator end() const;

   std::shared_ptr<IOperationResultFluxIterator> _collectionBaseIterator;
};

inline OperationResultFluxCollection::iterator OperationResultFluxCollection::begin() {
   return iterator(_collectionBaseIterator->createIterator(), false);
}

inline OperationResultFluxCollection::iterator OperationResultFluxCollection::end() {
   return iterator(_collectionBaseIterator->createIterator(true), true);
}

inline OperationResultFluxCollection::const_iterator OperationResultFluxCollection::begin() const {
   return const_iterator(_collectionBaseIterator->createIterator(), false);
}

inline OperationResultFluxCollection::const_iterator OperationResultFluxCollection::end() const {
   return const_iterator(_collectionBaseIterator->createIterator(true), true);
}

template <bool is_const_iterator>
OperationResultFluxCollection::_iterator<is_const_iterator>::_iterator(std::shared_ptr<IOperationResultFluxIterator> it,
                                                                       bool atEnd)
    : _iteratorBase(it) {}

template <bool is_const_iterator>
inline typename OperationResultFluxCollection::_iterator<is_const_iterator>::self_type&
OperationResultFluxCollection::_iterator<is_const_iterator>::operator++() {
   increment();
   return *this;
}

template <bool is_const_iterator>
inline typename OperationResultFluxCollection::_iterator<is_const_iterator>::self_type
OperationResultFluxCollection::_iterator<is_const_iterator>::operator++(int) {
   const self_type old(*this);
   ++(*this);
   return old;
}

template <bool is_const_iterator>
inline typename OperationResultFluxCollection::_iterator<is_const_iterator>::ValueType
OperationResultFluxCollection::_iterator<is_const_iterator>::operator*() {
   return _iteratorBase->dereference();
   // return _result;
}

template <bool is_const_iterator>
inline bool OperationResultFluxCollection::_iterator<is_const_iterator>::operator==(const self_type& rhs) {
   return (_iteratorBase->isEqual(*(rhs._iteratorBase.get())));
}

template <bool is_const_iterator>
inline bool OperationResultFluxCollection::_iterator<is_const_iterator>::operator!=(const self_type& rhs) {
   return !(_iteratorBase->isEqual(*(rhs._iteratorBase.get())));
}

template <bool is_const_iterator>
inline void OperationResultFluxCollection::_iterator<is_const_iterator>::increment() const {
   _iteratorBase->increment();
}

}  // namespace moja::flint
