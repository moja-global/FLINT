#pragma once

#include "moja/flint/iflintdata.h"
#include "moja/flint/imodule.h"
#include "moja/flint/itransform.h"

#include <moja/datarepository/iproviderinterface.h>

#include <functional>
#include <map>
#include <string>

#if defined(_WIN32)
#define MOJA_LIB_API __declspec(dllexport)
#endif

#if !defined(MOJA_LIB_API)
#if defined(__GNUC__) && (__GNUC__ >= 4)
#define MOJA_LIB_API __attribute__((visibility("default")))
#else
#define MOJA_LIB_API
#endif
#endif

namespace moja::flint {

/**
 * A module package advertises which modules are available to moja using
 * ModuleRegistrations, which consist of the module name and a pointer
 * to a no-arg initialization function. Usually this is declared
 * automatically using the IMPLEMENT_MODULE macro below.
 */
typedef IModule* (*InitializeModuleFunctionPtr)();
struct ModuleRegistration {
   const char* moduleName;
   InitializeModuleFunctionPtr initializer;
};

typedef ITransform* (*InitializeTransformFunctionPtr)();
struct TransformRegistration {
   const char* transformName;
   InitializeTransformFunctionPtr initializer;
};

typedef IFlintData* (*InitializeFlintDataFunctionPtr)();
struct FlintDataRegistration {
   const char* flintDataName;
   InitializeFlintDataFunctionPtr initializer;
};

typedef std::shared_ptr<IFlintData> (*InitializeFlintDataFactoryFunctionPtr)(const std::string&, int,
                                                                             const std::string&, const DynamicObject&);
struct FlintDataFactoryRegistration {
   const char* libraryName;
   const char* flintDataName;
   InitializeFlintDataFactoryFunctionPtr initializer;
};

typedef std::shared_ptr<datarepository::IProviderInterface> (*InitializeDataRepositoryProviderFunctionPtr)(const DynamicObject&);

struct DataRepositoryProviderRegistration {
   const char* providerName;
   int providerType;
   InitializeDataRepositoryProviderFunctionPtr initializer;
};

/**
 * Function pointer type for getModuleRegistrations().
 *
 * All modules must have a getModuleRegistrations() function. Usually this
 * is declared automatically using the IMPLEMENT_MODULE macro below. The
 * function must be declared using as 'extern "C"' so that the name remains
 * undecorated.
 */
typedef int (*GetModuleRegistrationsFunctionPtr)                  (ModuleRegistration*);
typedef int (*GetTransformRegistrationsFunctionPtr)               (TransformRegistration*);
typedef int (*GetFlintDataRegistrationsFunctionPtr)               (FlintDataRegistration*);
typedef int (*GetFlintDataFactoryRegistrationsFunctionPtr)        (FlintDataFactoryRegistration*);
typedef int (*GetDataRepositoryProviderRegistrationsFunctionPtr)  (DataRepositoryProviderRegistration*);

/**
 * The module registry maps modules by name to a function wrapper around
 * the module's initializer to convert the C-style raw pointer to a smart
 * pointer.
 */

typedef std::shared_ptr<IModule> ModuleInterfacePtr;
typedef std::pair<std::string, std::string> ModuleKey;
typedef std::map<ModuleKey, std::function<ModuleInterfacePtr(void)>> ModuleRegistry;

typedef std::pair<std::string, std::string> TransformKey;
typedef std::shared_ptr<ITransform> TransformInterfacePtr;
typedef std::map<TransformKey, std::function<TransformInterfacePtr(void)>> TransformRegistry;

typedef std::pair<std::string, std::string> FlintDataKey;
typedef std::shared_ptr<IFlintData> FlintDataInterfacePtr;
typedef std::map<FlintDataKey, std::function<FlintDataInterfacePtr(void)>> FlintDataRegistry;

typedef std::pair<std::string, std::string> FlintDataFactoryKey;
typedef std::shared_ptr<IFlintData> FlintDataFactoryInterfacePtr;
typedef std::multimap<FlintDataFactoryKey, std::function<FlintDataFactoryInterfacePtr(
                                               const std::string&, int, const std::string&, const DynamicObject&)>>
    FlintDataFactoryRegistry;

typedef std::pair<std::string, std::string> ProviderKey;
typedef std::shared_ptr<datarepository::IProviderInterface> ProviderInterfacePtr;
typedef std::map<ProviderKey, std::function<ProviderInterfacePtr(const DynamicObject&)>> ProviderRegistry;

#define IMPLEMENT_MODULE(ModuleImplClass, ModuleName)                                                            \
   extern "C" MOJA_LIB_API moja::flint::IModule* initializeModule() { return new ModuleImplClass(); }            \
   /**/                                                                                                          \
   extern "C" MOJA_LIB_API int getModuleRegistrations(moja::flint::ModuleRegistration* outModuleRegistrations) { \
      outModuleRegistrations[0] = moja::flint::ModuleRegistration{#ModuleName, &initializeModule};               \
      return 1;                                                                                                  \
   }

}  // namespace flint

