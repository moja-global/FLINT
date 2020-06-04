#include "moja/flint/configuration/json2configurationprovider.h"

#include "moja/flint/configuration/configurationexceptions.h"
#include "moja/flint/configuration/iterationbase.h"
#include "moja/flint/configuration/iterationtileindex.h"
#include "moja/flint/configuration/library.h"
#include "moja/flint/configuration/localdomain.h"
#include "moja/flint/configuration/spinup.h"
#include "moja/flint/configuration/uncertaintyvariable.h"

#include <moja/logging.h>
#include <moja/pocojsonutils.h>

#include <Poco/File.h>
#include <Poco/JSON/ParseHandler.h>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Stringifier.h>
#include <Poco/Path.h>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>


using Poco::DynamicStruct;
using Poco::Dynamic::Var;
using Poco::JSON::Object;
using Poco::JSON::ParseHandler;
using Poco::JSON::Parser;
using Poco::JSON::Stringifier;

#include <fstream>

namespace moja {
namespace flint {
namespace configuration {

JSON2ConfigurationProvider::JSON2ConfigurationProvider(const std::vector<std::string>& configFilePath) {
   for (const std::string configFie : configFilePath) {
      Poco::File file(configFie);
      if (!file.exists()) {
         BOOST_THROW_EXCEPTION(FileNotFoundException() << FileName(configFie));
      }
   }
   _configFilePath = configFilePath;
   //_configProviderFilePath = null;
   _hasProviderConfigFile = false;
}

JSON2ConfigurationProvider::JSON2ConfigurationProvider(const std::vector<std::string>& configFilePath,
                                                       const std::vector<std::string>& configProviderFilePath) {
   for (const auto& configFie : configFilePath) {
      Poco::File file(configFie);
      if (!file.exists()) {
         BOOST_THROW_EXCEPTION(FileNotFoundException() << FileName(configFie));
      }
   }
   _configFilePath = std::move(configFilePath);

   if (!configProviderFilePath.empty()) {
      for (const auto& configProviderFile : configProviderFilePath) {
         Poco::File fileProvider(configProviderFile);
         if (!fileProvider.exists()) {
            BOOST_THROW_EXCEPTION(FileNotFoundException() << FileName(configProviderFile));
         }
      }
      _configProviderFilePath = std::move(configProviderFilePath);
      _hasProviderConfigFile = true;
   } else {
      //_configProviderFilePath = "";
      _hasProviderConfigFile = false;
   }
}

std::shared_ptr<Configuration> JSON2ConfigurationProvider::createConfiguration() {
   // parse the first run configuration file
   auto fileName = _configFilePath[0];
   std::ifstream file(fileName);
   Poco::JSON::Parser jsonParser;
   auto parsedResult = jsonParser.parse(file);

   // parse and merge rest run configuration files
   for (int index = 1; index < _configFilePath.size(); index++) {
      auto curFileName = _configFilePath[index];
      mergeJsonConfigFile(curFileName, parsedResult);
   }

   Poco::DynamicStruct jsonStruct = *parsedResult.extract<Poco::JSON::Object::Ptr>();
   auto localDomainStruct = jsonStruct["LocalDomain"].extract<Poco::DynamicStruct>();
   auto start = parseSimpleDate(localDomainStruct["start_date"].extract<std::string>());
   auto end = parseSimpleDate(localDomainStruct["end_date"].extract<std::string>());

   auto config = std::make_shared<Configuration>(start, end);

   if (_hasProviderConfigFile) {
      Poco::JSON::Parser jsonProviderParser;
      std::ifstream fileProvider(_configProviderFilePath[0]);

      // parse the first provider configuration file
      auto providerParsedJSON = jsonProviderParser.parse(fileProvider);
      auto providerParsedResult = jsonProviderParser.result();

      // parse and merge the reset provider configuration files
      for (int index = 1; index < _configProviderFilePath.size(); index++) {
         auto curProviderFileName = _configProviderFilePath[index];
         mergeJsonProviderConfigFile(curProviderFileName, providerParsedResult);
      }

      createProviders(providerParsedResult, *config);
   }

   createLocalDomain(parsedResult, *config);
   createSpinup(parsedResult, *config);
   createLibraries(parsedResult, *config);
   createPools(parsedResult, *config);
   createVariables(parsedResult, *config);
   createUncertainty(parsedResult, *config);
   createModules(parsedResult, *config);
   if (config->spinup()->enabled()) {  // only expect this list if spinup enabled
      createSpinupModules(parsedResult, *config);
      createVariables(parsedResult, *config, true);
   }

   return config;
}

/*
Parse and merge run configuration files.
It only merges up to 2nd level configuration.
*/
void JSON2ConfigurationProvider::mergeJsonConfigFile(std::string fileName, DynamicVar parsedResult) {
   auto jsonStructFinal = parsedResult.extract<Object::Ptr>();
   std::ifstream file(fileName);

   if (file.is_open()) {
      Poco::JSON::Parser jsonParser;

      auto currentParsedResult = jsonParser.parse(file);
      auto jsonStructCurrent = currentParsedResult.extract<Object::Ptr>();

      std::vector<std::string> names;
      jsonStructCurrent->getNames(names);

      for (std::string name : names) {
         auto definedInParsedResult = jsonStructFinal->has(name);
         auto currentConfigObj = jsonStructCurrent->get(name);
         if (!definedInParsedResult) {
            // insert the undefined config struct
            jsonStructFinal->set(name, currentConfigObj);
         } else {
            auto finalObj = jsonStructFinal->get(name);
            auto finalDynamicObj = parsePocoVarToDynamic(finalObj);
            auto finalDynamicObjContents =
                finalDynamicObj.isEmpty() ? DynamicObject() : finalDynamicObj.extract<const DynamicObject>();

            auto currentDynamicObj = parsePocoVarToDynamic(currentConfigObj);
            auto currentObjContents =
                currentDynamicObj.isEmpty() ? DynamicObject() : currentDynamicObj.extract<const DynamicObject>();

            for (const auto& item : currentObjContents) {
               auto itemName = std::get<0>(item);
               auto findItem = finalDynamicObjContents.find(itemName);

               // not found, insert the config pair
               if (findItem == finalDynamicObjContents.end()) {
                  auto finalObjPtr = finalObj.extract<Object::Ptr>();
                  finalObjPtr->set(itemName, std::get<1>(item));
               }
            }
         }
      }
      file.close();
   }
}

/*
Parse and merge data provider configuration files
it merges only raster-tiled layers configuration. It assumes that other configurations are identical.
*/
void JSON2ConfigurationProvider::mergeJsonProviderConfigFile(std::string fileName, DynamicVar parsedResult) {
   auto jsonStructFinal = parsedResult.extract<Object::Ptr>();
   auto providerFinal = jsonStructFinal->getObject("Providers");
   std::ifstream file(fileName);

   if (file.is_open()) {
      Poco::JSON::Parser jsonParser;

      auto currentParsedResult = jsonParser.parse(file);
      auto jsonStructCurrent = currentParsedResult.extract<Object::Ptr>();
      auto providerCurrent = jsonStructCurrent->getObject("Providers");

      std::vector<std::string> names;
      providerCurrent->getNames(names);

      for (auto name : names) {
         bool definedInParsedResult = providerFinal->has(name);
         auto currentConfigObj = providerCurrent->get(name);
         if (!definedInParsedResult) {
            // insert the undefined config struct
            providerFinal->set(name, currentConfigObj);
         } else {
            auto finalObj = providerFinal->get(name);
            auto finalDynamicObj = parsePocoVarToDynamic(finalObj);
            auto finalDynamicObjContents =
                finalDynamicObj.isEmpty() ? DynamicObject() : finalDynamicObj.extract<const DynamicObject>();

            auto currentDynamicObj = parsePocoVarToDynamic(currentConfigObj);
            auto currentObjContents =
                currentDynamicObj.isEmpty() ? DynamicObject() : currentDynamicObj.extract<const DynamicObject>();

            for (const auto& item : currentObjContents) {
               auto itemName = std::get<0>(item);
               if (itemName.compare("layers") == 0) {
                  auto definedLayer = finalDynamicObjContents.find("layers");
                  auto ptrArr = finalObj.extract<Poco::JSON::Object::Ptr>();
                  auto layObj = ptrArr->get("layers");
                  auto ptrArrFinal = layObj.extract<Poco::JSON::Array::Ptr>();

                  auto currentLayerObj = currentConfigObj.extract<Poco::JSON::Object::Ptr>();
                  auto currentLayers = currentLayerObj->get("layers").extract<Poco::JSON::Array::Ptr>();
                  auto sizeCur = currentLayers->size();
                  for (auto i = 0; i < currentLayers->size(); i++) {
                     auto value = currentLayers->get(i);
                     ptrArrFinal->add(value);
                  }
               } else {
                  auto findItem = finalDynamicObjContents.find(itemName);

                  if (findItem == finalDynamicObjContents.end()) {
                     // not found, insert the new config pair
                     auto finalObjPtr = finalObj.extract<Object::Ptr>();
                     finalObjPtr->set(itemName, std::get<1>(item));
                  }
               }
            }
         }
      }
      file.close();
   }
}

void JSON2ConfigurationProvider::createLocalDomain(DynamicVar& parsedJSON, Configuration& config) const {
   Poco::DynamicStruct jsonStruct = *parsedJSON.extract<Poco::JSON::Object::Ptr>();
   auto localDomainStruct = jsonStruct["LocalDomain"].extract<Poco::DynamicStruct>();
   const auto localDomainType = parseLocalDomainType(localDomainStruct["type"]);
   const auto sequencerLibrary = localDomainStruct["sequencer_library"].extract<std::string>();
   const auto sequencer = localDomainStruct["sequencer"].extract<std::string>();
   const auto simulateLandUnit = localDomainStruct["simulateLandUnit"].extract<std::string>();
   const auto landUnitBuildSuccess = localDomainStruct["landUnitBuildSuccess"].extract<std::string>();
   auto stepping = TimeStepping::Monthly;
   if (localDomainStruct.contains("timing")) {
      const auto timing = localDomainStruct["timing"].extract<std::string>();
      if (timing == "annual") {
         stepping = TimeStepping::Annual;
      }
   }

   DynamicObject operationManagerSettings;
   if (localDomainStruct.contains("operationManager")) {
      auto s = localDomainStruct["operationManager"];
      if (s.isStruct()) {
         auto d = parsePocoVarToDynamic(localDomainStruct["operationManager"]);
         operationManagerSettings = d.isEmpty() ? DynamicObject() : d.extract<const DynamicObject>();
      } else {
         operationManagerSettings["name"] = s.extract<std::string>();  // operationManagerName;
      }
   } else {
      operationManagerSettings["name"] = "";
   }

   DynamicObject settings;
   if (localDomainStruct.contains("settings")) {
      auto d = parsePocoVarToDynamic(localDomainStruct["settings"]);
      settings = d.isEmpty() ? DynamicObject() : d.extract<const DynamicObject>();
   }

   if (localDomainType != LocalDomainType::Point) {
      auto d = parsePocoVarToDynamic(localDomainStruct["landscape"]);
      auto& landscape = d.isEmpty() ? DynamicObject() : d.extract<const DynamicObject>();

      auto iterationType =
          LocalDomainIterationType::LandscapeTiles;  // TODO: confirm this - I've set this as the default for now
      if (landscape.contains("iteration_type")) {
         const auto& str = landscape["iteration_type"];
         iterationType = convertStrToLocalDomainIterationType(str);
      }

      auto doLogging = true;
      if (landscape.contains("do_logging")) {
         doLogging = landscape["do_logging"];
      }

      auto numThreads = 1;
      if (landscape.contains("num_threads")) {
         numThreads = landscape["num_threads"];
      }

      config.setLocalDomain(localDomainType, iterationType, doLogging, numThreads, sequencerLibrary, sequencer,
                            simulateLandUnit, landUnitBuildSuccess, settings, stepping);

      config.localDomain()->setLandscapeObject(landscape["provider"].convert<std::string>(), iterationType);

      auto landscapeObject = config.localDomain()->landscapeObject();
      switch (iterationType) {
         case LocalDomainIterationType::AreaOfInterest: {
            // NEW STUFF
            landscapeObject->setIterationAreaOfInterest();
            auto aoiIteration = landscapeObject->iterationAreaOfInterest();
            // TODO: fill this in
            break;
         }
         case LocalDomainIterationType::ASpatialIndex: {
            landscapeObject->setIterationASpatialIndex();
            auto asiIteration = landscapeObject->iterationASpatialIndex();
            asiIteration->set_maxTileSize(landscape["max_tile_size"]);
            asiIteration->set_tileCacheSize(landscape["tile_cache_size"]);
            break;
         }
         case LocalDomainIterationType::ASpatialMongoIndex: {
            landscapeObject->setIterationASpatialMongoIndex();
            auto asmiIteration = landscapeObject->iterationASpatialMongoIndex();

            if (landscape.contains("process_all")) asmiIteration->set_processAlldocuments(landscape["process_all"]);

            if (landscape.contains("documents")) {
               for (auto& item : landscape["documents"].extract<DynamicVector>()) {
                  std::string id = item["id"];
                  asmiIteration->addDocumentId(id);
               }
            }
            break;
         }
         case LocalDomainIterationType::LandscapeTiles: {
            landscapeObject->setIterationLandscapeTiles();
            auto ltIteration = landscapeObject->iterationLandscapeTiles();
            ltIteration->set_tileSizeX(landscape["tile_size_x"]);
            ltIteration->set_tileSizeY(landscape["tile_size_y"]);
            ltIteration->set_xPixels(landscape["x_pixels"]);
            ltIteration->set_yPixels(landscape["y_pixels"]);
            for (auto& tileConfig : landscape["tiles"].extract<DynamicVector>()) {
               int xIndex = tileConfig["x"];
               int yIndex = tileConfig["y"];
               ltIteration->landscapeTiles().push_back(ConfigTile(xIndex, yIndex, ltIteration->tileSizeX(),
                                                                  ltIteration->tileSizeY(), ltIteration->xPixels(),
                                                                  ltIteration->yPixels()));
            }
            break;
         }
         case LocalDomainIterationType::TileIndex: {
            landscapeObject->setIterationTileIndex();
            auto tileIteration = landscapeObject->iterationTileIndex();

            for (auto& item : landscape["tiles"].extract<DynamicVector>()) {
               UInt32 tileIdx = item["tile_index"];
               tileIteration->addTileIndex(ConfigTileIdx(tileIdx));
            }
            break;
         }
         case LocalDomainIterationType::BlockIndex: {
            landscapeObject->setIterationBlockIndex();
            auto blockIteration = landscapeObject->iterationBlockIndex();
            for (auto& item : landscape["blocks"].extract<DynamicVector>()) {
               const UInt32 tileIdx = item["tile_index"];
               const UInt32 blockIdx = item["block_index"];
               blockIteration->addBlockIndex(ConfigBlockIdx(tileIdx, blockIdx));
            }
            break;
         }
         case LocalDomainIterationType::CellIndex: {
            landscapeObject->setIterationCellIndex();
            auto cellIteration = landscapeObject->iterationCellIndex();
            for (auto& item : landscape["cells"].extract<DynamicVector>()) {
               const UInt32 tileIdx = item["tile_index"];
               const UInt32 blockIdx = item["block_index"];
               const UInt32 cellIdx = item["cell_index"];
               cellIteration->addCellIndex(ConfigCellIdx(tileIdx, blockIdx, cellIdx));
            }
            break;
         }
         case LocalDomainIterationType::NotAnIteration:
            break;
         default:
            break;
      }
   } else {
      config.setLocalDomain(localDomainType, LocalDomainIterationType::NotAnIteration, true, 1, sequencerLibrary,
                            sequencer, simulateLandUnit, landUnitBuildSuccess, settings);
   }
   config.localDomain()->operationManagerObject()->set_name(
       operationManagerSettings["name"].extract<const std::string>());
   config.localDomain()->operationManagerObject()->set_settings(operationManagerSettings);
}

void JSON2ConfigurationProvider::createSpinup(DynamicVar& parsedJSON, Configuration& config) const {
   Poco::DynamicStruct jsonStruct = *parsedJSON.extract<Poco::JSON::Object::Ptr>();
   if (!jsonStruct.contains("Spinup")) {
      DynamicObject settings;
      config.setSpinup(false, "NA", "NA", "NA", "NA", settings);
      return;
   }

   auto spinupStruct = jsonStruct["Spinup"].extract<Poco::DynamicStruct>();
   bool enabled = false;
   if (spinupStruct.contains("enabled")) {
      enabled = spinupStruct["enabled"].extract<bool>();
   }

   if (enabled) {
      for (auto param : {"sequencer_library", "sequencer", "simulateLandUnit", "landUnitBuildSuccess"}) {
         if (!spinupStruct.contains(param)) {
            BOOST_THROW_EXCEPTION(ModuleParamsException() << Param((boost::format("Spinup.%1%") % param).str()));
         }
      }

      auto sequencerLibrary = spinupStruct["sequencer_library"].extract<std::string>();
      auto sequencer = spinupStruct["sequencer"].extract<std::string>();
      auto simulateLandUnit = spinupStruct["simulateLandUnit"].extract<std::string>();
      auto landUnitBuildSuccess = spinupStruct["landUnitBuildSuccess"].extract<std::string>();
      auto d = parsePocoVarToDynamic(jsonStruct["Spinup"]);
      auto& settings = d.isEmpty() ? DynamicObject() : d.extract<const DynamicObject>();
      config.setSpinup(enabled, sequencerLibrary, sequencer, simulateLandUnit, landUnitBuildSuccess, settings);
   } else {
      DynamicObject settings;
      config.setSpinup(false, "NA", "NA", "NA", "NA", settings);
   }
}

void JSON2ConfigurationProvider::createLibraries(DynamicVar& parsedJSON, Configuration& config) const {
   Poco::DynamicStruct jsonStruct = *parsedJSON.extract<Poco::JSON::Object::Ptr>();
   auto librariesStruct = jsonStruct["Libraries"].extract<Poco::DynamicStruct>();
   for (auto item : librariesStruct) {
      if (item.second.isStruct()) {
         auto libraryStruct = item.second.extract<Poco::DynamicStruct>();
         auto libraryName = libraryStruct["library"].extract<std::string>();
         auto libraryPath = libraryStruct["path"].extract<std::string>();  // item.first;
         const auto libraryTypeStr = libraryStruct["type"].extract<std::string>();

         MOJA_LOG_DEBUG << "details (" << item.first << "): path - " << libraryPath << ", filename -" << libraryName;

         config.addLibrary(libraryPath, libraryName, item.first, parseLibraryType(libraryTypeStr));
      } else {
         config.addLibrary(item.first, parseLibraryType(item.second.extract<std::string>()));
      }
   }
}

bool JSON2ConfigurationProvider::fileExists(const std::string& path) {
   Poco::File pf(path);
   return pf.exists();
}

void JSON2ConfigurationProvider::createProviders(DynamicVar& parsedJSON, Configuration& config) const {
   auto jsonStruct2 = *parsedJSON.extract<Poco::JSON::Object::Ptr>();
   auto provider = jsonStruct2.getObject("Providers");
   auto& data = *(provider.get());
   for (auto& item : data) {
      auto d = parsePocoJSONToDynamic(item.second);
      if (d.isEmpty()) {
         BOOST_THROW_EXCEPTION(ProviderSettingsException() << ProviderName(item.first));
      }
      auto xx = d.extract<Poco::DynamicStruct>();
      std::string libName;
      if (xx.contains("library"))
         libName = d["library"].convert<std::string>();
      else
         libName = "internal.flint";

      const std::string providerType = d["type"].convert<std::string>();
      auto& settings = d.extract<const DynamicObject>();
      config.addProvider(item.first, libName, providerType, settings);
   }
}

void JSON2ConfigurationProvider::createPools(DynamicVar& parsedJSON, Configuration& config) const {
   Poco::DynamicStruct jsonStruct = *parsedJSON.extract<Poco::JSON::Object::Ptr>();
   auto poolsItem = jsonStruct["Pools"];

   if (poolsItem.isStruct()) {
      auto poolsStruct = poolsItem.extract<Poco::DynamicStruct>();
      for (auto item : poolsStruct) {
         if (item.second.isStruct()) {
            auto poolStruct = item.second.extract<Poco::DynamicStruct>();
            if (createSpecialPools(item.first, poolStruct, config)) {
               continue;
            }

            auto poolDesc = poolStruct["description"].extract<std::string>();
            auto poolUnits = poolStruct["units"].extract<std::string>();
            auto poolScale = poolStruct["scale"].extract<double>();
            auto poolOrder = int(poolStruct["order"]);
            config.addPool(item.first, poolDesc, poolUnits, poolScale, poolOrder, item.second.extract<double>());
         } else {
            auto v = parsePocoVarToDynamic(item.second);
            config.addPool(item.first, item.second.extract<double>());
         }
      }
   } else if (poolsItem.isArray()) {
      auto poolsArr = poolsItem.extract<Poco::JSON::Array::ValueVec>();
      for (auto item : poolsArr) {
         if (item.isStruct()) {
            auto poolStruct = item.extract<Poco::DynamicStruct>();
            if (poolStruct.size() == 1) {
               auto poolName = poolStruct.begin()->first;
               if (createSpecialPools(poolName, poolStruct, config)) {
                  continue;
               }

               auto poolValue = poolStruct.begin()->second.extract<double>();
               config.addPool(poolName, poolValue);
            } else {
               auto poolName = poolStruct["name"].extract<std::string>();
               if (createSpecialPools(poolName, poolStruct, config)) {
                  continue;
               }

               auto poolDesc = poolStruct["description"].extract<std::string>();
               auto poolUnits = poolStruct["units"].extract<std::string>();
               auto poolScale = poolStruct["scale"].extract<double>();
               auto poolOrder = int(poolStruct["order"]);
               auto poolValue = poolStruct["value"].extract<double>();
               config.addPool(poolName, poolDesc, poolUnits, poolScale, poolOrder, poolValue);
            }
         } else {
            // TODO: Check this, perhaps we just have a list of Pool names to set to 0?
            const auto poolName = item.extract<std::string>();
            config.addPool(poolName, 0.0);
         }
      }
   }
}

bool JSON2ConfigurationProvider::createSpecialPools(const std::string& poolName, const DynamicStruct& poolSettings,
                                                    Configuration& config) const {
   if (!poolSettings.contains("transform")) {
      return false;
   }

   auto poolDesc = poolSettings.contains("description") ? poolSettings["description"].extract<std::string>() : "";
   auto poolUnits = poolSettings.contains("units") ? poolSettings["units"].extract<std::string>() : "";
   auto poolScale = poolSettings.contains("scale") ? poolSettings["scale"].extract<double>() : 1.0;
   auto poolOrder = poolSettings.contains("order") ? int(poolSettings["order"]) : 0;

   auto transformSettings = parsePocoVarToDynamic(poolSettings["transform"]);
   auto settings = transformSettings.extract<const DynamicObject>();
   auto typeName = transformSettings["type"].extract<const std::string>();
   auto libraryName = transformSettings["library"].extract<const std::string>();

   config.addExternalPool(poolName, poolDesc, poolUnits, poolScale, poolOrder, libraryName, typeName, settings);

   return true;
}

bool JSON2ConfigurationProvider::createSpecialVariables(const std::pair<const std::string, DynamicVar>& keyValPair,
                                                        Configuration& config, bool isSpinup) const {
   auto spinup = config.spinup();
   auto varName = keyValPair.first;
   if (keyValPair.second.isStruct()) {
      auto ss = keyValPair.second.extract<Poco::DynamicStruct>();
      if (ss.size() == 1) {
         auto keyValPairSS = ss.begin();
         if (keyValPairSS->first == "flintdata") {
            auto v = parsePocoVarToDynamic(keyValPairSS->second);
            auto varStruct = v.extract<const DynamicObject>();
            auto typeName = varStruct["type"].extract<const std::string>();
            auto libraryName = varStruct["library"].extract<const std::string>();
            auto& settings =
                varStruct.contains("settings") ? varStruct["settings"].extract<const DynamicObject>() : DynamicObject();
            isSpinup ? spinup->addFlintDataVariable(varName, libraryName, typeName, settings)
                     : config.addFlintDataVariable(varName, libraryName, typeName, settings);
            return true;
         }
         if (keyValPairSS->first == "transform") {
            auto v = parsePocoVarToDynamic(keyValPairSS->second);
            auto varStruct = v.extract<const DynamicObject>();
            auto typeName = varStruct["type"].extract<const std::string>();
            auto libraryName = varStruct["library"].extract<const std::string>();
            isSpinup ? spinup->addExternalVariable(varName, libraryName, typeName, varStruct)
                     : config.addExternalVariable(varName, libraryName, typeName, varStruct);
            return true;
         }
      }
   }
   return false;
}

void JSON2ConfigurationProvider::createVariables(DynamicVar& parsedJSON, Configuration& config, bool isSpinup) const {
   auto spinup = config.spinup();
   const std::string configSection = isSpinup ? "SpinupVariables" : "Variables";

   Poco::DynamicStruct jsonStruct2 = *parsedJSON.extract<Poco::JSON::Object::Ptr>();
   auto& variablesItemVar = jsonStruct2[configSection];

   if (variablesItemVar.isStruct()) {
      const auto& variablesItem = variablesItemVar.extract<Poco::DynamicStruct>();
      int index = 0;
      for (const auto& keyValPair : variablesItem) {
         index++;
         const auto varName = keyValPair.first;
         if (createSpecialVariables(keyValPair, config, isSpinup)) continue;
         auto v = parsePocoVarToDynamic(keyValPair.second);
         isSpinup ? spinup->addVariable(varName, v) : config.addVariable(varName, v);
      }
   } else if (variablesItemVar.isArray()) {
      auto arr = variablesItemVar.extract<Poco::JSON::Array::ValueVec>();

      int index = 0;
      for (auto& value : arr) {
         index++;
         if (value.isStruct()) {
            const auto& s = value.extract<Poco::DynamicStruct>();
            if (value.size() == 1) {
               auto keyValPair = s.begin();
               auto varName = keyValPair->first;
               if (createSpecialVariables(*keyValPair, config, isSpinup)) continue;
               auto v = parsePocoVarToDynamic(keyValPair->second);
               isSpinup ? spinup->addVariable(keyValPair->first, v) : config.addVariable(keyValPair->first, v);
            } else {
               MOJA_LOG_ERROR << "Variable defined in array (index " << index
                              << ") has has struct with more than one item defined)";
            }
         } else {
            // TODO: do we set a default value or ignore this Variable? I'd say log error and move on
            auto varName = value.extract<const std::string>();
            MOJA_LOG_ERROR << "Variable defined in array (index " << index
                           << ") has no value (not Struct or Array): " << varName;
         }
      }
   }
}

void JSON2ConfigurationProvider::createUncertainty(DynamicVar& parsedJSON,
                                                                  Configuration& config) const {
   Poco::DynamicStruct jsonStruct = *parsedJSON.extract<Poco::JSON::Object::Ptr>();
   if (!jsonStruct.contains("Uncertainty")) {
      return;
   }
   auto uncertaintyStruct = jsonStruct["Uncertainty"].extract<Poco::DynamicStruct>();
   if (uncertaintyStruct.contains("enabled")) {
      bool enabled = uncertaintyStruct["enabled"].extract<bool>();
      if (!enabled) return;
   }
   auto& uncertainty = config.uncertainty();
   uncertainty.set_enabled(true);
   uncertainty.set_iterations(uncertaintyStruct["iterations"]);

   auto variables = uncertaintyStruct["variables"].extract<Poco::JSON::Array::ValueVec>();
   for (auto& variable_var : variables) {
      auto variable = variable_var["variable"].extract<const std::string>();
      auto& uncertainty_variable = uncertainty.variables().emplace_back(variable);

      auto replacements = variable_var["replacements"].extract<Poco::JSON::Array::ValueVec>();
      for (auto& replacement_var : replacements) {
         auto query = replacement_var["query"];
         auto query_doc = query.isEmpty() ? DynamicObject() : query.extract<const DynamicObject>();

         auto& replacement = uncertainty_variable.replacements().emplace_back(query_doc);

         auto replace_fields = replacement_var["replace"].extract<Poco::DynamicStruct>();
         for (auto& replace_field : replace_fields) {
            

            const auto& key = replace_field.first;
            const auto& field_var = replace_field.second;
            auto type = field_var["type"].extract<const std::string>();
            if (type == "triangular") {
               auto field = std::make_shared<UncertaintyFieldTriangular>();
               field->key = key;
               field->min = field_var["min"];
               field->max = field_var["max"];
               field->peak = field_var["peak"];
               field->seed = field_var["seed"];
               replacement.fields().emplace_back(field);
            } else if (type == "normal") {
               auto field = std::make_shared<UncertaintyFieldNormal>();
               field->key = key;
               field->mean = field_var["mean"];
               field->std_dev = field_var["std_dev"];
               field->seed = field_var["seed"];
               replacement.fields().emplace_back(field);
            } else if (type == "manual") {
               auto field = std::make_shared<UncertaintyFieldManual>();
               field->key = key;
               for (auto& val : field_var["distribution"]) {
                  field->distribution.emplace_back(val.extract<double>());
               }
               replacement.fields().emplace_back(field);
            }
         }
      }
   }
}

void JSON2ConfigurationProvider::createModules(DynamicVar& parsedJSON, Configuration& config) const {
   Poco::DynamicStruct jsonStruct = *parsedJSON.extract<Poco::JSON::Object::Ptr>();
   auto modulesStruct = jsonStruct["Modules"].extract<Poco::DynamicStruct>();
   for (auto item : modulesStruct) {
      auto moduleStruct = item.second.extract<Poco::DynamicStruct>();
      if (moduleStruct.contains("enabled")) {
         const bool enabled = moduleStruct["enabled"].extract<bool>();
         if (!enabled) continue;
      }
      if (!moduleStruct.contains("library")) {
         BOOST_THROW_EXCEPTION(ModuleParamsException() << Param("library"));
      }
      if (!moduleStruct.contains("order")) {
         BOOST_THROW_EXCEPTION(ModuleParamsException() << Param("order"));
      }

      const auto& moduleLibraryName = moduleStruct["library"].extract<std::string>();
      auto moduleOrder = int(moduleStruct["order"]);

      bool isProxy = false;
      if (moduleStruct.contains("is_proxy")) {
         isProxy = moduleStruct["is_proxy"].extract<bool>();
      }

      const auto& d = parsePocoVarToDynamic(moduleStruct["settings"]);
      auto& settings = d.isEmpty() ? DynamicObject() : d.extract<const DynamicObject>();
      config.addModule(moduleLibraryName, item.first, moduleOrder, isProxy, settings);
   }
}

void JSON2ConfigurationProvider::createSpinupModules(DynamicVar& parsedJSON, Configuration& config) const {
   Poco::DynamicStruct jsonStruct = *parsedJSON.extract<Poco::JSON::Object::Ptr>();
   auto modulesStruct = jsonStruct["SpinupModules"].extract<Poco::DynamicStruct>();
   for (auto item : modulesStruct) {
      auto moduleStruct = item.second.extract<Poco::DynamicStruct>();

      if (moduleStruct.contains("enabled")) {
         bool enabled = moduleStruct["enabled"].extract<bool>();
         if (!enabled) continue;
      }

      if (!moduleStruct.contains("library")) {
         BOOST_THROW_EXCEPTION(ModuleParamsException() << Param("library"));
      }
      if (!moduleStruct.contains("order")) {
         BOOST_THROW_EXCEPTION(ModuleParamsException() << Param("order"));
      }

      const auto moduleLibraryName = moduleStruct["library"].extract<std::string>();
      auto moduleOrder = int(moduleStruct["order"]);
      auto moduleCreateNew = moduleStruct.contains("create_new") ? moduleStruct["create_new"].extract<bool>() : false;

      const auto& d = parsePocoVarToDynamic(moduleStruct["settings"]);
      auto& settings = d.isEmpty() ? DynamicObject() : d.extract<const DynamicObject>();
      auto spinup = config.spinup();
      spinup->addSpinupModule(moduleLibraryName, item.first, moduleOrder, moduleCreateNew, settings);
   }
}

LocalDomainType JSON2ConfigurationProvider::parseLocalDomainType(const std::string& type) {
   if (type == "spatial_tiled") {
      return LocalDomainType::SpatialTiled;
   } else if (type == "spatially_referenced_sql") {
      return LocalDomainType::SpatiallyReferencedSQL;
   } else if (type == "spatially_referenced_nosql") {
      return LocalDomainType::SpatiallyReferencedNoSQL;
   } else if (type == "threaded_spatially_referenced_nosql") {
      return LocalDomainType::ThreadedSpatiallyReferencedNoSQL;
   } else if (type == "point") {
      return LocalDomainType::Point;
   }
   throw std::runtime_error("Invalid LocalDomain Type specified");
}

LibraryType JSON2ConfigurationProvider::parseLibraryType(const std::string& type) {
   if (type == "internal") {
      return LibraryType::Internal;
   } else if (type == "external") {
      return LibraryType::External;
   } else if (type == "managed") {
      return LibraryType::Managed;
   } else if (type == "python") {
      return LibraryType::Python;
   }

   throw std::runtime_error("TODO: specific exception here");
}

}  // namespace configuration
}  // namespace flint
}  // namespace moja
