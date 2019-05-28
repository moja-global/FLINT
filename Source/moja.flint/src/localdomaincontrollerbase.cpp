#include "moja/flint/localdomaincontrollerbase.h"

#include "moja/flint/externalvariable.h"
#include "moja/flint/flintdatavariable.h"
#include "moja/flint/flintexceptions.h"
#include "moja/flint/ioperationmanager.h"
#include "moja/flint/moduleproxybase.h"
#include "moja/flint/sequencermodulebase.h"
#include "moja/flint/variable.h"

#include <moja/flint/configuration/configuration.h>
#include <moja/flint/configuration/externalpool.h>
#include <moja/flint/configuration/externalvariable.h>
#include <moja/flint/configuration/flintdata.h>
#include <moja/flint/configuration/flintdatavariable.h>
#include <moja/flint/configuration/library.h>
#include <moja/flint/configuration/localdomain.h>
#include <moja/flint/configuration/module.h>
#include <moja/flint/configuration/pool.h>
#include <moja/flint/configuration/transform.h>
#include <moja/flint/configuration/variable.h>

#include <moja/logging.h>
#include <moja/signals.h>

using moja::flint::SequencerNotFoundException;

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------

LocalDomainControllerBase::LocalDomainControllerBase()
    : _localDomainId(0), _config(nullptr), _landUnitController(), _dataRepository(), _libraryManager() {
   _dataRepository.setProviderRegistry(_libraryManager.getProviderRegistry());
}

// --------------------------------------------------------------------------------------------

LocalDomainControllerBase::LocalDomainControllerBase(std::shared_ptr<FlintLibraryHandles> libraryHandles)
    : _localDomainId(0), _config(nullptr), _landUnitController(), _dataRepository(), _libraryManager(libraryHandles) {
   _dataRepository.setProviderRegistry(_libraryManager.getProviderRegistry());
}

// --------------------------------------------------------------------------------------------

void LocalDomainControllerBase::configure(const configuration::Configuration& config) {
   // Keep pointer to config
   _config = &config;

   // Set start and end dates for simulation
   auto& timing = _landUnitController.timing();
   timing.setStartDate(config.startDate());
   timing.setEndDate(config.endDate());
   timing.init();

   // Call the LUC config
   _landUnitController.configure(config);

   // Load the configured Libraries
   for (auto lib : config.libraries()) {
      switch (lib->type()) {
         case configuration::LibraryType::Internal:
            // Not required, all internals modules are loaded automatically
            break;
         case configuration::LibraryType::Managed:
            _libraryManager.LoadManagedLibrary();
            break;
         case configuration::LibraryType::External: {
            if (lib->hasPath()) {
               MOJA_LOG_DEBUG << "details (" << lib->name() << "): path - " << lib->path() << ", filename -"
                              << lib->library();
               _libraryManager.LoadExternalLibrary(lib->name(), lib->path(), lib->library());
            } else {
               _libraryManager.LoadExternalLibrary(lib->name());
            }
            break;
         }
#if defined(ENABLE_MOJAPY)
         case configuration::LibraryType::Python: {
            _libraryManager.LoadPythonLibrary(lib->name());
            break;
         }
#endif
         default:
            break;
      }
   }

   // Configure Providers
   _dataRepository.configure(config.providers());

   // Load the configured modules (and handle Proxies)
   _moduleMap.clear();
   for (const auto& module : config.modules()) {
      ModuleMapKey key(module->libraryName(), module->name());
      if (_moduleMap.find(key) != _moduleMap.end()) {
         // duplicate Key found
         throw DuplicateModuleDefinedException() << Details("Error duplicate module defined")
                                                 << LibraryName(module->libraryName()) << SequencerName(module->name());
      }
      _moduleMap[key] = _libraryManager.GetModule(module->libraryName(), module->name());
      _moduleMap[key]->setLandUnitController(_landUnitController);
      if (module->isProxy()) {
         auto proxy = static_cast<ModuleProxyBase*>(modules()[key]);
         auto proxyModules = module->settings()["proxyModules"];
         for (auto& item : proxyModules) {
            DynamicObject proxyModule = item.extract<const DynamicObject>();
            if (proxyModule.contains("enabled")) {
               bool enabled = proxyModule["enabled"];
               if (!enabled) continue;
            }
            std::string libraryName = proxyModule["library"];
            std::string moduleName = proxyModule["module"];
            auto moduleSettings = proxyModule.contains("settings")
                                      ? proxyModule["settings"].extract<const DynamicObject>()
                                      : DynamicObject();
            auto moduleInstance = _libraryManager.GetModule(libraryName, moduleName);
            moduleInstance->setLandUnitController(_landUnitController);
            proxy->addModule(libraryName, moduleName, moduleInstance, moduleSettings);
         }
      }
      _moduleMap[key]->configure(module->settings());
      _moduleMap[key]->subscribe(_notificationCenter);
   }

   // Create Sequence Module to drive sims
   ModuleMapKey sequencerKey(config.localDomain()->sequencerLibrary(), config.localDomain()->sequencer());
   _sequencer = std::dynamic_pointer_cast<SequencerModuleBase>(_moduleMap[sequencerKey]);
   if (_sequencer == nullptr) {
      throw SequencerNotFoundException() << Details("Error finding sequencer")
                                         << LibraryName(config.localDomain()->sequencerLibrary())
                                         << SequencerName(config.localDomain()->sequencer());
   }
   _sequencer->configure(timing);

   // Configure Pools
   for (const auto pool : config.pools()) {
      _landUnitController.operationManager()->addPool(pool->name(), pool->description(), pool->units(), pool->scale(),
                                                      pool->order(), pool->initValue());
   }

   // New version of Variables
   // Add external, internal and flintdata

   std::map<std::pair<std::string, std::string>, TransformInterfacePtr> transforms;
   std::map<std::pair<std::string, std::string>, FlintDataInterfacePtr> flintData;

#if 0
	for (const auto var : config.variables2()) {
		if (var->variableType() == configuration::VariableType::Internal) {
			auto variable = static_cast<const configuration::Variable*>(var);
			_landUnitController.addVariable(variable->name(), std::make_shared<Variable>(variable->value(), VariableInfo{ variable->name() }));
		}
		else if (var->variableType() == configuration::VariableType::External) {
			auto variable = static_cast<const configuration::ExternalVariable*>(var);

			const auto& transformConfig = variable->transform();
			const auto libraryName = transformConfig.libraryName();
			const auto variableName = variable->name();
			auto key = std::make_pair(libraryName, variableName);
			transforms[key] = _libraryManager.GetTransform(libraryName, transformConfig.typeName());
			_landUnitController.addVariable(variable->name(), std::make_shared<ExternalVariable>(transforms[key], VariableInfo{ variable->name() }));
		}
		else if (var->variableType() == configuration::VariableType::FlintData) {
			auto variable = static_cast<const configuration::FlintDataVariable*>(var);

			const auto& flintDataConfig = variable->flintdata();
			const auto libraryName = flintDataConfig.libraryName();
			const auto variableName = variable->name();
			auto key = std::make_pair(libraryName, variableName);
			flintData[key] = _libraryManager.GetFlintData(libraryName, flintDataConfig.typeName());
			_landUnitController.addVariable(variable->name(), std::make_shared<FlintDataVariable>(flintData[key], libraryName, flintDataConfig.typeName(), VariableInfo{ variable->name() }));
		}
	}

	//// Now go back over and check for nested things
	//for (auto var : config.variables2()) {
	//	if (var->variableType() == configuration::VariableType::Internal) {
	//		auto variable = static_cast<const configuration::Variable*>(var);
	//		auto value = variable->value();
	//		variable->set_value(checkForNestedVariables(variable->name(), value));
	//	}
	//}

	// New version of Variables
	// Now Configure external, internal and flintdata 

	// config external (transforms)
	for (const auto var : config.variables2()) {
		if (var->variableType() == configuration::VariableType::External) {
			auto variable = static_cast<const configuration::ExternalVariable*>(var);

			const auto& transformConfig = variable->transform();
			const auto libraryName = transformConfig.libraryName();
			const auto variableName = variable->name();
			auto key = std::make_pair(libraryName, variableName);
			transforms[key]->configure(transformConfig.settings(), _landUnitController, _dataRepository);
		}
	}

	// config flintdata
	for (const auto var : config.variables2()) {
		if (var->variableType() == configuration::VariableType::FlintData) {
			auto variable = static_cast<const configuration::FlintDataVariable*>(var);

			const auto& flintDataConfig = variable->flintdata();
			const auto libraryName = flintDataConfig.libraryName();
			const auto variableName = variable->name();
			auto key = std::make_pair(libraryName, variableName);
			flintData[key]->configure(flintDataConfig.settings(), _landUnitController, _dataRepository);
		}
	}

#else
   // Configure Variables
   for (const auto variable : config.variables()) {
      auto value = variable->value();
      if (value.isStruct()) {
         auto& s = value.extract<const DynamicObject>();
         _landUnitController.addVariable(variable->name(),
                                         std::make_shared<Variable>(variable->value(), VariableInfo{variable->name()}));
      } else {
         _landUnitController.addVariable(variable->name(),
                                         std::make_shared<Variable>(variable->value(), VariableInfo{variable->name()}));
      }
   }

   // Configure External Variables (transforms)
   for (const auto variable : config.externalVariables()) {
      const auto& transformConfig = variable->transform();
      const auto libraryName = transformConfig.libraryName();
      const auto variableName = variable->name();
      auto key = std::make_pair(libraryName, variableName);
      transforms[key] = _libraryManager.GetTransform(libraryName, transformConfig.typeName());

      _landUnitController.addVariable(
          variable->name(), std::make_shared<ExternalVariable>(transforms[key], VariableInfo{variable->name()}));
   }

   // Configure External Pools (transforms)
   for (const auto pool : config.externalPools()) {
      const auto& transformConfig = pool->transform();
      const auto libraryName = transformConfig.libraryName();
      const auto poolName = pool->name();
      auto key = std::make_pair(libraryName, poolName);
      transforms[key] = _libraryManager.GetTransform(libraryName, transformConfig.typeName());

      _landUnitController.operationManager()->addPool(poolName, pool->description(), pool->units(), pool->scale(),
                                                      pool->order(), transforms[key]);
   }

   // Configure FlintData Variables
   for (const auto variable : config.flintDataVariables()) {
      const auto& flintDataConfig = variable->flintdata();
      const auto libraryName = flintDataConfig.libraryName();
      const auto variableName = variable->name();
      auto key = std::make_pair(libraryName, variableName);
      flintData[key] = _libraryManager.GetFlintData(libraryName, flintDataConfig.typeName());
      _landUnitController.addVariable(
          variable->name(), std::make_shared<FlintDataVariable>(flintData[key], libraryName, flintDataConfig.typeName(),
                                                                VariableInfo{variable->name()}));
   }

   // Delayed configuration call, so all flintdata are constructed
   for (const auto variable : config.flintDataVariables()) {
      const auto& flintDataConfig = variable->flintdata();
      const auto libraryName = flintDataConfig.libraryName();
      const auto variableName = variable->name();
      auto key = std::make_pair(libraryName, variableName);
      flintData[key]->configure(flintDataConfig.settings(), _landUnitController, _dataRepository);
   }

   // Delayed configuration call, so all transforms are constructed
   for (const auto variable : config.externalVariables()) {
      const auto& transformConfig = variable->transform();
      const auto libraryName = transformConfig.libraryName();
      const auto variableName = variable->name();
      auto key = std::make_pair(libraryName, variableName);
      transforms[key]->configure(transformConfig.settings(), _landUnitController, _dataRepository);
   }

   // Delayed configuration call, so all transforms are constructed
   for (const auto pool : config.externalPools()) {
      const auto& transformConfig = pool->transform();
      const auto libraryName = transformConfig.libraryName();
      const auto poolName = pool->name();
      auto key = std::make_pair(libraryName, poolName);
      transforms[key]->configure(transformConfig.settings(), _landUnitController, _dataRepository);
   }

#endif

   // Configure the simulateLandUnit & landUnitBuildSuccess variables here
   _simulateLandUnit = _landUnitController.getVariable(config.localDomain()->simulateLandUnit());
   _landUnitBuildSuccess = _landUnitController.getVariable(config.localDomain()->landUnitBuildSuccess());

   // Set initial values of Pools (init value given in pool definition, not init for a simulation) and
   // variables (some special handling for vectors & structs
   _landUnitController.initialiseData(false);  /// TODO: check if this is required here. initialiseData is also called
                                               /// in SpatialTiledLocalDomainController::runCell
}

// --------------------------------------------------------------------------------------------

void LocalDomainControllerBase::run() {
   _notificationCenter.postNotification(moja::signals::PreTimingSequence);
   _notificationCenter.postNotification(signals::LocalDomainProcessingUnitInit);
   _sequencer->Run(_notificationCenter, _landUnitController);
   _notificationCenter.postNotification(signals::LocalDomainProcessingUnitShutdown);
}

// --------------------------------------------------------------------------------------------

void LocalDomainControllerBase::startup() { _notificationCenter.postNotification(moja::signals::LocalDomainInit); }

// --------------------------------------------------------------------------------------------

void LocalDomainControllerBase::shutdown() { _notificationCenter.postNotification(moja::signals::LocalDomainShutdown); }

// --------------------------------------------------------------------------------------------

std::map<LocalDomainControllerBase::ModuleMapKey, IModule*> LocalDomainControllerBase::modules() const {
   std::map<ModuleMapKey, IModule*> results;
   for (auto module : _moduleMap) {
      results.insert(std::pair<ModuleMapKey, IModule*>(module.first, module.second.get()));
   }
   return results;
}

}  // namespace flint
}  // namespace moja
