#pragma once

#include "moja/flint/modulebase.h"

namespace moja::flint {

class FLINT_API ModuleProxyBase : public ModuleBase {
  public:
   ModuleProxyBase() : ModuleBase() {}
   virtual ~ModuleProxyBase() = default;

   void configure(const DynamicObject& config) override{};
   void subscribe(NotificationCenter& notificationCenter) override{};

   ModuleTypes moduleType() override { return ModuleTypes::Proxy; };

   virtual void addModule(const std::string& libraryName, const std::string& moduleName,
                          std::shared_ptr<IModule> module, const DynamicObject& config) {}
   virtual std::vector<std::tuple<std::string, std::string, std::string>> listModules() const {
      return std::vector<std::tuple<std::string, std::string, std::string>>();
   }
};

}  // namespace moja::flint
