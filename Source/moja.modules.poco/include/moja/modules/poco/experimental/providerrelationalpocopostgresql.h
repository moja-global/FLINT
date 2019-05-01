#ifndef MOJA_MODULES_POCO_PROVIDERRELATIONALPOCOPOSTGRESQL_H_
#define MOJA_MODULES_POCO_PROVIDERRELATIONALPOCOPOSTGRESQL_H_

//#if defined(_WIN32)
//#include <winsock2.h>
//#include <windows.h>
//#endif

#if 0
https://www.postgresql.org/docs/9.0/static/populate.html

https://www.postgresql.org/docs/current/static/libpq-copy.html

http://stackoverflow.com/questions/10510539/simple-postgresql-libpq-code-too-slow

#endif

#include "moja/datarepository/_datarepository_exports.h"
#include "moja/datarepository/iproviderrelationalinterface.h"

#include <moja/dynamic.h>
#include "Poco/LRUCache.h"
#include "Poco/Data/PostgreSQL/Utility.h"
#include "Poco/Post

//Parameters for testing
#define POSTGRESQL_USER "postgres"
#define POSTGRESQL_PWD  "admin"
#define POSTGRESQL_HOST "localhost"
#define POSTGRESQL_PORT "5432"
#define POSTGRESQL_DB   "postgres"

namespace moja {
namespace datarepository {

class DATAREPOSITORY_API ProviderRelationalPocoPostgreSQL : public IProviderRelationalInterface{
public:
	
	explicit ProviderRelationalPocoPostgreSQL(DynamicObject settings);
	virtual ~ProviderRelationalPocoPostgreSQL();
	DynamicVar GetDataSet(const std::string& query) const override;

private:
	PGconn *  _conn = 0;
	mutable PGresult * pResult = 0;
	std::string _dbConnString;
    mutable Poco::LRUCache<std::string, DynamicVar> _cache;
};

}} // moja::datarepository

#endif // MOJA_MODULES_POCO_PROVIDERRELATIONALPOCOPOSTGRESQL_H_