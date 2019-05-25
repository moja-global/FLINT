#ifndef MOJA_FLINT_ASPATIALNOSQLLOCALDOMAINCONTROLLER_H_
#define MOJA_FLINT_ASPATIALNOSQLLOCALDOMAINCONTROLLER_H_
#include "moja/flint/localdomaincontrollerbase.h"

#include <moja/datarepository/iprovidernosqlinterface.h>

namespace moja {
namespace flint {

class FLINT_API AspatialNoSQLLocalDomainController final : public flint::LocalDomainControllerBase {
  public:
   AspatialNoSQLLocalDomainController() : _count(0) {}
   ~AspatialNoSQLLocalDomainController() = default;

   virtual void configure(const flint::configuration::Configuration& config) override;
   virtual void run() override;

  private:
   std::shared_ptr<datarepository::IProviderNoSQLInterface> _provider;
   int _count;
   DynamicVector _idSet;
   // std::unique_ptr<moja::datarepository::AspatialTileInfoCollection> _tiles;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_ASPATIALNOSQLLOCALDOMAINCONTROLLER_H_