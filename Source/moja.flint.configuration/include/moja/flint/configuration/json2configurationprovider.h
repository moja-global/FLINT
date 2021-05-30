#ifndef MOJA_FLINT_CONFIGURATION_JSON2CONFIGURATIONPROVIDER_H_
#define MOJA_FLINT_CONFIGURATION_JSON2CONFIGURATIONPROVIDER_H_

#include "moja/flint/configuration/iconfigurationprovider.h"

#include <Poco/DynamicStruct.h>

namespace moja {
namespace flint {
namespace configuration {

enum class LibraryType;
enum class LocalDomainType;

class CONFIGURATION_API JSON2ConfigurationProvider : public IConfigurationProvider {
  public:
   explicit JSON2ConfigurationProvider(const std::vector<std::string>& configFilePath);
   explicit JSON2ConfigurationProvider(const std::vector<std::string>& configFilePath,
                                       const std::vector<std::string>& configProviderFilePath);
   virtual ~JSON2ConfigurationProvider() = default;

   virtual std::shared_ptr<Configuration> createConfiguration() override;

   std::vector<std::string> configFilePath() const { return _configFilePath; }
   std::vector<std::string> configProviderFilePath() const { return _configProviderFilePath; }

  private:
   std::vector<std::string> _configFilePath;
   std::vector<std::string> _configProviderFilePath;
   bool _hasProviderConfigFile;

   static LocalDomainType parseLocalDomainType(const std::string& type);
   static LibraryType parseLibraryType(const std::string& type);

   static bool fileExists(const std::string& path);

   void createLocalDomain(DynamicVar& parsedJSON, Configuration& config) const;
   void createSpinup(DynamicVar& parsedJSON, Configuration& config) const;
   void createLibraries(DynamicVar& parsedJSON, Configuration& config) const;
   void createProviders(DynamicVar& parsedJSON, Configuration& config) const;
   void add_pool_from_struct(Configuration& config, const Poco::DynamicStruct& poolStruct) const;
   void createPools(DynamicVar& parsedJSON, Configuration& config) const;
   bool createSpecialPools(const std::string& poolName, const Poco::DynamicStruct& poolSettings,
                           Configuration& config) const;
   bool createSpecialVariables(const std::pair<const std::string, DynamicVar>& keyValPair, Configuration& config,
                               bool isSpinup) const;
   void createVariables(DynamicVar& parsedJSON, Configuration& config, bool isSpinup = false) const;
   void createModules(DynamicVar& parsedJSON, Configuration& config) const;
   void createSpinupModules(DynamicVar& parsedJSON, Configuration& config) const;

   void mergeJsonConfigFile(std::string fileName, DynamicVar parsedResult);
   void mergeJsonProviderConfigFile(std::string fileName, DynamicVar parsedResult);
};

}  // namespace configuration
}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_CONFIGURATION_JSON2CONFIGURATIONPROVIDER_H_
