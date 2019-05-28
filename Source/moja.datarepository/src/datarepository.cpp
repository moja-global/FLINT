#include "moja/datarepository/datarepository.h"

#include "moja/datarepository/datarepositoryexceptions.h"
#include "moja/datarepository/providerrelationalsqlite.h"
#include "moja/datarepository/providerspatialrastertiled.h"

#include <moja/flint/configuration/provider.h>

#include <boost/algorithm/string.hpp>

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
   auto it = _providerRegistry->find(providerKey);
   if (it == _providerRegistry->end()) {
      BOOST_THROW_EXCEPTION(moja::datarepository::ProviderUnsupportedException() << ProviderType(providerType));
   }
   auto providerInit = _providerRegistry->at(providerKey);
   auto providerInterface = providerInit(settings);
   if (providerInterface == nullptr) {
      BOOST_THROW_EXCEPTION(moja::datarepository::ProviderUnsupportedException() << ProviderType(providerType));
   }
   return providerInterface;
}

void DataRepository::addProvider(const std::string& name, const std::string& providerType,
                                 std::shared_ptr<IProviderInterface> provider) {
   const auto& p = _providers.find(name);
   if (p == _providers.end()) {
      _providers.insert(std::pair<std::string, std::shared_ptr<IProviderInterface>>(name, provider));
   } else {
      BOOST_THROW_EXCEPTION(moja::datarepository::ProviderAlreadyExistsException()
                            << ProviderName(name) << ProviderType(providerType));
   }
}

std::shared_ptr<IProviderInterface> DataRepository::getProvider(const std::string& name) {
   const auto& p = _providers.find(name);
   if (p != _providers.end()) {
      return p->second;
   }
   BOOST_THROW_EXCEPTION(moja::datarepository::ProviderNotFoundException() << ProviderName(name));
}

}  // namespace datarepository
}  // namespace moja