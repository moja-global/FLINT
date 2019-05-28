#ifndef MOJA_FLINT_MODULEMANAGER_H_
#define MOJA_FLINT_MODULEMANAGER_H_

#include "moja/flint/ilandunitcontroller.h"
#include "moja/flint/imodule.h"
#include "moja/flint/libraryinfobase.h"
#include "moja/flint/librarytype.h"
#include "moja/flint/modulebase.h"
#include "moja/flint/mojalibapi.h"

#include <map>
#include <string>

namespace moja {
namespace flint {

class FLINT_API LibraryManager {
  public:
   LibraryManager();
   explicit LibraryManager(std::shared_ptr<FlintLibraryHandles> libraryHandles);

   ~LibraryManager();

   /** Static that tracks the current load number. Incremented whenever we add a new library. */
   static Int32 currentLoadOrder;

   void AddLibrary(LibraryType type, const std::string& libraryName, const std::string& path = std::string(),
                   const std::string& fileName = std::string());

   ModuleInterfacePtr GetModule(const std::string& libraryName, const std::string& moduleName);
   TransformInterfacePtr GetTransform(const std::string& libraryName, const std::string& transformName);
   FlintDataInterfacePtr GetFlintData(const std::string& libraryName, const std::string& flintDataName);
   ModuleInterfacePtr GetManagedModule(const std::string& managedModuleName);
   ProviderInterfacePtr GetProvider(const std::string& libraryName, const std::string& moduleName,
                                    const DynamicObject& settings);

   bool LoadInternalLibrary(std::shared_ptr<FlintLibraryHandles> libraryHandles);

   bool LoadManagedLibrary();

   bool LoadExternalLibrary(const std::string& libraryName, const std::string& path = std::string(),
                            const std::string& fileName = std::string());

   bool UnloadModule(const std::string& /*libraryName*/, bool /*isShutdown*/) { return true; }

   const ProviderRegistry& getProviderRegistry() { return _providerRegistry; }

#if defined(ENABLE_MOJAPY)
   bool LoadPythonLibrary(const std::string& libraryName);
#endif

  private:
   /**
    * Compares file versions between the current executing Flint version
    * and the specified library.
    */
   static bool CheckModuleCompatibility(const std::string& filename);

   void RegisterModules(std::shared_ptr<LibraryInfoBase> libraryInfo, const std::string& libraryName);
   void RegisterTransforms(std::shared_ptr<LibraryInfoBase> libraryInfo, const std::string& libraryName);
   void RegisterFlintData(std::shared_ptr<LibraryInfoBase> libraryInfo, const std::string& libraryName);
   void RegisterFlintDataFactory(std::shared_ptr<LibraryInfoBase> libraryInfo, const std::string& libraryName);
   void RegisterProviders(std::shared_ptr<LibraryInfoBase> libraryInfo, const std::string& libraryName);

   /** Finds modules matching a given name wildcard. */
   std::map<std::string, std::string> FindLibraryPaths(const std::string& namePattern) const;
   std::map<std::string, std::string> FindLibraryPathsExact(const std::string& namePattern) const;

   std::map<std::string, std::string> FindLibraryPathsInDirectory(const std::string& inDirectoryName,
                                                                  const std::string& namePattern, bool useSuffix) const;

   /**
    * Map of all libraries. Maps the case-insensitive library name to
    * information about that library, loaded or not.
    */
   typedef std::pair<std::string, std::string> LibraryMapKey;
   typedef std::map<LibraryMapKey, std::shared_ptr<LibraryInfoBase>> LibraryMap;

   typedef std::tuple<std::string, std::string, std::string> LibraryMapKey2;
   typedef std::map<LibraryMapKey2, std::shared_ptr<LibraryInfoBase>> LibraryMap2;

   std::map<std::string, std::shared_ptr<LibraryInfoBase>> _libraries;
   LibraryMap _modules;
   LibraryMap _transforms;
   LibraryMap _flintData;
   LibraryMap _providers;
   LibraryMap2 _flintDataFactories;

   ModuleRegistry _moduleRegistry;
   TransformRegistry _transformRegistry;
   FlintDataRegistry _flintDataRegistry;
   FlintDataFactoryRegistry _flintDataFactoryRegistry;
   ProviderRegistry _providerRegistry;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_MODULEMANAGER_H_
