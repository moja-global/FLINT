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

   void createLocalDomain(Poco::Dynamic::Var& parsedJSON, Configuration& config) const;
   void createSpinup(Poco::Dynamic::Var& parsedJSON, Configuration& config) const;
   void createLibraries(Poco::Dynamic::Var& parsedJSON, Configuration& config) const;
   void createProviders(Poco::Dynamic::Var& parsedJSON, Configuration& config) const;
   void createPools(Poco::Dynamic::Var& parsedJSON, Configuration& config) const;
   bool createSpecialPools(const std::string& poolName, const Poco::DynamicStruct& poolSettings,
                           Configuration& config) const;
   bool createSpecialVariables(const std::pair<const std::string, Poco::Dynamic::Var>& keyValPair,
                               Configuration& config, bool isSpinup) const;
   void createVariables(Poco::Dynamic::Var& parsedJSON, Configuration& config, bool isSpinup = false) const;
   void createModules(Poco::Dynamic::Var& parsedJSON, Configuration& config) const;
   void createSpinupModules(Poco::Dynamic::Var& parsedJSON, Configuration& config) const;

   void mergeJsonConfigFile(std::string fileName, Poco::Dynamic::Var parsedResult);
   void mergeJsonProviderConfigFile(std::string fileName, Poco::Dynamic::Var parsedResult);
};

}  // namespace configuration
}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_CONFIGURATION_JSON2CONFIGURATIONPROVIDER_H_