#ifndef MOJA_DATAREPOSITORY_IPROVIDERSPATIALVECTORINTERFACE_H_
#define MOJA_DATAREPOSITORY_IPROVIDERSPATIALVECTORINTERFACE_H_

#include "moja/datarepository/iproviderinterface.h"

#include "moja/dynamic.h"

namespace moja {
namespace datarepository {

// --------------------------------------------------------------------------------------------
/**
* Interface class that all provider implementations for Spatial Vector data should derive from.
*/
class IProviderSpatialVectorInterface : public IProviderInterface {
public:
	/**
	* required because providers are deleted via a pointer to this interface
	*/
	IProviderSpatialVectorInterface() = default;
	virtual ~IProviderSpatialVectorInterface() = default;

	virtual DynamicVar GetValue(const std::string& name, double Latitude, double Longitude) = 0;
	virtual DynamicVar GetValueStack(const std::string& name, double Latitude, double Longitude) = 0;

	virtual DynamicVar GetAttribute(const std::string& name, double Latitude, double Longitude) = 0;
	virtual DynamicVar GetAttributeStack(const std::string& name, double Latitude, double Longitude) = 0;

	virtual ProviderTypes providerType() override { return ProviderTypes::Vector; }
};

}} // moja::datarepository

#endif // MOJA_DATAREPOSITORY_IPROVIDERSPATIALVECTORINTERFACE_H_