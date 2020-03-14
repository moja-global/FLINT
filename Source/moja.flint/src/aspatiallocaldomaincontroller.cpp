#include "moja/flint/aspatiallocaldomaincontroller.h"

#include "moja/flint/externalvariable.h"
#include "moja/flint/imodule.h"
#include "moja/flint/sequencermodulebase.h"
#include "moja/flint/variable.h"

#include <moja/datarepository/aspatialtileinfocollection.h>
#include <moja/datarepository/iproviderrelationalinterface.h>
#include <moja/datarepository/landunitinfo.h>

#include <moja/flint/configuration/configuration.h>
#include <moja/flint/configuration/externalvariable.h>
#include <moja/flint/configuration/localdomain.h>
#include <moja/flint/configuration/spinup.h>
#include <moja/flint/configuration/variable.h>

#include <moja/exception.h>
#include <moja/logging.h>
#include <moja/signals.h>

#include <boost/exception/diagnostic_information.hpp>

using moja::datarepository::AspatialTileInfo;
using moja::datarepository::LandUnitInfo;
using moja::flint::ILocalDomainController;
using moja::flint::configuration::LocalDomainType;

namespace moja {
namespace flint {

void AspatialLocalDomainController::configure(const configuration::Configuration& config) {
   LocalDomainControllerBase::configure(config);

   // Build landscape.
   const auto landscapeObject = config.localDomain()->landscapeObject();
   auto iterator = landscapeObject->iterationASpatialIndex();
   auto provider = std::static_pointer_cast<moja::datarepository::IProviderRelationalInterface>(
       _dataRepository.getProvider(landscapeObject->providerName()));

   _landscape = std::make_unique<moja::datarepository::AspatialTileInfoCollection>(
       *provider.get(), iterator->maxTileSize(), iterator->tileCacheSize());

   // Build spinup config.
   const auto spinup = config.spinup();
   _runSpinUp = false;
   if (spinup->enabled()) {
      _runSpinUp = true;

      // Set start and end dates for the spinup.
      auto& timing = _spinupLandUnitController.timing();
      auto now = DateTime::now();
      timing.setStartDate(now);
      timing.setEndDate(now.addYears(2));
      timing.init();

      // Load the configured spinup modules.
      _spinupModules.clear();
      const auto& modules = config.modules();
      for (const auto& spinupModuleConfig : spinup->modules()) {
         ModuleMapKey key(spinupModuleConfig->libraryName(), spinupModuleConfig->name());
         _spinupModules[key] = _libraryManager.GetModule(spinupModuleConfig->libraryName(), spinupModuleConfig->name());

         _spinupModules[key]->setLandUnitController(_spinupLandUnitController);
         _spinupModules[key]->configure(spinupModuleConfig->settings());
         _spinupModules[key]->subscribe(_spinupNotificationCenter);
      }

      // Get Sequence Module to drive spinups
      ModuleMapKey sequencerKey(spinup->sequencerLibrary(), spinup->sequencerName());
      _spinupSequencer = std::dynamic_pointer_cast<SequencerModuleBase>(_spinupModules[sequencerKey]);
      _spinupSequencer->configure(timing);

      // Configure Variables:
      // First copy in from main LUC if name not found in spinup config list of variables.
      for (const auto& variable : _landUnitController.variables()) {
         bool found = false;
         for (auto var : spinup->variables()) {
            if (var->name() == variable->info().name) {
               found = true;
               break;
            }
         }

         if (!found) {
            for (auto var : spinup->externalVariables()) {
               if (var->name() == variable->info().name) {
                  found = true;
                  break;
               }
            }
         }

         if (!found) {
            if (variable->isExternal()) {
               auto ptr = std::static_pointer_cast<ExternalVariable>(variable);
               ptr->controllerChanged(_landUnitController);
            }
            _spinupLandUnitController.addVariable(variable->info().name, variable);
         }
      }

      for (const auto& variable : spinup->variables()) {
         _spinupLandUnitController.addVariable(
             variable->name(), std::make_shared<Variable>(variable->value(), VariableInfo{variable->name()}));
      }

      // Configure External Variables (transforms)
      std::map<std::string, TransformInterfacePtr> spinuptransforms;
      for (const auto& variable : spinup->externalVariables()) {
         const auto& transformConfig = variable->transform();
         const auto& variableName = variable->name();
         spinuptransforms[variableName] =
             _libraryManager.GetTransform(transformConfig.libraryName(), transformConfig.typeName());

         _spinupLandUnitController.addVariable(
             variable->name(), std::make_shared<moja::flint::ExternalVariable>(spinuptransforms[variableName],
                                                                               VariableInfo{variable->name()}));
      }

      // Delayed configuration call, so all transforms are constructed
      for (const auto& variable : spinup->externalVariables()) {
         const auto& transformConfig = variable->transform();
         const auto& variableName = variable->name();
         spinuptransforms[variableName]->configure(transformConfig.settings(), _spinupLandUnitController,
                                                   _dataRepository);
      }

      // Set initial values of Pools (init value given in pool definition, not init for a simulation) and
      // variables (some special handling for vectors & structs
      _spinupLandUnitController.initialiseData(false);
      _luId = _landUnitController.getVariable("LandUnitId");
   }
}

void AspatialLocalDomainController::run(const LandUnitInfo& lu) {
   try {
      _luId->set_value(lu.id());
      _landUnitController.initialiseData(true);
      if (_runSpinUp) {
         // (shared) pools are done by line above so only do variables for spinup.
         _spinupNotificationCenter.postNotification(moja::signals::PreTimingSequence);
         _spinupSequencer->Run(_spinupNotificationCenter, _landUnitController);
         _spinupLandUnitController.clearAllOperationResults();
      }

      if (!_simulateLandUnit->value()) {
         return;
      }

      _notificationCenter.postNotification(moja::signals::PreTimingSequence);
      if (!_landUnitBuildSuccess->value()) {
         return;
      }

      _sequencer->Run(_notificationCenter, _landUnitController);
   } catch (const Exception& e) {
      MOJA_LOG_FATAL << e.displayText();
   } catch (const boost::exception& e) {
      MOJA_LOG_FATAL << boost::diagnostic_information(e);
   } catch (const std::exception& e) {
      MOJA_LOG_FATAL << e.what();
   }

   _landUnitController.clearAllOperationResults();
}

void AspatialLocalDomainController::run(AspatialTileInfo& tile) {
   auto startTime = DateTime::now();

   for (auto lu : tile) {
      run(lu);
   }

   auto endTime = DateTime::now();
   auto ldSpan = endTime - startTime;
   MOJA_LOG_INFO << "LocalDomain: Start Time           : " << startTime;
   MOJA_LOG_INFO << "LocalDomain: Finish Time          : " << endTime;
   MOJA_LOG_INFO << "LocalDomain: Total Time (seconds) : " << ldSpan.totalSeconds();
}

void AspatialLocalDomainController::startup() {
   LocalDomainControllerBase::startup();
   if (_runSpinUp) {
      _spinupNotificationCenter.postNotification(moja::signals::LocalDomainInit);
   }
}

void AspatialLocalDomainController::shutdown() {
   LocalDomainControllerBase::shutdown();
   if (_runSpinUp) {
      _spinupNotificationCenter.postNotification(moja::signals::LocalDomainShutdown);
   }
}

void AspatialLocalDomainController::run() {
   startup();
   auto startTime = DateTime::now();

   auto total = _landscape->getTotalLUCount();
   auto count = 0;
   for (auto lu : *_landscape) {
      count++;
      MOJA_LOG_INFO << std::setfill(' ') << std::setw(10) << count << " of " << std::setfill(' ') << std::setw(10)
                    << total;
      run(lu);
   }
   shutdown();

   //_notificationCenter.postNotification(moja::signals::SystemShutdown);

   auto endTime = DateTime::now();
   auto ldSpan = endTime - startTime;
   MOJA_LOG_INFO << "LocalDomain: Start Time           : " << startTime;
   MOJA_LOG_INFO << "LocalDomain: Finish Time          : " << endTime;
   MOJA_LOG_INFO << "LocalDomain: Total Time (seconds) : " << ldSpan.totalSeconds();
}

}  // namespace flint
}  // namespace moja
