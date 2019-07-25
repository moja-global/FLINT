#ifndef MOJA_DATAREPOSITORY_PROVIDERRELATIONALSQLITE_H_
#define MOJA_DATAREPOSITORY_PROVIDERRELATIONALSQLITE_H_

#include "moja/datarepository/_datarepository_exports.h"
#include "moja/datarepository/iproviderrelationalinterface.h"

#include <moja/dynamic.h>

namespace moja {
namespace datarepository {

class DATAREPOSITORY_API ProviderRelationalSQLite : public IProviderRelationalInterface {
   class impl;
   std::unique_ptr<impl> pimpl;

  public:
   explicit ProviderRelationalSQLite(DynamicObject settings);
   virtual ~ProviderRelationalSQLite();
   DynamicVar GetDataSet(const std::string& query) const override;
};

}  // namespace datarepository
}  // namespace moja

#endif  // MOJA_DATAREPOSITORY_PROVIDERRELATIONALSQLITE_H_
