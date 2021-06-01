#pragma once

#include "moja/flint/librarytype.h"
#include "moja/flint/mojalibapi.h"

namespace moja::flint {

class FlintLibraryHandles {
  public:
   FlintLibraryHandles()
       : libraryName(""),
         getModuleRegistrations(nullptr),
         getTransformRegistrations(nullptr),
         getFlintDataRegistrations(nullptr),
         getFlintDataFactoryRegistrations(nullptr),
         getDataRepositoryProviderRegistrations(nullptr){};

   FlintLibraryHandles(const std::string& libraryName, GetModuleRegistrationsFunctionPtr m,
                       GetTransformRegistrationsFunctionPtr t, GetFlintDataRegistrationsFunctionPtr d,
                       GetFlintDataFactoryRegistrationsFunctionPtr f,
                       GetDataRepositoryProviderRegistrationsFunctionPtr p)
       : libraryName(libraryName),
         getModuleRegistrations(m),
         getTransformRegistrations(t),
         getFlintDataRegistrations(d),
         getFlintDataFactoryRegistrations(f),
         getDataRepositoryProviderRegistrations(p){};

   std::string libraryName;
   GetModuleRegistrationsFunctionPtr getModuleRegistrations;
   GetTransformRegistrationsFunctionPtr getTransformRegistrations;
   GetFlintDataRegistrationsFunctionPtr getFlintDataRegistrations;
   GetFlintDataFactoryRegistrationsFunctionPtr getFlintDataFactoryRegistrations;
   GetDataRepositoryProviderRegistrationsFunctionPtr getDataRepositoryProviderRegistrations;
};

class LibraryInfoBase {
  public:
   LibraryInfoBase();
   virtual ~LibraryInfoBase() {}

   virtual LibraryType GetLibraryType() const = 0;

   std::shared_ptr<FlintLibraryHandles> libraryHandles;

   /**
    * The module object for this module. We actually *own* this module,
    * so its lifetime is controlled by the scope of this shared pointer.
    */
   std::vector<ModuleInterfacePtr> moduleList;
   std::vector<TransformInterfacePtr> transformList;
   std::vector<FlintDataInterfacePtr> flintDataList;
   std::vector<FlintDataFactoryInterfacePtr> flintDataFactoryList;
   std::vector<ProviderInterfacePtr> providerList;

   /**
    * Arbitrary number that encodes the load order of this module, so
    *  we can shut them down in reverse order.
    */
   Int32 loadOrder;

   /** Count of how many of this type have been instantiated. */
   Int32 moduleCount;
   Int32 transformCount;
   Int32 flintDataCount;
   Int32 flintDataFactoryCount;
   Int32 providerCount;
};

}  // namespace moja::flint
