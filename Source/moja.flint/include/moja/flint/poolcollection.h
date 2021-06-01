#pragma once

#include "moja/flint/_flint_exports.h"

#include <string>
#include <vector>

namespace moja::flint {

class IPool;

class FLINT_API PoolCollection {
  public:
   explicit PoolCollection(std::vector<std::shared_ptr<IPool>>& poolObjects) : _poolObjects(poolObjects) {}
   virtual ~PoolCollection() {}

   using iterator = std::vector<std::shared_ptr<IPool>>::iterator;
   using const_iterator = std::vector<std::shared_ptr<IPool>>::const_iterator;
   using reference = std::vector<std::shared_ptr<IPool>>::reference;
   using const_reference = std::vector<std::shared_ptr<IPool>>::const_reference;
   using size_type = std::vector<std::shared_ptr<IPool>>::size_type;

   virtual void push_back(const std::shared_ptr<IPool>& val) const { _poolObjects.push_back(val); }
   virtual void pop_back() const { _poolObjects.pop_back(); }
   virtual void clear() const { _poolObjects.clear(); }

   [[nodiscard]] virtual size_type size() const { return _poolObjects.size(); }
   [[nodiscard]] virtual iterator begin() { return _poolObjects.begin(); }
   [[nodiscard]] virtual const_iterator begin() const { return _poolObjects.begin(); }

   [[nodiscard]] virtual iterator end() { return _poolObjects.end(); }
   [[nodiscard]] virtual const_iterator end() const { return _poolObjects.end(); }

   [[nodiscard]] virtual reference operator[](size_type n) { return _poolObjects[n]; }
   [[nodiscard]] virtual const_reference operator[](size_type n) const { return _poolObjects[n]; }

   // Custom
   [[nodiscard]] IPool* findPool(const std::string& name);
   [[nodiscard]] const IPool* findPool(const std::string& name) const;

  private:
   std::vector<std::shared_ptr<IPool>>& _poolObjects;

};

}  // namespace flint
