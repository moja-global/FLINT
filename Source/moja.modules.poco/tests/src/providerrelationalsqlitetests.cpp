#include "moja/datarepository/providerrelationalsqlite.h"
#include "moja/datarepository/datarepositoryexceptions.h"

#include "moja/dynamic.h"
#include "moja/types.h"

#include <Poco/Data/Session.h>
#include <Poco/Data/SQLite/Connector.h>
#include <Poco/File.h>

#include <boost/test/unit_test.hpp>

using moja::Int64;
using moja::datarepository::ProviderRelationalSQLite;
using moja::datarepository::FileNotFoundException;
using moja::datarepository::QueryException;

using Poco::Data::Keywords::now;

class SQLiteConnector {
public:
    SQLiteConnector() {
        Poco::Data::SQLite::Connector::registerConnector();
    }
};

struct ProviderRelationalSQLiteTestsFixture {
	const std::string DB_PATH = "ProviderRelationalSQLiteTests.db";
	moja::DynamicObject settings;
    SQLiteConnector connector;
	Poco::Data::Session session;

	ProviderRelationalSQLiteTestsFixture() : session("SQLite", DB_PATH) {
		settings["path"] = DB_PATH;
		Poco::Data::SQLite::Connector::registerConnector();
	}

	~ProviderRelationalSQLiteTestsFixture() {
		session.close();
		Poco::File db(DB_PATH);
		db.remove();
	}
};

BOOST_FIXTURE_TEST_SUITE(ProviderRelationalSQLiteTests, ProviderRelationalSQLiteTestsFixture);

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalSQLite_ConstructorThrowsExceptionIfTargetDatabaseNotFound) {
	settings["path"] = "test";
	BOOST_CHECK_THROW(ProviderRelationalSQLite provider(settings), FileNotFoundException);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalSQLite_OpenInMemoryDatabase) {
	auto inMemoryPaths = { ":memory:", "file::memory:?cache=shared" };
	for (auto path : inMemoryPaths) {
		settings["path"] = path;
		ProviderRelationalSQLite provider(settings);
	}
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalSQLite_OpenValidDatabase) {
	ProviderRelationalSQLite provider(settings);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalSQLite_GetDataSetThrowsExceptionOnBadSQL) {
	ProviderRelationalSQLite provider(settings);
	auto sql = "not a valid query";
	BOOST_CHECK_THROW(provider.GetDataSet(sql), QueryException);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalSQLite_GetDataSetThrowsExceptionIfTableNotFound) {
	ProviderRelationalSQLite provider(settings);
	auto sql = "SELECT * FROM nonexistent_table";
	BOOST_CHECK_THROW(provider.GetDataSet(sql), QueryException);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalSQLite_GetDataSetReturnsEmptyTableIfQueryReturnsNoResults) {
	// Setup
	session << "CREATE TABLE Test (data VARCHAR)", now;
	session << "INSERT INTO Test VALUES ('test')", now;

	// Test
	ProviderRelationalSQLite provider(settings);
	auto sql = "SELECT * FROM Test WHERE Test.data LIKE 'should not be found'";
	const auto result = provider.GetDataSet(sql).extract<std::vector<moja::DynamicObject>>();
	BOOST_CHECK_EQUAL(result.size(), 0);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalSQLite_GetDataSetReturnsNullDatabaseValues) {
	// Setup
	session << "CREATE TABLE Test (id, data VARCHAR)", now;
	session << "INSERT INTO Test VALUES (1, NULL)", now;

	// Test
	ProviderRelationalSQLite provider(settings);
	auto sql = "SELECT id, data FROM Test WHERE id = 1";
	const auto result = provider.GetDataSet(sql).extract<std::vector<moja::DynamicObject>>();
	BOOST_CHECK_EQUAL(result.size(), 1);

	auto& r0 = result[0];
	auto r0data = r0["data"];
	BOOST_CHECK_EQUAL(r0data.isEmpty(), true);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalSQLite_SimpleStringDataSet) {
	// Setup
	session << "CREATE TABLE SimpleStringDataSet (data VARCHAR)", now;
	session << "INSERT INTO SimpleStringDataSet VALUES ('test')", now;

	// Test
	ProviderRelationalSQLite provider(settings);
	auto sql = "SELECT * FROM SimpleStringDataSet";
	const auto result = provider.GetDataSet(sql).extract<std::vector<moja::DynamicObject>>();
	BOOST_CHECK_EQUAL(result.size(), 1);

	auto& r0 = result[0];
	auto r0c0 = r0["data"].convert<std::string>();
	BOOST_CHECK_EQUAL(r0c0, "test");
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalSQLite_SimpleIntDataSet) {
	// Setup
	session << "CREATE TABLE SimpleIntDataSet (data INTEGER)", now;
	session << "INSERT INTO SimpleIntDataSet VALUES (1)", now;

	// Test
	ProviderRelationalSQLite provider(settings);
	auto sql = "SELECT * FROM SimpleIntDataSet";
	const auto result = provider.GetDataSet(sql).extract<std::vector<moja::DynamicObject>>();
	BOOST_CHECK_EQUAL(result.size(), 1);

	auto r0 = result[0];
	auto r0c0 = r0["data"].convert<int>();
	BOOST_CHECK_EQUAL(r0c0, 1);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalSQLite_SimpleBigIntDataSet) {
	// Setup
	session << "CREATE TABLE SimpleBigIntDataSet (data BIGINT)", now;
	session << "INSERT INTO SimpleBigIntDataSet VALUES (3000000000)", now;

	// Test
	ProviderRelationalSQLite provider(settings);
	auto sql = "SELECT * FROM SimpleBigIntDataSet";
	const auto result = provider.GetDataSet(sql).extract<std::vector<moja::DynamicObject>>();
	BOOST_CHECK_EQUAL(result.size(), 1);

	auto& r0 = result[0];
	auto r0c0 = r0["data"].convert<Int64>();
	BOOST_CHECK_EQUAL(r0c0, 3000000000);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalSQLite_SimpleDoubleDataSet) {
	// Setup
	session << "CREATE TABLE SimpleDoubleDataSet (data DOUBLE)", now;
	session << "INSERT INTO SimpleDoubleDataSet VALUES (1.5)", now;

	// Test
	ProviderRelationalSQLite provider(settings);
	auto sql = "SELECT * FROM SimpleDoubleDataSet";
	const auto result = provider.GetDataSet(sql).extract<std::vector<moja::DynamicObject>>();
	BOOST_CHECK_EQUAL(result.size(), 1);

	auto& r0 = result[0];
	auto r0c0 = r0["data"].convert<double>();
	BOOST_CHECK_EQUAL(r0c0, 1.5);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderRelationalSQLite_ComplexMixedDataSet) {
	// Setup
	session << "CREATE TABLE ComplexMixedDataSet (col0 VARCHAR, col1 INTEGER, col2 DOUBLE)", now;
	session << "INSERT INTO ComplexMixedDataSet VALUES ('first', 1, 1.5)", now;
	session << "INSERT INTO ComplexMixedDataSet VALUES ('second', 2, 2.5)", now;

	// Test
	ProviderRelationalSQLite provider(settings);
	auto sql = "SELECT * FROM ComplexMixedDataSet";
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

BOOST_AUTO_TEST_SUITE_END();
