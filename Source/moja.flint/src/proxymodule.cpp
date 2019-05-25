#include "moja/flint/proxymodule.h"

#include "moja/flint/ilandunitdatawrapper.h"
#include "moja/flint/ivariable.h"

#include <moja/notificationcenter.h>
#include <moja/signals.h>

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------

moduleProxyData::moduleProxyData(const std::string& libraryName, std::string& moduleName, int order,
                                 std::shared_ptr<IModule> module, const DynamicObject& config) {
   _libraryName = libraryName;
   _moduleName = moduleName;
   _order = order;
   _module = module;
   _useVariable = false;
   _variableName = "not required";
   _variable = nullptr;
   _notificationCenterPtr = std::make_shared<NotificationCenter>();
   _module->configure(config);
   _module->subscribe(*_notificationCenterPtr.get());
}

moduleProxyData::moduleProxyData(const std::string& libraryName, std::string& moduleName, int order,
                                 std::shared_ptr<IModule> module, const DynamicObject& config, std::string variableName,
                                 const IVariable* variable) {
   _libraryName = libraryName;
   _moduleName = moduleName;
   _order = order;
   _module = module;
   _useVariable = true;
   _variableName = variableName;
   _variable = variable;
   _notificationCenterPtr = std::make_shared<NotificationCenter>();
   _module->configure(config);
   _module->subscribe(*_notificationCenterPtr.get());
}

void ProxyModule::configure(const DynamicObject& config) {
   auto proxyModules = config["proxyModules"];
   for (auto& item : proxyModules) {
      const auto& proxyModule = item.extract<const DynamicObject>();
      if (proxyModule.contains("enabled")) {
         const bool enabled = proxyModule["enabled"];
         if (!enabled) continue;
      }
      std::string libraryName = proxyModule["library"];
      std::string moduleName = proxyModule["module"];
      int moduleOrder = proxyModule["order"];
      auto moduleSettings =
          proxyModule.contains("settings") ? proxyModule["settings"].extract<const DynamicObject>() : DynamicObject();
      // auto moduleSettings = proxyModule["settings"].extract<const DynamicObject>();

      const auto key = std::make_pair(libraryName, moduleName);
      auto module = _modules[key];

      if (proxyModule.contains("variable")) {
         _modulesData.emplace_back(libraryName, moduleName, proxyModule["order"], module, moduleSettings,
                                   proxyModule["variable"], nullptr);
      } else {
         _modulesData.emplace_back(libraryName, moduleName, proxyModule["order"], module, moduleSettings);
      }
   }

   std::sort(_modulesData.begin(), _modulesData.end(),
             [](const moduleProxyData& lhs, const moduleProxyData& rhs) { return lhs._order < rhs._order; });
}

// --------------------------------------------------------------------------------------------

void ProxyModule::subscribe(NotificationCenter& notificationCenter) {
   notificationCenter.subscribe(signals::SystemInit, &ProxyModule::onSystemInit, *this);
   notificationCenter.subscribe(signals::SystemShutdown, &ProxyModule::onSystemShutdown, *this);
   notificationCenter.subscribe(signals::LocalDomainInit, &ProxyModule::onLocalDomainInit, *this);
   notificationCenter.subscribe(signals::LocalDomainShutdown, &ProxyModule::onLocalDomainShutdown, *this);
   notificationCenter.subscribe(signals::LocalDomainInit, &ProxyModule::onLocalDomainProcessingUnitInit, *this);
   notificationCenter.subscribe(signals::LocalDomainShutdown, &ProxyModule::onLocalDomainProcessingUnitShutdown, *this);
   notificationCenter.subscribe(signals::PreTimingSequence, &ProxyModule::onPreTimingSequence, *this);
   notificationCenter.subscribe(signals::TimingInit, &ProxyModule::onTimingInit, *this);
   notificationCenter.subscribe(signals::TimingPostInit, &ProxyModule::onTimingPostInit, *this);
   notificationCenter.subscribe(signals::TimingShutdown, &ProxyModule::onTimingShutdown, *this);
   notificationCenter.subscribe(signals::TimingStep, &ProxyModule::onTimingStep, *this);
   notificationCenter.subscribe(signals::TimingPreEndStep, &ProxyModule::onTimingPreEndStep, *this);
   notificationCenter.subscribe(signals::TimingEndStep, &ProxyModule::onTimingEndStep, *this);
   notificationCenter.subscribe(signals::TimingPostStep, &ProxyModule::onTimingPostStep, *this);
   notificationCenter.subscribe(signals::OutputStep, &ProxyModule::onOutputStep, *this);
   notificationCenter.subscribe(signals::DisturbanceEvent, &ProxyModule::onDisturbanceEvent, *this);
   notificationCenter.subscribe(signals::PostDisturbanceEvent, &ProxyModule::onPostDisturbanceEvent, *this);
   notificationCenter.subscribe(signals::PostNotification, &ProxyModule::onPostNotification, *this);
}

void ProxyModule::onSystemInit() {
   for (auto& m : _modulesData) {
      if (m._useVariable) {
         m._variable = _landUnitData->getVariable(m._variableName);
      }
   }
   for (auto& m : _modulesData) {
      if (m._useVariable ? m._variable->value().convert<bool>() : true)
         m._notificationCenterPtr->postNotification(moja::signals::SystemInit);
   }
}

void ProxyModule::onSystemShutdown() {
   for (auto& m : _modulesData) {
      if (m._useVariable ? m._variable->value().convert<bool>() : true)
         m._notificationCenterPtr->postNotification(moja::signals::SystemShutdown);
   }
}

void ProxyModule::onLocalDomainInit() {
   for (auto& m : _modulesData) {
      if (m._useVariable) {
         m._variable = _landUnitData->getVariable(m._variableName);
      }
   }
   for (auto& m : _modulesData) {
      if (m._useVariable ? m._variable->value().convert<bool>() : true)
         m._notificationCenterPtr->postNotification(moja::signals::LocalDomainInit);
   }
}

void ProxyModule::onLocalDomainShutdown() {
   for (auto& m : _modulesData) {
      if (m._useVariable ? m._variable->value().convert<bool>() : true)
         m._notificationCenterPtr->postNotification(moja::signals::LocalDomainShutdown);
   }
}

void ProxyModule::onLocalDomainProcessingUnitInit() {
   for (auto& m : _modulesData) {
      if (m._useVariable ? m._variable->value().convert<bool>() : true)
         m._notificationCenterPtr->postNotification(moja::signals::LocalDomainProcessingUnitInit);
   }
}

void ProxyModule::onLocalDomainProcessingUnitShutdown() {
   for (auto& m : _modulesData) {
      if (m._useVariable ? m._variable->value().convert<bool>() : true)
         m._notificationCenterPtr->postNotification(moja::signals::LocalDomainProcessingUnitShutdown);
   }
}

void ProxyModule::onPreTimingSequence() {
   for (auto& m : _modulesData) {
      if (m._useVariable ? m._variable->value().convert<bool>() : true)
         m._notificationCenterPtr->postNotification(moja::signals::PreTimingSequence);
   }
}

void ProxyModule::onTimingInit() {
   for (auto& m : _modulesData) {
      if (m._useVariable ? m._variable->value().convert<bool>() : true)
         m._notificationCenterPtr->postNotification(moja::signals::TimingInit);
   }
}

void ProxyModule::onTimingPostInit() {
   for (auto& m : _modulesData) {
      if (m._useVariable ? m._variable->value().convert<bool>() : true)
         m._notificationCenterPtr->postNotification(moja::signals::TimingPostInit);
   }
}

void ProxyModule::onTimingShutdown() {
   for (auto& m : _modulesData) {
      if (m._useVariable ? m._variable->value().convert<bool>() : true)
         m._notificationCenterPtr->postNotification(moja::signals::TimingShutdown);
   }
}

void ProxyModule::onTimingStep() {
   for (auto& m : _modulesData) {
      if (m._useVariable ? m._variable->value().convert<bool>() : true)
         m._notificationCenterPtr->postNotification(moja::signals::TimingStep);
   }
}

void ProxyModule::onTimingPreEndStep() {
   for (auto& m : _modulesData) {
      if (m._useVariable ? m._variable->value().convert<bool>() : true)
         m._notificationCenterPtr->postNotification(moja::signals::TimingPreEndStep);
   }
}

void ProxyModule::onTimingEndStep() {
   for (auto& m : _modulesData) {
      if (m._useVariable ? m._variable->value().convert<bool>() : true)
         m._notificationCenterPtr->postNotification(moja::signals::TimingEndStep);
   }
}

void ProxyModule::onTimingPostStep() {
   for (auto& m : _modulesData) {
      if (m._useVariable ? m._variable->value().convert<bool>() : true)
         m._notificationCenterPtr->postNotification(moja::signals::TimingPostStep);
   }
}

void ProxyModule::onOutputStep() {
   for (auto& m : _modulesData) {
      if (m._useVariable ? m._variable->value().convert<bool>() : true)
         m._notificationCenterPtr->postNotification(moja::signals::OutputStep);
   }
}

void ProxyModule::onError(std::string msg) {
   for (auto& m : _modulesData) {
      if (m._useVariable ? m._variable->value().convert<bool>() : true)
         m._notificationCenterPtr->postNotification(moja::signals::Error, msg);
   }
}

void ProxyModule::onDisturbanceEvent(DynamicVar n) {
   for (auto& m : _modulesData) {
      if (m._useVariable ? m._variable->value().convert<bool>() : true)
         m._notificationCenterPtr->postNotification(moja::signals::DisturbanceEvent, n);
   }
}

void ProxyModule::onPostDisturbanceEvent() {
   for (auto& m : _modulesData) {
      if (m._useVariable ? m._variable->value().convert<bool>() : true)
         m._notificationCenterPtr->postNotification(moja::signals::PostDisturbanceEvent);
   }
}

void ProxyModule::onPostNotification(short preMessageSignal) {
   for (auto& m : _modulesData) {
      if (m._useVariable ? m._variable->value().convert<bool>() : true)
         m._notificationCenterPtr->postNotification(moja::signals::PostNotification, preMessageSignal);
   }
}

}  // namespace flint
}  // namespace moja
