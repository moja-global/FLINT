#ifndef MOJA_DATAREPOSITORY_IPROVIDERNOSQLINTERFACE_H_
#define MOJA_DATAREPOSITORY_IPROVIDERNOSQLINTERFACE_H_

#include "moja/datarepository/iproviderinterface.h"

#include <moja/dynamic.h>

namespace moja {
namespace datarepository {

// --------------------------------------------------------------------------------------------
/**
 * Interface class that all provider implementations for NoSQL data sources should derive from.
 */

class IProviderNoSQLInterface : public IProviderInterface {
  public:
   /**
    * required because providers are deleted via a pointer to this interface
    */
   IProviderNoSQLInterface() = default;
   virtual ~IProviderNoSQLInterface() = default;

   virtual DynamicVector GetDataSet(const std::string& query) const = 0;

   virtual int Count() const = 0;

   ProviderTypes providerType() override { return ProviderTypes::NoSQL; }
};

}  // namespace datarepository
}  // namespace moja

#endif  // MOJA_DATAREPOSITORY_IPROVIDERNOSQLINTERFACE_H_