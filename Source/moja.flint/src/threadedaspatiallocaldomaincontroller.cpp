#include "moja/flint/threadedaspatiallocaldomaincontroller.h"

#include <moja/datarepository/aspatialtileinfocollection.h>
#include <moja/datarepository/iproviderrelationalinterface.h>

#include <moja/flint/configuration/externalvariable.h>
#include <moja/flint/configuration/json2configurationprovider.h>
#include <moja/flint/configuration/localdomain.h>
#include <moja/flint/configuration/spinup.h>

#include <moja/datetime.h>
#include <moja/logging.h>

#include <boost/exception/diagnostic_information.hpp>

using moja::flint::ILocalDomainController;
using moja::flint::configuration::LocalDomainType;

namespace conf = moja::flint::configuration;

namespace moja {
namespace flint {

AspatialLocalDomainThread::AspatialLocalDomainThread(ThreadedAspatialLocalDomainController* parent, int threadId,
                                                     std::mutex& tileListMutex,
                                                     std::queue<datarepository::AspatialTileInfo>& tileList,
                                                     const configuration::Configuration* config)
    : _parent(parent),
      _threadConfig(config->startDate(), config->endDate()),
      _threadId(threadId),
      _tileListMutex(tileListMutex),
      _tileList(tileList),
      _config(config) {
   _ldc = std::make_shared<AspatialLocalDomainController>();
   _ldc->set_localDomainId(_threadId);
   _threadConfig.Copy(*_config);
   _ldc->configure(_threadConfig);

   MOJA_LOG_INFO << "Thread Task Created (id = " << std::setfill(' ') << std::setw(3) << _threadId << ")";
}

void AspatialLocalDomainThread::operator()() {
   MOJA_LOG_INFO << std::setfill(' ') << std::setw(3) << _threadId << ": "
                 << "Thread Started ";

   _ldc->startup();
   auto keepRunning = true;
   while (keepRunning) {
      // Pop a block index from the queue
      std::unique_lock<std::mutex> lock(_tileListMutex);
      if (_tileList.size() > 0) {
         auto tile = _tileList.front();
         _tileList.pop();
         MOJA_LOG_INFO << std::setfill(' ') << std::setw(3) << _threadId
                       << ": Current tile queue length is: " << _tileList.size();
         lock.unlock();
         _ldc->run(tile);
      } else {
         lock.unlock();
         keepRunning = false;
      }
   }

   _ldc->shutdown();
   MOJA_LOG_INFO << std::setfill(' ') << std::setw(3) << _threadId << ": "
                 << "Thread Finished ";
}

ThreadedAspatialLocalDomainController::ThreadedAspatialLocalDomainController() : LocalDomainControllerBase() {}

void ThreadedAspatialLocalDomainController::configure(const configuration::Configuration& config) {
   // Call base class configure
   LocalDomainControllerBase::configure(config);

   // Build landscape.
   const auto& landscapeObject = config.localDomain()->landscapeObject();
   auto iterator = landscapeObject->iterationASpatialIndex();
   auto provider = std::static_pointer_cast<moja::datarepository::IProviderRelationalInterface>(
       _dataRepository.getProvider(landscapeObject->providerName()));

   _landscape = std::make_unique<moja::datarepository::AspatialTileInfoCollection>(
       *provider.get(), iterator->maxTileSize(), iterator->tileCacheSize());

   for (auto tile : _landscape->getTiles()) {
      _tileList.push(tile);
   }

   _numThreads = config.localDomain()->numThreads();
   // Create tasks to be put into threads later
   for (auto t = 0; t < _numThreads; t++) {
      auto task = std::make_shared<AspatialLocalDomainThread>(this, t + 100, _tileListMutex, _tileList, _config);

      _tasks.push_back(task);
   }
}

void ThreadedAspatialLocalDomainController::run() {
   auto startTime = DateTime::now();

   for (const auto& task : _tasks) {
      _threads.push_back(std::thread{*(task.get())});
   }

   for (auto& thread : _threads) {
      thread.join();
   }

   //_notificationCenter.postNotification(moja::signals::SystemShutdown);

   auto endTime = DateTime::now();
   auto ldSpan = endTime - startTime;
   MOJA_LOG_INFO << "LocalDomain: Start Time           : " << startTime;
   MOJA_LOG_INFO << "LocalDomain: Finish Time          : " << endTime;
   MOJA_LOG_INFO << "LocalDomain: Total Time (seconds) : " << ldSpan.totalSeconds();
}

}  // namespace flint
}  // namespace moja
