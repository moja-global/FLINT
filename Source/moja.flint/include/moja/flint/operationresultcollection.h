#ifndef MOJA_FLINT_OPERATIONRESULTCOLLECTION_H_
#define MOJA_FLINT_OPERATIONRESULTCOLLECTION_H_

#include "moja/flint/_flint_exports.h"

#include <vector>

namespace moja {
namespace flint {
class IOperationResult;

class FLINT_API OperationResultCollection {
public:
	typedef std::vector<std::shared_ptr<IOperationResult>>::value_type value_type;
	typedef std::vector<std::shared_ptr<IOperationResult>>::reference reference;
	typedef std::vector<std::shared_ptr<IOperationResult>>::const_reference const_reference;
	typedef std::vector<std::shared_ptr<IOperationResult>>::iterator iterator;
	typedef std::vector<std::shared_ptr<IOperationResult>>::const_iterator const_iterator;
	typedef std::vector<std::shared_ptr<IOperationResult>>::size_type size_type;

	OperationResultCollection() = default;
	virtual ~OperationResultCollection() = default;
	OperationResultCollection(const OperationResultCollection&) = delete;
	OperationResultCollection& operator= (const OperationResultCollection&) = delete;

	iterator begin();
	const_iterator begin() const MOJA_NOEXCEPT;
	iterator end() MOJA_NOEXCEPT;
	const_iterator end() const MOJA_NOEXCEPT;
	void clear();
	size_type size() const MOJA_NOEXCEPT;
	template <class... Args>
	void emplace_back(Args&&... args);

	void push_back(const_reference operationResult);
	void push_back(value_type&& operationResult);
	bool empty() const MOJA_NOEXCEPT;
	void reserve(size_type i);
private:
	std::vector<std::shared_ptr<IOperationResult>> _vec;
};

template <class ... Args>
void OperationResultCollection::emplace_back(Args&&... args) {
	_vec.emplace_back(std::forward<Args>(args)...);
}

inline OperationResultCollection::iterator OperationResultCollection::begin() {
	return _vec.begin();
}

inline OperationResultCollection::const_iterator OperationResultCollection::begin() const MOJA_NOEXCEPT {
	return _vec.begin();
}

inline OperationResultCollection::iterator OperationResultCollection::end() MOJA_NOEXCEPT	{
	return _vec.end();
}

inline OperationResultCollection::const_iterator OperationResultCollection::end() const MOJA_NOEXCEPT {
	return _vec.end();
}

inline void OperationResultCollection::clear() {
	_vec.clear();
}

inline OperationResultCollection::size_type OperationResultCollection::size() const MOJA_NOEXCEPT {
	return _vec.size();
}

inline void OperationResultCollection::push_back(const_reference operationResult) {
	_vec.push_back(operationResult);
}

inline void OperationResultCollection::push_back(value_type&& operationResult) {
	_vec.push_back(operationResult);
}

inline bool OperationResultCollection::empty() const MOJA_NOEXCEPT {
	return _vec.empty();
}

inline void OperationResultCollection::reserve(size_type count) {
	_vec.reserve(count);
}
}
} // moja::flint

#endif // MOJA_FLINT_OPERATIONRESULTCOLLECTION_H_
