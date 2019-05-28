#ifndef MOJA_DATAREPOSITORY_DATAREPOSITORY_H_
#define MOJA_DATAREPOSITORY_DATAREPOSITORY_H_

#include "moja/datarepository/_datarepository_exports.h"

#include <moja/dynamic.h>

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace moja {
namespace flint {
namespace configuration {
class Provider;
}
}  // namespace flint

namespace datarepository {

class IProviderInterface;

typedef std::pair<std::string, std::string> ProviderKey;
typedef std::shared_ptr<datarepository::IProviderInterface> ProviderInterfacePtr;
typedef std::map<ProviderKey, std::function<ProviderInterfacePtr(const DynamicObject&)>> ProviderRegistry;

class DATAREPOSITORY_API DataRepository {
  public:
   DataRepository() : _providerRegistry(nullptr){};
   virtual ~DataRepository() = default;

   virtual void configure(const std::vector<const flint::configuration::Provider*>& providers);
   virtual std::shared_ptr<IProviderInterface> getProvider(const std::string& name);

   void setProviderRegistry(const ProviderRegistry& providerRegistry) { _providerRegistry = &providerRegistry; }

  private:
   DataRepository(DataRepository const&) = delete;
   void operator=(DataRepository const&) = delete;

   std::shared_ptr<IProviderInterface> createProvider(const std::string& name, const std::string& library,
                                                      const std::string& providerType,
                                                      const DynamicObject& settings) const;
   void addProvider(const std::string& name, const std::string& providerType,
                    std::shared_ptr<IProviderInterface> provider);

   const ProviderRegistry* _providerRegistry;
   std::map<std::string, std::shared_ptr<IProviderInterface>> _providers;
};

}  // namespace datarepository
}  // namespace moja

#endif  // MOJA_DATAREPOSITORY_DATAREPOSITORY_H_
