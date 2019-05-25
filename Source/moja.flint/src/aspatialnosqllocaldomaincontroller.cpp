#include "moja/flint/aspatialnosqllocaldomaincontroller.h"

#include "moja/flint/configuration/configuration.h"
#include "moja/flint/configuration/localdomain.h"
#include "moja/flint/configuration/spinup.h"
#include "moja/flint/ivariable.h"
#include "moja/flint/sequencermodulebase.h"

#include <moja/datarepository/iprovidernosqlinterface.h>
#include <moja/datarepository/providernosqlpocomongodb.h>

#include <moja/exception.h>
#include <moja/logging.h>
#include <moja/signals.h>

#include <boost/exception/diagnostic_information.hpp>

using moja::flint::ILocalDomainController;
using moja::flint::configuration::LocalDomainType;

namespace moja {
namespace flint {

void AspatialNoSQLLocalDomainController::configure(const configuration::Configuration& config) {
   // Call base class configure
   LocalDomainControllerBase::configure(config);

   // Build landscape
   const auto& landscapeObject = config.localDomain()->landscapeObject();
   auto iterator = landscapeObject->iterationASpatialIndex();
   _provider = std::static_pointer_cast<moja::datarepository::IProviderNoSQLInterface>(
       _dataRepository.getProvider(landscapeObject->providerName()));
   _count = _provider->Count();

   auto mongoProvider = std::static_pointer_cast<moja::datarepository::ProviderNoSQLPocoMongoDB>(_provider);

   std::string queryStr = "{}";
   std::string fieldsStr = "{\"_id\": 1 }";
   _idSet = mongoProvider->SendQueryRequest(queryStr, fieldsStr);

   //_tiles = std::make_unique<moja::datarepository::AspatialTileInfoCollection>(*provider.get(),
   //iterator->maxTileSize(), iterator->tileCacheSize());
}

void AspatialNoSQLLocalDomainController::run() {
   auto startTime = DateTime::now();

   startup();

   _notificationCenter.postNotification(moja::signals::LocalDomainInit);

   MOJA_LOG_DEBUG << "LandUnit Count: " << _count;

   auto current = 0;
   for (auto lu : _idSet) {
      auto obj = lu.extract<const DynamicObject>();
      auto id = obj["_id"].extract<const std::string>();

      current++;
      MOJA_LOG_DEBUG << "LandUnit Current #: " << current;

      try {
         MOJA_LOG_INFO << std::setfill(' ') << std::setw(10) << current << " of " << std::setfill(' ') << std::setw(10)
                       << _count;
         MOJA_LOG_DEBUG << "LandUnit Id: " << id;

         // auto idVariable = _landUnitController.getVariable("LandUnitId");
         // idVariable->set_value(id);

         _landUnitController.initialiseData();
         if (!_simulateLandUnit->value()) continue;
         _notificationCenter.postNotification(moja::signals::PreTimingSequence);
         if (!_landUnitBuildSuccess->value()) continue;
         _sequencer->Run(_notificationCenter, _landUnitController);
      } catch (const Exception& e) {
         MOJA_LOG_FATAL << "[" << current << "]";
         MOJA_LOG_FATAL << e.displayText();
      } catch (const boost::exception& e) {
         MOJA_LOG_FATAL << "[" << current << "]";
         MOJA_LOG_FATAL << boost::diagnostic_information(e);
      } catch (const std::exception& e) {
         MOJA_LOG_FATAL << "[" << current << "]";
         MOJA_LOG_FATAL << e.what();
      }
      _landUnitController.clearAllOperationResults();
   }

   shutdown();

   auto endTime = DateTime::now();
   auto ldSpan = endTime - startTime;
   MOJA_LOG_INFO << "LocalDomain: Start Time           : " << startTime;
   MOJA_LOG_INFO << "LocalDomain: Finish Time          : " << endTime;
   MOJA_LOG_INFO << "LocalDomain: Total Time (seconds) : " << ldSpan.totalSeconds();
}

}  // namespace flint
}  // namespace moja
