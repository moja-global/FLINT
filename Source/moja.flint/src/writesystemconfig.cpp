#include "moja/flint/writesystemconfig.h"

#include "moja/flint/externalvariable.h"
#include "moja/flint/flintdatavariable.h"
#include "moja/flint/ilandunitdatawrapper.h"
#include "moja/flint/ipool.h"
#include "moja/flint/itiming.h"
#include "moja/flint/spatiallocationinfo.h"
#include "moja/flint/variable.h"

#include <moja/flint/configuration/configuration.h>
#include <moja/flint/configuration/iterationbase.h>
#include <moja/flint/configuration/landscape.h>
#include <moja/flint/configuration/library.h>
#include <moja/flint/configuration/localdomain.h>
#include <moja/flint/configuration/module.h>
#include <moja/flint/configuration/operationmanager.h>

#include <moja/dynamic.h>
#include <moja/logging.h>
#include <moja/notificationcenter.h>
#include <moja/signals.h>

#include <Poco/File.h>
#include <Poco/Path.h>

#include <boost/format.hpp>

#include <fstream>
#include <ostream>

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------

void WriteSystemConfig::configure(const DynamicObject& config) {
   _name = config["name"].extract<std::string>();
   _outputPath = config.contains("output_path") ? config["output_path"].extract<std::string>() : "";
   _writeFrequency = config.contains("write_frequency") ? config["write_frequency"].extract<UInt32>() : 0;
   _writeOutstepFrequency =
       config.contains("write_outstep_frequency") ? config["write_outstep_frequency"].extract<UInt32>() : 0;

   // set all notifications to false
   for (auto& val : _onNotificationArray) val = false;
   if (config.contains("on_notification")) {
      // is it an array or single entry
      auto dynamic = config["on_notification"];
      if (dynamic.isVector()) {
         const auto& arr = dynamic.extract<const DynamicVector>();
         for (auto& val : arr) {
            auto notStr = val.extract<std::string>();
            _onNotificationArray[convertNotificationStringToIndex(notStr)] = true;
         }
      } else {
         _onNotificationArray[convertNotificationStringToIndex(config["on_notification"].extract<std::string>())] =
             true;
      }
   } else {
      _onNotificationArray[static_cast<int>(OnNotificationType::TimingInit)] = true;
   }
}

// --------------------------------------------------------------------------------------------

void WriteSystemConfig::subscribe(NotificationCenter& notificationCenter) {
   notificationCenter.subscribe(signals::SystemInit, &WriteSystemConfig::onSystemInit, *this);
   notificationCenter.subscribe(signals::LocalDomainInit, &WriteSystemConfig::onLocalDomainInit, *this);
   notificationCenter.subscribe(signals::PreTimingSequence, &WriteSystemConfig::onPreTimingSequence, *this);
   notificationCenter.subscribe(signals::TimingInit, &WriteSystemConfig::onTimingInit, *this);
   notificationCenter.subscribe(signals::TimingShutdown, &WriteSystemConfig::onTimingShutdown, *this);
   notificationCenter.subscribe(signals::OutputStep, &WriteSystemConfig::onOutputStep, *this);
   notificationCenter.subscribe(signals::Error, &WriteSystemConfig::onError, *this);
}

// --------------------------------------------------------------------------------------------

void WriteSystemConfig::onSystemInit() {
   Poco::File workingFolder(_outputPath);
   if (!workingFolder.exists()) {
      try {
         workingFolder.createDirectories();
      } catch (Poco::FileExistsException&) { /* Poco has a bug here, exception shouldn't be thrown, has been fixed
                                                in 1.7.8 */
      }
   }
   if (workingFolder.exists() && !workingFolder.isDirectory()) {
      MOJA_LOG_ERROR << "Error creating spatial tiled point configurations output folder: " << _outputPath;
   }
   auto outputFolderPath = (boost::format("%1%%2%%3%") % workingFolder.path() % Poco::Path::separator() % _name).str();
   Poco::File outputFolder(outputFolderPath);
   if (!outputFolder.exists()) {
      try {
         outputFolder.createDirectories();
      } catch (Poco::FileExistsException&) { /* Poco has a bug here, exception shouldn't be thrown, has been fixed
                                                in 1.7.8 */
      }
   }
}

// --------------------------------------------------------------------------------------------

void WriteSystemConfig::onLocalDomainInit() {
   _writeCellProcessed = 0;
   _spatialLocationInfo = nullptr;
   if (_landUnitData->hasVariable("spatialLocationInfo")) {
      _spatialLocationInfo = std::static_pointer_cast<flint::SpatialLocationInfo>(
          _landUnitData->getVariable("spatialLocationInfo")->value().extract<std::shared_ptr<flint::IFlintData>>());
   }
}

// --------------------------------------------------------------------------------------------

void WriteSystemConfig::onPreTimingSequence() {
   _writeThisCell = (_writeFrequency == 0) || ((_writeCellProcessed % _writeFrequency) == 0);
   _writeCellProcessed++;

   if (_writeThisCell && _onNotificationArray[static_cast<int>(OnNotificationType::PreTimingSequence)])
      WriteConfig("PreTimingSequence");
}

// --------------------------------------------------------------------------------------------

void WriteSystemConfig::onTimingInit() {
   if (_writeThisCell && _onNotificationArray[static_cast<int>(OnNotificationType::TimingInit)])
      WriteConfig("TimingInit");
}

// --------------------------------------------------------------------------------------------

void WriteSystemConfig::onTimingShutdown() {
   if (_writeThisCell && _onNotificationArray[static_cast<int>(OnNotificationType::TimingShutdown)])
      WriteConfig("TimingShutdown");
}

// --------------------------------------------------------------------------------------------

void WriteSystemConfig::onOutputStep() {
   const auto timing = _landUnitData->timing();
   int timestep = timing->step();
   int timesubstep = timing->subStep();

   if ((_writeOutstepFrequency != 0) && ((timestep - 1) % _writeOutstepFrequency) != 0) return;

   if (_writeThisCell && _onNotificationArray[static_cast<int>(OnNotificationType::OutputStep)])
      WriteConfig("OutputStep");
}

// --------------------------------------------------------------------------------------------

void WriteSystemConfig::onError(std::string msg) {
   if (_writeThisCell && _onNotificationArray[static_cast<int>(OnNotificationType::Error)]) WriteConfig("Error");
}

// --------------------------------------------------------------------------------------------
void outputDynamicToStream(std::ofstream& fout, const DynamicVar& object, int level);
void outputDynamicObjectToStream(std::ofstream& fout, const DynamicObject& object, int level);
void outputVectorToStream(std::ofstream& fout, const std::vector<DynamicObject>& object, int level);
void outputVectorToStream(std::ofstream& fout, const DynamicVector& object, int level);

#define OUTPUT_LEVEL_WS(level) \
   for (int i = 0; i < level; i++) fout << "\t";

// --------------------------------------------------------------------------------------------

void outputDynamicObjectToStream(std::ofstream& fout, const DynamicObject& object, int level) {
   fout << "{" << std::endl;
   auto first = true;
   for (auto& item : object) {
      if (!first) fout << "," << std::endl;
      OUTPUT_LEVEL_WS(level + 1);
      fout << "\"" << item.first << "\": ";
      outputDynamicToStream(fout, item.second, level + 1);
      first = false;
   }
   fout << std::endl;
   OUTPUT_LEVEL_WS(level);
   fout << "}";
}

// --------------------------------------------------------------------------------------------
std::string escape_json(const std::string& s) {
   std::ostringstream o;
   for (auto c = s.cbegin(); c != s.cend(); ++c) {
      if (*c == '"' || *c == '\\' || ('\x00' <= *c && *c <= '\x1f'))
         // o << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(*c);
         o << "\\";
      o << *c;
   }
   return o.str();
}

// --------------------------------------------------------------------------------------------

template <typename T>
inline void outputBoostVectorToStream(std::ofstream& fout, const std::vector<boost::optional<T>>& object, int level) {
   fout << "[" << std::endl;
   auto first = true;
   for (auto item : object) {
      if (!first) {
         fout << "," << std::endl;
      }
      OUTPUT_LEVEL_WS(level + 1);
      if (item) {
         T val = item.get();
         outputDynamicToStream(fout, val, level + 1);
      } else
         outputDynamicToStream(fout, DynamicVar(0.00), level + 1);

      first = false;
   }
   fout << std::endl;
   OUTPUT_LEVEL_WS(level);
   fout << "]";
}

// --------------------------------------------------------------------------------------------

void outputDynamicToStream(std::ofstream& fout, const DynamicVar& object, int level) {
   if (object.isStruct()) {
      auto& obj = object.extract<const DynamicObject>();
      outputDynamicObjectToStream(fout, obj, level);
   } else if (object.isVector()) {
      if (object.type() == typeid(std::vector<boost::optional<int>>)) {
         auto& vec = object.extract<const std::vector<boost::optional<int>>>();
         outputBoostVectorToStream(fout, vec, level);
      } else if (object.type() == typeid(std::vector<boost::optional<unsigned char>>)) {
         auto& vec = object.extract<const std::vector<boost::optional<unsigned char>>>();
         outputBoostVectorToStream(fout, vec, level);
      } else if (object.type() == typeid(std::vector<boost::optional<double>>)) {
         auto& vec = object.extract<const std::vector<boost::optional<double>>>();
         outputBoostVectorToStream(fout, vec, level);
      } else if (object.type() == typeid(std::vector<DynamicObject>)) {
         auto& vec = object.extract<const std::vector<DynamicObject>>();
         outputVectorToStream(fout, vec, level);
      } else {
         auto& vec = object.extract<const DynamicVector>();
         outputVectorToStream(fout, vec, level);
      }
   } else if (object.isBoolean()) {
      bool val = object;
      fout << (val ? "true" : "false");
   } else if (object.isEmpty()) {
      fout << "null";
   } else if (object.isString()) {
      fout << "\"" << escape_json(object.extract<const std::string>()) << "\"";
   } else if (object.isInteger()) {
      int val = object;
      fout << val;
   } else if (object.isNumeric()) {
      auto val = object.extract<double>();
      fout << val;
   } else if (object.isSigned()) {
      fout << "\"** Signed\"";
   } else {
      if (object.type() == typeid(DateTime)) {
         DateTime dt = object.extract<DateTime>();
         std::string simpleDateStr =
             (boost::format("{ \"$date\": \"%1%/%2%/%3%\" }") % dt.year() % dt.month() % dt.day()).str();
         fout << simpleDateStr;
         // fout << "\"" << escape_json(simpleDateStr) << "\"";
      } else if (object.type() == typeid(Int16)) {
         fout << object.extract<const Int16>();
      } else if (object.type() == typeid(std::shared_ptr<IFlintData>)) {
         auto ptr = std::static_pointer_cast<IFlintData>(object.extract<std::shared_ptr<IFlintData>>());
         auto flintDataVar = ptr->exportObject();
         outputDynamicToStream(fout, flintDataVar, level);
      } else
         fout << "\"** UNKNOWN VALUE TYPE\"";
   }
}

// --------------------------------------------------------------------------------------------

void outputVectorToStream(std::ofstream& fout, const std::vector<DynamicObject>& object, int level) {
   fout << "[" << std::endl;
   auto first = true;
   for (auto& item : object) {
      if (!first) {
         fout << "," << std::endl;
      }
      OUTPUT_LEVEL_WS(level + 1);
      outputDynamicToStream(fout, item, level + 1);
      first = false;
   }
   fout << std::endl;
   OUTPUT_LEVEL_WS(level);
   fout << "]";
}

// --------------------------------------------------------------------------------------------

void outputVectorToStream(std::ofstream& fout, const DynamicVector& object, int level) {
   fout << "[" << std::endl;
   auto first = true;
   for (auto& item : object) {
      if (!first) {
         fout << "," << std::endl;
      }
      OUTPUT_LEVEL_WS(level + 1);
      outputDynamicToStream(fout, item, level + 1);
      first = false;
   }
   fout << std::endl;
   OUTPUT_LEVEL_WS(level);
   fout << "]";
}

// --------------------------------------------------------------------------------------------

void WriteSystemConfig::WriteConfig(std::string notificationStr) const {
   const auto timing = _landUnitData->timing();
   auto timestep = timing->step();
   auto timesubstep = timing->subStep();

   Poco::File workingFolder(_outputPath);
   auto configFilename =
       (boost::format("%1%%2%%3%%4%%5%_%6%_%7%_%8%_%9%_%10%.json") % workingFolder.path() % Poco::Path::separator() %
        _name % Poco::Path::separator() %
        boost::io::group(std::setfill('0'), std::setw(5), _spatialLocationInfo ? _spatialLocationInfo->_tileIdx : 0) %
        boost::io::group(std::setfill('0'), std::setw(3), _spatialLocationInfo ? _spatialLocationInfo->_blockIdx : 0) %
        boost::io::group(std::setfill('0'), std::setw(6), _spatialLocationInfo ? _spatialLocationInfo->_cellIdx : 0) %
        notificationStr % timestep % timesubstep)
           .str();

   Poco::File configFile(configFilename);
   if (configFile.exists()) configFile.remove(false);  // delete existing config file

   // configFile.createFile()
   FileHandle pFile(configFilename, "wb");

   std::ofstream fout;
   fout.open(configFilename);  // , ios::out | ios::trunc);

   if (fout.fail()) {
      return;
   }

   auto config = _landUnitData->config();
   if (config == nullptr) return;

   // Build libraries
   DynamicObject libraries;
   for (auto& library : config->libraries()) {
      if (library->hasPath()) {
         libraries[library->path()] = DynamicObject({{"library", library->library()}, {"order", library->name()}});
      } else {
         libraries[library->name()] = configuration::libraryTypeToStr(library->type());
      }
   }

   // Build localdomain
   DynamicObject localdomain;
   auto localDomainConfig = config->localDomain();
   localdomain["type"] = configuration::LocalDomain::localDomainTypeToStr(localDomainConfig->type());
   localdomain["start_date_init"] = (boost::format("%1%/%2%/%3%") % config->startDate().year() %
                                     config->startDate().month() % config->startDate().day())
                                        .str();
   localdomain["start_date"] = (boost::format("%1%/%2%/%3%") % timing->curStartDate().year() %
                                timing->curStartDate().month() % timing->curStartDate().day())
                                   .str();
   localdomain["end_date"] =
       (boost::format("%1%/%2%/%3%") % config->endDate().year() % config->endDate().month() % config->endDate().day())
           .str();
   localdomain["sequencer_library"] = localDomainConfig->sequencerLibrary();
   localdomain["sequencer"] = localDomainConfig->sequencer();
   localdomain["simulateLandUnit"] = localDomainConfig->simulateLandUnit();
   localdomain["landUnitBuildSuccess"] = localDomainConfig->landUnitBuildSuccess();
   localdomain["operationManager"] = localDomainConfig->operationManagerObject()->settings();

   // TODO: fillin other types of LocalDomain and iteration
   switch (localDomainConfig->type()) {
      case configuration::LocalDomainType::SpatialTiled: {
         switch (localDomainConfig->landscapeObject()->iterationType()) {
            case configuration::LocalDomainIterationType::NotAnIteration:
               break;
            case configuration::LocalDomainIterationType::ASpatialIndex:
               break;
            case configuration::LocalDomainIterationType::ASpatialMongoIndex:
               break;
            case configuration::LocalDomainIterationType::AreaOfInterest:
               break;
            case configuration::LocalDomainIterationType::LandscapeTiles:
               break;
            case configuration::LocalDomainIterationType::TileIndex:
            case configuration::LocalDomainIterationType::BlockIndex:
            case configuration::LocalDomainIterationType::CellIndex: {
               localdomain["landscape"] = DynamicObject(
                   {//{ "iteration_type",
                    // configuration::convertLocalDomainIterationTypeToStr(localDomainConfig->landscapeObject()->iterationType())
                    //},
                    {"iteration_type", "CellIndex"},
                    {"num_threads", localDomainConfig->numThreads()},
                    {"provider", localDomainConfig->landscapeObject()->providerName()},
                    {"cells", DynamicVector({DynamicObject({{"tile_index", _spatialLocationInfo->_tileIdx},
                                                            {"block_index", _spatialLocationInfo->_blockIdx},
                                                            {"cell_index", _spatialLocationInfo->_cellIdx}})})}});
            } break;
            default:
               break;
         }
      } break;
      case configuration::LocalDomainType::SpatiallyReferencedSQL: {
      } break;
      case configuration::LocalDomainType::SpatiallyReferencedNoSQL: {
      } break;
      case configuration::LocalDomainType::ThreadedSpatiallyReferencedNoSQL: {
      } break;
      case configuration::LocalDomainType::Point: {
      } break;
      default: {
      } break;
   }

   // Build pools
   DynamicObject pools;
   for (auto& pool : _landUnitData->poolCollection()) {
      pools[pool->name()] = pool->value();
   }

   // Build pools with Init values
   DynamicObject poolsInit;
   for (auto& pool : _landUnitData->poolCollection()) {
      poolsInit[pool->name()] = pool->initValue();
   }

   // Build variables
   DynamicObject variables;

   for (auto& variable : _landUnitData->variables()) {
      auto& variableName = variable->info().name;
      if (variable->isFlintData()) {
         auto ptr = std::static_pointer_cast<FlintDataVariable>(variable);
         auto flintDataVar =
             DynamicObject({{"flintdata", DynamicObject({{"library", ptr->libraryName()},
                                                         {"type", ptr->variableName()},
                                                         {"settings", ptr->flintdata()->exportObject()}})}});
         variables[variableName] = flintDataVar;
      } else if (variable->isExternal()) {
         auto ptr = std::static_pointer_cast<ExternalVariable>(variable);

         auto& val = ptr->value();
         if (val.type() == typeid(std::shared_ptr<IFlintData>)) {
            auto flindata = val.extract<std::shared_ptr<IFlintData>>();
            auto flintDataVar = DynamicObject({{"flintdata", DynamicObject({{"library", flindata->libraryName},
                                                                            {"type", flindata->typeName},
                                                                            {"settings", flindata->exportObject()}})}});
            variables[variableName] = flintDataVar;
         } else
            variables[variableName] = ptr->value();
      } else {
         auto ptr = std::static_pointer_cast<Variable>(variable);
         variables[variableName] = ptr->value();
      }
   }

   // Build modules
   DynamicObject modules;
   for (auto& module : config->modules()) {
      modules[module->name()] = DynamicObject({{"library", module->libraryName()},
                                               {"order", module->order()},
                                               {"is_proxy", module->isProxy()},
                                               {"settings", module->settings()}});
   }

   // put bits together in config Dynamic
   auto configOutput = DynamicObject({{"LocalDomain", localdomain},
                                      {"Libraries", libraries},
                                      //{ "Spinup",				DynamicObject() },
                                      //{ "SpinupModules",		DynamicObject() },
                                      //{ "SpinupVariables",	DynamicObject() },
                                      {"Pools", pools},
                                      {"Pools_Init", poolsInit},
                                      {"Variables", variables},
                                      {"Modules", modules}});

   int level = 0;
   outputDynamicObjectToStream(fout, configOutput, level);

   fout << std::flush;
   fout.close();
}

#undef OUTPUT_LEVEL_WS
#undef RETRY_ATTEMPTS
#undef RETRY_SLEEP

}  // namespace flint
}  // namespace moja
