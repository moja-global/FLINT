//#if defined(_WIN32)
//#include <winsock2.h>
//#include <windows.h>
//#endif

#include <Poco/Data/Session.h>
#include <Poco/Data/PostgreSQL/Connector.h>
#include <Poco/File.h>
#include "moja/types.h"
#include "moja/datarepository/datarepositoryexceptions.h"
#include "moja/datarepository/providerrelationalpocopostgresql.h"
#include "moja/dynamic.h"
#include "moja/dynamicstruct.h"
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <boost/test/unit_test.hpp>

#include <Poco/Data/Session.h>
#include <Poco/Data/SQLite/Connector.h>
#include <Poco/File.h>

#include <stdexcept>

#include "moja/types.h"
#include "moja/datarepository/providerrelationalpocopostgresql.h"
#include "moja/datarepository/datarepositoryexceptions.h"
#include "moja/dynamic.h"
#include "moja/dynamicstruct.h"
#include <moja/logging.h>

using moja::Int64;
using moja::datarepository::ProviderRelationalPocoPostgreSQL;
using moja::datarepository::FileNotFoundException;
using moja::datarepository::ConnectionFailedException;
using moja::datarepository::QueryException;

using Poco::Data::Keywords::now;

#define TEST_SCHEMA "ProviderRelationalPostgresTestsFixture_Schema"

static void noticeProcessor(void *arg, const char *message) {
	//MOJA_LOG_DEBUG << message;
}

static void noticeReceiver(void *arg, const PGresult *res) {
}

struct ProviderRelationalPostgresTestsFixture {
	moja::DynamicObject settings;
	std::string connectionstring;
	
	ProviderRelationalPostgresTestsFixture() {
		connectionstring = "host=" POSTGRESQL_HOST
			" user=" POSTGRESQL_USER
			" password=" POSTGRESQL_PWD
			" dbname=" POSTGRESQL_DB
			" port=" POSTGRESQL_PORT;
		
		settings["connectionstring"] = connectionstring;

		auto _conn = PQconnectdb(connectionstring.c_str());
		PQsetNoticeReceiver(_conn, noticeReceiver, nullptr);
		PQsetNoticeProcessor(_conn, noticeProcessor, nullptr);
		auto pResult = PQexec(_conn, "DROP Schema IF EXISTS ProviderRelationalPostgresTestsFixture_Schema CASCADE; CREATE SCHEMA ProviderRelationalPostgresTestsFixture_Schema; DROP TABLE IF EXISTS ProviderRelationalPostgresTestsFixture_Schema.JunkTable; CREATE TABLE ProviderRelationalPostgresTestsFixture_Schema.JunkTable (id INTEGER);");
		if (PQresultStatus(pResult) != PGRES_COMMAND_OK) {
			MOJA_LOG_ERROR << "Command failed: " << PQerrorMessage(_conn);
			PQclear(pResult);
		}
		PQfinish(_conn);
	}

	bool SendQuery(std::string query) const {
		auto _conn = PQconnectdb(connectionstring.c_str());
		PQsetNoticeReceiver(_conn, noticeReceiver, nullptr);
		PQsetNoticeProcessor(_conn, noticeProcessor, nullptr);
		auto pResult = PQexec(_conn, query.c_str());
		if (PQresultStatus(pResult) != PGRES_COMMAND_OK) {
			MOJA_LOG_ERROR << "Command failed: " << PQerrorMessage(_conn);
			PQclear(pResult);
			return false;
		}
		PQfinish(_conn);
		return true;
	}

	~ProviderRelationalPostgresTestsFixture() {
		auto _conn = PQconnectdb(connectionstring.c_str());
		PQsetNoticeReceiver(_conn, noticeReceiver, nullptr);
		PQsetNoticeProcessor(_conn, noticeProcessor, nullptr);
		auto pResult = PQexec(_conn, "DROP Schema IF EXISTS ProviderRelationalPostgresTestsFixture_Schema CASCADE;");
		if (PQresultStatus(pResult) != PGRES_COMMAND_OK) {
			MOJA_LOG_ERROR << "Command failed: " << PQerrorMessage(_conn);
			PQclear(pResult);
		}
		PQfinish(_conn);
	}
};

BOOST_FIXTURE_TEST_SUITE(ProviderRelationalPostgresTests, ProviderRelationalPostgresTestsFixture);

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPocoPostgreSQL_ConstructorThrowsExceptionIfTargetDatabaseNotFound) {
	connectionstring = "host=" POSTGRESQL_HOST
		" user=" POSTGRESQL_USER
		" password=" POSTGRESQL_PWD
		" dbname=DoesNotExist"
		" port=" POSTGRESQL_PORT;
	settings["connectionstring"] = connectionstring;

	BOOST_CHECK_THROW(ProviderRelationalPocoPostgreSQL provider(settings), ConnectionFailedException);
}

//BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPocoPostgreSQL_OpenDatabase) {
//	auto inMemoryPaths = { ":memory:", "file::memory:?cache=shared" };
//	for (auto path : inMemoryPaths) {
//		settings["path"] = path;
//		ProviderRelationalPocoPostgreSQL provider(settings);
//	}
//}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPocoPostgreSQL_OpenValidDatabase) {
	ProviderRelationalPocoPostgreSQL provider(settings);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPocoPostgreSQL_GetDataSetThrowsExceptionOnBadSQL) {
	ProviderRelationalPocoPostgreSQL provider(settings);
	auto sql = "not a valid query";
	BOOST_CHECK_THROW(provider.GetDataSet(sql), QueryException);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPocoPostgreSQL_GetDataSetThrowsExceptionIfTableNotFound) {
	ProviderRelationalPocoPostgreSQL provider(settings);
	auto sql = "SELECT * FROM ProviderRelationalPostgresTestsFixture_Schema.nonexistent_table";
	BOOST_CHECK_THROW(provider.GetDataSet(sql), QueryException);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPocoPostgreSQL_GetDataSetReturnsEmptyTableIfQueryReturnsNoResults) {
	// Setup
	SendQuery("CREATE TABLE ProviderRelationalPostgresTestsFixture_Schema.Test (data VARCHAR)");
	SendQuery("INSERT INTO ProviderRelationalPostgresTestsFixture_Schema.Test VALUES ('test')");

	// Test
	ProviderRelationalPocoPostgreSQL provider(settings);
	auto sql = "SELECT * FROM ProviderRelationalPostgresTestsFixture_Schema.Test WHERE Test.data LIKE 'should not be found'";
	const auto result = provider.GetDataSet(sql).extract<std::vector<moja::DynamicObject>>();
	BOOST_CHECK_EQUAL(result.size(), 0);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPocoPostgreSQL_GetDataSetReturnsNullDatabaseValues) {
	// Setup
	SendQuery("CREATE TABLE ProviderRelationalPostgresTestsFixture_Schema.Test (id INT, data VARCHAR)");
	SendQuery("INSERT INTO ProviderRelationalPostgresTestsFixture_Schema.Test VALUES (1, NULL)");

	// Test
	ProviderRelationalPocoPostgreSQL provider(settings);
	auto sql = "SELECT id, data FROM ProviderRelationalPostgresTestsFixture_Schema.ProviderRelationalPostgresTestsFixture_Schema.Test WHERE id = 1";
	const auto result = provider.GetDataSet(sql).extract<std::vector<moja::DynamicObject>>();
	BOOST_CHECK_EQUAL(result.size(), 1);

	auto& r0 = result[0];
	auto r0data = r0["data"];
	BOOST_CHECK(r0data.isEmpty());
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPocoPostgreSQL_SimpleStringDataSet) {
	// Setup
	SendQuery("CREATE TABLE ProviderRelationalPostgresTestsFixture_Schema.SimpleStringDataSet (data VARCHAR)");
	SendQuery("INSERT INTO ProviderRelationalPostgresTestsFixture_Schema.SimpleStringDataSet VALUES ('test')");

	// Test
	ProviderRelationalPocoPostgreSQL provider(settings);
	auto sql = "SELECT * FROM ProviderRelationalPostgresTestsFixture_Schema.SimpleStringDataSet";
	const auto result = provider.GetDataSet(sql).extract<std::vector<moja::DynamicObject>>();
	BOOST_CHECK_EQUAL(result.size(), 1);

	auto& r0 = result[0];
	auto r0c0 = r0["data"].convert<std::string>();
	BOOST_CHECK_EQUAL(r0c0, "test");
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPocoPostgreSQL_SimpleIntDataSet) {
	// Setup
	SendQuery("CREATE TABLE ProviderRelationalPostgresTestsFixture_Schema.SimpleIntDataSet (data INT)");
	SendQuery("INSERT INTO ProviderRelationalPostgresTestsFixture_Schema.SimpleIntDataSet VALUES (1)");

	// Test
	ProviderRelationalPocoPostgreSQL provider(settings);
	auto sql = "SELECT * FROM ProviderRelationalPostgresTestsFixture_Schema.SimpleIntDataSet";
	const auto result = provider.GetDataSet(sql).extract<std::vector<moja::DynamicObject>>();
	BOOST_CHECK_EQUAL(result.size(), 1);

	auto r0 = result[0];
	auto r0c0 = r0["data"].convert<int>();
	BOOST_CHECK_EQUAL(r0c0, 1);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPocoPostgreSQL_SimpleBigIntDataSet) {
	// Setup
	SendQuery("CREATE TABLE ProviderRelationalPostgresTestsFixture_Schema.SimpleBigIntDataSet (data INT8)");
	SendQuery("INSERT INTO ProviderRelationalPostgresTestsFixture_Schema.SimpleBigIntDataSet VALUES (3000000000)");

	// Test
	ProviderRelationalPocoPostgreSQL provider(settings);
	auto sql = "SELECT * FROM ProviderRelationalPostgresTestsFixture_Schema.SimpleBigIntDataSet";
	const auto result = provider.GetDataSet(sql).extract<std::vector<moja::DynamicObject>>();
	BOOST_CHECK_EQUAL(result.size(), 1);

	auto& r0 = result[0];
	auto r0c0 = r0["data"].convert<Int64>();
	BOOST_CHECK_EQUAL(r0c0, 3000000000);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPocoPostgreSQL_SimpleDoubleDataSet) {
	// Setup
	SendQuery("CREATE TABLE ProviderRelationalPostgresTestsFixture_Schema.SimpleDoubleDataSet (data DOUBLE PRECISION)");
	SendQuery("INSERT INTO ProviderRelationalPostgresTestsFixture_Schema.SimpleDoubleDataSet VALUES (1.5)");

	// Test
	ProviderRelationalPocoPostgreSQL provider(settings);
	auto sql = "SELECT * FROM ProviderRelationalPostgresTestsFixture_Schema.SimpleDoubleDataSet";
	const auto result = provider.GetDataSet(sql).extract<std::vector<moja::DynamicObject>>();
	BOOST_CHECK_EQUAL(result.size(), 1);

	auto& r0 = result[0];
	auto r0c0 = r0["data"].convert<double>();
	BOOST_CHECK_EQUAL(r0c0, 1.5);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPocoPostgreSQL_ComplexMixedDataSet) {
	// Setup
	SendQuery("CREATE TABLE ProviderRelationalPostgresTestsFixture_Schema.ComplexMixedDataSet (col0 VARCHAR, col1 INT, col2 DOUBLE PRECISION)");
	SendQuery("INSERT INTO ProviderRelationalPostgresTestsFixture_Schema.ComplexMixedDataSet VALUES ('first', 1, 1.5)");
	SendQuery("INSERT INTO ProviderRelationalPostgresTestsFixture_Schema.ComplexMixedDataSet VALUES ('second', 2, 2.5)");

	// Test
	ProviderRelationalPocoPostgreSQL provider(settings);
	auto sql = "SELECT * FROM ProviderRelationalPostgresTestsFixture_Schema.ComplexMixedDataSet";
	const auto result = provider.GetDataSet(sql).extract<std::vector<moja::DynamicObject>>();
	BOOST_CHECK_EQUAL(result.size(), 2);

	auto& r0 = result[0];
	std::string r0c0 = r0["col0"].convert<std::string>();
	auto r0c1 = r0["col1"].convert<int>();
	auto r0c2 = r0["col2"].convert<double>();
	BOOST_CHECK_EQUAL(r0c0, "first");
	BOOST_CHECK_EQUAL(r0c1, 1);
	BOOST_CHECK_EQUAL(r0c2, 1.5);

	auto& r1 = result[1];
	auto r1c0 = r1["col0"].convert<std::string>();
	auto r1c1 = r1["col1"].convert<int>();
	auto r1c2 = r1["col2"].convert<double>();
	BOOST_CHECK_EQUAL(r1c0, "second");
	BOOST_CHECK_EQUAL(r1c1, 2);
	BOOST_CHECK_EQUAL(r1c2, 2.5);
}

#if 0
BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPostgres_BasicTableDataTests1) {

		moja::datarepository::ProviderRelationalPocoPostgreSQL provider(settings);
		auto sql = "DROP TABLE IF EXISTS public.PTEST1; \
					CREATE TABLE public.PTEST1 as SELECT * FROM public.NEW_TEST nt where nt.amname like 'h%'; \
					SELECT * FROM public.PTEST1";
		provider.GetDataSet(sql);

}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPostgres_BasicTableDataTests3) {

	moja::datarepository::ProviderRelationalPocoPostgreSQL provider(settings);
	auto sql = "DROP TABLE IF EXISTS public.COMPANY; \
					CREATE TABLE public.COMPANY (Id INT PRIMARY KEY NOT NULL, Name TEXT,AGE INT,Address CHAR(50),Sal REAL); \
INSERT INTO public.COMPANY VALUES(1,'a123',29,'123 abc st',2.2); \
					SELECT * FROM public.COMPANY";

	provider.GetDataSet(sql);

}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPostgres_GetDataSetThrowsExceptionOnBadSQL) {
	
	moja::datarepository::ProviderRelationalPocoPostgreSQL provider(settings);
	auto sql = "not a valid query";
	BOOST_CHECK_THROW(provider.GetDataSet(sql), moja::datarepository::QueryException);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPostgres_GetDataSetThrowsExceptionIfTableNotFound) {
	
	moja::datarepository::ProviderRelationalPocoPostgreSQL provider(settings);
	auto sql = "SELECT * FROM public.nonexistent_table";
	BOOST_CHECK_THROW(provider.GetDataSet(sql), moja::datarepository::QueryException);
}


BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPostgres_GetDataSetReturnsEmptyTableIfQueryReturnsNoResults) {
	
	moja::datarepository::ProviderRelationalPocoPostgreSQL provider(settings);
	auto sql = "DROP TABLE IF EXISTS public.TEST; \
					CREATE TABLE public.TEST (data varchar); \
INSERT INTO public.TEST VALUES('testt');\
					SELECT * FROM public.TEST WHERE TEST.data like 'should not be found'";
	
	const auto result = provider.GetDataSet(sql).extract<std::vector<moja::DynamicObject>>();
	BOOST_CHECK_EQUAL(result.size(), 0);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPostgres_GetDataSetReturnsNullDatabaseValues) {
	
	moja::datarepository::ProviderRelationalPocoPostgreSQL provider(settings);
	auto sql = "DROP TABLE IF EXISTS public.TEST1 ;\
CREATE TABLE public.TEST1 (id INT, data varchar) ; \
INSERT INTO public.TEST1 VALUES (1, NULL) ; \
SELECT id, data FROM public.Test1 WHERE id = 1";

	const auto result = provider.GetDataSet(sql).extract<std::vector<moja::DynamicObject>>();
	BOOST_CHECK_EQUAL(result.size(), 1);

	auto r0 = result.at(0);
	auto r0data = r0["data"];
	
	/*
	auto& r0 = result[0];
	auto r0data = r0["data"];
	BOOST_CHECK(r0data.isEmpty());
	*/
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPostgres_SimpleStringDataSet) {
	
	moja::datarepository::ProviderRelationalPocoPostgreSQL provider(settings);
	auto sql = "DROP TABLE IF EXISTS public.SimpleStringDataSet; \
CREATE TABLE public.SimpleStringDataSet (data varchar); \
INSERT INTO public.SimpleStringDataSet VALUES ('test');\
SELECT * FROM public.SimpleStringDataSet";

	const auto result = provider.GetDataSet(sql).extract<std::vector<moja::DynamicObject>>();
	BOOST_CHECK_EQUAL(result.size(), 1);

	/*
	auto& r0 = result[0];
	auto r0c0 = r0["data"].convert<std::string>();
	BOOST_CHECK_EQUAL(r0c0, "test");
	*/
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPostgres_SimpleIntDataSet) {

	moja::datarepository::ProviderRelationalPocoPostgreSQL provider(settings);
	auto sql = "DROP TABLE IF EXISTS public.SimpleIntDataSet; \
CREATE TABLE public.SimpleIntDataSet (data INT); \
INSERT INTO public.SimpleIntDataSet VALUES (1);\
SELECT * FROM public.SimpleIntDataSet";
	
	const auto result = provider.GetDataSet(sql).extract<std::vector<moja::DynamicObject>>();
	BOOST_CHECK_EQUAL(result.size(), 1);

	/*
	auto r0 = result[0];
	//auto r0c0 = r0["data"].convert<int>();
	auto r0c0 = r0["data"];
	//BOOST_CHECK_EQUAL(r0c0, 1);
	*/
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPostgres_SimpleBigIntDataSet) {
	
	moja::datarepository::ProviderRelationalPocoPostgreSQL provider(settings);
	auto sql = "DROP TABLE IF EXISTS public.SimpleBigIntDataSet;\
CREATE TABLE public.SimpleBigIntDataSet (data INT8); INSERT INTO public.SimpleBigIntDataSet VALUES (3000000000);\
SELECT * FROM public.SimpleBigIntDataSet";
		
	const auto result = provider.GetDataSet(sql).extract<std::vector<moja::DynamicObject>>();
	BOOST_CHECK_EQUAL(result.size(), 1);

	/*
	auto& r0 = result[0];
	//auto r0c0 = r0["data"].convert<Int64>();
	INT64 r0c0 = r0["data"];
	BOOST_CHECK_EQUAL(r0c0, 3000000000);
	*/
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPostgres_SimpleFloatDataSet) {

	moja::datarepository::ProviderRelationalPocoPostgreSQL provider(settings);
	auto sql = "DROP TABLE IF EXISTS public.SimpleFloatDataSet; \
CREATE TABLE public.SimpleFloatDataSet (data FLOAT4);\
INSERT INTO public.SimpleFloatDataSet VALUES (1.52);\
SELECT * FROM public.SimpleFloatDataSet";

	const auto result = provider.GetDataSet(sql).extract<std::vector<moja::DynamicObject>>();
	BOOST_CHECK_EQUAL(result.size(), 1);

	/*
	auto& r0 = result[0];
	auto r0c0 = r0["data"].convert<double>();
	BOOST_CHECK_EQUAL(r0c0, 1.5);
	*/
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPostgres_SimpleDoubleDataSet) {
	
	moja::datarepository::ProviderRelationalPocoPostgreSQL provider(settings);
	auto sql = "DROP TABLE IF EXISTS public.SimpleDoubleDataSet;\
CREATE TABLE public.SimpleDoubleDataSet (data FLOAT8);\
INSERT INTO public.SimpleDoubleDataSet VALUES (1.5345645);\
SELECT * FROM public.SimpleDoubleDataSet";
	
	const auto result = provider.GetDataSet(sql).extract<std::vector<moja::DynamicObject>>();
	BOOST_CHECK_EQUAL(result.size(), 1);

	/*
	auto& r0 = result[0];
	auto r0c0 = r0["data"].convert<double>();
	BOOST_CHECK_EQUAL(r0c0, 1.5);
	*/
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPostgres_ComplexMixedDataSet) {
	
	moja::datarepository::ProviderRelationalPocoPostgreSQL provider(settings);
	auto sql = "DROP TABLE IF EXISTS public.ComplexMixedDataSet;\
CREATE TABLE public.ComplexMixedDataSet (col0 varchar, col1 INT, col2 FLOAT8);\
INSERT INTO public.ComplexMixedDataSet VALUES ('first', 1, 1.5); \
INSERT INTO public.ComplexMixedDataSet VALUES ('second', 2, 2.5); \
SELECT * FROM public.ComplexMixedDataSet";

		
	const auto result = provider.GetDataSet(sql).extract<std::vector<moja::DynamicObject>>();
	BOOST_CHECK_EQUAL(result.size(), 2);

	/*
	auto& r0 = result[0];
	std::string r0c0 = r0["col0"].convert<std::string>();
	auto r0c1 = r0["col1"].convert<int>();
	auto r0c2 = r0["col2"].convert<double>();
	BOOST_CHECK_EQUAL(r0c0, "first");
	BOOST_CHECK_EQUAL(r0c1, 1);
	BOOST_CHECK_EQUAL(r0c2, 1.5);

	auto& r1 = result[1];
	auto r1c0 = r1["col0"].convert<std::string>();
	auto r1c1 = r1["col1"].convert<int>();
	auto r1c2 = r1["col2"].convert<double>();
	BOOST_CHECK_EQUAL(r1c0, "second");
	BOOST_CHECK_EQUAL(r1c1, 2);
	BOOST_CHECK_EQUAL(r1c2, 2.5);
	*/
}
#endif


BOOST_AUTO_TEST_SUITE_END();