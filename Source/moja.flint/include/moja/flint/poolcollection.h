#ifndef MOJA_FLINT_POOLCOLLECTION_H_
#define MOJA_FLINT_POOLCOLLECTION_H_

#include "moja/flint/_flint_exports.h"

#include <string>
#include <map>
#include <vector>

namespace moja {
namespace flint {
class IPool;

class FLINT_API PoolCollection {
public:
	explicit PoolCollection(std::vector<std::shared_ptr<IPool>>& poolObjects)
		: _poolObjects(poolObjects) {}
	virtual ~PoolCollection() {}

	typedef std::vector<std::shared_ptr<IPool>>::iterator iterator;
	typedef std::vector<std::shared_ptr<IPool>>::const_iterator const_iterator;
	typedef std::vector<std::shared_ptr<IPool>>::reference reference;
	typedef std::vector<std::shared_ptr<IPool>>::const_reference const_reference;
	typedef std::vector<std::shared_ptr<IPool>>::size_type size_type;

	virtual void push_back(const std::shared_ptr<IPool>& val) const { _poolObjects.push_back(val); }
	virtual void pop_back() const { _poolObjects.pop_back(); }
	virtual void clear() const { _poolObjects.clear(); }

	virtual size_type size() const { return _poolObjects.size(); }
	virtual iterator begin() { return _poolObjects.begin(); }
	virtual const_iterator begin() const { return _poolObjects.begin(); }

	virtual iterator end() { return _poolObjects.end(); }
	virtual const_iterator end() const { return _poolObjects.end(); }

	virtual reference operator[](size_t n) { return _poolObjects[n]; }
	virtual const_reference operator[](size_t n) const { return _poolObjects[n]; }

	// Custom

	const IPool* findPool(const std::string& name);

	const IPool* findPool(const std::string& name) const;


private:
	std::vector<std::shared_ptr<IPool>>& _poolObjects;

	// faster way to find items from name. This map needs to be maintained with inserts and deletes
	typedef std::map<const std::string, IPool*> StringToItemMap;
};


}
} // namespace moja::flint

#endif // MOJA_FLINT_POOLCOLLECTION_H_