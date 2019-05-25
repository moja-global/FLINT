#include "moja/flint/poolcollection.h"

#include "moja/flint/ipool.h"

namespace moja {
namespace flint {

const IPool* PoolCollection::findPool(const std::string& name) {
   for (auto it = _poolObjects.begin(); it != end(); ++it) {
      if (name == (*it)->name()) return (*it).get();
   }
   return nullptr;
}

const IPool* PoolCollection::findPool(const std::string& name) const {
   for (auto it = _poolObjects.begin(); it != end(); ++it) {
      if (name == (*it)->name()) return (*it).get();
   }
   return nullptr;
}

}  // namespace flint
}  // namespace moja