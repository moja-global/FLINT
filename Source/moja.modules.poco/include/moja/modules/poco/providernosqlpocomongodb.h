#ifndef MOJA_MODULES_POCO_PROVIDERNOSQLPOCOMONGODB_H_
#define MOJA_MODULES_POCO_PROVIDERNOSQLPOCOMONGODB_H_

#include "moja/modules/poco/_modules.poco_exports.h"
#include "moja/datarepository/iprovidernosqlinterface.h"

#include <moja/dynamic.h>

#include <Poco/MongoDB/Connection.h>
#include <Poco/LRUCache.h>

#include <string>

namespace moja {
namespace modules {
namespace poco {

/**
* Moja Implmentation of a NoSQL data provider (using MongoDB as a data source) derived
* from IProviderSpatialVectorInterface.
*/
class POCO_API ProviderNoSQLPocoMongoDB : public datarepository::IProviderNoSQLInterface {
public:
	typedef std::shared_ptr<ProviderNoSQLPocoMongoDB> Ptr;

	ProviderNoSQLPocoMongoDB() = default;
	explicit ProviderNoSQLPocoMongoDB(DynamicObject settings);
	virtual ~ProviderNoSQLPocoMongoDB();

	DynamicVector GetDataSet(const std::string& query) const override;

	int Count() const override;

	virtual DynamicVector	SendCmdRequest(const std::string& command) const;
	virtual int				SendCountRequest() const;
	virtual DynamicVector	SendDeleteRequest(const std::string& query) const;
	virtual DynamicVector	SendInsertRequest(const std::string& query) const;
	virtual DynamicVector	SendQueryRequest(const std::string& query, int numberToReturn = 100) const;
	virtual DynamicVector	SendQueryRequest(const std::string& query, const std::string& fields, int numberToReturn = 100) const;
	virtual DynamicVector	SendUpdateRequest(const std::string& query) const;

private:
	std::string _host;
	int _port;
	std::string _collection;
	std::string _database;

	mutable bool _connected;
	mutable Poco::MongoDB::Connection _connection;
    mutable Poco::LRUCache<std::string, DynamicVector> _cache;
};

}}}

#endif // MOJA_MODULES_POCO_PROVIDERNOSQLPOCOMONGODB_H_
