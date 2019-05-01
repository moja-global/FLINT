#ifndef MOJA_FLINT_OPERATIONRESULTFLUXITERATORSIMPLECACHE_H_
#define MOJA_FLINT_OPERATIONRESULTFLUXITERATORSIMPLECACHE_H_

#include "moja/flint/ioperationresult.h"
#include "moja/flint/operationresultfluxsimplecache.h"
#include "moja/flint/ioperationresultfluxiterator.h"

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------

#define USE_INT_ITERATOR

class OperationResultFluxIteratorSimpleCache : public IOperationResultFluxIterator {
public:
	explicit OperationResultFluxIteratorSimpleCache(std::vector<OperationResultFluxSimpleCache>& results, bool atEnd = false) : _results(results) {
#ifdef USE_INT_ITERATOR
		if (atEnd || results.size() == 0)
			_currentIndex = -1;
		else
			_currentIndex = 0;
#else
		if (atEnd)
			_it = _results.end();
		else
			_it = _results.begin();
#endif
	}

	virtual ~OperationResultFluxIteratorSimpleCache() {};

	virtual std::shared_ptr<IOperationResultFluxIterator> createIterator(bool atEnd = false) override { return std::make_shared<OperationResultFluxIteratorSimpleCache>(_results, atEnd); }

	virtual IOperationResultFlux* dereference() const override {
#ifdef USE_INT_ITERATOR
		if (_currentIndex < 0)
			return nullptr;
		//return _results[_currentIndex].get();
		return &_results[_currentIndex];
#else
		if (_it != _results.end())
			return (*_it).get();
		else
			return nullptr;
#endif
	}

	virtual void increment() override {
#ifdef USE_INT_ITERATOR
		_currentIndex++;
		if (_currentIndex >= _results.size())
			_currentIndex = -1;
#else
		++_it;
#endif
	}

	virtual void decrement() override {
#ifdef USE_INT_ITERATOR
		_currentIndex--;
		if (_currentIndex < 0)
			_currentIndex = -1;
#else
		--_it;
#endif
	}

	virtual bool isEqual(IOperationResultFluxIterator& it) const override {
		auto* p = static_cast<OperationResultFluxIteratorSimpleCache*>(&it);
#ifdef USE_INT_ITERATOR
		return (_currentIndex == p->_currentIndex);
#else
		return _it == p->_it;
#endif
	};

protected:
	std::vector<OperationResultFluxSimpleCache>& _results;
#ifdef USE_INT_ITERATOR
	int _currentIndex;
#else
	std::vector<std::shared_ptr<IOperationResultFlux>>::iterator _it;
#endif
};

#undef USE_INT_ITERATOR

}
} // moja::flint

#endif // MOJA_FLINT_OPERATIONRESULTFLUXITERATORSIMPLECACHE_H_
