//
// ProviderNoSQLPocoJSON.h
//

#ifndef ProviderNoSQLPocoJSON_INCLUDED
#define ProviderNoSQLPocoJSON_INCLUDED

#include "moja/datarepository/iproviderinterface.h"

#include "moja/Dynamic.h"

#include <string>

namespace moja {
namespace datarepository {

// --------------------------------------------------------------------------------------------
/**
* SLEEK Implmentation of a NoSQL data provider (using MongoDB as a data source) derived
* from IProviderSpatialVectorInterface.
*/
class DATAREPOSITORY_API ProviderNoSQLPocoJSON : public IProviderNoSQLInterface {

public:
	ProviderNoSQLPocoJSON();
	virtual ~ProviderNoSQLPocoJSON() {}

	virtual Dynamic GetObject(int id) override;
	virtual Dynamic GetObjectStack(const std::string& query) override;

	typedef std::tuple<std::string, int, double, bool> provider_object;
};

}} // moja::datarepository

#endif //ProviderNoSQLPocoJSON_INCLUDED
