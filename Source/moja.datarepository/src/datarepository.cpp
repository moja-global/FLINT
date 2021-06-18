#include "moja/datarepository/datarepository.h"

#include "moja/datarepository/providerrelationalsqlite.h"
#include "moja/datarepository/providerspatialrastertiled.h"

#include <moja/flint/configuration/provider.h>

namespace moja {
namespace datarepository {

void DataRepository::configure(const std::vector<const flint::configuration::Provider*>& providers) {
   for (const auto provider : providers) {
      auto p = createProvider(provider->name(), provider->library(), provider->providerType(), provider->settings());
      addProvider(provider->name(), provider->providerType(), p);
   }
}

std::shared_ptr<IProviderInterface> DataRepository::createProvider(const std::string& name, const std::string& library,
                                                                   const std::string& providerType,
                                                                   const DynamicObject& settings) const {
   const auto providerKey = ProviderKey(library, providerType);
   const auto it = _providerRegistry->find(providerKey);
   if (it == _providerRegistry->end()) {
      throw std::runtime_error("Error unsupported provider " + library + ":" + providerType);
   }
   auto providerInit = _providerRegistry->at(providerKey);
   auto providerInterface = providerInit(settings);
   if (providerInterface == nullptr) {
      throw std::runtime_error("Error provider init failed " + library + ":" + providerType);
   }
   return providerInterface;
}

void DataRepository::addProvider(const std::string& name, const std::string& providerType,
                                 std::shared_ptr<IProviderInterface> provider) {
   const auto& p = _providers.find(name);
   if (p == _providers.end()) {
      _providers.insert(std::pair<std::string, std::shared_ptr<IProviderInterface>>(name, provider));
   } else {
      throw std::runtime_error("Error provider already exists " + name + ":" + providerType);
   }
}

std::shared_ptr<IProviderInterface> DataRepository::getProvider(const std::string& name) {
   const auto& p = _providers.find(name);
   if (p != _providers.end()) {
      return p->second;
   }
   throw std::runtime_error("Error provider not found " + name);
}

}  // namespace datarepository
}  // namespace moja