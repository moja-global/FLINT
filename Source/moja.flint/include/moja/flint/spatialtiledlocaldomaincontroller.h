#ifndef MOJA_FLINT_SPATIALTILEDLOCALDOMAINCONTROLLER_H_
#define MOJA_FLINT_SPATIALTILEDLOCALDOMAINCONTROLLER_H_

#include "moja/flint/localdomaincontrollerbase.h"
#include "moja/flint/record.h"
#include "moja/flint/recordaccumulatorwithmutex.h"
#include "moja/flint/spatiallocationinfo.h"
#include "moja/flint/spinuplandunitcontroller.h"

#include <moja/datarepository/iproviderspatialrasterinterface.h>
#include <moja/datarepository/tileblockcellindexer.h>

#include <queue>
#include <thread>

namespace moja {
namespace flint {
namespace configuration {
enum class LocalDomainIterationType;
}

// --------------------------------------------------------------------------------------------

class finally {
   std::function<void(void)> functor;

  public:
   explicit finally(const std::function<void(void)>& functor) : functor(functor) {}
   ~finally() { functor(); }
};

// --------------------------------------------------------------------------------------------
// - Record structure used to accumulate processing times in the LDC.
// by setting the Tile & Block index values, or using null values stats can be aggregated to
// Global, Tile and Tile/Block levels

enum class StatsDurationType : int {
   Minutes,
   Seconds,
   Milliseconds,
   Nanoseconds,
};

// id, elapsedTimeTotal, elapsedTimeFramework, elapsedTimeSpinup, elapsedTimeProcessed,
//	   unitsTotal, unitsProcessed, unitsNotProcessed, unitsWithError
//	   tileIdx, blockIdx
typedef Poco::Tuple<Int64, std::string, Int64, Int64, Int64, Int64, Int64, Int64, Int64, Int64, Poco::Nullable<UInt32>,
                    Poco::Nullable<UInt32>>
    StatsUnitRow;
class StatsUnitRecord : public Record<StatsUnitRow> {
  public:
   using duration = std::chrono::nanoseconds;

   StatsUnitRecord() = delete;
   explicit StatsUnitRecord(std::shared_ptr<StatsUnitRecord>& ptr, StatsDurationType durationType);
   StatsUnitRecord(std::string runId,                // 1
                   duration elapsedTimeTotal,        // 2
                   duration elapsedTimeFramework,    // 3
                   duration elapsedTimeSpinup,       // 4
                   duration elapsedTimeProcessed,    // 5
                   Int64 unitsTotal,                 // 6
                   Int64 unitsProcessed,             // 7
                   Int64 unitsNotProcessed,          // 8
                   Int64 unitsWithError,             // 9
                   Poco::Nullable<UInt32> tileIdx,   // 10
                   Poco::Nullable<UInt32> blockIdx,  // 11
                   StatsDurationType durationType);  // 12
   ~StatsUnitRecord() {}

   bool operator==(const Record<StatsUnitRow>& other) const override;
   size_t hash() const override;
   StatsUnitRow asPersistable() const override;
   void merge(Record<StatsUnitRow>* other) override;

   void mojaLog(const std::string& levelStr, int localDomainId, datarepository::TileIdx* tileIdxObject,
                datarepository::BlockIdx* blockIdxObject, bool inSeconds, bool showRecordNumber, int recordPosition,
                int recordCount);

   // static void sqliteCreateTable(bool dropExisting, int localDomainId, std::string databaseName, std::string
   // tableName); static void sqliteWrite(std::shared_ptr<RecordAccumulatorWithMutex<StatsUnitRow>>& collection, int
   // localDomainId, std::string databaseName, std::string tableName);

   std::string _runId;
   duration _elapsedTimeTotal;
   duration _elapsedTimeFramework;
   duration _elapsedTimeSpinup;
   duration _elapsedTimeProcessed;

   Int64 _unitsTotal;
   Int64 _unitsProcessed;
   Int64 _unitsNotProcessed;
   Int64 _unitsWithError;

   Poco::Nullable<UInt32> _tileIdx;
   Poco::Nullable<UInt32> _blockIdx;

   Stopwatch<> _stopWatchTotal;
   Stopwatch<> _stopWatchFramework;
   Stopwatch<> _stopWatchSpinup;
   Stopwatch<> _stopWatchProcessed;

   // Internal only
   StatsDurationType _durationType;
};

// --------------------------------------------------------------------------------------------

class FLINT_API SpatialTiledLocalDomainController : public LocalDomainControllerBase {
  public:
   SpatialTiledLocalDomainController(void);
   SpatialTiledLocalDomainController(bool threadedSystem, bool isMaster, bool isThread);
   SpatialTiledLocalDomainController(std::shared_ptr<FlintLibraryHandles> libraryHandles, bool threadedSystem,
                                     bool isMaster, bool isThread);
   explicit SpatialTiledLocalDomainController(
       std::shared_ptr<RecordAccumulatorWithMutex<StatsUnitRow>>& globalStatsDimension,
       std::shared_ptr<RecordAccumulatorWithMutex<StatsUnitRow>>& tileStatsDimension,
       std::shared_ptr<RecordAccumulatorWithMutex<StatsUnitRow>>& blockStatsDimension, int blockIdxListSize,
       std::shared_ptr<FlintLibraryHandles> libraryHandles, bool threadedSystem = false, bool isMaster = false,
       bool isThread = false);
   ~SpatialTiledLocalDomainController(void) = default;

   void configure(const flint::configuration::Configuration& config) override;
   void run() override;

   void startup() override;

   void shutdown() override;

  private:
   class InternalThreadBlocks;

  protected:
   std::shared_ptr<RecordAccumulatorWithMutex<StatsUnitRow>> _globalStatsDimension;
   std::shared_ptr<RecordAccumulatorWithMutex<StatsUnitRow>> _tileStatsDimension;
   std::shared_ptr<RecordAccumulatorWithMutex<StatsUnitRow>> _blockStatsDimension;

  private:
   // -- General
   mutable std::shared_ptr<datarepository::IProviderSpatialRasterInterface> _provider;
   std::shared_ptr<SpatialLocationInfo> _spatiallocationinfo;
   Poco::Mutex _blockListMutex;
   std::queue<datarepository::BlockIdx> _blockIdxList;  // list of block to sim in thread/non-threaded processing.
   std::map<std::pair<UInt32, UInt32>, std::vector<datarepository::CellIdx>>
       _blockCellIdxList;  // list of cells to sim, mapped by blockIdx. Used in thread/non-threaded processing
   int _samplingInterval;  // sample size to only simulate every nth pixel

   int _blockIdxListSize;
   int _blockIdxListPosition;

   // -- Spinup
   bool _runSpinUp;
   NotificationCenter _spinupNotificationCenter;
   SpinupLandUnitController _spinupLandUnitController;
   // Variable used to check if landUnit spinup should be simulated
   const flint::IVariable* _spinupSimulateLandUnit;
   flint::IVariable* _spinupLandUnitBuildSuccess;
   std::shared_ptr<SequencerModuleBase> _spinupSequencer;
   ModuleMap _spinupModules;

   // -- non/Thread
   bool _threadedSystem;
   int _numThreads;
   bool _isMaster;
   bool _isThread;
   std::shared_ptr<FlintLibraryHandles> _internalLIbraryHandles;
   std::vector<std::shared_ptr<InternalThreadBlocks>> _tasks;
   std::vector<std::thread> _threads;

   // random seed values
   Poco::Nullable<UInt32> _randomSeedGlobal;
   Poco::Nullable<UInt32> _randomSeedTile;
   Poco::Nullable<UInt32> _randomSeedBlock;
   Poco::Nullable<UInt32> _randomSeedCell;

   // -- Logging
   bool _doLogging;  // set to stop threads logging too much info, might not be required now?
   bool _resetVariablesForEachLU;
   std::string _runId;               // unique id for Run (uuid)
   std::string _runDesc;             // string to describe run
   bool _writeStatsToSQLIte;         // SQLIte Stats Logging
   bool _createStatsTablesSQLIte;    // SQLIte drop existing table
   std::string _sqliteDatabaseName;  // SQLIte Database name (including any variables that need to be expanded)

   bool runCell(std::shared_ptr<StatsUnitRecord>& blockStatsUnit,
                std::shared_ptr<StatsUnitRecord>& blockStatsSpinUpUnit, const datarepository::CellIdx& cell);
   bool runCellSpinUp(std::shared_ptr<StatsUnitRecord>& blockStatsUnit, const datarepository::CellIdx& cell);
   void simulateLandUnitArea(const datarepository::CellIdx& cell);
   // -- Utility methods
   typedef Poco::Tuple<std::string, std::string, std::string, std::string> RunSummaryDataRecord;

   // void writeRunSummaryToSQLite(bool insertRunList, std::vector<RunSummaryDataRecord>& runSummaryData);
   void buildBlockIndexQueue(configuration::LocalDomainIterationType iterationType);
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_SPATIALTILEDLOCALDOMAINCONTROLLER_H_
