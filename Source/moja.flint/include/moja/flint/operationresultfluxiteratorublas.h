#ifndef MOJA_FLINT_OPERATIONRESULTFLUXITERATORUBLAS_H_
#define MOJA_FLINT_OPERATIONRESULTFLUXITERATORUBLAS_H_

#include "moja/flint/matrixublas.h"
#include "moja/flint/operationresultfluxublas.h"

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------

class OperationResultFluxIteratorUblas : public IOperationResultFluxIterator {
  public:
   explicit OperationResultFluxIteratorUblas(OperationTransferType transferType, const ModuleMetaData* metaData,
                                             moja_ublas_matrix& flux, bool atEnd = false)
       : _transferType(transferType), _metaData(metaData), /*_isEmpty(true), _atEnd(atEnd), */ _flux(flux) {
      _result = std::make_shared<OperationResultFluxUblas>(_transferType, _metaData, -1, -1, 0.0);

      if (atEnd) {
         _outerIt = _flux.end1();
         //_isEmpty = _outerIt == _flux.end1();
         //_innerIt = nullptr;
         return;
      }

      _outerIt = _flux.begin1();
      if (_outerIt == _flux.end1()) {
         //_isEmpty = true;
         //_innerIt = nullptr;
         return;
      } else {
         //_isEmpty = false;
         _innerIt = _outerIt.begin();
      }

      while (_outerIt != _flux.end1()) {
         if (_innerIt == _outerIt.end()) {
            ++_outerIt;
            _innerIt = _outerIt.begin();
         } else {
            return;
         }
      }
      // if (_outerIt == _flux.end1())
      //	_atEnd = true;
      //_isEmpty = false;
   }

   virtual ~OperationResultFluxIteratorUblas(){};

   virtual std::shared_ptr<IOperationResultFluxIterator> createIterator(bool atEnd = false) override {
      return std::make_shared<OperationResultFluxIteratorUblas>(_transferType, _metaData, _flux, atEnd);
   }

   virtual IOperationResultFlux* dereference() const override {
      if (_outerIt == _flux.end1())
         // if (_atEnd)
         return nullptr;

      _result->_source = static_cast<int>(_innerIt.index1());
      _result->_sink = static_cast<int>(_innerIt.index2());
      _result->_value = *_innerIt;
      return _result.get();
   }

   virtual void increment() override {
      if ((_innerIt != _outerIt.end())) {  // we have not reached the end, yet...
         ++_innerIt;
         while (_innerIt == _outerIt.end() && _outerIt != _flux.end1()) {
            ++_outerIt;
            _innerIt = _outerIt.begin();
         }
      }
   }

   void decrement() override {
      // TODO: do we need this?
   }

   bool isEqual(IOperationResultFluxIterator& it) const override {
      auto* p = static_cast<OperationResultFluxIteratorUblas*>(&it);

      if (_outerIt == _flux.end1() && p->_outerIt == _flux.end1()) return true;

      if (_outerIt == _flux.end1() || p->_outerIt == _flux.end1()) return false;

      auto source1 = _innerIt.index1();
      auto sink1 = _innerIt.index2();
      auto source2 = _innerIt.index1();
      auto sink2 = _innerIt.index2();

      return (source1 == source2 && sink1 == sink2);
   };

  protected:
   OperationTransferType _transferType;
   const ModuleMetaData* _metaData;

   moja_ublas_matrix::const_iterator1 _outerIt;
   moja_ublas_matrix::const_iterator2 _innerIt;
   moja_ublas_matrix& _flux;
   std::shared_ptr<OperationResultFluxUblas> _result;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_OPERATIONRESULTFLUXITERATORUBLAS_H_
