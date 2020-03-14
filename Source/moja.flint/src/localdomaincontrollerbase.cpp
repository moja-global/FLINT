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

   // Add external, internal and flintdata variables

   // Configure Variables
   for (const auto variable : config.variables()) {
      _landUnitController.addVariable(variable->name(),
                                      std::make_shared<Variable>(variable->value(), VariableInfo{variable->name()}));
   }

   std::vector<std::pair<const configuration::Transform*, TransformInterfacePtr>> config_transform_pairs;

   // Construct External Variables (transforms)
   for (const auto variable : config.externalVariables()) {
      const auto& transform_config = variable->transform();
      auto transform = _libraryManager.GetTransform(transform_config.libraryName(), transform_config.typeName());
      _landUnitController.addVariable(
          variable->name(), std::make_shared<ExternalVariable>(transform, VariableInfo{variable->name()}));
      config_transform_pairs.emplace_back(std::make_pair(&transform_config, transform));
   }

   // Construct External Pools (transforms)
   for (const auto pool : config.externalPools()) {
      const auto& transform_config = pool->transform();
      auto transform = _libraryManager.GetTransform(transform_config.libraryName(), transform_config.typeName());

      _landUnitController.operationManager()->addPool(pool->name(), pool->description(), pool->units(), pool->scale(),
                                                      pool->order(), transform);
      config_transform_pairs.emplace_back(std::make_pair(&transform_config, transform));
   }

   std::vector<std::pair<const configuration::FlintData*, FlintDataInterfacePtr>> config_flintdata_pairs;

   // Construct FlintData Variables
   for (const auto variable : config.flintDataVariables()) {
      const auto& flint_data_config = variable->flintdata();
      auto flint_data = _libraryManager.GetFlintData(flint_data_config.libraryName(), flint_data_config.typeName());
      _landUnitController.addVariable(
          variable->name(),
          std::make_shared<FlintDataVariable>(flint_data, flint_data_config.libraryName(), flint_data_config.typeName(),
                                              VariableInfo{variable->name()}));
      config_flintdata_pairs.emplace_back(std::make_pair(&flint_data_config, flint_data));
   }

      // Configure uncertainty
   _landUnitController.configure_uncertainty(config.uncertainty());

   // Delayed configuration call, so all transforms are constructed
   for (auto& config_transform : config_transform_pairs) {
      config_transform.second->configure(config_transform.first->settings(), _landUnitController, _dataRepository);
   }

   // Delayed configuration call, so all FlintData are constructed
   for (auto& config_flintdata : config_flintdata_pairs) {
      config_flintdata.second->configure(config_flintdata.first->settings(), _landUnitController, _dataRepository);
   }

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
