#ifndef MOJA_FLINT_ASPATIALNOSQLLOCALDOMAINCONTROLLER_H_
#define MOJA_FLINT_ASPATIALNOSQLLOCALDOMAINCONTROLLER_H_
#include "localdomaincontrollerbase.h"
#include "moja/datarepository/iprovidernosqlinterface.h"

//#include "moja/datarepository/aspatialtileinfocollection.h"
//#include "moja/flint/configuration/configuration.h"
//#include "moja/flint/localdomaincontrollerbase.h"
//#include "moja/flint/spinuplandunitcontroller.h"
//#include <moja/datarepository/iprovidernosqlinterface.h>

namespace moja {
namespace flint {

class FLINT_API AspatialNoSQLLocalDomainController final : public flint::LocalDomainControllerBase {
public:
	AspatialNoSQLLocalDomainController(): _count(0) {}
	~AspatialNoSQLLocalDomainController() = default;

	virtual void configure(const flint::configuration::Configuration& config) override;
	virtual void run() override;

private:
	std::shared_ptr<datarepository::IProviderNoSQLInterface> _provider;
	int _count;
	DynamicVector _idSet;
	//std::unique_ptr<moja::datarepository::AspatialTileInfoCollection> _tiles;
};

}
}

#endif // MOJA_FLINT_ASPATIALNOSQLLOCALDOMAINCONTROLLER_H_