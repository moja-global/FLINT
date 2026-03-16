#include "moja/flint/spatialtiledlocaldomaincontroller.h"

#include "moja/flint/externalvariable.h"
#include "moja/flint/flintdatavariable.h"
#include "moja/flint/flintexceptions.h"
#include "moja/flint/ilocaldomaincontroller.h"
#include "moja/flint/ivariable.h"
#include "moja/flint/sequencermodulebase.h"
#include "moja/flint/variable.h"

#include <moja/datarepository/datarepository.h>
#include <moja/datarepository/iproviderspatialrasterinterface.h>

#include <moja/flint/configuration/configuration.h>
#include <moja/flint/configuration/externalvariable.h>
#include <moja/flint/configuration/flintdatavariable.h>
#include <moja/flint/configuration/localdomain.h>
#include <moja/flint/configuration/pool.h>
#include <moja/flint/configuration/spinup.h>
#include <moja/flint/configuration/variable.h>

#include <moja/datetime.h>
#include <moja/exception.h>
#include <moja/logging.h>
#include <moja/signals.h>

#include <Poco/ScopedLock.h>
#include <Poco/UUID.h>
#include <Poco/UUIDGenerator.h>

#include <boost/format.hpp>

#include <chrono>
#include <memory>
#include <queue>
#include <thread>

namespace conf = moja::flint::configuration;
namespace data = moja::datarepository;

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------

StatsUnitRecord::StatsUnitRecord(std::shared_ptr<StatsUnitRecord>& ptr, StatsDurationType durationType)
    : _runId(ptr->_runId),
      _elapsedTimeTotal(ptr->_elapsedTimeTotal),
      _elapsedTimeFramework(ptr->_elapsedTimeFramework),
      _elapsedTimeSpinup(ptr->_elapsedTimeSpinup),
      _elapsedTimeProcessed(ptr->_elapsedTimeProcessed),
      _unitsTotal(ptr->_unitsTotal),
      _unitsProcessed(ptr->_unitsProcessed),
      _unitsNotProcessed(ptr->_unitsNotProcessed),
      _unitsWithError(ptr->_unitsWithError),
      _tileIdx(ptr->_tileIdx),
      _blockIdx(ptr->_blockIdx),
      _durationType(durationType) {}

// -- StatsUnitRecord
StatsUnitRecord::StatsUnitRecord(std::string runId, duration elapsedTimeTotal, duration elapsedTimeFramework,
                                 duration elapsedTimeSpinup, duration elapsedTimeProcessed, Int64 unitsTotal,
                                 Int64 unitsProcessed, Int64 unitsNotProcessed, Int64 unitsWithError,
                                 Poco::Nullable<UInt32> tileIdx, Poco::Nullable<UInt32> blockIdx,
                                 StatsDurationType durationType)
    : _runId(runId),
      _elapsedTimeTotal(elapsedTimeTotal),
      _elapsedTimeFramework(elapsedTimeFramework),
      _elapsedTimeSpinup(elapsedTimeSpinup),
      _elapsedTimeProcessed(elapsedTimeProcessed),
      _unitsTotal(unitsTotal),
      _unitsProcessed(unitsProcessed),
      _unitsNotProcessed(unitsNotProcessed),
      _unitsWithError(unitsWithError),
      _tileIdx(tileIdx),
      _blockIdx(blockIdx),
      _durationType(durationType) {}

bool StatsUnitRecord::operator==(const Record<StatsUnitRow>& other) const {
   auto otherRow = other.asPersistable();
   return _tileIdx == otherRow.get<10>() && _blockIdx == otherRow.get<11>();
}

size_t StatsUnitRecord::hash() const { return moja::hash::hash_combine(_tileIdx, _blockIdx); }

StatsUnitRow StatsUnitRecord::asPersistable() const {
   using namespace std::chrono;

   Int64 elapsedTimeTotal = 0;
   Int64 elapsedTimeFramework = 0;
   Int64 elapsedTimeSpinup = 0;
   Int64 elapsedTimeProcessed = 0;

   switch (_durationType) {
      case StatsDurationType::Minutes: {
         elapsedTimeTotal = duration_cast<minutes>(_elapsedTimeTotal).count();
         elapsedTimeFramework = duration_cast<minutes>(_elapsedTimeFramework).count();
         elapsedTimeSpinup = duration_cast<minutes>(_elapsedTimeSpinup).count();
         elapsedTimeProcessed = duration_cast<minutes>(_elapsedTimeProcessed).count();
      } break;
      case StatsDurationType::Seconds: {
         elapsedTimeTotal = duration_cast<seconds>(_elapsedTimeTotal).count();
         elapsedTimeFramework = duration_cast<seconds>(_elapsedTimeFramework).count();
         elapsedTimeSpinup = duration_cast<seconds>(_elapsedTimeSpinup).count();
         elapsedTimeProcessed = duration_cast<seconds>(_elapsedTimeProcessed).count();
      } break;
      case StatsDurationType::Milliseconds: {
         elapsedTimeTotal = duration_cast<milliseconds>(_elapsedTimeTotal).count();
         elapsedTimeFramework = duration_cast<milliseconds>(_elapsedTimeFramework).count();
         elapsedTimeSpinup = duration_cast<milliseconds>(_elapsedTimeSpinup).count();
         elapsedTimeProcessed = duration_cast<milliseconds>(_elapsedTimeProcessed).count();
      } break;
      case StatsDurationType::Nanoseconds: {
         elapsedTimeTotal = duration_cast<nanoseconds>(_elapsedTimeTotal).count();
         elapsedTimeFramework = duration_cast<nanoseconds>(_elapsedTimeFramework).count();
         elapsedTimeSpinup = duration_cast<nanoseconds>(_elapsedTimeSpinup).count();
         elapsedTimeProcessed = duration_cast<nanoseconds>(_elapsedTimeProcessed).count();
      } break;
      default:
         break;
   }
   return StatsUnitRow{
       _id,         _runId,          elapsedTimeTotal,   elapsedTimeFramework, elapsedTimeSpinup, elapsedTimeProcessed,
       _unitsTotal, _unitsProcessed, _unitsNotProcessed, _unitsWithError,      _tileIdx,          _blockIdx};
}

void StatsUnitRecord::merge(Record<StatsUnitRow>* other) {
   auto otherRow = static_cast<StatsUnitRecord*>(other);
   _elapsedTimeTotal += otherRow->_elapsedTimeTotal;
   _elapsedTimeFramework += otherRow->_elapsedTimeFramework;
   _elapsedTimeSpinup += otherRow->_elapsedTimeSpinup;
   _elapsedTimeProcessed += otherRow->_elapsedTimeProcessed;
   _unitsTotal += otherRow->_unitsTotal;
   _unitsProcessed += otherRow->_unitsProcessed;
   _unitsNotProcessed += otherRow->_unitsNotProcessed;
   _unitsWithError += otherRow->_unitsWithError;
}

void StatsUnitRecord::mojaLog(const std::string& levelStr, int localDomainId, datarepository::TileIdx* tileIdxObject,
                              datarepository::BlockIdx* blockIdxObject, bool inSeconds = false,
                              bool showRecordNumber = false, int recordPosition = 0, int recordCount = 0) {
   using namespace std::chrono;
   auto elapsed = duration_cast<std::chrono::duration<double, seconds::period>>(_elapsedTimeTotal).count();
   int luPerSecond = _elapsedTimeTotal == 0s ? 0 : int(_unitsTotal / elapsed);
   auto timeTotal =
       inSeconds ? duration_cast<seconds>(_elapsedTimeTotal) : duration_cast<milliseconds>(_elapsedTimeTotal);
   auto timeProcessed =
       inSeconds ? duration_cast<seconds>(_elapsedTimeProcessed) : duration_cast<milliseconds>(_elapsedTimeProcessed);

   std::string recordInfoStr = "";
   if (showRecordNumber) {
      recordInfoStr = (boost::format("Record [%1% of %2%]: ") % boost::io::group(std::setw(5), recordPosition) %
                       boost::io::group(std::setw(5), recordCount))
                          .str();
   }
   if (tileIdxObject != nullptr) {
      recordInfoStr +=
          (boost::format("Index [%1%]: ") % boost::io::group(std::setw(5), tileIdxObject->toString())).str();
   }
   if (blockIdxObject != nullptr) {
      recordInfoStr +=
          (boost::format("Index [%1%]: ") % boost::io::group(std::setw(5), blockIdxObject->toString())).str();
   }
   MOJA_LOG_INFO << std::setfill(' ') << std::setw(3) << localDomainId << ": Stats level [" << levelStr
                 << "]: "  // "Global: "
                 << recordInfoStr << "Time" << (inSeconds ? "(sec)" : "(ms)") << " ["
                 << "Total: " << std::setfill(' ') << std::setw(8) << timeTotal.count() << ", "
                 << "Processed: " << std::setfill(' ') << std::setw(8) << timeProcessed.count() << "]: "
                 << "Units ["
                 << "Total: " << std::setfill(' ') << std::setw(8) << _unitsTotal << ", "
                 << "Processed: " << std::setfill(' ') << std::setw(8) << _unitsProcessed << ", "
                 << "NotProcessed: " << std::setfill(' ') << std::setw(8) << _unitsNotProcessed << ", "
                 << "Errors: " << std::setfill(' ') << std::setw(8) << _unitsWithError << ", "
                 << "LU/second: " << std::setfill(' ') << std::setw(8) << luPerSecond << "]";
}

#define RETRY_ATTEMPTS 10000
#define RETRY_SLEEP std::chrono::milliseconds(200)

// void StatsUnitRecord::sqliteCreateTable(bool dropExisting, int localDomainId, std::string databaseName, std::string
// tableName) { 	auto retry = false; 	auto maxRetries = RETRY_ATTEMPTS; 	do { 		try { 			retry = false;
//			SQLite::Connector::registerConnector();
//			Session session("SQLite", databaseName);
//
//			if (dropExisting)
//				session << "DROP TABLE IF EXISTS " << tableName, now;
//			session << "CREATE TABLE IF NOT EXISTS " << tableName << " (id UNSIGNED BIG INT NOT NULL, runId
//VARCHAR(255), elapsedTimeTotal UNSIGNED BIG INT NOT NULL, elapsedTimeFramework UNSIGNED BIG INT NOT NULL,
//elapsedTimeSpinup UNSIGNED BIG INT NOT NULL, elapsedTimeProcessed UNSIGNED BIG INT NOT NULL, unitsTotal UNSIGNED BIG
//INT NOT NULL, unitsProcessed UNSIGNED BIG INT NOT NULL, unitsNotProcessed UNSIGNED BIG INT NOT NULL, unitsWithErrors
//UNSIGNED BIG INT NOT NULL, tileIdx INTEGER, blockIdx INTEGER)", now;
//
//			SQLite::Connector::unregisterConnector();
//		}
//		catch (SQLite::DBLockedException&) {
//			MOJA_LOG_DEBUG << localDomainId << ":DBLockedException - " << maxRetries << " retries
//remaining"; 			std::this_thread::sleep_for(RETRY_SLEEP); 			retry = maxRetries-- > 0; 			if (!retry) { 				MOJA_LOG_DEBUG <<
//localDomainId << ":Exceeded MAX RETIRES (" << RETRY_ATTEMPTS << ")"; 				throw;
//			}
//		}
//	} while (retry);
//}
//
// void StatsUnitRecord::sqliteWrite(std::shared_ptr<RecordAccumulatorWithMutex<StatsUnitRow>>& collection, int
// localDomainId, std::string databaseName, std::string tableName) { 	auto retry = false; 	auto maxRetries =
//RETRY_ATTEMPTS; 	do { 		try { 			retry = false; 			SQLite::Connector::registerConnector(); 			Session session("SQLite",
//databaseName);
//
//			// -- collection
//			if (collection->size() != 0) {
//				MOJA_LOG_DEBUG << localDomainId << ":SQLite " << tableName << " - inserted " <<
//collection->size() << " records"; 				session.begin(); 				session << "INSERT INTO " << tableName << " VALUES(?, ?, ?, ?, ?,
//?, ?, ?, ?, ?, ?, ?)", bind(collection->getPersistableCollection()), now; 				session.commit();
//			}
//
//			SQLite::Connector::unregisterConnector();
//		}
//		catch (SQLite::DBLockedException&) {
//			MOJA_LOG_DEBUG << localDomainId << ":DBLockedException - " << maxRetries << " retries
//remaining"; 			std::this_thread::sleep_for(RETRY_SLEEP); 			retry = maxRetries-- > 0; 			if (!retry) { 				MOJA_LOG_DEBUG <<
//localDomainId << ":Exceeded MAX RETIRES (" << RETRY_ATTEMPTS << ")"; 				throw;
//			}
//		}
//	} while (retry);
//}

// --

// --------------------------------------------------------------------------------------------

class SpatialTiledLocalDomainController::InternalThreadBlocks {
  public:
   InternalThreadBlocks(int threadId, std::string runId, Poco::Mutex& blockIdxListMutex,
                        std::queue<datarepository::BlockIdx>& blockIdxList, int blockIdxListSize,
                        std::map<std::pair<UInt32, UInt32>, std::vector<datarepository::CellIdx>>& blockCellIdxList,
                        const configuration::Configuration* config,
                        std::shared_ptr<RecordAccumulatorWithMutex<StatsUnitRow>>& globalStatsDimension,
                        std::shared_ptr<RecordAccumulatorWithMutex<StatsUnitRow>>& tileStatsDimension,
                        std::shared_ptr<RecordAccumulatorWithMutex<StatsUnitRow>>& blockStatsDimension,
                        std::shared_ptr<FlintLibraryHandles> internalLIbraryHandles);

   void operator()();

   std::shared_ptr<LocalDomainControllerBase> localDomainController() const { return _ldc; }
   int threadId() const { return _threadId; }
   std::string runId() const { return _runId; }

  private:
   int _threadId;
   std::string _runId;
   Poco::Mutex& _blockIdxListMutex;
   std::queue<datarepository::BlockIdx>& _blockIdxList;
   std::map<std::pair<UInt32, UInt32>, std::vector<datarepository::CellIdx>>& _blockCellIdxList;
   const configuration::Configuration* _config;
   std::shared_ptr<SpatialTiledLocalDomainController> _ldc;
   configuration::Configuration _threadConfig;
};

// --------------------------------------------------------------------------------------------

SpatialTiledLocalDomainController::InternalThreadBlocks::InternalThreadBlocks(
    int threadId, std::string runId, Poco::Mutex& blockIdxListMutex, std::queue<datarepository::BlockIdx>& blockIdxList,
    int blockIdxListSize, std::map<std::pair<UInt32, UInt32>, std::vector<datarepository::CellIdx>>& blockCellIdxList,
    const configuration::Configuration* config,
    std::shared_ptr<RecordAccumulatorWithMutex<StatsUnitRow>>& globalStatsDimension,
    std::shared_ptr<RecordAccumulatorWithMutex<StatsUnitRow>>& tileStatsDimension,
    std::shared_ptr<RecordAccumulatorWithMutex<StatsUnitRow>>& blockStatsDimension,
    std::shared_ptr<FlintLibraryHandles> internalLIbraryHandles)
    : _threadId(threadId),
      _runId(runId),
      _blockIdxListMutex(blockIdxListMutex),
      _blockIdxList(blockIdxList),
      _blockCellIdxList(blockCellIdxList),
      _config(config),
      _threadConfig(config->startDate(), config->endDate()) {
   _ldc = std::make_shared<SpatialTiledLocalDomainController>(globalStatsDimension, tileStatsDimension,
                                                              blockStatsDimension, blockIdxListSize,
                                                              internalLIbraryHandles, true, false, true);

   _ldc->_runId = runId;
   _ldc->set_localDomainId(_threadId);
   _threadConfig.Copy(*_config);  // make a copy so we can edit it
   _threadConfig.localDomain()->set_doLogging(false);
   _threadConfig.localDomain()->set_iterationType(configuration::LocalDomainIterationType::BlockIndex);
   _threadConfig.localDomain()->setLandscapeObject(_config->localDomain()->landscapeObject()->providerName(),
                                                   conf::LocalDomainIterationType::BlockIndex);

   auto ldInfoSettings = DynamicObject({{"controllerName", "SpatialTiledLocalDomainController"},
                                        {"controllerVersion", "1.0"},
                                        {"numberOfThreads", _threadConfig.localDomain()->numThreads()},
                                        {"isThreadSystem", true},
                                        {"isMaster", false},
                                        {"isThread", true}});
   _threadConfig.addFlintDataVariable("flint_system_localdomaininfo", "internal.flint", "LocalDomainInformation",
                                      ldInfoSettings);
   _ldc->configure(_threadConfig);
   auto& _luc = _ldc->landUnitController();
   auto localDomainId = _luc.getVariable("localDomainId");
   localDomainId->set_value(threadId);
   _ldc->initialiseData();
   MOJA_LOG_INFO << std::setfill(' ') << std::setw(3) << _threadId << ":"
                 << "Thread Created";
}

// --------------------------------------------------------------------------------------------

void SpatialTiledLocalDomainController::InternalThreadBlocks::operator()() {
   MOJA_LOG_INFO << std::setfill(' ') << std::setw(3) << _threadId << ":"
                 << "Thread Started ";

   _ldc->startup();
   auto keepRunning = true;
   while (keepRunning) {
      // Pop a block index from the queue.
      // ToDo: could make the number of blocks to pop here configurable
      Poco::ScopedLockWithUnlock<Poco::Mutex> lock(_blockIdxListMutex);
      if (_blockIdxList.size() > 0) {
         _ldc->_blockIdxListPosition = int(_blockIdxList.size());
         auto blockIdx = _blockIdxList.front();
         _blockIdxList.pop();
         lock.unlock();
         _ldc->_blockIdxList = std::queue<datarepository::BlockIdx>();
         if (!_blockCellIdxList.empty()) {
            std::pair<UInt32, UInt32> key(blockIdx.tileIdx, blockIdx.blockIdx);
            auto& vec = _blockCellIdxList[key];
            _ldc->_blockCellIdxList.insert(std::make_pair(key, vec));
         }
         _ldc->_blockIdxList.push(blockIdx);
         _ldc->run();
      } else {
         lock.unlock();
         keepRunning = false;
      }
   }
   _ldc->shutdown();
   MOJA_LOG_INFO << std::setfill(' ') << std::setw(3) << _threadId << ":"
                 << "Thread Finished ";
}

// --------------------------------------------------------------------------------------------

SpatialTiledLocalDomainController::SpatialTiledLocalDomainController()
    : LocalDomainControllerBase(),
      _globalStatsDimension(nullptr),
      _tileStatsDimension(nullptr),
      _blockStatsDimension(nullptr),
      _blockIdxListSize(0),
      _blockIdxListPosition(0),
      _runSpinUp(false),
      _spinupLandUnitController(_landUnitController),
      _spinupSimulateLandUnit(nullptr),
      _spinupLandUnitBuildSuccess(nullptr),
      _threadedSystem(false),
      _numThreads(0),
      _isMaster(false),
      _isThread(false),
      _internalLIbraryHandles(nullptr),
      _doLogging(false),
      _resetVariablesForEachLU(false),
      _writeStatsToSQLIte(false),
      _createStatsTablesSQLIte(true) {}

// --------------------------------------------------------------------------------------------

SpatialTiledLocalDomainController::SpatialTiledLocalDomainController(bool threadedSystem, bool isMaster, bool isThread)
    : LocalDomainControllerBase(),
      _globalStatsDimension(nullptr),
      _tileStatsDimension(nullptr),
      _blockStatsDimension(nullptr),
      _blockIdxListSize(0),
      _blockIdxListPosition(0),
      _runSpinUp(false),
      _spinupLandUnitController(_landUnitController),
      _spinupSimulateLandUnit(nullptr),
      _spinupLandUnitBuildSuccess(nullptr),
      _threadedSystem(threadedSystem),
      _numThreads(0),
      _isMaster(isMaster),
      _isThread(isThread),
      _internalLIbraryHandles(nullptr),
      _doLogging(false),
      _resetVariablesForEachLU(false),
      _writeStatsToSQLIte(false),
      _createStatsTablesSQLIte(true) {}

// --------------------------------------------------------------------------------------------

SpatialTiledLocalDomainController::SpatialTiledLocalDomainController(
    std::shared_ptr<FlintLibraryHandles> libraryHandles, bool threadedSystem, bool isMaster, bool isThread)
    : LocalDomainControllerBase(),
      _globalStatsDimension(nullptr),
      _tileStatsDimension(nullptr),
      _blockStatsDimension(nullptr),
      _blockIdxListSize(0),
      _blockIdxListPosition(0),
      _runSpinUp(false),
      _spinupLandUnitController(_landUnitController),
      _spinupSimulateLandUnit(nullptr),
      _spinupLandUnitBuildSuccess(nullptr),
      _threadedSystem(threadedSystem),
      _numThreads(0),
      _isMaster(isMaster),
      _isThread(isThread),
      _internalLIbraryHandles(nullptr),
      _doLogging(false),
      _resetVariablesForEachLU(false),
      _writeStatsToSQLIte(false),
      _createStatsTablesSQLIte(true) {
   _internalLIbraryHandles = libraryHandles;
}

// --------------------------------------------------------------------------------------------

SpatialTiledLocalDomainController::SpatialTiledLocalDomainController(
    std::shared_ptr<RecordAccumulatorWithMutex<StatsUnitRow>>& globalStatsDimension,
    std::shared_ptr<RecordAccumulatorWithMutex<StatsUnitRow>>& tileStatsDimension,
    std::shared_ptr<RecordAccumulatorWithMutex<StatsUnitRow>>& blockStatsDimension, int blockIdxListSize,
    std::shared_ptr<FlintLibraryHandles> libraryHandles, bool threadedSystem, bool isMaster, bool isThread)
    : LocalDomainControllerBase(libraryHandles),
      _globalStatsDimension(globalStatsDimension),
      _tileStatsDimension(tileStatsDimension),
      _blockStatsDimension(blockStatsDimension),
      _blockIdxListSize(blockIdxListSize),
      _blockIdxListPosition(0),
      _runSpinUp(false),
      _spinupLandUnitController(_landUnitController),
      _spinupSimulateLandUnit(nullptr),
      _spinupLandUnitBuildSuccess(nullptr),
      _threadedSystem(threadedSystem),
      _numThreads(0),
      _isMaster(isMaster),
      _isThread(isThread),
      _doLogging(false),
      _resetVariablesForEachLU(false),
      _writeStatsToSQLIte(false),
      _createStatsTablesSQLIte(true) {
   _internalLIbraryHandles = libraryHandles;
}

// --------------------------------------------------------------------------------------------

void SpatialTiledLocalDomainController::configure(const configuration::Configuration& config) {
   LocalDomainControllerBase::configure(config);
   _doLogging = config.localDomain()->doLogging();
   _numThreads = config.localDomain()->numThreads();
   _provider = std::static_pointer_cast<data::IProviderSpatialRasterInterface>(
       _dataRepository.getProvider(config.localDomain()->landscapeObject()->providerName()));

   auto& localDomainSettings = config.localDomain()->settings();

   _resetVariablesForEachLU = false;
   if (localDomainSettings.contains("reset_variables")) {
      _resetVariablesForEachLU = localDomainSettings["reset_variables"];
   }

   _writeStatsToSQLIte = false;
   if (localDomainSettings.contains("stats_on")) {
      _writeStatsToSQLIte = localDomainSettings["stats_on"];
   }

   _createStatsTablesSQLIte = false;
   if (localDomainSettings.contains("drop_and_create_tables")) {
      _createStatsTablesSQLIte = localDomainSettings["drop_and_create_tables"];
   }

   _sqliteDatabaseName = "SpatialTiledLocalDomainController.db";
   if (localDomainSettings.contains("stats_databasename")) {
      _sqliteDatabaseName = localDomainSettings["stats_databasename"].extract<const std::string>();
   }
   _runDesc = "empty";
   if (localDomainSettings.contains("stats_description")) {
      _runDesc = localDomainSettings["stats_description"].extract<const std::string>();
   }

   _samplingInterval =
       localDomainSettings.contains("sampling_interval") ? int(localDomainSettings["sampling_interval"]) : 1;

   // Look for given random seed values
   _randomSeedGlobal.clear();
   _randomSeedTile.clear();
   _randomSeedBlock.clear();
   _randomSeedCell.clear();

   if (localDomainSettings.contains("randomSeedGlobal"))
      _randomSeedGlobal = localDomainSettings["randomSeedGlobal"].extract<const UInt32>();
   if (localDomainSettings.contains("randomSeedTile"))
      _randomSeedTile = localDomainSettings["randomSeedTile"].extract<const UInt32>();
   if (localDomainSettings.contains("randomSeedBlock"))
      _randomSeedBlock = localDomainSettings["randomSeedBlock"].extract<const UInt32>();
   if (localDomainSettings.contains("randomSeedCell"))
      _randomSeedCell = localDomainSettings["randomSeedCell"].extract<const UInt32>();

   // Build spinup configuration.
   auto spinup = config.spinup();
   _runSpinUp = false;
   if (spinup->enabled()) {
      _runSpinUp = true;

      // Set start and end dates for the spinup.
      auto& timingSP = _spinupLandUnitController.timing();
      auto& timingLD = _landUnitController.timing();
      timingSP.setStartDate(timingLD.startDate());
      timingSP.setEndDate(timingLD.endDate());
      timingSP.init();

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

      // Get sequence module to drive spinups.
      ModuleMapKey sequencerKey(spinup->sequencerLibrary(), spinup->sequencerName());
      _spinupSequencer = std::dynamic_pointer_cast<SequencerModuleBase>(_spinupModules[sequencerKey]);
      _spinupSequencer->configure(timingSP);

      // TODO: check with Max on this - spinup and main controller share pools, so the list should already be populated!
      // Configure pools.
      // for (const auto pool : config.pools()) {
      //	_spinupLandUnitController.operationManager()->addPool(
      //		pool->name(), pool->description(), pool->units(), pool->scale(),
      //		pool->order(), pool->initValue());
      //}

      // Configure variables.
      // Copy in from main LUC if name not found in list of variables in spinup
      // configuration.
      for (const auto& variable : _landUnitController.variables()) {
         bool found = false;
         for (auto& var : spinup->variables()) {
            if (var->name() == variable->info().name) {
               found = true;
               break;
            }
         }
         if (!found) {
            for (auto& var : spinup->externalVariables()) {
               if (var->name() == variable->info().name) {
                  found = true;
                  break;
               }
            }
         }
         if (!found) {
            for (auto& var : spinup->flintDataVariables()) {
               if (var->name() == variable->info().name) {
                  found = true;
                  break;
               }
            }
         }
         if (!found) {
            // TODO: Check if this is required?
            // if (variable->isExternal()) {
            //	auto ptr = std::static_pointer_cast<ExternalVariable>(variable);
            //	ptr->controllerChanged(_landUnitController);
            //}
            _spinupLandUnitController.addVariable(variable->info().name, variable);
         }
      }

      for (const auto& variable : spinup->variables()) {
         _spinupLandUnitController.addVariable(
             variable->name(), std::make_shared<Variable>(variable->value(), VariableInfo{variable->name()}));
      }

      // Configure external variables (transforms).
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

      // Configure flimtdata variables (transforms).
      std::map<std::string, FlintDataInterfacePtr> spinupflintdata;
      for (const auto& variable : spinup->flintDataVariables()) {
         const auto& transformConfig = variable->flintdata();
         const auto& variableName = variable->name();
         spinupflintdata[variableName] =
             _libraryManager.GetFlintData(transformConfig.libraryName(), transformConfig.typeName());
         _spinupLandUnitController.addVariable(variable->name(),
                                               std::make_shared<moja::flint::FlintDataVariable>(
                                                   spinupflintdata[variableName], transformConfig.libraryName(),
                                                   transformConfig.typeName(), VariableInfo{variable->name()}));
      }

      // Delayed configuration call, so all flintdata are constructed.
      for (const auto& variable : spinup->flintDataVariables()) {
         const auto& flintDataConfig = variable->flintdata();
         const auto& variableName = variable->name();
         spinupflintdata[variableName]->configure(flintDataConfig.settings(), _spinupLandUnitController,
                                                  _dataRepository);
      }

      // Delayed configuration call, so all transforms are constructed.
      for (const auto& variable : spinup->externalVariables()) {
         const auto& transformConfig = variable->transform();
         const auto& variableName = variable->name();
         spinuptransforms[variableName]->configure(transformConfig.settings(), _spinupLandUnitController,
                                                   _dataRepository);
      }

      // Set initial values of pools (init value given in pool definition, not init
      // for a simulation) and variables (some special handling for vectors & structs).
      _spinupLandUnitController.initialiseData(false);
      _spinupSimulateLandUnit = _spinupLandUnitController.getVariable(spinup->simulateLandUnit());
      _spinupLandUnitBuildSuccess = _spinupLandUnitController.getVariable(spinup->landUnitBuildSuccess());
   }

   // Only the isMaster in a threaded system or a non-threaded system need to build block list from config
   // here. Threaded/isThread will have shared blockidx queue and this will populate the _blockIdxList used
   // to simulate when taking a block from the queue
   if (!_threadedSystem || (_threadedSystem && _isMaster)) {
      auto uuid = Poco::UUIDGenerator::defaultGenerator().create();
      _runId = uuid.toString();

      // Build a list of blocks to process.
      buildBlockIndexQueue(_config->localDomain()->iterationType());

      // create the tile & block level stats collections
      _globalStatsDimension = std::make_shared<RecordAccumulatorWithMutex<StatsUnitRow>>();
      _tileStatsDimension = std::make_shared<RecordAccumulatorWithMutex<StatsUnitRow>>();
      _blockStatsDimension = std::make_shared<RecordAccumulatorWithMutex<StatsUnitRow>>();

      MOJA_LOG_INFO << std::setfill(' ') << std::setw(3) << localDomainId() << ":"
                    << "Total Blocks queued for simulation: " << _blockIdxList.size();
   }
   if (_landUnitController.hasVariable("system_runid")) {
      auto runIdVar = _landUnitController.getVariable("system_runid");

      auto configRunId = runIdVar->value();
      if (configRunId.isEmpty()) {
         runIdVar->set_value(_runId);
      } else if (configRunId.isString()) {
         // don't generate a runId, use the ocnfig version
         _runId = configRunId.extract<const std::string>();
      } else {
         MOJA_LOG_ERROR << "Run id in config requires a string value ('system_runid')";
      }
   }
   MOJA_LOG_INFO << std::setfill(' ') << std::setw(3) << localDomainId() << ":"
                 << "Run ID = '" << _runId << "'";

   if (_threadedSystem && _isMaster) {
      // If configured as a threaded system - create threads ready for processing.
      MOJA_LOG_INFO << std::setfill(' ') << std::setw(3) << localDomainId() << ":"
                    << "Threads created for processing: " << _numThreads;

      // Create tasks to be put into threads later.
      for (auto t = 0; t < _numThreads; t++) {
         auto task = std::make_shared<InternalThreadBlocks>(
             (t + 100), _runId, _blockListMutex, _blockIdxList, _blockIdxListSize, _blockCellIdxList, _config,
             _globalStatsDimension, _tileStatsDimension, _blockStatsDimension, _internalLIbraryHandles);
         _tasks.push_back(task);
      }
   }
}

// --------------------------------------------------------------------------------------------

void SpatialTiledLocalDomainController::buildBlockIndexQueue(configuration::LocalDomainIterationType iterationType) {
   switch (iterationType) {
      case configuration::LocalDomainIterationType::LandscapeTiles: {
         // Build a queue of block indexes which the thread pool can extract from.
         auto tileProcessedCount = 0;
         auto& tiles = _config->localDomain()->landscapeObject()->iterationLandscapeTiles()->landscapeTiles();
         for (auto tile : tiles) {
            tileProcessedCount++;
            datarepository::TileIdx tileIdx(Point{double(tile.yIndex()), double(tile.xIndex())}, _provider->indexer());

            MOJA_LOG_INFO << std::setfill(' ') << std::setw(3) << localDomainId() << ":"
                          << "Queuing tile: " << tileIdx.toString() << " " << tileProcessedCount << " of "
                          << tiles.size();

            for (const auto block : _provider->blocks(tileIdx)) {
               _blockIdxList.push(block);
            }
         }
         break;
      }
      case configuration::LocalDomainIterationType::TileIndex: {
         // Build a queue of block indexes which the thread pool can extract from.
         auto tileProcessedCount = 0;
         auto& tiles = _config->localDomain()->landscapeObject()->iterationTileIndex()->tileIndexList();
         for (auto& tile : tiles) {
            tileProcessedCount++;
            datarepository::TileIdx tileIdx(tile.tileIdx(), _provider->indexer());

            MOJA_LOG_INFO << std::setfill(' ') << std::setw(3) << localDomainId() << ":"
                          << "Queuing tile: " << tileIdx.toString() << " " << tileProcessedCount << " of "
                          << tiles.size();

            for (const auto block : _provider->blocks(tileIdx)) {
               _blockIdxList.push(block);
            }
         }
         break;
      }
      case configuration::LocalDomainIterationType::BlockIndex: {
         // Build a queue of block indexes which the thread pool can extract from.
         auto blockProcessedCount = 0;
         auto& blocks = _config->localDomain()->landscapeObject()->iterationBlockIndex()->blockIndexList();
         for (auto& block : blocks) {
            blockProcessedCount++;
            datarepository::BlockIdx blockIdx(block.tileIdx(), block.blockIdx(), _provider->indexer());

            MOJA_LOG_INFO << std::setfill(' ') << std::setw(3) << localDomainId() << ":"
                          << "Queuing block: " << blockIdx.toString() << " " << blockProcessedCount << " of "
                          << blocks.size();

            _blockIdxList.push(blockIdx);
         }
         break;
      }
      case configuration::LocalDomainIterationType::CellIndex: {
         // Build a queue of block indexes which the thread pool can extract from.
         auto blockProcessedCount = 0;
         auto cellProcessedCount = 0;
         auto& cells = _config->localDomain()->landscapeObject()->iterationCellIndex()->cellIndexList();
         // std::map<std::pair<UInt32, UInt32>, std::vector<datarepository::CellIdx>> blockCellIdxMap;
         for (auto& cell : cells) {
            cellProcessedCount++;

            std::pair<UInt32, UInt32> key(cell.tileIdx(), cell.blockIdx());
            if (_blockCellIdxList.find(key) == _blockCellIdxList.end()) {
               blockProcessedCount++;
               datarepository::BlockIdx blockIdx(cell.tileIdx(), cell.blockIdx(), _provider->indexer());

               MOJA_LOG_INFO << std::setfill(' ') << std::setw(3) << localDomainId() << ":"
                             << "Queuing block for a cell: " << blockIdx.toString() << " "
                             << " cells " << cellProcessedCount << " of " << cells.size()
                             << " -- blocks queue length:" << blockProcessedCount;

               _blockCellIdxList.insert(std::make_pair(key, std::vector<datarepository::CellIdx>()));
               _blockIdxList.push(blockIdx);
            }
            auto& vec = _blockCellIdxList[key];
            datarepository::CellIdx cellIdx(cell.tileIdx(), cell.blockIdx(), cell.cellIdx(), _provider->indexer());
            vec.push_back(cellIdx);
         }
         break;
      }
      default: { break; }
   }
   _blockIdxListSize = int(_blockIdxList.size());
   _blockIdxListPosition = 0;
}

// --------------------------------------------------------------------------------------------

bool SpatialTiledLocalDomainController::runCellSpinUp(std::shared_ptr<StatsUnitRecord>& blockStatsUnit,
                                                      const datarepository::CellIdx& cell) {
   finally stop_all_stopwatches([&] {
      blockStatsUnit->_stopWatchProcessed.stop();
      blockStatsUnit->_stopWatchFramework.stop();
   });

   if (_runSpinUp) {
      blockStatsUnit->_unitsTotal++;
      blockStatsUnit->_stopWatchFramework.start();

      try {
         if (!_spinupSimulateLandUnit->value()) {
            blockStatsUnit->_unitsNotProcessed++;
            return true;
         }
         _spinupLandUnitBuildSuccess->set_value(true);  // set initial state of build success to TRUE
         _spinupNotificationCenter.postNotification(moja::signals::PreTimingSequence);

         if (!_spinupLandUnitBuildSuccess->value()) {
            blockStatsUnit->_unitsNotProcessed++;
            return true;
         }
         blockStatsUnit->_stopWatchFramework.stop();
         blockStatsUnit->_stopWatchProcessed.start();

         _spinupSequencer->Run(_spinupNotificationCenter, _spinupLandUnitController);
         _spinupLandUnitController.clearAllOperationResults();

         blockStatsUnit->_stopWatchProcessed.stop();
         blockStatsUnit->_unitsProcessed++;
         return true;
      } catch (const flint::SimulationError& e) {
         std::string details = *(boost::get_error_info<Details>(e));
         std::string libraryName = *(boost::get_error_info<LibraryName>(e));
         std::string moduleName = *(boost::get_error_info<ModuleName>(e));
         const int* errorCode = boost::get_error_info<ErrorCode>(e);
         _spatiallocationinfo->_errorCode = *errorCode;
         _spatiallocationinfo->_library = libraryName;
         _spatiallocationinfo->_module = moduleName;
         _spatiallocationinfo->_message = details;
         _spinupNotificationCenter.postNotification(moja::signals::Error, details);
         return true;
      } catch (const flint::LocalDomainError& e) {
         std::string details = *(boost::get_error_info<Details>(e));
         std::string libraryName = *(boost::get_error_info<LibraryName>(e));
         std::string moduleName = *(boost::get_error_info<ModuleName>(e));
         const int* errorCode = boost::get_error_info<ErrorCode>(e);
         _spatiallocationinfo->_errorCode = *errorCode;
         _spatiallocationinfo->_library = libraryName;
         _spatiallocationinfo->_module = moduleName;
         _spatiallocationinfo->_message = details;
         _spinupNotificationCenter.postNotification(moja::signals::Error, details);
      } catch (flint::VariableNotFoundException& e) {
         std::string str =
             ((boost::format("Variable not found: %1%") % *(boost::get_error_info<VariableName>(e))).str());
         _spinupNotificationCenter.postNotification(moja::signals::Error, str);
      } catch (flint::VariableEmptyWhenValueExpectedException& e) {
         std::string str =
             ((boost::format("Variable not found: %1%") % *(boost::get_error_info<VariableName>(e))).str());
         _spinupNotificationCenter.postNotification(moja::signals::Error, str);
         blockStatsUnit->_unitsNotProcessed++;
         blockStatsUnit->_unitsWithError++;
         return true;
      } catch (const Poco::Exception& e) {
         std::string str = ((boost::format("In Spinup: %1%") % e.displayText()).str());
         _spinupNotificationCenter.postNotification(moja::signals::Error, str);
      } catch (const boost::exception& e) {
         std::string str = ((boost::format("In Spinup: %1%") % boost::diagnostic_information(e)).str());
         _spinupNotificationCenter.postNotification(moja::signals::Error, str);
      } catch (const Exception& e) {
         std::string str = e.displayText();
         _spinupNotificationCenter.postNotification(moja::signals::Error, str);
      } catch (const std::exception& e) {
         std::string str = e.what();
         _spinupNotificationCenter.postNotification(moja::signals::Error, str);
      } catch (...) {
         _spinupNotificationCenter.postNotification(moja::signals::Error, "unknown exception");
      }
      blockStatsUnit->_unitsNotProcessed++;
      blockStatsUnit->_unitsWithError++;
      return false;
   }
   return true;
}

// ------------------------------------------------------------------------------------------
void SpatialTiledLocalDomainController::simulateLandUnitArea(const datarepository::CellIdx& cell){
  
      _landUnitController.initialiseData(true);
      _spatiallocationinfo->_landUnitArea = _provider->area(cell);
      _landUnitController.getVariable("landUnitArea")->set_value(_spatiallocationinfo->_landUnitArea);
                            
}

// --------------------------------------------------------------------------------------------

bool SpatialTiledLocalDomainController::runCell(std::shared_ptr<StatsUnitRecord>& blockStatsUnit,
                                                std::shared_ptr<StatsUnitRecord>& blockStatsSpinUpUnit,
                                                const datarepository::CellIdx& cell) {
   finally cleanup_at_end([&] {
      // Stop timers
      blockStatsUnit->_stopWatchSpinup.stop();
      blockStatsUnit->_stopWatchProcessed.stop();
      blockStatsUnit->_stopWatchFramework.stop();

      blockStatsSpinUpUnit->_stopWatchTotal.stop();

      // Reset variables if required
      if (_resetVariablesForEachLU) {
         _landUnitController.resetVariables();
         _spinupLandUnitController.resetVariables();
      }

      // Calculate elapsed times for SpinUp
      blockStatsSpinUpUnit->_elapsedTimeTotal = blockStatsSpinUpUnit->_stopWatchTotal.elapsed();
      blockStatsSpinUpUnit->_elapsedTimeFramework = blockStatsSpinUpUnit->_stopWatchFramework.elapsed();
      blockStatsSpinUpUnit->_elapsedTimeSpinup = blockStatsSpinUpUnit->_stopWatchSpinup.elapsed();
      blockStatsSpinUpUnit->_elapsedTimeProcessed = blockStatsSpinUpUnit->_stopWatchProcessed.elapsed();
   });

   try {
      _spatiallocationinfo->_library = "unknown";
      _spatiallocationinfo->_module = "unknown";
      _spatiallocationinfo->_errorCode = 1;
      _spatiallocationinfo->_tileIdx = cell.tileIdx;
      _spatiallocationinfo->_blockIdx = cell.blockIdx;
      _spatiallocationinfo->_cellIdx = cell.cellIdx;
      _spatiallocationinfo->_cellLatLon = _provider->indexer().getLatLonFromIndex(cell);

      if (_randomSeedCell.isNull()) {
         std::uniform_int_distribution<unsigned> distribution(
             0, std::numeric_limits<unsigned>().max());  // define the range
         _spatiallocationinfo->_randomSeedCell = distribution(_spatiallocationinfo->_engBlock);
      } else {
         _spatiallocationinfo->_randomSeedCell = _randomSeedCell.value();
      }
      _spatiallocationinfo->_engCell.seed(_spatiallocationinfo->_randomSeedCell);

      blockStatsUnit->_unitsTotal++;
      blockStatsUnit->_stopWatchFramework.start();

      // Uses the base class variable set in config to decide if a land unit
      // should be simulated and then if the build worked.
      if (!_simulateLandUnit->value()) {
         blockStatsUnit->_stopWatchFramework.stop();
         blockStatsUnit->_unitsNotProcessed++;
         return true;
      }

      SpatialTiledLocalDomainController::simulateLandUnitArea(cell);
      
      blockStatsUnit->_stopWatchSpinup.start();
      blockStatsSpinUpUnit->_stopWatchTotal.start();

      // ** Check Spinup HERE first ***
      if (runCellSpinUp(blockStatsSpinUpUnit, cell)) {
         blockStatsUnit->_stopWatchSpinup.stop();
         blockStatsSpinUpUnit->_stopWatchTotal.stop();

         _landUnitBuildSuccess->set_value(true);  // set initial state of build success to TRUE
         _notificationCenter.postNotification(moja::signals::PreTimingSequence);
         if (!_landUnitBuildSuccess->value()) {
            blockStatsUnit->_unitsNotProcessed++;
            return true;
         }
         blockStatsUnit->_stopWatchFramework.stop();
         blockStatsUnit->_stopWatchProcessed.start();

         _sequencer->Run(_notificationCenter, _landUnitController);
         _landUnitController.clearAllOperationResults();

         blockStatsUnit->_stopWatchProcessed.stop();
         blockStatsUnit->_unitsProcessed++;
         return true;
      }

   }
   // This error is recoverable, retunr true for success
   catch (const flint::SimulationError& e) {
      // This error is recoverable, drop cell and continue simulation
      std::string details = *(boost::get_error_info<Details>(e));
      std::string libraryName = *(boost::get_error_info<LibraryName>(e));
      std::string moduleName = *(boost::get_error_info<ModuleName>(e));
      const int* errorCode = boost::get_error_info<ErrorCode>(e);
      _spatiallocationinfo->_errorCode = *errorCode;
      _spatiallocationinfo->_library = libraryName;
      _spatiallocationinfo->_module = moduleName;
      _spatiallocationinfo->_message = details;
      _notificationCenter.postNotification(moja::signals::Error, details);
      blockStatsUnit->_unitsNotProcessed++;
      blockStatsUnit->_unitsWithError++;
      return true;
   }
   // All other catches are failures for the localdomain, return false!
   catch (const flint::LocalDomainError& e) {
      std::string details = *(boost::get_error_info<Details>(e));
      std::string libraryName = *(boost::get_error_info<LibraryName>(e));
      std::string moduleName = *(boost::get_error_info<ModuleName>(e));
      const int* errorCode = boost::get_error_info<ErrorCode>(e);
      _spatiallocationinfo->_errorCode = *errorCode;
      _spatiallocationinfo->_library = libraryName;
      _spatiallocationinfo->_module = moduleName;
      _spatiallocationinfo->_message = details;
      _notificationCenter.postNotification(moja::signals::Error, details);
   } catch (flint::VariableNotFoundException& e) {
      std::string str = ((boost::format("Variable not found: %1%") % *(boost::get_error_info<VariableName>(e))).str());
      _notificationCenter.postNotification(moja::signals::Error, str);
   } catch (flint::VariableEmptyWhenValueExpectedException& e) {
      std::string str = ((boost::format("Variable not found: %1%") % *(boost::get_error_info<VariableName>(e))).str());
      _notificationCenter.postNotification(moja::signals::Error, str);
      blockStatsUnit->_unitsNotProcessed++;
      blockStatsUnit->_unitsWithError++;
      return true;
   } catch (const Poco::Exception& e) {
      std::string str = ((boost::format("In Main: %1%") % e.displayText()).str());
      _notificationCenter.postNotification(moja::signals::Error, str);
   } catch (const boost::exception& e) {
      std::string str = boost::diagnostic_information(e);
      _notificationCenter.postNotification(moja::signals::Error, str);
   } catch (const Exception& e) {
      std::string str = e.displayText();
      blockStatsUnit->_unitsNotProcessed++;
      blockStatsUnit->_unitsWithError++;
      _notificationCenter.postNotification(moja::signals::Error, str);
   } catch (const std::exception& e) {
      std::string str = e.what();
      _notificationCenter.postNotification(moja::signals::Error, str);
   } catch (...) {
      _notificationCenter.postNotification(moja::signals::Error, "unknown exception");
   }
   blockStatsUnit->_unitsNotProcessed++;
   blockStatsUnit->_unitsWithError++;
   return false;
}

// --------------------------------------------------------------------------------------------

// void SpatialTiledLocalDomainController::writeRunSummaryToSQLite(bool insertRunList,
// std::vector<RunSummaryDataRecord>& runSummaryData) { 	auto retry = false; 	auto maxRetries = RETRY_ATTEMPTS; 	do { 		try {
//			retry = false;
//			SQLite::Connector::registerConnector();
//			Session session("SQLite", _sqliteDatabaseName);
//			if (_createStatsTablesSQLIte) {
//
//				if (insertRunList)
//					session << "DROP TABLE IF EXISTS run_list", now;
//				session << "DROP TABLE IF EXISTS run_properties", now;
//			}
//			if (insertRunList)
//				session << "CREATE TABLE IF NOT EXISTS run_list (id INTEGER PRIMARY KEY AUTOINCREMENT, run_id
//VARCHAR(64) NOT NULL, run_desc VARCHAR(255))", now; 			session << "CREATE TABLE IF NOT EXISTS run_properties (id INTEGER
//PRIMARY KEY AUTOINCREMENT, run_id_fk VARCHAR(64), property_name VARCHAR(255), property_info VARCHAR(255),
//property_value VARCHAR(255))", now;
//
//			if (insertRunList) {
//				auto insertStr = (boost::format("INSERT INTO run_list (run_id, run_desc) VALUES('%1%','%2%')")
//% _runId % _runDesc).str(); 				session.begin(); 				session << insertStr, now; 				session.commit();
//			}
//
//			session.begin();
//			session << "INSERT INTO run_properties (run_id_fk, property_name, property_info, property_value)
//VALUES(?, ?, ?, ?)", bind(runSummaryData), now; 			session.commit(); 			SQLite::Connector::unregisterConnector();
//		}
//		catch (SQLite::DBLockedException&) {
//			MOJA_LOG_DEBUG << localDomainId() << ":DBLockedException - " << maxRetries << " retries
//remaining"; 			std::this_thread::sleep_for(RETRY_SLEEP); 			retry = maxRetries-- > 0; 			if (!retry) { 				MOJA_LOG_DEBUG <<
//localDomainId() << ":Exceeded MAX RETIRES (" << RETRY_ATTEMPTS << ")"; 				throw;
//			}
//		}
//	} while (retry);
//}

// --------------------------------------------------------------------------------------------

void SpatialTiledLocalDomainController::run() {
   using namespace std::chrono;
   using clock = system_clock;
   using timepoint = time_point<clock>;
   using duration = nanoseconds;

   auto startTime = clock::now();
   auto t1 = moja::localtime(system_clock::to_time_t(startTime));

   std::string runSummaryInfoStr;

   if (_writeStatsToSQLIte && (!_threadedSystem || (_threadedSystem && _isMaster))) {
      /// Write first part of Stats to SQLite
      /// Stats are finalized after simulation
      // StatsUnitRecord::sqliteCreateTable(_createStatsTablesSQLIte, localDomainId(), _sqliteDatabaseName,
      // "run_stats_global"); StatsUnitRecord::sqliteCreateTable(_createStatsTablesSQLIte, localDomainId(),
      // _sqliteDatabaseName, "run_stats_tile"); StatsUnitRecord::sqliteCreateTable(_createStatsTablesSQLIte,
      // localDomainId(), _sqliteDatabaseName, "run_stats_block");

      //(boost::format("Thread %1%") % _localDomainId).str();
      runSummaryInfoStr = !_threadedSystem ? "Process" : "Master";
      if (!_threadedSystem && _landUnitController.hasVariable("spatialLocationInfo")) {
         auto _spatiallocationinfo = std::static_pointer_cast<moja::flint::SpatialLocationInfo>(
             _landUnitController.getVariable("spatialLocationInfo")
                 ->value()
                 .extract<std::shared_ptr<moja::flint::IFlintData>>());
         runSummaryInfoStr = (boost::format("Single process %1%_%2%_%3%") % _spatiallocationinfo->_tileIdx %
                              _spatiallocationinfo->_blockIdx % _spatiallocationinfo->_cellIdx)
                                 .str();
      }

      std::vector<RunSummaryDataRecord> runSummaryData;
      runSummaryData.push_back(RunSummaryDataRecord(_runId, "run_system_start", runSummaryInfoStr,
                                                    (boost::format("%1%") % moja::put_time(&t1, "%c %Z")).str()));
      runSummaryData.push_back(
          RunSummaryDataRecord(_runId, "run_threaded", runSummaryInfoStr,
                               (boost::format("%1%") % (_threadedSystem ? "true" : "false")).str()));
      runSummaryData.push_back(RunSummaryDataRecord(_runId, "run_num_threads", runSummaryInfoStr,
                                                    (boost::format("%1%") % _numThreads).str()));
      runSummaryData.push_back(RunSummaryDataRecord(_runId, "run_description", runSummaryInfoStr, _runDesc));

      // writeRunSummaryToSQLite(true, runSummaryData);
   }
   if (_writeStatsToSQLIte && (_threadedSystem && !_isMaster)) {
      runSummaryInfoStr = (boost::format("Thread %1%") % _localDomainId).str();
      std::vector<RunSummaryDataRecord> runSummaryData;
      runSummaryData.push_back(RunSummaryDataRecord(_runId, "run_thread_system_start", runSummaryInfoStr,
                                                    (boost::format("%1%") % moja::put_time(&t1, "%c %Z")).str()));

      // writeRunSummaryToSQLite(false, runSummaryData);
   }

   if (_threadedSystem && _isMaster) {
      for (const auto& task : _tasks) {
         _threads.push_back(std::thread{*(task.get())});
      }
      for (auto& thread : _threads) {
         thread.join();
      }
   } else {
      try {
         _spatiallocationinfo = std::static_pointer_cast<flint::SpatialLocationInfo>(
             _landUnitController.getVariable("spatialLocationInfo")
                 ->value()
                 .extract<std::shared_ptr<flint::IFlintData>>());

         if (_randomSeedGlobal.isNull()) {
            std::random_device rd;  // obtain a random number from hardware
            _spatiallocationinfo->_randomSeedGlobal = rd();
         } else {
            _spatiallocationinfo->_randomSeedGlobal = _randomSeedGlobal.value();
         }
         _spatiallocationinfo->_engGlobal.seed(_spatiallocationinfo->_randomSeedGlobal);

         while (_blockIdxList.size() > 0) {
            if (!_threadedSystem) {
               _blockIdxListPosition = int(_blockIdxList.size());
            }
            auto blockIdx = _blockIdxList.front();
            _blockIdxList.pop();

            auto blockStatsRecord =
                std::make_shared<StatsUnitRecord>(_runId, 0ns, 0ns, 0ns, 0ns, 0, 0, 0, 0, blockIdx.tileIdx,
                                                  blockIdx.blockIdx, StatsDurationType::Milliseconds);
            auto blockStatsSpinUpRecord =
                std::make_shared<StatsUnitRecord>(_runId, 0ns, 0ns, 0ns, 0ns, 0, 0, 0, 0, blockIdx.tileIdx,
                                                  blockIdx.blockIdx, StatsDurationType::Milliseconds);

            blockStatsRecord->_stopWatchTotal.start();
            blockStatsRecord->_stopWatchFramework.start();

            datarepository::TileIdx tileIdx(blockIdx.tileIdx, _provider->indexer());
            _spatiallocationinfo->_tileIdx = blockIdx.tileIdx;
            _spatiallocationinfo->_blockIdx = blockIdx.blockIdx;
            _spatiallocationinfo->_tileLatLon = _provider->indexer().getLatLonFromIndex(tileIdx);
            _spatiallocationinfo->_blockLatLon = _provider->indexer().getLatLonFromIndex(blockIdx);
            _spatiallocationinfo->_tileCols = _provider->indexer().tileDesc.cols;
            _spatiallocationinfo->_tileRows = _provider->indexer().tileDesc.rows;
            _spatiallocationinfo->_blockCols = _provider->indexer().blockDesc.cols;
            _spatiallocationinfo->_blockRows = _provider->indexer().blockDesc.rows;
            _spatiallocationinfo->_cellCols = _provider->indexer().cellDesc.cols;
            _spatiallocationinfo->_cellRows = _provider->indexer().cellDesc.rows;

            if (_randomSeedBlock.isNull()) {
               std::uniform_int_distribution<unsigned> distribution(
                   0, std::numeric_limits<unsigned>().max());  // define the range
               _spatiallocationinfo->_randomSeedBlock = distribution(_spatiallocationinfo->_engGlobal);
            } else {
               _spatiallocationinfo->_randomSeedBlock = _randomSeedBlock.value();
            }
            _spatiallocationinfo->_engBlock.seed(_spatiallocationinfo->_randomSeedBlock);

            _notificationCenter.postNotification(
                signals::LocalDomainProcessingUnitInit);  // Processing unit is a Set of blocks
            if (_runSpinUp) {
               _spinupNotificationCenter.postNotification(
                   signals::LocalDomainProcessingUnitInit);  // Processing unit is a Set of blocks
            }
            blockStatsRecord->_stopWatchFramework.stop();

            if (_blockCellIdxList.empty()) {
               for (const auto& cell : _provider->cells(blockIdx)) {
                  if (_samplingInterval > 1 && cell.cellIdx % _samplingInterval > 0) {
                     continue;
                  }
                  MOJA_LOG_TRACE << "Processing cell: " << _spatiallocationinfo->_tileIdx << ", "
                                 << _spatiallocationinfo->_blockIdx << ", " << _spatiallocationinfo->_cellIdx;
                  if (!runCell(blockStatsRecord, blockStatsSpinUpRecord, cell))
                     break;  // This will exit on error return from runCell
               }
            } else {
               std::pair<UInt32, UInt32> key(blockIdx.tileIdx, blockIdx.blockIdx);
               if (_blockCellIdxList.find(key) != _blockCellIdxList.end()) {
                  auto& blockCells = _blockCellIdxList[key];
                  for (const auto& cell : blockCells) {
                     if (!runCell(blockStatsRecord, blockStatsSpinUpRecord, cell))
                        break;  // This will exit on error return from runCell
                  }
               }
            }
            blockStatsRecord->_stopWatchTotal.stop();
            blockStatsRecord->_elapsedTimeTotal = blockStatsRecord->_stopWatchTotal.elapsed();
            blockStatsRecord->_elapsedTimeFramework = blockStatsRecord->_stopWatchFramework.elapsed();
            blockStatsRecord->_elapsedTimeSpinup = blockStatsRecord->_stopWatchSpinup.elapsed();
            blockStatsRecord->_elapsedTimeProcessed = blockStatsRecord->_stopWatchProcessed.elapsed();
            _blockStatsDimension->accumulate(blockStatsRecord);

            // - Accumulate Tile level stats
            auto tileStatsRecord = std::make_shared<StatsUnitRecord>(blockStatsRecord, StatsDurationType::Seconds);
            tileStatsRecord->_blockIdx.clear();
            _tileStatsDimension->accumulate(tileStatsRecord);

            // - Accumulate Global level stats
            auto globalStatsRecord = std::make_shared<StatsUnitRecord>(blockStatsRecord, StatsDurationType::Seconds);
            globalStatsRecord->_tileIdx.clear();
            globalStatsRecord->_blockIdx.clear();
            _globalStatsDimension->accumulate(globalStatsRecord);

            // - Moja Log block information
            blockStatsRecord->mojaLog("Block", localDomainId(), nullptr, &blockIdx, false, true, _blockIdxListPosition,
                                      _blockIdxListSize);

            if (_runSpinUp) {
               _spinupNotificationCenter.postNotification(
                   signals::LocalDomainProcessingUnitShutdown);  // End Processing unit
            }
            _notificationCenter.postNotification(signals::LocalDomainProcessingUnitShutdown);  // End Processing unit
         }
      } catch (const Exception& e) {
         _spatiallocationinfo->_errorCode = 1;
         MOJA_LOG_FATAL << "Exception caught at LocalDomain level, exiting..."
                        << "Module: " << _spatiallocationinfo->_module << ", "
                        << "Location: " << _spatiallocationinfo->_tileIdx << "," << _spatiallocationinfo->_blockIdx
                        << "," << _spatiallocationinfo->_cellIdx << ", "
                        << "msg:" << e.displayText();
         exit(1);
      } catch (const boost::exception& e) {
         _spatiallocationinfo->_errorCode = 1;
         MOJA_LOG_FATAL << "boost::exception caught at LocalDomain level, exiting..."
                        << "Module: " << _spatiallocationinfo->_module << ", "
                        << "Location: " << _spatiallocationinfo->_tileIdx << "," << _spatiallocationinfo->_blockIdx
                        << "," << _spatiallocationinfo->_cellIdx << ", "
                        << "msg:" << boost::diagnostic_information(e);
         exit(1);
      } catch (const Poco::FileException& e) {
         _spatiallocationinfo->_errorCode = 1;
         MOJA_LOG_FATAL << "Poco::FileException caught at LocalDomain level, exiting..."
                        << "Module: " << _spatiallocationinfo->_module << ", "
                        << "Location: " << _spatiallocationinfo->_tileIdx << "," << _spatiallocationinfo->_blockIdx
                        << "," << _spatiallocationinfo->_cellIdx << ", "
                        << "msg:" << e.displayText();
         ;
         exit(1);
      }
      // catch (const Poco::Data::SQLite::SQLiteException& e) {
      //	_spatiallocationinfo->_errorCode = 1;
      //	MOJA_LOG_FATAL
      //		<< "Poco::Data::SQLite::SQLiteException caught at LocalDomain level, exiting..."
      //		<< "Module: " << _spatiallocationinfo->_module << ", "
      //		<< "Location: " << _spatiallocationinfo->_tileIdx << "," << _spatiallocationinfo->_blockIdx <<
      //"," << _spatiallocationinfo->_cellIdx << ", "
      //		<< "msg:" << e.displayText();
      //	exit(1);
      //}
      catch (const Poco::Exception& e) {
         _spatiallocationinfo->_errorCode = 1;
         MOJA_LOG_FATAL << "Poco::Exception caught at LocalDomain level, exiting..."
                        << "Module: " << _spatiallocationinfo->_module << ", "
                        << "Location: " << _spatiallocationinfo->_tileIdx << "," << _spatiallocationinfo->_blockIdx
                        << "," << _spatiallocationinfo->_cellIdx << ", "
                        << "msg: " << e.displayText();
         exit(1);
      } catch (const std::exception& e) {
         _spatiallocationinfo->_errorCode = 1;
         MOJA_LOG_FATAL << "std::exception caught at LocalDomain level, exiting..."
                        << "Module: " << _spatiallocationinfo->_module << ", "
                        << "Location: " << _spatiallocationinfo->_tileIdx << "," << _spatiallocationinfo->_blockIdx
                        << "," << _spatiallocationinfo->_cellIdx << ", "
                        << "msg:" << e.what();
         exit(1);
      } catch (...) {
         _spatiallocationinfo->_errorCode = 1;
         MOJA_LOG_FATAL << "exception caught at LocalDomain level, exiting..."
                        << "Module: " << _spatiallocationinfo->_module << ", "
                        << "Location: " << _spatiallocationinfo->_tileIdx << "," << _spatiallocationinfo->_blockIdx
                        << "," << _spatiallocationinfo->_cellIdx << ", "
                        << "msg:"
                        << "unknown exception";
         exit(1);
      }
   }

   auto endTime = clock::now();

   Int64 totalUnits = 0;
   Int64 totalUnitsProcessed = 0;
   auto runTime = endTime - startTime;
   auto finish = system_clock::to_time_t(endTime);
   auto t2 = moja::localtime(finish);

   if (_doLogging) {
      if (!_threadedSystem || (_threadedSystem && _isMaster)) {
         MOJA_LOG_INFO << std::setfill(' ') << std::setw(3) << localDomainId() << ":"
                       << "Summary of processing by Tile";

         for (auto& rec : _tileStatsDimension->records()) {
            StatsUnitRecord* tileRec = static_cast<StatsUnitRecord*>(rec.get());
            datarepository::TileIdx tileIdxObject(tileRec->_tileIdx, _provider->indexer());
            tileRec->mojaLog("Tile", localDomainId(), &tileIdxObject, nullptr, true);
         }
         // for (auto& i : _tileStatsDimension->getPersistableCollection()) {
         //	StatsUnitRecord tileStatsRecord(i.get<1>(), seconds(i.get<2>()), seconds(i.get<3>()),
         //seconds(i.get<4>()), seconds(i.get<5>()), i.get<6>(), i.get<7>(), i.get<8>(), i.get<9>(), i.get<10>(),
         //i.get<11>(), StatsDurationType::Seconds); 	datarepository::TileIdx tileIdxObject(i.get<10>(),
         //_provider->indexer()); 	tileStatsRecord.mojaLog("Tile", localDomainId(), &tileIdxObject, nullptr, true);
         //}

         MOJA_LOG_INFO << std::setfill(' ') << std::setw(3) << localDomainId() << ":"
                       << "Summary of processing for full run";

         // for (auto& i : _globalStatsDimension->getPersistableCollection()) {
         //	StatsUnitRecord tileStatsRecord(i.get<1>(), seconds(i.get<2>()), seconds(i.get<3>()),
         //seconds(i.get<4>()), seconds(i.get<5>()), i.get<6>(), i.get<7>(), i.get<8>(), i.get<9>(), i.get<10>(),
         //i.get<11>(), StatsDurationType::Seconds); 	totalUnits += tileStatsRecord._unitsTotal; 	totalUnitsProcessed +=
         //tileStatsRecord._unitsProcessed; 	tileStatsRecord.mojaLog("Global", localDomainId(), nullptr, nullptr, true);
         //}

         for (auto& rec : _globalStatsDimension->records()) {
            StatsUnitRecord* tileRec = static_cast<StatsUnitRecord*>(rec.get());
            totalUnits += tileRec->_unitsTotal;
            totalUnitsProcessed += tileRec->_unitsProcessed;
            tileRec->mojaLog("Global", localDomainId(), nullptr, nullptr, true);
         }

         MOJA_LOG_INFO << std::setfill(' ') << std::setw(3) << localDomainId() << ":"
                       << "Start Time           : " << moja::put_time(&t1, "%c %Z");
         MOJA_LOG_INFO << std::setfill(' ') << std::setw(3) << localDomainId() << ":"
                       << "Finish Time          : " << moja::put_time(&t2, "%c %Z");
         MOJA_LOG_INFO << std::setfill(' ') << std::setw(3) << localDomainId() << ":"
                       << "Total Time (seconds) : " << duration_cast<seconds>(runTime).count();
      }
      // - Display a calculated effective LU/second time across all threads?
      // - won't be accurate but a good inidication of system time.
      if (_threadedSystem && _isMaster) {
         auto durMS = duration_cast<milliseconds>(runTime).count();
         auto durS = duration_cast<seconds>(runTime).count();

         auto luPerMillisecond = runTime == 0ms ? 0 : (durMS == 0) ? 0 : totalUnits / durMS;
         auto luPerSecond = runTime == 0s ? 0 : (durS == 0) ? 0 : totalUnits / durS;
         auto luProcessedPerMillisecond = runTime == 0ms ? 0 : (durMS == 0) ? 0 : totalUnitsProcessed / durMS;
         auto luProcessedPerSecond = runTime == 0s ? 0 : (durS == 0) ? 0 : totalUnitsProcessed / durS;

         MOJA_LOG_INFO << std::setfill(' ') << std::setw(3) << localDomainId() << ":"
                       << "Effective All LU/millisecond:       " << std::setfill(' ') << std::setw(8)
                       << luPerMillisecond;
         MOJA_LOG_INFO << std::setfill(' ') << std::setw(3) << localDomainId() << ":"
                       << "Effective All LU/second:            " << std::setfill(' ') << std::setw(8) << luPerSecond;
         MOJA_LOG_INFO << std::setfill(' ') << std::setw(3) << localDomainId() << ":"
                       << "Effective Processed LU/millisecond: " << std::setfill(' ') << std::setw(8)
                       << luProcessedPerMillisecond;
         MOJA_LOG_INFO << std::setfill(' ') << std::setw(3) << localDomainId() << ":"
                       << "Effective Processed LU/second:      " << std::setfill(' ') << std::setw(8)
                       << luProcessedPerSecond;
      }
   }

   if (_writeStatsToSQLIte && (!_threadedSystem || (_threadedSystem && _isMaster))) {
      auto durMS = duration_cast<milliseconds>(runTime).count();
      auto durS = duration_cast<seconds>(runTime).count();

      // StatsUnitRecord::sqliteWrite(_globalStatsDimension, localDomainId(), _sqliteDatabaseName, "run_stats_global");
      // StatsUnitRecord::sqliteWrite(_tileStatsDimension,	localDomainId(), _sqliteDatabaseName, "run_stats_tile");
      // StatsUnitRecord::sqliteWrite(_blockStatsDimension,	localDomainId(), _sqliteDatabaseName,
      // "run_stats_block");

      std::vector<RunSummaryDataRecord> runSummaryData;
      runSummaryData.push_back(RunSummaryDataRecord(_runId, "run_system_finish", runSummaryInfoStr,
                                                    (boost::format("%1%") % moja::put_time(&t2, "%c %Z")).str()));
      runSummaryData.push_back(
          RunSummaryDataRecord(_runId, "run_time_seconds", runSummaryInfoStr, (boost::format("%1%") % durS).str()));
      runSummaryData.push_back(RunSummaryDataRecord(_runId, "run_time_milliseconds", runSummaryInfoStr,
                                                    (boost::format("%1%") % durMS).str()));

      if (_threadedSystem && _isMaster) {
         auto luPerMillisecond = runTime == 0ms ? 0 : (durMS == 0) ? 0 : totalUnits / durMS;
         auto luPerSecond = runTime == 0s ? 0 : (durS == 0) ? 0 : totalUnits / durS;
         auto luProcessedPerMillisecond = runTime == 0ms ? 0 : (durMS == 0) ? 0 : totalUnitsProcessed / durMS;
         auto luProcessedPerSecond = runTime == 0s ? 0 : (durS == 0) ? 0 : totalUnitsProcessed / durS;

         runSummaryData.push_back(RunSummaryDataRecord(_runId, "run_all_effective_lu_per_seconds", runSummaryInfoStr,
                                                       (boost::format("%1%") % luPerMillisecond).str()));
         runSummaryData.push_back(RunSummaryDataRecord(_runId, "run_all_effective_lu_per_milliseconds",
                                                       runSummaryInfoStr, (boost::format("%1%") % luPerSecond).str()));
         runSummaryData.push_back(RunSummaryDataRecord(_runId, "run_processed_effective_lu_per_seconds",
                                                       runSummaryInfoStr,
                                                       (boost::format("%1%") % luProcessedPerMillisecond).str()));
         runSummaryData.push_back(RunSummaryDataRecord(_runId, "run_processed_effective_lu_per_milliseconds",
                                                       runSummaryInfoStr,
                                                       (boost::format("%1%") % luProcessedPerSecond).str()));
      }
      // writeRunSummaryToSQLite(false, runSummaryData);
   }
   if (_writeStatsToSQLIte && (_threadedSystem && !_isMaster)) {
      auto durMS = duration_cast<milliseconds>(runTime).count();
      auto durS = duration_cast<seconds>(runTime).count();

      std::vector<RunSummaryDataRecord> runSummaryData;
      runSummaryData.push_back(RunSummaryDataRecord(_runId, "run_thread_%1%_system_finish", runSummaryInfoStr,
                                                    (boost::format("%1%") % moja::put_time(&t2, "%c %Z")).str()));
      runSummaryData.push_back(RunSummaryDataRecord(_runId, "run_thread_%1%_time_seconds", runSummaryInfoStr,
                                                    (boost::format("%1%") % durS).str()));
      runSummaryData.push_back(RunSummaryDataRecord(_runId, "run_thread_%1%_time_milliseconds", runSummaryInfoStr,
                                                    (boost::format("%1%") % durMS).str()));

      // writeRunSummaryToSQLite(false, runSummaryData);
   }
}

void SpatialTiledLocalDomainController::startup() {
   try {
      if (_runSpinUp) {
         _spinupNotificationCenter.postNotification(moja::signals::SystemInit);
      }
      // When running threaded we don't want main process to fire LocalDomainInit
      if (!_threadedSystem || (_threadedSystem && _isThread)) {
         _notificationCenter.postNotification(moja::signals::LocalDomainInit);
         if (_runSpinUp) {
            _spinupNotificationCenter.postNotification(moja::signals::LocalDomainInit);
         }
      }
   } catch (const flint::SimulationError& e) {
      std::string details = *(boost::get_error_info<Details>(e));
      std::string libraryName = *(boost::get_error_info<LibraryName>(e));
      std::string moduleName = *(boost::get_error_info<ModuleName>(e));
      MOJA_LOG_FATAL << "Exception caught at LocalDomain level, exiting."
                     << " | Library: " << libraryName << " | Module: " << moduleName << " | Details: " << details;
      exit(1);
   } catch (const std::exception& e) {
      MOJA_LOG_FATAL << "std::exception caught at LocalDomain level, exiting..."
                     << "msg:" << e.what();
      exit(1);
   }
}

void SpatialTiledLocalDomainController::shutdown() {
   try {
      // When running threaded we don't want main process to fire LocalDomainShutdown
      if (!_threadedSystem || (_threadedSystem && _isThread)) {
         _notificationCenter.postNotification(moja::signals::LocalDomainShutdown);
         if (_runSpinUp) {
            _spinupNotificationCenter.postNotification(moja::signals::LocalDomainShutdown);
         }
      }
      if (_runSpinUp) {
         _spinupNotificationCenter.postNotification(moja::signals::SystemShutdown);
      }
   } catch (const flint::SimulationError& e) {
      std::string details = *(boost::get_error_info<Details>(e));
      std::string libraryName = *(boost::get_error_info<LibraryName>(e));
      std::string moduleName = *(boost::get_error_info<ModuleName>(e));
      MOJA_LOG_FATAL << "Exception caught at LocalDomain level, exiting."
                     << " | Library: " << libraryName << " | Module: " << moduleName << " | Details: " << details;
      exit(1);
   } catch (const std::exception& e) {
      MOJA_LOG_FATAL << "std::exception caught at LocalDomain level, exiting..."
                     << "msg:" << e.what();
      exit(1);
   }
}

#undef RETRY_ATTEMPTS
#undef RETRY_SLEEP

}  // namespace flint
}  // namespace moja
