#pragma once

#include "moja/flint/moduleproxybase.h"

namespace moja::flint {

class IVariable;

struct moduleProxyData {
   explicit moduleProxyData(const std::string& libraryName, std::string& moduleName, int order,
                            std::shared_ptr<IModule> module, const DynamicObject& config);

   explicit moduleProxyData(const std::string& libraryName, std::string& moduleName, int order,
                            std::shared_ptr<IModule> module, const DynamicObject& config, std::string variableName,
                            const IVariable* variable);

   std::string _libraryName;
   std::string _moduleName;
   int _order;
   bool _useVariable;
   std::string _variableName;
   const IVariable* _variable;
   std::shared_ptr<IModule> _module;
   std::shared_ptr<NotificationCenter> _notificationCenterPtr;
};

class ProxyModule : public ModuleProxyBase {
  public:
   using ModuleMapKey = std::pair<std::string, std::string>;
   using ModuleMap = std::map<ModuleMapKey, std::shared_ptr<IModule>>;

   ProxyModule() = default;
   virtual ~ProxyModule() = default;

   void configure(const DynamicObject& config) override;
   void subscribe(NotificationCenter& notificationCenter) override;

   void addModule(const std::string& libraryName, const std::string& moduleName, std::shared_ptr<IModule> module,
                  const DynamicObject& config) override {
      auto key = std::make_pair(libraryName, moduleName);
      _modules.insert(std::make_pair(key, module));
   }

   std::vector<std::tuple<std::string, std::string, std::string>> listModules() const override {
      std::vector<std::tuple<std::string, std::string, std::string>> result;
      for (auto item : _modulesData) {
         result.emplace_back(item._libraryName, item._moduleName,
                             item._useVariable ? item._variableName : "varibale not used");
      }
      return result;
   }

   void onSystemInit() override;
   void onSystemShutdown() override;
   void onLocalDomainInit() override;
   void onLocalDomainShutdown() override;
   void onLocalDomainProcessingUnitInit() override;
   void onLocalDomainProcessingUnitShutdown() override;
   void onPreTimingSequence() override;
   void onTimingInit() override;
   void onTimingPostInit() override;
   void onTimingShutdown() override;
   void onTimingStep() override;
   void onTimingPreEndStep() override;
   void onTimingEndStep() override;
   void onTimingPostStep() override;
   void onOutputStep() override;
   void onError(std::string msg) override;
   void onDisturbanceEvent(DynamicVar) override;
   void onPostDisturbanceEvent() override;
   void onPostNotification(short preMessageSignal) override;

  private:
   std::vector<moduleProxyData> _modulesData;  // proxy for these modules
   ModuleMap _modules;
};

}  // namespace moja::flint
