#ifndef MOJA_DATAREPOSITORY_IPROVIDERREATIONALINTERFACE_H_
#define MOJA_DATAREPOSITORY_IPROVIDERREATIONALINTERFACE_H_

#include "moja/datarepository/iproviderinterface.h"

#include "moja/dynamic.h"

namespace moja {
namespace datarepository {

// --------------------------------------------------------------------------------------------
/**
* Interface class that all provider implementations for Relational data sources should derive from.
*/

class IProviderRelationalInterface : public IProviderInterface {
public:
	/**
	* required because providers are deleted via a pointer to this interface
	*/
	IProviderRelationalInterface() = default;
	virtual ~IProviderRelationalInterface() = default;

	virtual DynamicVar GetDataSet(const std::string& query) const = 0;

	virtual ProviderTypes providerType() override { return ProviderTypes::Relational; }
};

}
} // moja::datarepository

#endif // MOJA_DATAREPOSITORY_IPROVIDERREATIONALINTERFACE_H_