#ifndef MOJA_DATAREPOSITORY_IPROVIDERINTERFACE_H_
#define MOJA_DATAREPOSITORY_IPROVIDERINTERFACE_H_

#include "moja/datarepository/_datarepository_exports.h"

namespace moja {
namespace datarepository {

// --------------------------------------------------------------------------------------------
enum class ProviderTypes {
	Raster = 0,
	Vector = 1,
	Relational = 2,
	NoSQL = 3,
	RasterStack = 4
};

// --------------------------------------------------------------------------------------------
/**
* Interface class that all provider implementations should derive from.
*/
class DATAREPOSITORY_API IProviderInterface {
public:
	/**
	* required because providers are deleted via a pointer to this interface
	*/
	IProviderInterface() = default;
	virtual ~IProviderInterface() = default;
	virtual ProviderTypes providerType() = 0;
};

}} // moja::datarepository

#endif // MOJA_DATAREPOSITORY_IPROVIDERINTERFACE_H_
