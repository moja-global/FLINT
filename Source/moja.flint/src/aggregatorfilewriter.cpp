#include "moja/flint/aggregatorfilewriter.h"

#include "moja/flint/aggregatorlandunit.h"
#include "moja/flint/idnamedesccollection.h"
#include "moja/flint/ilandunitdatawrapper.h"
#include "moja/flint/ipool.h"
#include "moja/flint/ivariable.h"
#include "moja/flint/simulationunitdatabase.h"

#include <moja/logging.h>
#include <moja/notificationcenter.h>
#include <moja/signals.h>

#include <Poco/Exception.h>
#include <Poco/File.h>
#include <Poco/FileStream.h>
#include <Poco/Format.h>
#include <Poco/TeeStream.h>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include <thread>

using Poco::format;
using Poco::NotFoundException;

namespace moja {
namespace flint {

static const auto DL_CHR = ",";
static const auto EOL_CHR = "\r\n";

// --------------------------------------------------------------------------------------------

void AggregatorFileWriter::configure(const DynamicObject& config) {
   _fileName = config["filename"].extract<std::string>();

   _writeFileHeader = true;
   if (config.contains("write_header")) {
      _writeFileHeader = config["write_header"];
   }
   _writeToScreen = false;
   if (config.contains("output_to_screen")) {
      _writeToScreen = config["output_to_screen"];
   }
}

// --------------------------------------------------------------------------------------------

void AggregatorFileWriter::subscribe(NotificationCenter& notificationCenter) {
   notificationCenter.subscribe(signals::SystemInit, &AggregatorFileWriter::onSystemInit, *this);
   notificationCenter.subscribe(signals::SystemShutdown, &AggregatorFileWriter::onSystemShutdown, *this);
   notificationCenter.subscribe(signals::LocalDomainInit, &AggregatorFileWriter::onLocalDomainInit, *this);
   notificationCenter.subscribe(signals::LocalDomainShutdown, &AggregatorFileWriter::onLocalDomainShutdown, *this);
   notificationCenter.subscribe(signals::LocalDomainProcessingUnitShutdown,
                                &AggregatorFileWriter::onLocalDomainProcessingUnitShutdown, *this);
}

// --------------------------------------------------------------------------------------------

void AggregatorFileWriter::onSystemInit() {
   _simulationUnitData = std::static_pointer_cast<SimulationUnitDataBase>(
       _landUnitData->getVariable("simulationUnitDataBase")->value().extract<std::shared_ptr<flint::IFlintData>>());

   try {
      // create file here and append flux data later, this will work in threaded mode as well
      // file created on onSystemInit, shared Mutex around file access used to append to it.
      Poco::File outputFile(_fileName);
      if (outputFile.exists()) outputFile.remove();
      outputFile.createFile();

      if (_writeFileHeader) {
         Poco::FileOutputStream streamFile(_fileName);
         Poco::TeeOutputStream output(streamFile);
         if (_writeToScreen) output.addStream(std::cout);

         if (_aggregatorLandUnitSharedData._outputMonth12Only) {
            output << "year" << DL_CHR;
         } else {
            output << "date_id" << DL_CHR << "step" << DL_CHR << "substep" << DL_CHR << "year" << DL_CHR << "month"
                   << DL_CHR << "day" << DL_CHR << "fracOfStep" << DL_CHR << "yearsInStep" << DL_CHR;
         }

         if (_aggregatorLandUnitSharedData._moduleInfoOn) {
            output << "module_id" << DL_CHR << "library_type" << DL_CHR << "library_info_id" << DL_CHR << "module_type"
                   << DL_CHR << "module_id" << DL_CHR << "module_name" << DL_CHR << "disturbance_type" << DL_CHR;
         }
         output /*<< "flux_id" << DL_CHR*/ << "localdomain_id" << DL_CHR << "date_id" << DL_CHR << "module_info_id"
                                           << DL_CHR << "item_count" << DL_CHR << "src pool" << DL_CHR << "sink_pool"
                                           << DL_CHR << "flux" << EOL_CHR;
         streamFile.close();
      }
   } catch (Poco::AssertionViolationException& exc) {
      MOJA_LOG_DEBUG << -1 << ":AssertionViolationException - " << exc.displayText();
      throw;
   } catch (const std::exception& e) {
      MOJA_LOG_DEBUG << -1 << ":Unknown exception: - " << e.what();
      throw;
   } catch (...) {
      MOJA_LOG_DEBUG << -1 << ":Unknown exception";
      throw;
   }
}

// --------------------------------------------------------------------------------------------

void AggregatorFileWriter::onSystemShutdown() {
   try {
      // std::vector<runStatDataRecord> runStatData;

      // MOJA_LOG_DEBUG << _simulationUnitData->_localDomainId << ":File write start"; // << Filename perhaps
   } catch (Poco::AssertionViolationException& exc) {
      MOJA_LOG_DEBUG << _simulationUnitData->_localDomainId << ":AssertionViolationException - " << exc.displayText();
      throw;
   } catch (const std::exception& e) {
      MOJA_LOG_DEBUG << _simulationUnitData->_localDomainId << ":Unknown exception: - " << e.what();
      throw;
   } catch (...) {
      MOJA_LOG_DEBUG << _simulationUnitData->_localDomainId << ":Unknown Exception";
      throw;
   }
}

// --------------------------------------------------------------------------------------------

void AggregatorFileWriter::onLocalDomainInit() {
   _simulationUnitData = std::static_pointer_cast<SimulationUnitDataBase>(
       _landUnitData->getVariable("simulationUnitDataBase")->value().extract<std::shared_ptr<flint::IFlintData>>());
}

// --------------------------------------------------------------------------------------------

void AggregatorFileWriter::onLocalDomainShutdown() { writeFlux(); }

// --------------------------------------------------------------------------------------------

void AggregatorFileWriter::onLocalDomainProcessingUnitShutdown() {
   // if (_systemSettings._blockIndexOn) {
   // writeFlux();
   //}
}

// --------------------------------------------------------------------------------------------

inline static std::string recordToStringFunc(FluxRow& rec, const std::string& dl, const IPool* srcPool,
                                             const IPool* dstPool) {
   return (boost::format("%2%%1%%3%%1%%4%%1%%5%%1%%6%%1%%7%%1%%8%") % dl /*% rec.get<0>()*/ % rec.get<1>() %
           rec.get<2>() % rec.get<3>() % rec.get<4>() % srcPool->name() /*rec.get<5>()*/ %
           dstPool->name() /*rec.get<6>()*/ % boost::io::group(std::setprecision(15), rec.get<7>()))
       .str();
};
inline static std::string recordToStringFunc(DateRow& rec, const std::string& dl) {
   return (boost::format("%2%%1%%3%%1%%4%%1%%5%%1%%6%%1%%7%%1%%8%%1%%9%") % dl % rec.get<0>() % rec.get<1>() %
           rec.get<2>() % rec.get<3>() % rec.get<4>() % rec.get<5>() % rec.get<6>() % rec.get<7>())
       .str();
};
// inline static std::string recordToStringFunc(Date2Row		& rec, const std::string& dl) { return
// (boost::format("%2%%1%%3%")										% dl % rec.get<0>() %
// rec.get<1>()).str(); };
inline static std::string recordToStringFunc(Date2Row& rec, const std::string& dl) {
   return (boost::format("%1%") % rec.get<1>()).str();
};
inline static std::string recordToStringFunc(ModuleInfoRow& rec, const std::string& dl) {
   return (boost::format("%2%%1%%3%%1%%4%%1%%5%%1%%6%%1%%7%%1%") % dl % rec.get<0>() % rec.get<1>() % rec.get<2>() %
           rec.get<3>() % rec.get<4>() % rec.get<5>())
       .str();
};

// --------------------------------------------------------------------------------------------

void AggregatorFileWriter::writeFlux() {
   try {
      std::vector<FluxRow> persistables = _simulationUnitData->_fluxResults.getPersistableCollection();

      MOJA_LOG_DEBUG << _simulationUnitData->_localDomainId << ":File writer for Land Unit aggregation)";

      // -- Write Flux Facts
      // append to end of file using _fileMutex to keep things safe
      Poco::ScopedLockWithUnlock<Poco::Mutex> lock(_fileMutex);

      // append to file here
      // Poco::OutputLineEndingConverter output(stream, Poco::LineEnding::NEWLINE_CRLF);

      Poco::FileOutputStream streamFile(_fileName, std::ios::ate);
      Poco::TeeOutputStream output(streamFile);
      if (_writeToScreen) output.addStream(std::cout);

      for (auto& rec : persistables) {
         auto srcPool = _landUnitData->getPool(rec.get<5>());
         auto dstPool = _landUnitData->getPool(rec.get<6>());
         auto str = recordToStringFunc(rec, DL_CHR, srcPool, dstPool);

         std::string dateStr;
         Int64 dateRecId = rec.get<2>();
         if (_aggregatorLandUnitSharedData._outputMonth12Only) {
            auto storedDateRecord = _date2Dimension->searchId(dateRecId);
            if (storedDateRecord != nullptr) {
               auto dateRec = storedDateRecord->asPersistable();
               dateStr = recordToStringFunc(dateRec, DL_CHR);
            }
         } else {
            auto storedDateRecord = _dateDimension->searchId(dateRecId);
            if (storedDateRecord != nullptr) {
               auto dateRec = storedDateRecord->asPersistable();
               dateStr = recordToStringFunc(dateRec, DL_CHR);
            }
         }
         output << dateStr << DL_CHR;

         std::string moduleInfoStr;
         Int64 moduleInfoRecId = rec.get<3>();
         if (_aggregatorLandUnitSharedData._moduleInfoOn) {
            auto storedModuleInfoRecord = _moduleInfoDimension->searchId(moduleInfoRecId);
            if (storedModuleInfoRecord != nullptr) {
               auto moduleInfoRec = storedModuleInfoRecord->asPersistable();
               moduleInfoStr = recordToStringFunc(moduleInfoRec, DL_CHR);
            }
            output << moduleInfoStr << DL_CHR;
         }
         output << str << EOL_CHR;  // TODO: workout how to make this adjust depending on plateform
      }

      streamFile.close();
   } catch (Poco::FileException& exc) {
      MOJA_LOG_DEBUG << _simulationUnitData->_localDomainId << ":FileException - " << exc.displayText();
      throw;
   } catch (Poco::AssertionViolationException& exc) {
      MOJA_LOG_DEBUG << _simulationUnitData->_localDomainId << ":AssertionViolationException - " << exc.displayText();
      throw;
   } catch (const std::exception& e) {
      MOJA_LOG_DEBUG << _simulationUnitData->_localDomainId << ":Unknown exception: - " << e.what();
      throw;
   } catch (...) {
      MOJA_LOG_DEBUG << _simulationUnitData->_localDomainId << ":Unknown Exception";
      throw;
   }
}

}  // namespace flint
}  // namespace moja
