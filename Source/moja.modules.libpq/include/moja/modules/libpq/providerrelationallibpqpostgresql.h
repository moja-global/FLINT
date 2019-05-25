#ifndef MOJA_DATAREPOSITORY_PROVIDERRELATIONALLIBPQPOSTGRESQL_H_
#define MOJA_DATAREPOSITORY_PROVIDERRELATIONALLIBPQPOSTGRESQL_H_

#include "moja/modules/libpq/_modules.libpq_exports.h"

#include <moja/datarepository/iproviderrelationalinterface.h>

#include <moja/dynamic.h>

namespace moja {
namespace modules {
namespace libpq {

class LIBPQ_API ProviderRelationalLibpqPostgreSQL : public datarepository::IProviderRelationalInterface {
   class impl;
   std::unique_ptr<impl> pimpl;

  public:
   explicit ProviderRelationalLibpqPostgreSQL(DynamicObject settings);
   virtual ~ProviderRelationalLibpqPostgreSQL();
   DynamicVar GetDataSet(const std::string& query) const override;
};

}  // namespace libpq
}  // namespace modules
}  // namespace moja

#endif  // MOJA_DATAREPOSITORY_PROVIDERRELATIONALLIBPQPOSTGRESQL_H_