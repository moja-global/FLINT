//
// ProviderManager.h
//

#ifndef ProviderManager_INCLUDED
#define ProviderManager_INCLUDED

#include "moja/datarepository/_datarepository_exports.h"
#include "moja/datarepository/iproviderinterface.h"

#include <moja/sharedlibrary.h>

#include <map>
#include <string>

#if defined(_WIN32)
#define PROVIDERMANAGER_LIB_API __declspec(dllexport)
#endif

#if !defined(PROVIDERMANAGER_LIB_API)
#if defined(__GNUC__) && (__GNUC__ >= 4)
#define PROVIDERMANAGER_LIB_API __attribute__((visibility("default")))
#else
#define PROVIDERMANAGER_LIB_API
#endif
#endif

namespace moja {
namespace datarepository {

// class IInTApi;
// class ILandUnitController;

/**
 * Function pointer type for InitializeProvider().
 *
 * All providers must have an InitializeProvider() function. Usually this is declared automatically using
 * the IMPLEMENT_PROVIDER macro below. The function must be declared using as 'extern "C"' so that the
 * name remains undecorated. The object returned will be "owned" by the caller, and will be deleted
 * by the caller before the provider is unloaded.
 */
typedef IProviderInterface* (*InitializeProviderFunctionPtr)(const char* providerName);
typedef void (*DestroyProviderFunctionPtr)(void* obj);

// --------------------------------------------------------------------------------------------
enum class ProviderType { Unknown, Internal, External };

// --------------------------------------------------------------------------------------------
class ProviderInfoBase {
  public:
   /** Constructor */
   ProviderInfoBase();
   virtual ~ProviderInfoBase(){
       // std::cout << "[ProviderInfoBase::Destructor]:" << "LoadOrder ("<< LoadOrder << ")"<< std::endl;
   };

   virtual ProviderType GetProviderType() const = 0;

   InitializeProviderFunctionPtr InitializeProvider;
   DestroyProviderFunctionPtr DestroyProvider;

   /** The provider object for this provider.  We actually *own* this provider, so it's lifetime is controlled by the
    * scope of this shared pointer. */
   std::vector<IProviderInterface::Ptr> ProviderList;

   /** Arbitrary number that encodes the load order of this provider, so we can shut them down in reverse order. */
   Int32 LoadOrder;

   /** count of how many of this type have been instantiate **/
   Int32 ProviderNumber;
};

// --------------------------------------------------------------------------------------------
class ExternalProviderInfo : public ProviderInfoBase {
  public:
   ExternalProviderInfo();
   virtual ~ExternalProviderInfo() {
      // std::cout << "[ExternalProviderInfo::Destructor]" << std::endl;
   }
   ProviderType GetProviderType() const { return ProviderType::External; }

   std::string OriginalFilename;

   /** File name of this Provider (.dll or .so file name) */
   std::string Filename;

   /** Handle to this Provider (library handle), if it's currently loaded */
   std::unique_ptr<moja::SharedLibrary> Handle;
};

// --------------------------------------------------------------------------------------------
class InternalProviderInfo : public ProviderInfoBase {
  public:
   InternalProviderInfo();
   virtual ~InternalProviderInfo() {
      // std::cout << "[InternalProviderInfo::Destructor]" << std::endl;
   }
   ProviderType GetProviderType() const { return ProviderType::Internal; }
};

// --------------------------------------------------------------------------------------------
class DATAREPOSITORY_API ProviderManager {
  public:
   ProviderManager() = default;
   ~ProviderManager();

   /** static that tracks the current load number. Incremented whenever we add a new Provider */
   static Int32 CurrentLoadOrder;

   void AddProvider(ProviderType type, const std::string& ProviderName);
   IProviderInterface::Ptr GetProvider(const std::string& providerName);
   bool IsProviderLoaded(const std::string& providerName) const;
   bool LoadInternalProvider(const std::string& providerName);
   bool LoadExternalProvider(const std::string& ProviderName);

   bool UnloadProvider(const std::string& ProviderName, bool isShutdown = false) { return true; }

   typedef std::map<std::string, std::shared_ptr<ProviderInfoBase>> ProviderMap;

  private:
   /** Compares file versions between the current executing flint version and the specified library */
   static bool CheckProviderCompatibility(const std::string& filename);
   /** Finds Providers matching a given name wildcard. */
   std::map<std::string, std::string> FindProviderPaths(const std::string& NamePattern) const;
   std::map<std::string, std::string> FindProviderPathsExact(const std::string& NamePattern) const;
   std::map<std::string, std::string> FindProviderPathsInDirectory(const std::string& InDirectoryName,
                                                                   const std::string& NamePattern,
                                                                   bool useSuffix) const;

   /** Map of all Providers. Maps the case-insensitive Provider name to information about that Provider, loaded or not.
    */
   ProviderMap _providers;
};

// --------------------------------------------------------------------------------------------
// A default minimal implementation of a provider that does nothing at startup and shutdown
class DefaultProviderImpl : public IProviderInterface {};

// --------------------------------------------------------------------------------------------

#define IMPLEMENT_PROVIDER(ProviderImplClass, ProviderName)                                                           \
                                                                                                                      \
   /**/                                                                                                               \
   /* InitializeProvider function, called by provider manager after this provider's DLL has been loaded */            \
   extern "C" PROVIDERMANAGER_LIB_API moja::flint::IProviderInterface* InitializeProvider(const char* providerName) { \
      return new ProviderImplClass();                                                                                 \
   }                                                                                                                  \
   /**/                                                                                                               \
   /* DestroyProvider function */                                                                                     \
   extern "C" PROVIDERMANAGER_LIB_API void DestroyProvider(void* obj) {                                               \
      /* std::cout << "[DestroyProvider]: " << #ProviderImplClass << ", " << #ProviderName << std::endl;*/            \
      delete ((ProviderImplClass*)(obj));                                                                             \
   }

}  // namespace datarepository
}  // namespace moja
#endif  // ProviderManager_INCLUDED
