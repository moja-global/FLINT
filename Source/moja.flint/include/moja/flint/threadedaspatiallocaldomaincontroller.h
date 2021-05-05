#ifndef MOJA_FLINT_THREADEDASPATIALLOCALDOMAINCONTROLLER_H_
#define MOJA_FLINT_THREADEDASPATIALLOCALDOMAINCONTROLLER_H_

#include "moja/flint/aspatiallocaldomaincontroller.h"
#include "moja/flint/localdomaincontrollerbase.h"
#include "moja/flint/spinuplandunitcontroller.h"

#include <moja/datarepository/aspatialtileinfo.h>
#include <moja/datarepository/aspatialtileinfocollection.h>

#include <moja/flint/configuration/configuration.h>

#include <queue>
#include <thread>

namespace moja {
namespace flint {

class ThreadedAspatialLocalDomainController;

class AspatialLocalDomainThread {
  public:
   AspatialLocalDomainThread(ThreadedAspatialLocalDomainController* parent, int threadId, std::mutex& tileListMutex,
                             std::queue<datarepository::AspatialTileInfo>& tileList,
                             const configuration::Configuration* config);

   void operator()();

   std::shared_ptr<LocalDomainControllerBase> localDomainController() const { return _ldc; }
   int threadId() const { return _threadId; }

  private:
   ThreadedAspatialLocalDomainController* _parent;
   configuration::Configuration _threadConfig;
   int _threadId;
   std::mutex& _tileListMutex;
   std::queue<datarepository::AspatialTileInfo>& _tileList;
   const configuration::Configuration* _config;
   std::shared_ptr<AspatialLocalDomainController> _ldc;
};

class FLINT_API ThreadedAspatialLocalDomainController final : public flint::LocalDomainControllerBase {
  public:
   ThreadedAspatialLocalDomainController(void);
   ~ThreadedAspatialLocalDomainController() = default;

   virtual void configure(const flint::configuration::Configuration& config) override;
   virtual void run() override;

   virtual void startup() override {}
   virtual void shutdown() override {}

   std::vector<std::shared_ptr<AspatialLocalDomainThread>> tasks() const { return _tasks; }

  private:
   int _numThreads;

   std::vector<std::shared_ptr<AspatialLocalDomainThread>> _tasks;
   std::vector<std::thread> _threads;

   std::unique_ptr<datarepository::AspatialTileInfoCollection> _landscape;
   std::mutex _tileListMutex;
   std::queue<datarepository::AspatialTileInfo> _tileList;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_THREADEDASPATIALLOCALDOMAINCONTROLLER_H_
