#include "moja/flint/librarymanager.h"

#include "moja/flint/libraryfactory.h"
#include "moja/flint/libraryinfoexternal.h"
#include "moja/flint/libraryinfointernal.h"
#include "moja/flint/libraryinfomanaged.h"
#include "moja/flint/modulebase.h"

#include <moja/environment.h>

#include <moja/logging.h>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#if defined(ENABLE_MOJAPY)
#include "moja/flint/libraryinfopython.h"

#include <moja/py/pymodule.h>

#include <boost/python.hpp>
#endif

#include <Poco/DirectoryIterator.h>
#include <Poco/SharedLibrary.h>

#include <boost/algorithm/string.hpp>

#include <memory>

namespace moja {
namespace flint {

// --------------------------------------------------------------------------------------------
#if defined(_WIN32)
const char* libPrefix = "";
#elif defined(MOJA_OS_FAMILY_UNIX)
const char* libPrefix = "lib";
#endif

// --------------------------------------------------------------------------------------------

Int32 LibraryManager::currentLoadOrder = 1;
LibraryManager::LibraryManager() {
   // Load all internal
   MOJA_LOG_DEBUG << (boost::format("LibraryManager: %s") % "contructor no args" ).str();

   const auto internalHandles = std::make_shared<FlintLibraryHandles>(
       "internal.flint", getFlintModuleRegistrations, getFlintTransformRegistrations, getFlintFlintDataRegistrations,
       getFlintFlintDataFactoryRegistrations, getProviderRegistrations); //getDataRepositoryProviderRegistrations);

   LoadInternalLibrary(internalHandles);
}

LibraryManager::LibraryManager(std::shared_ptr<FlintLibraryHandles> libraryHandles) {
   // Load Flint internal modules
   MOJA_LOG_DEBUG << (boost::format("LibraryManager: %s") % "contructor no args").str();

   const auto internalHandles = std::make_shared<FlintLibraryHandles>(
       "internal.flint", getFlintModuleRegistrations, getFlintTransformRegistrations, getFlintFlintDataRegistrations,
       getFlintFlintDataFactoryRegistrations, getProviderRegistrations);

   LoadInternalLibrary(internalHandles);

   if (libraryHandles == nullptr) return;

   LoadInternalLibrary(libraryHandles);
}

LibraryManager::~LibraryManager() {
   // NOTE: It may not be safe to unload libraries by this point (static
   //	     deinitialization), as other libraries may have already been
   //		 unloaded, which means we can't safely call clean up methods.
}

void LibraryManager::AddLibrary(LibraryType libraryType, const std::string& inLibraryName, const std::string& path,
                                const std::string& fileName) {
   MOJA_LOG_DEBUG << (boost::format("AddLibrary: %s : %s") % "entered" % inLibraryName).str();

   // Do we already know about this library?
   if (_libraries.find(inLibraryName) != _libraries.end()) {
      MOJA_LOG_DEBUG << (boost::format("AddLibrary: %s : %s") % "we know this library" % inLibraryName).str();
      return;
   }

   // If not, we'll create information for this library now.
   std::shared_ptr<LibraryInfoBase> libraryInfo;
   switch (libraryType) {
      case LibraryType::External: {
         if (path.empty()) {
            auto LibraryPathMap = FindLibraryPaths(inLibraryName);
             if (LibraryPathMap.size() == 1) {
               // Add this library to the set of libraries that we know about
               auto library = std::make_shared<LibraryInfoExternal>();
               library->originalFilename = (*std::begin(LibraryPathMap)).second;
               library->filename = library->originalFilename;
               library->libraryHandles->libraryName = inLibraryName;
               libraryInfo = library;
            }
         } else {
	    // We can also allow expansion of Environment variables here to help library finding
            auto LibraryPathMap = FindLibraryPathsInDirectory(Poco::Path::expand(path), fileName, false);
            if (LibraryPathMap.size() == 1) {
               // Add this library to the set of libraries that we know about
               auto library = std::make_shared<LibraryInfoExternal>();
               library->originalFilename = (*std::begin(LibraryPathMap)).second;
               library->filename = library->originalFilename;
               library->libraryHandles->libraryName = inLibraryName;
               libraryInfo = library;
            }
         }
         break;
      }
#if defined(ENABLE_MOJAPY)
      case LibraryType::Python: {
         auto LibraryPathMap = FindLibraryPathsExact(inLibraryName + ".py");
         if (LibraryPathMap.size() == 1) {
            // Add this library to the set of libraries that we know about
            auto library = std::make_shared<LibraryInfoPython>();
            library->originalFilename = (*std::begin(LibraryPathMap)).second;
            library->filename = library->originalFilename;
            library->libraryHandles->libraryName = inLibraryName;
            libraryInfo = library;
         }
         break;
      }
#endif
      case LibraryType::Managed: {
         auto LibraryPathMap = FindLibraryPaths(inLibraryName);
         if (LibraryPathMap.size() == 1) {
            // Add this library to the set of libraries that we know about
            auto library = std::make_shared<LibraryInfoManaged>();
            library->originalFilename = (*std::begin(LibraryPathMap)).second;
            library->filename = library->originalFilename;
            library->libraryHandles->libraryName = inLibraryName;
            libraryInfo = library;
         }
         break;
      }
      case LibraryType::Internal: {
         // Add this library to the set of libraries that we know about
         auto library = std::make_shared<LibraryInfoInternal>();
         library->libraryHandles->libraryName = inLibraryName;
         libraryInfo = library;
         break;
      }
      case LibraryType::Unknown:
      default: {
         throw std::runtime_error("Unknown library type defined " + inLibraryName);
      }
   }

   // Update hash table
   _libraries[inLibraryName] = libraryInfo;
}

std::map<std::string, std::string> LibraryManager::FindLibraryPaths(const std::string& namePattern) const {
   return FindLibraryPathsInDirectory(moja::Environment::startProcessFolder(), namePattern, true);
}

std::map<std::string, std::string> LibraryManager::FindLibraryPathsExact(const std::string& namePattern) const {
   return FindLibraryPathsInDirectory(moja::Environment::startProcessFolder(), namePattern, false);
}

std::map<std::string, std::string> LibraryManager::FindLibraryPathsInDirectory(const std::string& directoryName,
                                                                               const std::string& NamePattern,
                                                                               bool useSuffix) const {
   std::map<std::string, std::string> outLibraryPaths;
   Poco::DirectoryIterator dirIterator(directoryName);
   Poco::DirectoryIterator end;
   std::string prefix = libPrefix;
   auto suffix = useSuffix ? Poco::SharedLibrary::suffix() : "";

   // Parse all the matching library names
   while (dirIterator != end) {
      auto libraryPath = Poco::Path(dirIterator->path());
      auto file = libraryPath.getFileName();

      bool prefixMatches = boost::iequals(file.substr(0, prefix.size()), prefix);
      bool suffixMatches =
          file.size() > suffix.size() && boost::iequals(file.substr(file.size() - suffix.size()), suffix);

      if (prefixMatches && suffixMatches) {
         auto it = file.begin() + prefix.size();
         auto fileEnd = file.end() - suffix.size();
         std::string libraryName;
         while (it != fileEnd) libraryName += *it++;

         if (NamePattern.size() == 0 || NamePattern == libraryName) {
            outLibraryPaths[libraryName] = libraryPath.toString();
         }
      }
      ++dirIterator;
   }

   return outLibraryPaths;
}

ModuleInterfacePtr LibraryManager::GetModule(const std::string& libraryName, const std::string& moduleName) {
   try {
      // Do we even know about this module?
      auto libraryInfo = _modules.at(std::make_pair(libraryName, moduleName));
      auto moduleKey = ModuleKey(libraryName, moduleName);
      auto moduleInit = _moduleRegistry.at(moduleKey);
      auto moduleInterface = moduleInit();
      if (!moduleInterface) {
         throw std::runtime_error("Failed to initialise the module " + moduleName);
      }

      libraryInfo->moduleList.push_back(moduleInterface);
      ModuleMetaData metaData;
      metaData.libraryType = int(libraryInfo->GetLibraryType());
      metaData.libraryInfoId = libraryInfo->loadOrder;
      metaData.moduleType = static_cast<int>(moduleInterface->moduleType());
      metaData.moduleId = libraryInfo->moduleCount++;
      metaData.moduleName = moduleName;
      moduleInterface->StartupModule(metaData);
      return moduleInterface;
   }
#if defined(ENABLE_MOJAPY)
   catch (boost::python::error_already_set) {
      PyErr_Print();
   }
#endif
   catch (...) {
      throw std::runtime_error("Module doesn't exist " + moduleName);
   }
}

TransformInterfacePtr LibraryManager::GetTransform(const std::string& libraryName, const std::string& transformName) {
   // Do we even know about this module?
   try {
      auto libraryInfo = _transforms.at(std::make_pair(libraryName, transformName));
      auto transformKey = TransformKey(libraryName, transformName);
      auto transformInit = _transformRegistry.at(transformKey);

      std::shared_ptr<ITransform> transformInterface = transformInit();
      if (!transformInterface) {
         throw std::runtime_error("Failed to initialise the transform " + transformName);
      }

      libraryInfo->transformList.push_back(transformInterface);
      return transformInterface;
   } catch (...) {
      throw std::runtime_error("Transform doesn't exist " + transformName);
   }
}

FlintDataInterfacePtr LibraryManager::GetFlintData(const std::string& libraryName, const std::string& flintDataName) {
   // Do we even know about this module?
   try {
      auto key = std::make_pair(libraryName, flintDataName);
      auto libraryInfo = _flintData.at(key);
      auto flintDataKey = FlintDataKey(libraryName, flintDataName);
      auto flintDataInit = _flintDataRegistry.at(flintDataKey);

      std::shared_ptr<IFlintData> flintDataInterface = flintDataInit();
      if (!flintDataInterface) {
         throw std::runtime_error("Failed to initialise the flintData " + flintDataName);
      }
      flintDataInterface->libraryName = libraryName;
      flintDataInterface->typeName = flintDataName;
      libraryInfo->flintDataList.push_back(flintDataInterface);

      // Check if we have any factories regsitered
      auto range = _flintDataFactoryRegistry.equal_range(key);
      for (auto it = range.first; it != range.second; ++it) {
         flintDataInterface->registerEventFactoryFunc(it->second);
      }
      return flintDataInterface;
   } catch (...) {
      throw std::runtime_error("flintData doesn't exist " + flintDataName);
   }
}

ModuleInterfacePtr LibraryManager::GetManagedModule(const std::string& managedModuleName) {
   // Do we even know about the managed module proxy?
   const std::string libraryName("moja.Modules.ManagedProxy");
   const auto libraryInfoIt = _libraries.find(libraryName);
   if (libraryInfoIt == std::end(_libraries)) {
      return nullptr;
   }

   auto moduleKey = ModuleKey(libraryName, managedModuleName);
   const auto moduleInit = _moduleRegistry[moduleKey];
   if (moduleInit == nullptr) {
      throw std::runtime_error("Failed to initialise the managed module " + managedModuleName);
   }

   std::shared_ptr<LibraryInfoBase> libraryInfo = (*libraryInfoIt).second;
   if (libraryInfo->GetLibraryType() != LibraryType::Managed) {
      throw std::runtime_error("Incorrect type for managed module " + managedModuleName);
   }

   std::shared_ptr<IModule> moduleInterface;
   std::map<std::string, std::string> modulePathMap = FindLibraryPathsExact(managedModuleName);
   if (modulePathMap.size() != 1) {
      throw std::runtime_error("Managed module path issue" + managedModuleName);
   }

   try {
      moduleInterface = moduleInit();
   } catch (...) {
      throw std::runtime_error("Managed module issue " + managedModuleName);
   }

   if (!moduleInterface) {
      throw std::runtime_error("Failed to initialise the module " + managedModuleName);
   }

   libraryInfo->moduleList.push_back(moduleInterface);
   ModuleMetaData metaData;
   metaData.libraryType = int(libraryInfo->GetLibraryType());
   metaData.libraryInfoId = libraryInfo->loadOrder;
   metaData.moduleType = static_cast<int>(moduleInterface->moduleType());
   metaData.moduleId = libraryInfo->moduleCount++;
   metaData.moduleName = libraryName;
   moduleInterface->StartupModule(metaData);

   return moduleInterface;
}

ProviderInterfacePtr LibraryManager::GetProvider(const std::string& libraryName, const std::string& providerName,
                                                 const DynamicObject& settings) {
   try {
      // Do we even know about this provider?
      auto libraryInfo = _providers.at(std::make_pair(libraryName, providerName));
      const auto providerKey = ProviderKey(libraryName, providerName);
      auto providerInit = _providerRegistry.at(providerKey);
      auto providerInterface = providerInit(settings);
      if (!providerInterface) {
         throw std::runtime_error("Failed to initialise the provider " + providerName);
      }
      libraryInfo->providerList.push_back(providerInterface);
      return providerInterface;
   } catch (...) {
      throw std::runtime_error("Provider doesn't exist " + providerName);
   }
}

void LibraryManager::RegisterModules(std::shared_ptr<LibraryInfoBase> libraryInfo, const std::string& libraryName) {
   ModuleRegistration registrations[100];

   if (libraryInfo->libraryHandles->getModuleRegistrations == nullptr) return;

   auto moduleCount = libraryInfo->libraryHandles->getModuleRegistrations(registrations);
   for (auto i = 0; i < moduleCount; i++) {
      auto registration = registrations[i];
      _modules[std::make_pair(libraryName, registration.moduleName)] = libraryInfo;
      auto fp = [registration]() { return ModuleInterfacePtr(registration.initializer()); };
      _moduleRegistry.insert(std::make_pair(std::make_pair(libraryName, registration.moduleName), fp));
   }
}

void LibraryManager::RegisterTransforms(std::shared_ptr<LibraryInfoBase> libraryInfo, const std::string& libraryName) {
   if (libraryInfo->libraryHandles->getTransformRegistrations == nullptr) return;

   TransformRegistration registrations[100];
   const auto transformCount = libraryInfo->libraryHandles->getTransformRegistrations(registrations);
   for (auto i = 0; i < transformCount; i++) {
      auto registration = registrations[i];
      _transforms[std::make_pair(libraryName, registration.transformName)] = libraryInfo;
      auto fp = [registration]() { return TransformInterfacePtr(registration.initializer()); };
      _transformRegistry.insert(std::make_pair(std::make_pair(libraryName, registration.transformName), fp));
   }
}

void LibraryManager::RegisterFlintData(std::shared_ptr<LibraryInfoBase> libraryInfo, const std::string& libraryName) {
   if (libraryInfo->libraryHandles->getFlintDataRegistrations == nullptr) return;

   FlintDataRegistration registrations[100];
   const auto flintDataCount = libraryInfo->libraryHandles->getFlintDataRegistrations(registrations);
   for (auto i = 0; i < flintDataCount; i++) {
      auto registration = registrations[i];
      _flintData[std::make_pair(libraryName, registration.flintDataName)] = libraryInfo;
      auto fp = [registration]() { return FlintDataInterfacePtr(registration.initializer()); };
      _flintDataRegistry.insert(std::make_pair(std::make_pair(libraryName, registration.flintDataName), fp));
   }
}

void LibraryManager::RegisterFlintDataFactory(std::shared_ptr<LibraryInfoBase> libraryInfo,
                                              const std::string& libraryName) {
   if (libraryInfo->libraryHandles->getFlintDataFactoryRegistrations == nullptr) return;

   FlintDataFactoryRegistration registrations[100];
   const auto flintDataFactoryCount = libraryInfo->libraryHandles->getFlintDataFactoryRegistrations(registrations);

   for (auto i = 0; i < flintDataFactoryCount; i++) {
      auto registration = registrations[i];
      _flintDataFactories[std::make_tuple(libraryName, registration.libraryName, registration.flintDataName)] =
          libraryInfo;

      auto fp = [registration](const std::string& a, int b, const std::string& c, const DynamicObject& d) {
         return FlintDataFactoryInterfacePtr(registration.initializer(a, b, c, d));
      };
      //_flintDataFactoryRegistry.insert(std::make_pair(std::make_pair(libraryName, registration.flintDataName), fp));
      _flintDataFactoryRegistry.insert(
          std::make_pair(std::make_pair(registration.libraryName, registration.flintDataName), fp));
   }
}

void LibraryManager::RegisterProviders(std::shared_ptr<LibraryInfoBase> libraryInfo, const std::string& libraryName) {
   if (libraryInfo->libraryHandles->getDataRepositoryProviderRegistrations == nullptr) return;

   DataRepositoryProviderRegistration registrations[100];

   auto count = -1;
   count = libraryInfo->libraryHandles->getDataRepositoryProviderRegistrations(registrations);
   MOJA_LOG_DEBUG << (boost::format("RegisterProviders: %s : %s : count %d") % "entered" % libraryName % count).str();

   for (auto i = 0; i < count; i++) {

      MOJA_LOG_DEBUG << (boost::format("RegisterProviders: %s: %d") % "loop" % i).str();

      auto registration = registrations[i];
      _providers[std::make_pair(libraryName, registration.providerName)] = libraryInfo;

      MOJA_LOG_DEBUG << (boost::format("RegisterProviders: %s: %s : %s") % "loop" % libraryName % registration.providerName).str();

      auto fp = [registration](const DynamicObject& settings) {
         return ProviderInterfacePtr(registration.initializer(settings));
      };
      _providerRegistry.insert(std::make_pair(std::make_pair(libraryName, registration.providerName), fp));
   }
}

bool LibraryManager::LoadInternalLibrary(std::shared_ptr<FlintLibraryHandles> libraryHandles) {

   MOJA_LOG_DEBUG << (boost::format("LibraryMLoadInternalLibraryanager: %s") % "entered").str();

   if (libraryHandles == nullptr) return true;

   MOJA_LOG_DEBUG << (boost::format("LibraryMLoadInternalLibraryanager: %s : %s") % "calling AddLibrary" % libraryHandles->libraryName).str();

   // Update our set of known libraries, in case we don't already know about this library
   AddLibrary(LibraryType::Internal, libraryHandles->libraryName);
   auto libraryInfo = _libraries[libraryHandles->libraryName];
   if (libraryInfo == nullptr) {
      throw std::runtime_error("Library not found " + libraryHandles->libraryName);
   }

   if (libraryInfo->libraryHandles->getModuleRegistrations == nullptr) {
      MOJA_LOG_DEBUG << (boost::format("LibraryMLoadInternalLibraryanager: %s") % "calling registrations").str();

      if (libraryInfo->GetLibraryType() != LibraryType::Internal) {
         throw std::runtime_error("Attempt to load library already loaded as different type" +
                                  libraryHandles->libraryName);
      }

      auto internalLibraryInfo = std::static_pointer_cast<LibraryInfoInternal>(libraryInfo);
      internalLibraryInfo->libraryHandles = libraryHandles;

      RegisterModules(internalLibraryInfo,            libraryHandles->libraryName);
      RegisterTransforms(internalLibraryInfo,         libraryHandles->libraryName);
      RegisterFlintData(internalLibraryInfo,          libraryHandles->libraryName);
      RegisterFlintDataFactory(internalLibraryInfo,   libraryHandles->libraryName);
      RegisterProviders(internalLibraryInfo,          libraryHandles->libraryName);
      RegisterProviders(internalLibraryInfo,          libraryHandles->libraryName);
   }
   return true;
}

bool LibraryManager::LoadManagedLibrary() {
   // Update our set of known libraries, in case we don't already know about this library
   const std::string libraryName("moja.Modules.ManagedProxy");
   AddLibrary(LibraryType::Managed, libraryName);

   // Grab the library info.  This has the file name of the library, as well as other info.
   auto libraryInfo = _libraries[libraryName];
   if (libraryInfo == nullptr) {
      throw std::runtime_error("Library not found " + libraryName);
   }

   if (libraryInfo->libraryHandles->getModuleRegistrations == nullptr) {
      if (libraryInfo->GetLibraryType() != LibraryType::Managed) {
         throw std::runtime_error("Attempt to load library already loaded as different type " + libraryName);
      }

      auto managedLibraryInfo = std::static_pointer_cast<LibraryInfoManaged>(_libraries[libraryName]);

      // Determine which file to load for this module.
      auto libraryFileToLoad = Poco::File(managedLibraryInfo->filename);

      // Clear the handle and set it again below if the library is successfully loaded
      managedLibraryInfo->handle = nullptr;

      // Skip this check if file manager has not yet been initialized
      if (!libraryFileToLoad.exists()) {
         throw std::runtime_error("Library not found " + managedLibraryInfo->filename);
      }

      if (!CheckModuleCompatibility(libraryFileToLoad.path())) {
         throw std::runtime_error("Library not compatible " + libraryFileToLoad.path());
      }

      managedLibraryInfo->handle = std::make_unique<Poco::SharedLibrary>(libraryFileToLoad.path());

      managedLibraryInfo->libraryHandles->getModuleRegistrations =
          managedLibraryInfo->handle->hasSymbol("getModuleRegistrations")
              ? reinterpret_cast<GetModuleRegistrationsFunctionPtr>(
                    managedLibraryInfo->handle->getSymbol("getModuleRegistrations"))
              : nullptr;
      managedLibraryInfo->libraryHandles->getTransformRegistrations =
          managedLibraryInfo->handle->hasSymbol("getTransformRegistrations")
              ? reinterpret_cast<GetTransformRegistrationsFunctionPtr>(
                    managedLibraryInfo->handle->getSymbol("getTransformRegistrations"))
              : nullptr;
      managedLibraryInfo->libraryHandles->getFlintDataRegistrations =
          managedLibraryInfo->handle->hasSymbol("getFlintDataRegistrations")
              ? reinterpret_cast<GetFlintDataRegistrationsFunctionPtr>(
                    managedLibraryInfo->handle->getSymbol("getFlintDataRegistrations"))
              : nullptr;
      managedLibraryInfo->libraryHandles->getFlintDataFactoryRegistrations =
          managedLibraryInfo->handle->hasSymbol("getFlintDataFactoryRegistrations")
              ? reinterpret_cast<GetFlintDataFactoryRegistrationsFunctionPtr>(
                    managedLibraryInfo->handle->getSymbol("getFlintDataFactoryRegistrations"))
              : nullptr;
      managedLibraryInfo->libraryHandles->getDataRepositoryProviderRegistrations =
          managedLibraryInfo->handle->hasSymbol("getDataRepositoryProviderRegistrations")
              ? reinterpret_cast<GetDataRepositoryProviderRegistrationsFunctionPtr>(
                    managedLibraryInfo->handle->getSymbol("getDataRepositoryProviderRegistrations"))
              : nullptr;

      RegisterModules(managedLibraryInfo, libraryName);
      RegisterTransforms(managedLibraryInfo, libraryName);
      RegisterFlintData(managedLibraryInfo, libraryName);
      RegisterFlintDataFactory(managedLibraryInfo, libraryName);
      RegisterProviders(managedLibraryInfo, libraryName);
   }

   return true;
}

bool LibraryManager::LoadExternalLibrary(const std::string& libraryName, const std::string& path,
                                         const std::string& fileName) {
   // Update our set of known libraries, in case we don't already know about this library
   AddLibrary(LibraryType::External, libraryName, path, fileName);

   // Grab the library info.  This has the file name of the library, as well as other info.
   auto libraryInfo = _libraries[libraryName];
   if (libraryInfo == nullptr) {
      throw std::runtime_error("Library not found " + libraryName);
   }

   if (libraryInfo->libraryHandles->getModuleRegistrations == nullptr) {
      if (libraryInfo->GetLibraryType() != LibraryType::External) {
         throw std::runtime_error("Attempt to load library already loaded as different type " + libraryName);
      }

      auto externalLibraryInfo = std::static_pointer_cast<LibraryInfoExternal>(_libraries[libraryName]);

      // Determine which file to load for this module.
      auto libraryFileToLoad = Poco::File(externalLibraryInfo->filename);

      // Clear the handle and set it again below if the module is successfully loaded
      externalLibraryInfo->handle = nullptr;

      if (!libraryFileToLoad.exists()) {
         throw std::runtime_error("Library not found " + externalLibraryInfo->filename);
      }

      if (!CheckModuleCompatibility(libraryFileToLoad.path())) {
         throw std::runtime_error("Library not compatible " + libraryFileToLoad.path());
      }

      externalLibraryInfo->handle = std::make_unique<Poco::SharedLibrary>(libraryFileToLoad.path());
      externalLibraryInfo->libraryHandles->getModuleRegistrations =
          externalLibraryInfo->handle->hasSymbol("getModuleRegistrations")
              ? reinterpret_cast<GetModuleRegistrationsFunctionPtr>(
                    externalLibraryInfo->handle->getSymbol("getModuleRegistrations"))
              : nullptr;
      externalLibraryInfo->libraryHandles->getTransformRegistrations =
          externalLibraryInfo->handle->hasSymbol("getTransformRegistrations")
              ? reinterpret_cast<GetTransformRegistrationsFunctionPtr>(
                    externalLibraryInfo->handle->getSymbol("getTransformRegistrations"))
              : nullptr;
      externalLibraryInfo->libraryHandles->getFlintDataRegistrations =
          externalLibraryInfo->handle->hasSymbol("getFlintDataRegistrations")
              ? reinterpret_cast<GetFlintDataRegistrationsFunctionPtr>(
                    externalLibraryInfo->handle->getSymbol("getFlintDataRegistrations"))
              : nullptr;
      externalLibraryInfo->libraryHandles->getFlintDataFactoryRegistrations =
          externalLibraryInfo->handle->hasSymbol("getFlintDataFactoryRegistrations")
              ? reinterpret_cast<GetFlintDataFactoryRegistrationsFunctionPtr>(
                    externalLibraryInfo->handle->getSymbol("getFlintDataFactoryRegistrations"))
              : nullptr;
      externalLibraryInfo->libraryHandles->getDataRepositoryProviderRegistrations =
          externalLibraryInfo->handle->hasSymbol("getDataRepositoryProviderRegistrations")
              ? reinterpret_cast<GetDataRepositoryProviderRegistrationsFunctionPtr>(
                    externalLibraryInfo->handle->getSymbol("getDataRepositoryProviderRegistrations"))
              : nullptr;

      RegisterModules(externalLibraryInfo, libraryName);
      RegisterTransforms(externalLibraryInfo, libraryName);
      RegisterFlintData(externalLibraryInfo, libraryName);
      RegisterFlintDataFactory(externalLibraryInfo, libraryName);
      RegisterProviders(externalLibraryInfo, libraryName);
   }
   return true;
}

bool LibraryManager::CheckModuleCompatibility(const std::string& /*filename*/) {
   // TODO: Check that the module API version matches MOJA_API_VERSION.
   return true;
}

#if defined(ENABLE_MOJAPY)
bool LibraryManager::LoadPythonLibrary(const std::string& libraryName) {
   using namespace boost::python;
   Py_Initialize();

   // Update our set of known libraries, in case we don't already know about this library
   AddLibrary(LibraryType::Python, libraryName);

   // Grab the library info.  This has the file name of the library, as well as other info.
   auto libraryInfo = _libraries[libraryName];
   if (libraryInfo == nullptr) {
      throw LibraryLoadException("Library not found", libraryName);
   }

   auto pyLibraryInfo = std::static_pointer_cast<LibraryInfoPython>(_libraries[libraryName]);
   if (pyLibraryInfo->moduleHandles.empty()) {
      if (libraryInfo->GetLibraryType() != LibraryType::Python) {
         throw LibraryLoadException("Attempt to load library already loaded as different type", libraryName);
      }

      // Determine which file to load for this module.
      auto libraryFileToLoad = Poco::File(pyLibraryInfo->filename);
      if (!libraryFileToLoad.exists()) {
         throw LibraryLoadException("Library not found", pyLibraryInfo->filename);
      }

      auto lib = import(libraryName.c_str());
      dict modules = extract<dict>(lib.attr("modules"));
      list keys = extract<list>(modules.keys());
      for (int i = 0; i < len(keys); i++) {
         std::string moduleName = extract<std::string>(keys[i]);
         object pyModule = modules[moduleName];
         _modules[std::make_pair(libraryName, moduleName)] = libraryInfo;
         _moduleRegistry.insert(std::make_pair(std::make_pair(libraryName, moduleName), [pyLibraryInfo, pyModule]() {
            auto pyModuleInstance = std::make_shared<object>(pyModule());
            pyLibraryInfo->moduleHandles.push_back(pyModuleInstance);
            Py::PyModule* pyModulePtr = extract<Py::PyModule*>(*pyModuleInstance);
            return ModuleInterfacePtr(pyModulePtr);
         }));
      }
   }

   return true;
}
#endif

}  // namespace flint
}  // namespace moja
