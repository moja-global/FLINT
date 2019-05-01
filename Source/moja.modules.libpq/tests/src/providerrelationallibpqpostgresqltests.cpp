#include "moja/datarepository/datarepositoryexceptions.h"
#include "moja/modules/libpq//providerrelationallibpqpostgresql.h"

#include "moja/types.h"
#include "moja/dynamic.h"
#include "moja/logging.h"

#include <Poco/File.h>

#include <libpq-fe.h>

#include <boost/test/unit_test.hpp>
#include <boost/format.hpp>

#include <cstdlib>


///* for ntohl/htonl */
#ifdef _WIN32

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <winsock2.h>
#include <windows.h>
#else
// http://stackoverflow.com/questions/15076977/linux-ntohl-does-not-work-correctly
// unix includes here
#include <arpa/inet.h>
//#include <netinet/in.h>

//This is here to determine what __BYTE_ORDER is set to in netinet/in.h.
// Not in original code 
#if __BYTE_ORDER == __BIG_ENDIAN
#warning BIG ENDIAN BYTE ORDER!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#endif 

//This is here to determine what __BYTE_ORDER is set to in netinet/in.h. 
// Not in original code
#if __BYTE_ORDER == __LITTLE_ENDIAN
#warning YAY LITTLE ENDIAN!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#endif 
#endif

using moja::Int64;
using moja::modules::libpq::ProviderRelationalLibpqPostgreSQL;
using moja::datarepository::FileNotFoundException;
using moja::datarepository::ConnectionFailedException;
using moja::datarepository::QueryException;

#define TEST_SCHEMA "ProviderRelationalLibpqPostgresTestsFixture_Schema"

// Postgres connection parameters for testing
#define POSTGRESQL_USER "postgres"
#define POSTGRESQL_PWD  "admin"
#define POSTGRESQL_HOST "localhost"
#define POSTGRESQL_PORT "5432"
#define POSTGRESQL_DB   "postgres"

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
		auto pResult = PQexec(_conn, "DROP Schema IF EXISTS ProviderRelationalLibpqPostgresTestsFixture_Schema CASCADE; CREATE SCHEMA ProviderRelationalLibpqPostgresTestsFixture_Schema; DROP TABLE IF EXISTS ProviderRelationalLibpqPostgresTestsFixture_Schema.JunkTable; CREATE TABLE ProviderRelationalLibpqPostgresTestsFixture_Schema.JunkTable (id INTEGER);");
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
		auto pResult = PQexec(_conn, "DROP Schema IF EXISTS ProviderRelationalLibpqPostgresTestsFixture_Schema CASCADE;");
		if (PQresultStatus(pResult) != PGRES_COMMAND_OK) {
			MOJA_LOG_ERROR << "Command failed: " << PQerrorMessage(_conn);
			PQclear(pResult);
		}
		PQfinish(_conn);
	}

	static void	exit_nicely(PGconn *conn) {
		PQfinish(conn);
		MOJA_LOG_ERROR << "******* LibPQ testing exit_nicely *******";
		exit(1);
	}

	/*
	* This function prints a query result that is a binary-format fetch from
	* a table defined as in the comment above.  We split it out because the
	* main() function uses it twice.
	*/
	static void
		show_binary_results(PGresult *res) {
		int         i,
			j;
		int         i_fnum,
			t_fnum,
			b_fnum;

		/* Use PQfnumber to avoid assumptions about field order in result */
		i_fnum = PQfnumber(res, "i");
		t_fnum = PQfnumber(res, "t");
		b_fnum = PQfnumber(res, "b");

		for (i = 0; i < PQntuples(res); i++)
		{
			char       *iptr;
			char       *tptr;
			char       *bptr;
			int         blen;
			int         ival;

			/* Get the field values (we ignore possibility they are null!) */
			iptr = PQgetvalue(res, i, i_fnum);
			tptr = PQgetvalue(res, i, t_fnum);
			bptr = PQgetvalue(res, i, b_fnum);

			/*
			* The binary representation of INT4 is in network byte order, which
			* we'd better coerce to the local byte order.
			*/
			ival = ntohl(*((uint32_t *)iptr));

			/*
			* The binary representation of TEXT is, well, text, and since libpq
			* was nice enough to append a zero byte to it, it'll work just fine
			* as a C string.
			*
			* The binary representation of BYTEA is a bunch of bytes, which could
			* include embedded nulls so we have to pay attention to field length.
			*/
			blen = PQgetlength(res, i, b_fnum);

			printf("tuple %d: got\n", i);
			printf(" i = (%d bytes) %d\n",		PQgetlength(res, i, i_fnum), ival);
			printf(" t = (%d bytes) '%s'\n",	PQgetlength(res, i, t_fnum), tptr);
			printf(" b = (%d bytes) ", blen);
			for (j = 0; j < blen; j++)
				printf("\\%03o", bptr[j]);
			printf("\n\n");
		}
	}

};

BOOST_FIXTURE_TEST_SUITE(ProviderRelationalPostgresTests, ProviderRelationalPostgresTestsFixture);

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalLibpqPostgreSQL_libpq_simple_query_with_bin_and_text) {

	SendQuery("CREATE TABLE ProviderRelationalLibpqPostgresTestsFixture_Schema.test1(i int4, t text, b bytea);");
	SendQuery("INSERT INTO ProviderRelationalLibpqPostgresTestsFixture_Schema.test1 values(1, 'joe''s place', '\\000\\001\\002\\003\\004');INSERT INTO ProviderRelationalLibpqPostgresTestsFixture_Schema.test1 values(2, 'ho there', '\\004\\003\\002\\001\\000');");

	//const char *conninfo;
	PGconn     *conn;
	PGresult   *res;
	const char *paramValues[1];
	int         paramLengths[1];
	int         paramFormats[1];
	uint32_t    binaryIntVal;

	/*
	* If the user supplies a parameter on the command line, use it as the
	* conninfo string; otherwise default to setting dbname=postgres and using
	* environment variables or defaults for all other connection parameters.
	*/
	//conninfo = "dbname = postgres";
	//conninfo = "host=localhost user=postgres password=admin dbname=postgres port=5432";
	/* Make a connection to the database */
	//conn = PQconnectdb(conninfo);
	conn = PQconnectdb(connectionstring.c_str());

	/* Check to see that the backend connection was successfully made */
	if (PQstatus(conn) != CONNECTION_OK)
	{
		fprintf(stderr, "Connection to database failed: %s",
			PQerrorMessage(conn));
		exit_nicely(conn);
	}

	/*
	* The point of this program is to illustrate use of PQexecParams() with
	* out-of-line parameters, as well as binary transmission of data.
	*
	* This first example transmits the parameters as text, but receives the
	* results in binary format.  By using out-of-line parameters we can avoid
	* a lot of tedious mucking about with quoting and escaping, even though
	* the data is text.  Notice how we don't have to do anything special with
	* the quote mark in the parameter value.
	*/

	/* Here is our out-of-line parameter value */
	paramValues[0] = "joe's place";

	res = PQexecParams(conn,
		"SELECT * FROM ProviderRelationalLibpqPostgresTestsFixture_Schema.test1 WHERE t = $1",
		1,       /* one param */
		NULL,    /* let the backend deduce param type */
		paramValues,
		NULL,    /* don't need param lengths since text */
		NULL,    /* default to all text params */
		1);      /* ask for binary results */

	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
		PQclear(res);
		exit_nicely(conn);
	}

	show_binary_results(res);

	PQclear(res);

	/*
	* In this second example we transmit an integer parameter in binary form,
	* and again retrieve the results in binary form.
	*
	* Although we tell PQexecParams we are letting the backend deduce
	* parameter type, we really force the decision by casting the parameter
	* symbol in the query text.  This is a good safety measure when sending
	* binary parameters.
	*/

	/* Convert integer value "2" to network byte order */
	binaryIntVal = htonl((uint32_t)2);

	/* Set up parameter arrays for PQexecParams */
	paramValues[0] = (char *)&binaryIntVal;
	paramLengths[0] = sizeof(binaryIntVal);
	paramFormats[0] = 1;        /* binary */

	res = PQexecParams(conn,
		"SELECT * FROM ProviderRelationalLibpqPostgresTestsFixture_Schema.test1 WHERE i = $1::int4",
		1,       /* one param */
		NULL,    /* let the backend deduce param type */
		paramValues,
		paramLengths,
		paramFormats,
		1);      /* ask for binary results */

	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
		PQclear(res);
		exit_nicely(conn);
	}

	show_binary_results(res);

	PQclear(res);

	/* close the connection to the database and cleanup */
	PQfinish(conn);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalLibpqPostgreSQL_libpq_using_copy_to_insert) {
	//const char *conninfo;
	PGconn     *conn;
	PGresult   *res;

	SendQuery("CREATE TABLE ProviderRelationalLibpqPostgresTestsFixture_Schema.test_copy_table (Id INT PRIMARY KEY NOT NULL, item_01 TEXT,item_02 INT,item_03 CHAR(50),item_04 REAL)");
//	SendQuery("INSERT INTO ProviderRelationalLibpqPostgresTestsFixture_Schema.test_copy_table VALUES (1, 'test',1,'test text', 1.1111)");

	conn = PQconnectdb(connectionstring.c_str());

	/* Check to see that the backend connection was successfully made */
	if (PQstatus(conn) != CONNECTION_OK) {
		MOJA_LOG_INFO << "Connection to database failed: %s", PQerrorMessage(conn);
		exit_nicely(conn);
	}

	res = PQexec(conn, "COPY ProviderRelationalLibpqPostgresTestsFixture_Schema.test_copy_table FROM STDIN WITH NULL AS '';");
	if (PQresultStatus(res) != PGRES_COPY_IN) {
		MOJA_LOG_INFO <<  "COPY failed: %s", PQerrorMessage(conn);
		PQclear(res);
		exit_nicely(conn);
	}

	const int numberOfTestRecords = 100000;
	//
	// Loop a number of times and build string to insert data into postgres
	//
	for (auto i = 1; i <= numberOfTestRecords; i++) {
		auto str = (boost::format("%1%\t%2%\t%3%\t%4%\t%5%\n") % i % "item 01" % 2 % "item 03" % 1.11111).str();
		if (PQputCopyData(conn, str.c_str(), int(str.length())) != 1) {
			MOJA_LOG_INFO << "PQputCopyData failed: %s", PQerrorMessage(conn);
			PQclear(res);
			exit_nicely(conn);
		}
	}

	if (PQputCopyEnd(conn, nullptr) == 1) {
		PGresult *res = PQgetResult(conn);
		if (PQresultStatus(res) == PGRES_COMMAND_OK) {
			MOJA_LOG_INFO << "done";
		} else {
			MOJA_LOG_INFO << PQerrorMessage(conn);
		}
		PQclear(res);
	} else {
		MOJA_LOG_INFO << "PQputCopyEnd failed: %s", PQerrorMessage(conn);
		PQclear(res);
		exit_nicely(conn);
	}
	PQfinish(conn);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalLibpqPostgreSQL_ConstructorThrowsExceptionIfTargetDatabaseNotFound) {
	connectionstring = "host=" POSTGRESQL_HOST
		" user=" POSTGRESQL_USER
		" password=" POSTGRESQL_PWD
		" dbname=DoesNotExist"
		" port=" POSTGRESQL_PORT;
	settings["connectionstring"] = connectionstring;

	BOOST_CHECK_THROW(ProviderRelationalLibpqPostgreSQL provider(settings), ConnectionFailedException);
}

//BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalLibpqPostgreSQL_OpenDatabase) {
//	auto inMemoryPaths = { ":memory:", "file::memory:?cache=shared" };
//	for (auto path : inMemoryPaths) {
//		settings["path"] = path;
//		ProviderRelationalLibpqPostgreSQL provider(settings);
//	}
//}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalLibpqPostgreSQL_OpenValidDatabase) {
	ProviderRelationalLibpqPostgreSQL provider(settings);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalLibpqPostgreSQL_GetDataSetThrowsExceptionOnBadSQL) {
	ProviderRelationalLibpqPostgreSQL provider(settings);
	auto sql = "not a valid query";
	BOOST_CHECK_THROW(provider.GetDataSet(sql), QueryException);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalLibpqPostgreSQL_GetDataSetThrowsExceptionIfTableNotFound) {
	ProviderRelationalLibpqPostgreSQL provider(settings);
	auto sql = "SELECT * FROM ProviderRelationalLibpqPostgresTestsFixture_Schema.nonexistent_table";
	BOOST_CHECK_THROW(provider.GetDataSet(sql), QueryException);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalLibpqPostgreSQL_GetDataSetReturnsEmptyTableIfQueryReturnsNoResults) {
	// Setup
	SendQuery("CREATE TABLE ProviderRelationalLibpqPostgresTestsFixture_Schema.Test (data VARCHAR)");
	SendQuery("INSERT INTO ProviderRelationalLibpqPostgresTestsFixture_Schema.Test VALUES ('test')");

	// Test
	ProviderRelationalLibpqPostgreSQL provider(settings);
	auto sql = "SELECT * FROM ProviderRelationalLibpqPostgresTestsFixture_Schema.Test WHERE Test.data LIKE 'should not be found'";
	const auto result = provider.GetDataSet(sql).extract<std::vector<moja::DynamicObject>>();
	BOOST_CHECK_EQUAL(result.size(), 0);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalLibpqPostgreSQL_GetDataSetReturnsNullDatabaseValues) {
	// Setup
	SendQuery("CREATE TABLE ProviderRelationalLibpqPostgresTestsFixture_Schema.Test (id INT, data VARCHAR)");
	SendQuery("INSERT INTO ProviderRelationalLibpqPostgresTestsFixture_Schema.Test VALUES (1, NULL)");

	// Test
	ProviderRelationalLibpqPostgreSQL provider(settings);
	auto sql = "SELECT id, data FROM ProviderRelationalLibpqPostgresTestsFixture_Schema.Test WHERE id = 1";
	const auto result = provider.GetDataSet(sql).extract<std::vector<moja::DynamicObject>>();
	BOOST_CHECK_EQUAL(result.size(), 1);

	auto& r0 = result[0];
	auto r0data = r0["data"];
	BOOST_CHECK(r0data.isEmpty());
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalLibpqPostgreSQL_SimpleStringDataSet) {
	// Setup
	SendQuery("CREATE TABLE ProviderRelationalLibpqPostgresTestsFixture_Schema.SimpleStringDataSet (data VARCHAR)");
	SendQuery("INSERT INTO ProviderRelationalLibpqPostgresTestsFixture_Schema.SimpleStringDataSet VALUES ('test')");

	// Test
	ProviderRelationalLibpqPostgreSQL provider(settings);
	auto sql = "SELECT * FROM ProviderRelationalLibpqPostgresTestsFixture_Schema.SimpleStringDataSet";
	const auto result = provider.GetDataSet(sql).extract<std::vector<moja::DynamicObject>>();
	BOOST_CHECK_EQUAL(result.size(), 1);

	auto& r0 = result[0];
	auto r0c0 = r0["data"].convert<std::string>();
	BOOST_CHECK_EQUAL(r0c0, "test");
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalLibpqPostgreSQL_SimpleIntDataSet) {
	// Setup
	SendQuery("CREATE TABLE ProviderRelationalLibpqPostgresTestsFixture_Schema.SimpleIntDataSet (data INT)");
	SendQuery("INSERT INTO ProviderRelationalLibpqPostgresTestsFixture_Schema.SimpleIntDataSet VALUES (1)");

	// Test
	ProviderRelationalLibpqPostgreSQL provider(settings);
	auto sql = "SELECT * FROM ProviderRelationalLibpqPostgresTestsFixture_Schema.SimpleIntDataSet";
	const auto result = provider.GetDataSet(sql).extract<std::vector<moja::DynamicObject>>();
	BOOST_CHECK_EQUAL(result.size(), 1);

	auto r0 = result[0];
	auto r0c0 = r0["data"].convert<int>();
	BOOST_CHECK_EQUAL(r0c0, 1);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalLibpqPostgreSQL_SimpleBigIntDataSet) {
	// Setup
	SendQuery("CREATE TABLE ProviderRelationalLibpqPostgresTestsFixture_Schema.SimpleBigIntDataSet (data INT8)");
	SendQuery("INSERT INTO ProviderRelationalLibpqPostgresTestsFixture_Schema.SimpleBigIntDataSet VALUES (3000000000)");

	// Test
	ProviderRelationalLibpqPostgreSQL provider(settings);
	auto sql = "SELECT * FROM ProviderRelationalLibpqPostgresTestsFixture_Schema.SimpleBigIntDataSet";
	const auto result = provider.GetDataSet(sql).extract<std::vector<moja::DynamicObject>>();
	BOOST_CHECK_EQUAL(result.size(), 1);

	auto& r0 = result[0];
	auto r0c0 = r0["data"].convert<Int64>();
	BOOST_CHECK_EQUAL(r0c0, 3000000000);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalLibpqPostgreSQL_SimpleDoubleDataSet) {
	// Setup
	SendQuery("CREATE TABLE ProviderRelationalLibpqPostgresTestsFixture_Schema.SimpleDoubleDataSet (data DOUBLE PRECISION)");
	SendQuery("INSERT INTO ProviderRelationalLibpqPostgresTestsFixture_Schema.SimpleDoubleDataSet VALUES (1.5)");

	// Test
	ProviderRelationalLibpqPostgreSQL provider(settings);
	auto sql = "SELECT * FROM ProviderRelationalLibpqPostgresTestsFixture_Schema.SimpleDoubleDataSet";
	const auto result = provider.GetDataSet(sql).extract<std::vector<moja::DynamicObject>>();
	BOOST_CHECK_EQUAL(result.size(), 1);

	auto& r0 = result[0];
	auto r0c0 = r0["data"].convert<double>();
	BOOST_CHECK_EQUAL(r0c0, 1.5);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalLibpqPostgreSQL_ComplexMixedDataSet) {
	// Setup
	SendQuery("CREATE TABLE ProviderRelationalLibpqPostgresTestsFixture_Schema.ComplexMixedDataSet (col0 VARCHAR, col1 INT, col2 DOUBLE PRECISION)");
	SendQuery("INSERT INTO ProviderRelationalLibpqPostgresTestsFixture_Schema.ComplexMixedDataSet VALUES ('first', 1, 1.5)");
	SendQuery("INSERT INTO ProviderRelationalLibpqPostgresTestsFixture_Schema.ComplexMixedDataSet VALUES ('second', 2, 2.5)");

	// Test
	ProviderRelationalLibpqPostgreSQL provider(settings);
	auto sql = "SELECT * FROM ProviderRelationalLibpqPostgresTestsFixture_Schema.ComplexMixedDataSet";
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

		moja::datarepository::ProviderRelationalLibpqPostgreSQL provider(settings);
		auto sql = "DROP TABLE IF EXISTS public.PTEST1; \
					CREATE TABLE public.PTEST1 as SELECT * FROM public.NEW_TEST nt where nt.amname like 'h%'; \
					SELECT * FROM public.PTEST1";
		provider.GetDataSet(sql);

}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPostgres_BasicTableDataTests3) {

	moja::datarepository::ProviderRelationalLibpqPostgreSQL provider(settings);
	auto sql = "DROP TABLE IF EXISTS public.COMPANY; \
					CREATE TABLE public.COMPANY (Id INT PRIMARY KEY NOT NULL, Name TEXT,AGE INT,Address CHAR(50),Sal REAL); \
INSERT INTO public.COMPANY VALUES(1,'a123',29,'123 abc st',2.2); \
					SELECT * FROM public.COMPANY";

	provider.GetDataSet(sql);

}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPostgres_GetDataSetThrowsExceptionOnBadSQL) {
	
	moja::datarepository::ProviderRelationalLibpqPostgreSQL provider(settings);
	auto sql = "not a valid query";
	BOOST_CHECK_THROW(provider.GetDataSet(sql), moja::datarepository::QueryException);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPostgres_GetDataSetThrowsExceptionIfTableNotFound) {
	
	moja::datarepository::ProviderRelationalLibpqPostgreSQL provider(settings);
	auto sql = "SELECT * FROM public.nonexistent_table";
	BOOST_CHECK_THROW(provider.GetDataSet(sql), moja::datarepository::QueryException);
}


BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPostgres_GetDataSetReturnsEmptyTableIfQueryReturnsNoResults) {
	
	moja::datarepository::ProviderRelationalLibpqPostgreSQL provider(settings);
	auto sql = "DROP TABLE IF EXISTS public.TEST; \
					CREATE TABLE public.TEST (data varchar); \
INSERT INTO public.TEST VALUES('testt');\
					SELECT * FROM public.TEST WHERE TEST.data like 'should not be found'";
	
	const auto result = provider.GetDataSet(sql).extract<std::vector<moja::DynamicObject>>();
	BOOST_CHECK_EQUAL(result.size(), 0);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalPostgres_GetDataSetReturnsNullDatabaseValues) {
	
	moja::datarepository::ProviderRelationalLibpqPostgreSQL provider(settings);
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
	
	moja::datarepository::ProviderRelationalLibpqPostgreSQL provider(settings);
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

	moja::datarepository::ProviderRelationalLibpqPostgreSQL provider(settings);
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
	
	moja::datarepository::ProviderRelationalLibpqPostgreSQL provider(settings);
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

	moja::datarepository::ProviderRelationalLibpqPostgreSQL provider(settings);
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
	
	moja::datarepository::ProviderRelationalLibpqPostgreSQL provider(settings);
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
	
	moja::datarepository::ProviderRelationalLibpqPostgreSQL provider(settings);
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

#undef POSTGRESQL_USER
#undef POSTGRESQL_PWD
#undef POSTGRESQL_HOST
#undef POSTGRESQL_PORT
#undef POSTGRESQL_DB

BOOST_AUTO_TEST_SUITE_END();