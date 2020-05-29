#include "moja/flint/uncertaintylandunitsqlitewriter.h"

#include "moja/flint/ivariable.h"
#include "moja/flint/spatiallocationinfo.h"
#include "moja/flint/uncertaintysimulationdata.h"
#include "moja/flint/variableandpoolstringbuilder.h"
#include "moja/logging.h"
#include "moja/notificationcenter.h"
#include "moja/signals.h"

#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Data/SQLite/SQLiteException.h>
#include <Poco/Data/Session.h>
#include <Poco/Exception.h>
#include <Poco/File.h>
#include <Poco/Path.h>

#include <fmt/format.h>

using namespace Poco::Data::Keywords;
using namespace Poco::Data;

namespace moja::flint {

constexpr unsigned int sqlite_retry_attempts = 10000;
constexpr std::chrono::milliseconds sqlite_retry_sleep = std::chrono::milliseconds(200);

UncertaintyLandUnitSQLiteWriter::UncertaintyLandUnitSQLiteWriter(
    std::shared_ptr<RecordAccumulatorWithMutex2<Date2Row, Date2Record>> date_dimension,
    std::shared_ptr<RecordAccumulatorWithMutex2<PoolInfoRow, PoolInfoRecord>> pool_info_dimension,
    std::shared_ptr<RecordAccumulatorWithMutex2<ModuleInfoRow, ModuleInfoRecord>> module_info_dimension,
    std::shared_ptr<RecordAccumulatorWithMutex2<TileInfoRow, TileInfoRecord>> tile_info_dimension,
    std::shared_ptr<RecordAccumulatorWithMutex2<ClassifierSetRow, ClassifierSetRecord>> classifier_set_dimension)
    : ModuleBase(),
      date_dimension_(std::move(date_dimension)),
      pool_info_dimension_(std::move(pool_info_dimension)),
      module_info_dimension_(std::move(module_info_dimension)),
      tile_info_dimension_(std::move(tile_info_dimension)),
      classifier_set_dimension_(std::move(classifier_set_dimension)),
      do_run_stats_on_(true),
      do_stock_(true),
      log_errors_(true),
      block_index_on_(false),
      log_error_per_block_(false) {}

void UncertaintyLandUnitSQLiteWriter::configure(const DynamicObject& config) {
   db_name_sqlite_ = config["databasename"].convert<std::string>();
}

void UncertaintyLandUnitSQLiteWriter::subscribe(NotificationCenter& notificationCenter) {
   notificationCenter.subscribe(signals::SystemInit, &UncertaintyLandUnitSQLiteWriter::onSystemInit, *this);
   notificationCenter.subscribe(signals::SystemShutdown, &UncertaintyLandUnitSQLiteWriter::onSystemShutdown, *this);
   notificationCenter.subscribe(signals::LocalDomainInit, &UncertaintyLandUnitSQLiteWriter::onLocalDomainInit, *this);
   notificationCenter.subscribe(signals::LocalDomainShutdown, &UncertaintyLandUnitSQLiteWriter::onLocalDomainShutdown,
                                *this);
   notificationCenter.subscribe(signals::LocalDomainProcessingUnitShutdown,
                                &UncertaintyLandUnitSQLiteWriter::onLocalDomainProcessingUnitShutdown, *this);
}

void UncertaintyLandUnitSQLiteWriter::onSystemInit() {
   simulation_unit_data_ = std::static_pointer_cast<UncertaintySimulationUnitData>(
       _landUnitData->getVariable("uncertaintySimulationUnitData")->value().extract<std::shared_ptr<IFlintData>>());
   try {
      // build the filename using pools and variable values
      VariableAndPoolStringBuilder databaseNameBuilder(_landUnitData.get(), db_name_sqlite_);
      generated_db_name_sqlite_ = databaseNameBuilder.result();

      // Force creation of specified directory, after expaned system environemnt variables
      Poco::File file(
          Poco::Path(Poco::Path::expand(generated_db_name_sqlite_), Poco::Path::PATH_NATIVE).parent().makeAbsolute());
      try {
         file.createDirectories();
      } catch (Poco::FileExistsException&) { /* Poco has a bug here, exception shouldn't be thrown, has been fixed
                                                in 1.7.8 */
      }

      std::string _systemRunIdStr = "NA";
      if (_landUnitData->hasVariable("system_runid")) {
         _systemRunIdStr = _landUnitData->getVariable("system_runid")->value().extract<const std::string>();
      }
      MOJA_LOG_INFO << "RunId (" << _systemRunIdStr << ") - "
                    << "LandUnitSQLiteWriter resolved output path is - " << file.path();

      auto retry = false;
      auto max_retries = sqlite_retry_attempts;
      do {
         try {
            retry = false;
            SQLite::Connector::registerConnector();
            Session session("SQLite", generated_db_name_sqlite_);

            std::vector<std::string> ddl{
                R"(DROP TABLE IF EXISTS run_information)", R"(DROP TABLE IF EXISTS date_dimension)",
                R"(DROP TABLE IF EXISTS poolinfo_dimension)", R"(DROP TABLE IF EXISTS tileinfo_dimension)",
                R"(DROP TABLE IF EXISTS flux_reporting_results)", R"(DROP TABLE IF EXISTS stock_reporting_results)",
                R"(DROP TABLE IF EXISTS error_log)", R"(DROP TABLE IF EXISTS error_summary)",
                R"(DROP TABLE IF EXISTS location_dimension)", R"(DROP TABLE IF EXISTS classifierset_dimension)",

                R"(CREATE TABLE poolinfo_dimension (
                     poolinfo_dimension_id_pk UNSIGNED BIG INT NOT NULL,
                     name VARCHAR(255),
                     description VARCHAR(255), 
                     internalIndex INTEGER,
                     displayOrder INTEGER,
                     scale FLOAT,
                     units VARCHAR(255)))",

                R"(CREATE TABLE tileinfo_dimension (
                     tileinfo_dimension_id_pk UNSIGNED BIG INT NOT NULL,
                     tile_index INTEGER,
                     block_index INTEGER,
                     cell_index INTEGER,
                     tile_latitude FLOAT,
                     tile_logitude FLOAT,
                     block_latitude FLOAT,
                     block_logitude FLOAT,
                     cell_latitude FLOAT,
                     cell_logitude FLOAT,
                     random_seed_global UNSIGNED BIG INT,
                     random_seed_tile UNSIGNED BIG INT,
                     random_seed_block UNSIGNED BIG INT,
                     random_seed_cell UNSIGNED BIG INT))",

                R"(CREATE TABLE flux_reporting_results (
                      flux_reporting_results_id_pk    UNSIGNED BIG INT NOT NULL,
                      local_domain_id                 INT NOT NULL,
                      date_dimension_id_fk            UNSIGNED BIG INT NOT NULL,
                      tileinfo_dimension_id_fk        UNSIGNED BIG INT NOT NULL, 
                      classifier_set_id_fk            UNSIGNED BIG INT NOT NULL,
                      module_info_id_fk               UNSIGNED BIG INT,
                      itemCount                       UNSIGNED BIG INT NOT NULL,
                      source_poolinfo_dimension_id_fk UNSIGNED BIG INT NOT NULL,
                      sink_poolinfo_dimension_id_fk   UNSIGNED BIG INT NOT NULL,
                      flux_values                     BLOB
                  );)",

                R"(CREATE TABLE location_dimension (
                     location_dimension_id_pk UNSIGNED BIG INT,
                     tileinfo_dimension_id_fk UNSIGNED BIG INT, 
                     veghistory_dimension_id_fk UNSIGNED BIG INT,
                     classifierset_dimension_id_fk UNSIGNED BIG INT, 
                     unitCount UNSIGNED BIG INT,
                     unitAreaSum FLOAT))",

                // New Classifier stuff
                R"(DROP TABLE IF EXISTS classifier_dimension)",
                R"(DROP TABLE IF EXISTS classifier_classifier_type_mapping)",
                R"(DROP TABLE IF EXISTS classifier_type_dimension)",

                R"(CREATE TABLE classifier_dimension (
                     id UNSIGNED BIG INT NOT NULL, 
                     itemCount BIG INT NOT NULL))",

                R"(CREATE TABLE classifier_classifier_type_mapping (
                     id UNSIGNED BIG INT NOT NULL, 
                     classifier_dimension_id BIG INT NOT NULL,
                     classifier_type_dimension_id BIG INT NOT NULL,
                     val VARCHAR(255)))",
                R"(CREATE TABLE classifier_type_dimension (
                     id UNSIGNED BIG INT NOT NULL,
                     name VARCHAR(255)))"};

            if (do_run_stats_on_)
               ddl.emplace_back(
                   R"(CREATE TABLE run_information (
                           localDomainId INTEGER NOT NULL, 
                           processingUnit INTEGER, 
                           module VARCHAR(255), 
                           name VARCHAR(255), 
                           value VARCHAR(255)))");
            ddl.emplace_back(
                R"(CREATE TABLE date_dimension (
                           date_dimension_id_pk UNSIGNED BIG INT, 
                           year INTEGER))");
            if (do_stock_)
               ddl.emplace_back(
                   R"(CREATE TABLE stock_reporting_results (
                           stock_reporting_results_id_pk    UNSIGNED BIG INT NOT NULL, 
                           date_dimension_id_fk             UNSIGNED BIG INT NOT NULL, 
                           local_domain_id                  INT NOT NULL, 
                           tileinfo_dimension_id_fk         UNSIGNED BIG INT NOT NULL, 
                           classifier_dimension_id_fk       UNSIGNED BIG INT NOT NULL, 
                           poolinfo_dimension_id_fk         UNSIGNED BIG INT NOT NULL, 
                           stock_values BLOB, 
                           itemCount UNSIGNED BIG INT NOT NULL))");
            if (log_errors_) {
               ddl.emplace_back(
                   R"(CREATE TABLE error_log (
                           error_log_id_pk UNSIGNED BIG INT, 
                           tile_index INTEGER, 
                           block_index INTEGER, 
                           cell_index INTEGER, 
                           global_seed UNSIGNED BIG INT, 
                           block_seed UNSIGNED BIG INT, 
                           cell_seed UNSIGNED BIG INT, 
                           tile_latitude DOUBLE PRECISION, 
                           tile_logitude DOUBLE PRECISION, 
                           block_latitude DOUBLE PRECISION, 
                           block_logitude DOUBLE PRECISION, 
                           cell_latitude DOUBLE PRECISION, 
                           cell_logitude DOUBLE PRECISION, 
                           date_dimension_id_fk UNSIGNED BIG INT, 
                           errorCode INTEGER, 
                           library VARCHAR(255), 
                           module VARCHAR(512), 
                           msg VARCHAR(2056)))");

               ddl.emplace_back(
                   R"(CREATE TABLE error_summary (
                           id UNSIGNED BIG INT, tile_index INTEGER, 
                           block_index INTEGER, 
                           msg VARCHAR(2056),
                           unitCount UNSIGNED BIG INT, 
                           unitAreaSum FLOAT))");
            }

            for (const auto& sql : ddl) {
               try_execute(session, [&sql](auto& sess) { sess << sql, now; });
            }

            SQLite::Connector::unregisterConnector();
         } catch (SQLite::DBLockedException&) {
            MOJA_LOG_DEBUG << ":DBLockedException - " << max_retries << " retries remaining";
            std::this_thread::sleep_for(sqlite_retry_sleep);
            retry = max_retries-- > 0;
            if (!retry) {
               MOJA_LOG_DEBUG << "Exceeded MAX RETIRES (" << sqlite_retry_attempts << ")";
               throw;
            }
         } catch (const SQLite::SQLiteException& /*e*/) {
            MOJA_LOG_ERROR << "SQLite operations failed, filename: " << generated_db_name_sqlite_;
            throw;
         } catch (const Poco::Exception& /*e*/) {
            MOJA_LOG_ERROR << "SQLite operations failed, filename: " << generated_db_name_sqlite_;
            throw;
         }
      } while (retry);
   } catch (Poco::AssertionViolationException& exc) {
      MOJA_LOG_DEBUG << "AssertionViolationException - " << exc.displayText();
      throw;
   } catch (SQLite::InvalidSQLStatementException& exc) {
      MOJA_LOG_DEBUG << "InvalidSQLStatementException - " << exc.displayText();
      throw;
   } catch (SQLite::ConstraintViolationException& exc) {
      MOJA_LOG_DEBUG << "ConstraintViolationException: - " << exc.displayText();
      throw;
   } catch (...) {
      MOJA_LOG_ERROR << "SQLite operations failed, filename: " << generated_db_name_sqlite_;
      if (_landUnitData->hasVariable("spatialLocationInfo")) {
         auto spatial_location_info = std::static_pointer_cast<SpatialLocationInfo>(
             _landUnitData->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<IFlintData>>());
         spatial_location_info->_library = "flint.internal";
         spatial_location_info->_module = BOOST_CURRENT_FUNCTION;
         spatial_location_info->_errorCode = 1;
      }
      throw;
   }
}

void UncertaintyLandUnitSQLiteWriter::onSystemShutdown() {}

void UncertaintyLandUnitSQLiteWriter::onLocalDomainInit() {
   simulation_unit_data_ = std::static_pointer_cast<UncertaintySimulationUnitData>(
       _landUnitData->getVariable("uncertaintySimulationUnitData")->value().extract<std::shared_ptr<IFlintData>>());
}

void UncertaintyLandUnitSQLiteWriter::onLocalDomainShutdown() {
   if (!block_index_on_) {
      writeFlux();
      writeStock();
   }
   if (!log_error_per_block_) {
      writeErrorLog();
   }
   writeRunStats("LocalDomain", simulation_unit_data_->start_local_domain_time,
                 simulation_unit_data_->end_local_domain_time, simulation_unit_data_->lu_count_local_domain);
   writeRunSummary("LocalDomain", simulation_unit_data_->start_local_domain_time,
                   simulation_unit_data_->end_local_domain_time, simulation_unit_data_->lu_count_local_domain);
}

void UncertaintyLandUnitSQLiteWriter::onLocalDomainProcessingUnitShutdown() {
   if (block_index_on_) {
      writeFlux();
      writeStock();
   }
   if (log_error_per_block_) {
      writeErrorLog();
   }
   writeRunStats("ProcessingUnit", simulation_unit_data_->start_processing_unit_time,
                 simulation_unit_data_->end_processing_unit_time, simulation_unit_data_->lu_count_processing_unit);
}

void UncertaintyLandUnitSQLiteWriter::writeStock() const {
   try {
      auto persistables = simulation_unit_data_->land_unit_stock_results.getPersistableCollection();
      if (persistables.empty()) return;

      auto retry = false;
      auto max_retries = sqlite_retry_attempts;
      do {
         try {
            retry = false;
            SQLite::Connector::registerConnector();
            Session session("SQLite", generated_db_name_sqlite_);

            // -- Stock Facts
            MOJA_LOG_DEBUG << "SQLite stock_reporting_results - inserted " << persistables.size() << " records";
            session.begin();
            session << "INSERT INTO stock_reporting_results VALUES(?, ?, ?, ?, ?, ?, ?, ?)", bind(persistables), now;
            session.commit();
            SQLite::Connector::unregisterConnector();
         } catch (SQLite::DBLockedException&) {
            MOJA_LOG_DEBUG << "DBLockedException - " << max_retries << " retries remaining";
            std::this_thread::sleep_for(sqlite_retry_sleep);
            retry = max_retries-- > 0;
            if (!retry) {
               MOJA_LOG_DEBUG << "Exceeded MAX RETIRES (" << sqlite_retry_attempts << ")";
               throw;
            }
         }
      } while (retry);
   } catch (Poco::AssertionViolationException& exc) {
      MOJA_LOG_DEBUG << "AssertionViolationException - " << exc.displayText();
      throw;
   } catch (SQLite::InvalidSQLStatementException& exc) {
      MOJA_LOG_DEBUG << "InvalidSQLStatementException: - " << exc.displayText();
      throw;
   } catch (SQLite::ConstraintViolationException& exc) {
      MOJA_LOG_DEBUG << "ConstraintViolationException: - " << exc.displayText();
      throw;
   } catch (...) {
      MOJA_LOG_ERROR << "SQLite operations failed, filename: " << generated_db_name_sqlite_;
      if (_landUnitData->hasVariable("spatialLocationInfo")) {
         auto spatial_location_info = std::static_pointer_cast<SpatialLocationInfo>(
             _landUnitData->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<IFlintData>>());
         spatial_location_info->_library = "flint.internal";
         spatial_location_info->_module = BOOST_CURRENT_FUNCTION;
         spatial_location_info->_errorCode = 1;
      }
      throw;
   }
}

void UncertaintyLandUnitSQLiteWriter::writeFlux() const {
   try {
      auto persistables = simulation_unit_data_->land_unit_flux_results.getPersistableCollection();
      if (persistables.empty()) return;

      auto retry = false;
      auto maxRetries = sqlite_retry_attempts;
      do {
         try {
            retry = false;
            SQLite::Connector::registerConnector();
            Session session("SQLite", generated_db_name_sqlite_);

            // -- Flux Facts
            MOJA_LOG_DEBUG << "SQLite flux_reporting_results - inserted " << persistables.size() << " records";
            session.begin();
            session << "INSERT INTO flux_reporting_results VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", bind(persistables), now;
            session.commit();

            SQLite::Connector::unregisterConnector();
         } catch (SQLite::DBLockedException&) {
            MOJA_LOG_DEBUG << "DBLockedException - " << maxRetries << " retries remaining";
            std::this_thread::sleep_for(sqlite_retry_sleep);
            retry = maxRetries-- > 0;
            if (!retry) {
               MOJA_LOG_DEBUG << "Exceeded MAX RETIRES (" << sqlite_retry_attempts << ")";
               throw;
            }
         }
      } while (retry);
   } catch (Poco::AssertionViolationException& exc) {
      MOJA_LOG_DEBUG << "AssertionViolationException - " << exc.displayText();
      throw;
   } catch (SQLite::InvalidSQLStatementException& exc) {
      MOJA_LOG_DEBUG << "InvalidSQLStatementException: - " << exc.displayText();
      throw;
   } catch (SQLite::ConstraintViolationException& exc) {
      MOJA_LOG_DEBUG << "ConstraintViolationException: - " << exc.displayText();
      throw;
   } catch (...) {
      MOJA_LOG_ERROR << "SQLite operations failed, filename: " << generated_db_name_sqlite_;
      if (_landUnitData->hasVariable("spatialLocationInfo")) {
         auto spatial_location_info = std::static_pointer_cast<SpatialLocationInfo>(
             _landUnitData->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<IFlintData>>());
         spatial_location_info->_library = "flint.internal";
         spatial_location_info->_module = BOOST_CURRENT_FUNCTION;
         spatial_location_info->_errorCode = 1;
      }
      throw;
   }
}

void UncertaintyLandUnitSQLiteWriter::writeErrorLog() const {
   try {
      auto retry = false;
      auto maxRetries = sqlite_retry_attempts;
      do {
         try {
            retry = false;
            SQLite::Connector::registerConnector();
            Session session("SQLite", generated_db_name_sqlite_);

            // -- Error Log
            if (simulation_unit_data_->error_log.size() != 0) {
               MOJA_LOG_DEBUG << "SQLite errors - inserted " << simulation_unit_data_->error_log.size() << " records";
               session.begin();
               session << "INSERT INTO error_log VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
                   bind(simulation_unit_data_->error_log.getPersistableCollection()), now;
               session.commit();
            }
            // -- Error Summary Log
            // if (simulation_unit_data_->errorSummaryLog.size() != 0) {
            //   MOJA_LOG_DEBUG << "SQLite error summaries - inserted " << simulation_unit_data_->errorSummaryLog.size()
            //                  << " records";
            //   session.begin();
            //   session << "INSERT INTO error_summary VALUES(?, ?, ?, ?, ?, ?)",
            //       bind(simulation_unit_data_->errorSummaryLog.getPersistableCollection()), now;
            //   session.commit();
            //}

            SQLite::Connector::unregisterConnector();
         } catch (SQLite::DBLockedException&) {
            MOJA_LOG_DEBUG << "DBLockedException - " << maxRetries << " retries remaining";
            std::this_thread::sleep_for(sqlite_retry_sleep);
            retry = maxRetries-- > 0;
            if (!retry) {
               MOJA_LOG_DEBUG << "Exceeded MAX RETIRES (" << sqlite_retry_attempts << ")";
               throw;
            }
         }
      } while (retry);

   } catch (Poco::AssertionViolationException& exc) {
      MOJA_LOG_DEBUG << "AssertionViolationException - " << exc.displayText();
      throw;  // TODO: throw a special exception here so we don't get in a loop
   } catch (Poco::Data::SQLite::InvalidSQLStatementException& exc) {
      MOJA_LOG_DEBUG << "InvalidSQLStatementException: - " << exc.displayText();
      throw;  // TODO: throw a special exception here so we don't get in a loop
   } catch (Poco::Data::SQLite::ConstraintViolationException& exc) {
      MOJA_LOG_DEBUG << "ConstraintViolationException: - " << exc.displayText();
      throw;  // TODO: throw a special exception here so we don't get in a loop
   } catch (...) {
      MOJA_LOG_ERROR << "SQLite operations failed, filename: " << generated_db_name_sqlite_;
      if (_landUnitData->hasVariable("spatialLocationInfo")) {
         auto spatial_location_info = std::static_pointer_cast<SpatialLocationInfo>(
             _landUnitData->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<IFlintData>>());
         spatial_location_info->_library = "flint.internal";
         spatial_location_info->_module = BOOST_CURRENT_FUNCTION;
         spatial_location_info->_errorCode = 1;
      }
      throw;
   }
}

void UncertaintyLandUnitSQLiteWriter::writeRunStats(const std::string& unit_label, DateTime& start_time,
                                                    DateTime& finish_time, Int64 lu_count) const {
   if (!do_run_stats_on_) return;

   std::vector<runStatDataRecord> run_stat_data;

   const auto span = finish_time - start_time;
   auto unit_span_in_microseconds = span.microseconds();

   run_stat_data.emplace_back(0, simulation_unit_data_->process_unit_count, "UncertaintyLandUnitSQLiteWriter",
                              fmt::format("{}_start", unit_label), start_time.toString());
   run_stat_data.emplace_back(0, simulation_unit_data_->process_unit_count, "UncertaintyLandUnitSQLiteWriter",
                              fmt::format("{}_finish", unit_label), finish_time.toString());
   run_stat_data.emplace_back(0, simulation_unit_data_->process_unit_count, "UncertaintyLandUnitSQLiteWriter",
                              fmt::format("{}_lu_count", unit_label), fmt::format("{}", lu_count));
   run_stat_data.emplace_back(0, simulation_unit_data_->process_unit_count, "UncertaintyLandUnitSQLiteWriter",
                              fmt::format("{}_span_microseconds", unit_label),
                              fmt::format("{}", unit_span_in_microseconds));

   try {

      auto retry = false;
      auto max_retries = sqlite_retry_attempts;
      do {
         try {
            retry = false;
            SQLite::Connector::registerConnector();
            Session session("SQLite", generated_db_name_sqlite_);

            // -- Run Stats
            session.begin();
            session << "INSERT INTO run_information VALUES(?, ?, ?, ?, ?)", bind(run_stat_data), now;
            session.commit();

            SQLite::Connector::unregisterConnector();
         } catch (SQLite::DBLockedException&) {
            MOJA_LOG_DEBUG << "DBLockedException - " << max_retries << " retries remaining";
            std::this_thread::sleep_for(sqlite_retry_sleep);
            retry = max_retries-- > 0;
            if (!retry) {
               MOJA_LOG_DEBUG << "Exceeded MAX RETIRES (" << sqlite_retry_attempts << ")";
               throw;
            }
         }
      } while (retry);
   } catch (Poco::AssertionViolationException& exc) {
      MOJA_LOG_DEBUG << "AssertionViolationException - " << exc.displayText();
      throw;
   } catch (Poco::Data::SQLite::InvalidSQLStatementException& exc) {
      MOJA_LOG_DEBUG << "InvalidSQLStatementException: - " << exc.displayText();
      throw;
   } catch (Poco::Data::SQLite::ConstraintViolationException& exc) {
      MOJA_LOG_DEBUG << "ConstraintViolationException: - " << exc.displayText();
      throw;
   } catch (...) {
      MOJA_LOG_ERROR << "SQLite operations failed, filename: " << generated_db_name_sqlite_;
      if (_landUnitData->hasVariable("spatialLocationInfo")) {
         auto spatial_location_info = std::static_pointer_cast<SpatialLocationInfo>(
             _landUnitData->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<IFlintData>>());
         spatial_location_info->_library = "flint.internal";
         spatial_location_info->_module = BOOST_CURRENT_FUNCTION;
         spatial_location_info->_errorCode = 1;
      }
      throw;
   }
}

void UncertaintyLandUnitSQLiteWriter::writeRunSummary(const std::string& unit_label, DateTime& start_time,
                                                      DateTime& finish_time, Int64 lu_count) const {
   if (!do_run_stats_on_) return;
   //try {
   //   auto retry = false;
   //   auto max_retries = sqlite_retry_attempts;
   //   do {
   //      try {
   //         retry = false;
   //         SQLite::Connector::registerConnector();
   //         Session session("SQLite", generated_db_name_sqlite_);

   //         session.begin();
   //         auto run_statistics = std::static_pointer_cast<RunStatistics>(
   //             _landUnitData->getVariable("runStatistics")->value().extract<std::shared_ptr<IFlintData>>());
   //         run_statistics->writeToSQLite(session, "run_information", 0, simulation_unit_data_->process_unit_count,
   //                                      "LandUnitSQLiteWriter");
   //         session.commit();

   //         SQLite::Connector::unregisterConnector();
   //      } catch (SQLite::DBLockedException&) {
   //         MOJA_LOG_DEBUG << "DBLockedException - " << max_retries << " retries remaining";
   //         std::this_thread::sleep_for(sqlite_retry_sleep);
   //         retry = max_retries-- > 0;
   //         if (!retry) {
   //            MOJA_LOG_DEBUG << "Exceeded MAX RETIRES (" << sqlite_retry_attempts << ")";
   //            throw;
   //         }
   //      }
   //   } while (retry);
   //} catch (Poco::AssertionViolationException& exc) {
   //   MOJA_LOG_DEBUG << "AssertionViolationException - " << exc.displayText();
   //   throw;
   //} catch (Poco::Data::SQLite::InvalidSQLStatementException& exc) {
   //   MOJA_LOG_DEBUG << "InvalidSQLStatementException: - " << exc.displayText();
   //   throw;
   //} catch (Poco::Data::SQLite::ConstraintViolationException& exc) {
   //   MOJA_LOG_DEBUG << "ConstraintViolationException: - " << exc.displayText();
   //   throw;
   //} catch (...) {
   //   MOJA_LOG_ERROR << "SQLite operations failed, filename: " << generated_db_name_sqlite_;
   //   if (_landUnitData->hasVariable("spatialLocationInfo")) {
   //      auto spatial_location_info = std::static_pointer_cast<SpatialLocationInfo>(
   //          _landUnitData->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<IFlintData>>());
   //      spatial_location_info->_library = "flint.internal";
   //      spatial_location_info->_module = BOOST_CURRENT_FUNCTION;
   //      spatial_location_info->_errorCode = 1;
   //   }
   //   throw;
   //}
}

template <typename TAccumulator>
void UncertaintyLandUnitSQLiteWriter::load(Session& session, const std::string& table,
                                           std::shared_ptr<TAccumulator> data_dimension) {
   MOJA_LOG_DEBUG << fmt::format("Loading {}", table);
   try_execute(session, [table, data_dimension](auto& sess) {
      auto data = data_dimension->getPersistableCollection();
      if (!data.empty()) {
         std::vector<std::string> placeholders;
         for (auto i = 0; i < data[0].length; i++) {
            placeholders.push_back("?");
         }
         auto sql = fmt::format("INSERT INTO {} VALUES ({})", table, fmt::join(placeholders, ", "));
         sess << sql, use(data), now;
      }
   });
   MOJA_LOG_DEBUG << fmt::format("Finished Loading {}", table);
}

void UncertaintyLandUnitSQLiteWriter::try_execute(Session& session, std::function<void(Session&)> fn) {
   try {
      session.begin();
      fn(session);
      session.commit();
   } catch (Poco::AssertionViolationException& exc) {
      MOJA_LOG_FATAL << exc.displayText() << std::endl;
   } catch (SQLite::InvalidSQLStatementException& exc) {
      MOJA_LOG_FATAL << exc.displayText() << std::endl;
   } catch (SQLite::ConstraintViolationException& exc) {
      MOJA_LOG_FATAL << exc.displayText() << std::endl;
   } catch (BindingException& exc) {
      MOJA_LOG_FATAL << exc.displayText() << std::endl;
   } catch (const std::exception& e) {
      MOJA_LOG_FATAL << e.what() << std::endl;
   } catch (...) {
      MOJA_LOG_FATAL << "Unknown exception." << std::endl;
   }
}

}  // namespace moja::flint