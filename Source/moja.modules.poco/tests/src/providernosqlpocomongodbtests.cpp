#include "moja/modules/poco/providernosqlpocomongodb.h"
#include "moja/datarepository/datarepositoryexceptions.h"
#include "moja/modules/poco/pocomongoutils.h"

#include "moja/dynamic.h"
#include "moja/logging.h"

#include <Poco/MongoDB/Connection.h>
#include <Poco/MongoDB/Database.h>
#include <Poco/MongoDB/QueryRequest.h>
#include <Poco/MongoDB/Document.h>
#include <Poco/MongoDB/Array.h>
#include <Poco/JSON/Array.h>
#include <Poco/JSON/Parser.h>
#include <Poco/File.h>
#include <Poco/FileStream.h>
#include <Poco/StreamCopier.h>

#include <boost/test/unit_test.hpp>
#include <boost/format.hpp>

using moja::modules::poco::ProviderNoSQLPocoMongoDB;
using moja::datarepository::FileNotFoundException;
using moja::datarepository::ConnectionFailedException;
using moja::datarepository::QueryException;
using moja::datarepository::AssertionViolationException;

struct ProviderNoSQLPocoMongoDBTestsFixture {
	const std::string TEST_JSON_PATH = "providernosqlpocomongodbtestsdata_trees.json";

	moja::DynamicObject settings;
	std::string host;
	int port;
	std::string database;
	std::string collection;


	ProviderNoSQLPocoMongoDBTestsFixture() {
		host = "localhost";
		port = 27017;
		database = "ProviderNoSQLPocoMongoDBTests_Database";
		collection = "ProviderNoSQLPocoMongoDBTests_collection";

		settings["host"] = host;
		settings["port"] = port;
		settings["database"] = database;
		settings["collection"] = collection;

		try {
			// -- Read test JSON file
			Poco::File file(TEST_JSON_PATH);
			std::ostringstream ostr;
			if (file.exists()) {
				Poco::FileInputStream fis(TEST_JSON_PATH);
				Poco::StreamCopier::copyStream(fis, ostr);
				fis.close();
			}

			// -- Parse JSON into Poco::MongoDb::Document
			Poco::JSON::Parser jsonParser;
			auto parsedJSON = jsonParser.parse(ostr.str());
			auto parsedResult = jsonParser.result();
			auto document = moja::modules::poco::parsePocoJSONToMongoDBObj(parsedResult);

			// -- Connection and Database
			auto con = Poco::MongoDB::Connection();
			con.connect(host, port);
			Poco::MongoDB::Database db(database);

			// -- Drop if it exists already
			auto dropPtr = db.createCommand();
			dropPtr->selector().add("dropDatabase", 1);
			Poco::MongoDB::ResponseMessage dropResponse;
			con.sendRequest(*dropPtr, dropResponse);

			// -- Create if it exists already
			auto cmdPtr = db.createCommand();
			cmdPtr->selector().add("create", collection);
			Poco::MongoDB::ResponseMessage createResponse;
			con.sendRequest(*cmdPtr, createResponse);

			// -- Insert records from Test JSON file
			auto insertPtr = db.createInsertRequest(collection);
#if 1
			std::vector<std::string> elementNames;
			document->elementNames(elementNames);

			for (auto& elementName : elementNames) {
				if (document->isType<Poco::MongoDB::Document::Ptr>(elementName)) {
					auto subDoc = document->get<Poco::MongoDB::Document::Ptr>(elementName);
					insertPtr->documents().push_back(subDoc);
				}
			}
#else
			for (int i = 0; i < 10000; ++i) {

				std::random_device rd; // obtain a random number from hardware
				std::mt19937 eng(rd()); // seed the generator
				std::uniform_int_distribution<> distr(0, 10); // define the range
				int forest_type = distr(eng);

				Poco::MongoDB::Document::Ptr doc = new Poco::MongoDB::Document();
				doc->add("number", i);
				doc->add("forest_type", forest_type);
				insertPtr->documents().push_back(doc);
			}
#endif
			con.sendRequest(*insertPtr);
			con.disconnect();
		}
		catch (Poco::FileNotFoundException e) {
			MOJA_LOG_INFO << "FileNotFoundException: " << e.message();
		}
		catch (Poco::Exception e) {
			MOJA_LOG_INFO << "Poco::Exception: " << e.message();
		}
		catch (...) {
			std::cout << "Exception" << std::endl;
		}
	}

	~ProviderNoSQLPocoMongoDBTestsFixture() {
		try {
			auto con = Poco::MongoDB::Connection();
			con.connect(host, port);

			Poco::MongoDB::Database db(database);

			// -- Drop database
			auto cmdPtr = db.createCommand();
			cmdPtr->selector().add("dropDatabase", 1);
			Poco::MongoDB::ResponseMessage response;
			con.sendRequest(*cmdPtr, response);

			con.disconnect();
		}
		catch (Poco::FileNotFoundException e) {
			MOJA_LOG_INFO << "FileNotFoundException: " << e.message();
		}
		catch (Poco::Exception e) {
			MOJA_LOG_INFO << "Exception caught, could be that mongo isn't running: " << host << ", " << port;
		}
		catch (...) {
			std::cout << "Exception" << std::endl;
		}
	}
};

BOOST_FIXTURE_TEST_SUITE(ProviderNoSQLPocoMongoDBTests, ProviderNoSQLPocoMongoDBTestsFixture);

BOOST_AUTO_TEST_CASE(datarepository_ProviderNoSQLPocoMongoDB_ParseJSON_001) {

	auto testName = boost::unit_test::framework::current_test_case().p_name;
	auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(boost::unit_test::framework::current_test_case().p_parent_id)).full_name();
	std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ') << testName << std::endl;

	std::vector<std::pair<std::string, std::string>> testQueryAndExpected = {
		{ "[{\"value\":2},{\"value\":2}]",			"[{\"value\":2},{\"value\":2}]" },
		{ "{\"forest_type\": { \"$in\" : [2, 3]}}",		"{\"forest_type\":{\"$in\":[2,3]}}" },
		{ "{\"forest_type\": { \"$in\" : [2, 3]}}",		"{\"forest_type\":{\"$in\":[2,3]}}" }
	};

	try {
		for (auto test : testQueryAndExpected) {
			// Parse the JSON
			std::string name;
			std::map<std::string, std::string> metaData;

			Poco::JSON::Parser jsonParser;
			auto parsedJSON = jsonParser.parse(test.first);
			auto parsedResult = jsonParser.result();
			auto doc = moja::modules::poco::parsePocoJSONToMongoDBObj(parsedResult);
			auto response = doc->toString();

			BOOST_CHECK_EQUAL(response, test.second);
		}
	}
	catch (Poco::FileNotFoundException e) {
		MOJA_LOG_INFO << "FileNotFoundException: " << e.message();
	}
	catch (Poco::Exception e) {
		MOJA_LOG_INFO << "Poco::Exception: " << e.message();
	}
	catch (...) {
		std::cout << "Exception" << std::endl;
	}
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderNoSQLPocoMongoDB_ConstructorThrowsExceptionIfTargetDatabaseNotFound) {
	auto testName = boost::unit_test::framework::current_test_case().p_name;
	auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(boost::unit_test::framework::current_test_case().p_parent_id)).full_name();
	std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ') << testName << std::endl;

	settings["host"] = "fail";
	settings["port"] = 27017;

	BOOST_CHECK_THROW(ProviderNoSQLPocoMongoDB provider(settings), ConnectionFailedException);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderNoSQLPocoMongoDB_OpenValidDatabase) {
	auto testName = boost::unit_test::framework::current_test_case().p_name;
	auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(boost::unit_test::framework::current_test_case().p_parent_id)).full_name();
	std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ') << testName << std::endl;

	try {
		ProviderNoSQLPocoMongoDB provider(settings);
	}
	catch (Poco::FileNotFoundException e) {
		MOJA_LOG_INFO << "FileNotFoundException: " << e.message();
	}
	catch (Poco::Exception e) {
		MOJA_LOG_INFO << "Poco::Exception: " << e.message();
	}
	catch (...) {
		std::cout << "Exception" << std::endl;
	}
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderNoSQLPocoMongoDB_QueryTest) {
	auto testName = boost::unit_test::framework::current_test_case().p_name;
	auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(boost::unit_test::framework::current_test_case().p_parent_id)).full_name();
	std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ') << testName << std::endl;

	try {
		std::string host = settings["host"];
		int port = settings["port"];
		std::string database = settings["database"];
		std::string collection = settings["collection"];

		auto con = Poco::MongoDB::Connection();
		con.connect(host, port);

		Poco::MongoDB::Database db(database);
		auto queryPtr = db.createQueryRequest("trees");

		Poco::MongoDB::ResponseMessage response;
		con.sendRequest(*queryPtr, response);

		if (response.documents().size() > 0) {
			auto doc = *(response.documents()[0]);
		}
		else {
			// fail
		}
		con.disconnect();
	}
	catch (Poco::FileNotFoundException e) {
		MOJA_LOG_INFO << "FileNotFoundException: " << e.message();
	}
	catch (Poco::Exception e) {
		MOJA_LOG_INFO << "Poco::Exception: " << e.message();
	}
	catch (...) {
		std::cout << "Exception" << std::endl;
	}
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderNoSQLPocoMongoDB_QueryTest__IN) {
	auto testName = boost::unit_test::framework::current_test_case().p_name;
	auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(boost::unit_test::framework::current_test_case().p_parent_id)).full_name();
	std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ') << testName << std::endl;

	//  {"forest_type": { $in : [2, 3]}}
	try {
		std::string host = settings["host"];
		int port = settings["port"];
		std::string database = settings["database"];
		std::string collection = settings["collection"];

		auto con = Poco::MongoDB::Connection();
		con.connect(host, port);

		Poco::MongoDB::Database db(database);
		auto queryPtr = db.createQueryRequest("trees");

		Poco::MongoDB::Array::Ptr orArray = new Poco::MongoDB::Array();
		orArray->add("0", 2);
		orArray->add("1", 3);

		queryPtr->selector().addNewDocument("forest_type").add("$in", orArray);
		queryPtr->setNumberToReturn(2);

		Poco::MongoDB::ResponseMessage response;
		con.sendRequest(*queryPtr, response);

		auto str = queryPtr->selector().toString();

		if (response.documents().size() > 0) {
			auto doc = *(response.documents()[0]);
		}
		else {
			// fail
		}

		con.disconnect();
	}
	catch (Poco::FileNotFoundException e) {
		MOJA_LOG_INFO << "FileNotFoundException: " << e.message();
	}
	catch (Poco::Exception e) {
		MOJA_LOG_INFO << "Poco::Exception: " << e.message();
	}
	catch (...) {
		std::cout << "Exception" << std::endl;
	}
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderNoSQLPocoMongoDB_QueryTest__OR) {
	auto testName = boost::unit_test::framework::current_test_case().p_name;
	auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(boost::unit_test::framework::current_test_case().p_parent_id)).full_name();
	std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ') << testName << std::endl;

	//  {$or: [
	//		{"forest_type":2},
	//		{"forest_type":3}
	//		]
	//  }

	try {
		std::string host = settings["host"];
		int port = settings["port"];
		std::string database = settings["database"];
		std::string collection = settings["collection"];

		auto con = Poco::MongoDB::Connection();
		con.connect(host, port);

		Poco::MongoDB::Database db(database);
		auto queryPtr = db.createQueryRequest("trees");

		Poco::MongoDB::Document::Ptr match1(new Poco::MongoDB::Document());
		Poco::MongoDB::Document::Ptr match2(new Poco::MongoDB::Document());

		match1->add("forest_type", 2);
		match2->add("forest_type", 3);

		Poco::MongoDB::Array::Ptr orArray = new Poco::MongoDB::Array();
		orArray->add("0", match1);
		orArray->add("1", match2);

		queryPtr->selector().add("$or", orArray);
		//queryPtr->setNumberToReturn(2);

		Poco::MongoDB::ResponseMessage response;
		con.sendRequest(*queryPtr, response);

		auto str = queryPtr->selector().toString();

		if (response.documents().size() > 0) {
			auto doc = *(response.documents()[0]);
		}
		else {
			// fail
		}
		con.disconnect();
	}
	catch (Poco::FileNotFoundException e) {
		MOJA_LOG_INFO << "FileNotFoundException: " << e.message();
	}
	catch (Poco::Exception e) {
		MOJA_LOG_INFO << "Poco::Exception: " << e.message();
	}
	catch (...) {
		std::cout << "Exception" << std::endl;
	}
}


#if 0
// Removed the binary reader/writer version
BOOST_AUTO_TEST_CASE(datarepository_ProviderNoSQLPocoMongoDB_QueryTest_BinaryWriter) {
	auto testName = boost::unit_test::framework::current_test_case().p_name;
	auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(boost::unit_test::framework::current_test_case().p_parent_id)).full_name();
	std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ') << testName << std::endl;

	try {
		ProviderNoSQLPocoMongoDB provider(settings);

		const int bufferSize = 4096;
		char _buffer[bufferSize];

		// -- WRITE BLOCK
		Poco::MemoryOutputStream outStream(_buffer, bufferSize);
		Poco::BinaryWriter myBinaryWriter(outStream);

		/// -- Make a test document
		Poco::MongoDB::Document::Ptr mainDoc(new Poco::MongoDB::Document());
		Poco::MongoDB::Document::Ptr match1(new Poco::MongoDB::Document());
		Poco::MongoDB::Document::Ptr match2(new Poco::MongoDB::Document());

		match1->add("forest_type", 2);
		match2->add("forest_type", 3);

		Poco::MongoDB::Array::Ptr orArray = new Poco::MongoDB::Array();
		orArray->add("0", match1);
		orArray->add("1", match2);
		mainDoc->add("$or", orArray);

		mainDoc->write(myBinaryWriter);

		Poco::MemoryInputStream inStream(_buffer, bufferSize);
		Poco::BinaryReader myBinaryReader(inStream);

		provider.GetDataSet(myBinaryReader);
	}
	catch (Poco::FileNotFoundException e) {
			MOJA_LOG_INFO << "FileNotFoundException: " << e.message();
	}
	catch (Poco::Exception e) {
		MOJA_LOG_INFO << "Poco::Exception: " << e.message();
	}
	catch (...) {
		std::cout << "Exception" << std::endl;
	}
}
#endif

// Use a binary writer and reader to load a poco::mongodb::document created with our poco json parsing code
// then put this document into a query and call mongo

BOOST_AUTO_TEST_CASE(datarepository_ProviderNoSQLPocoMongoDB_JSON_Parse_Test_001) {
	auto testName = boost::unit_test::framework::current_test_case().p_name;
	auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(boost::unit_test::framework::current_test_case().p_parent_id)).full_name();
	std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ') << testName << std::endl;

	try {
		ProviderNoSQLPocoMongoDB provider(settings);

		std::string host = settings["host"];
		int port = settings["port"];
		std::string database = settings["database"];
		std::string collection = settings["collection"];

		const int bufferSize = 4096;
		char _buffer[bufferSize];

		Poco::MemoryOutputStream outStream(_buffer, bufferSize);
		Poco::BinaryWriter myBinaryWriter(outStream);

		std::string queryStr = "{\"forest_type\": { \"$in\" : [2, 3]}}";

		Poco::JSON::Parser jsonParser;
		auto parsedJSON = jsonParser.parse(queryStr);
		auto parsedResult = jsonParser.result();
		auto document = moja::modules::poco::parsePocoJSONToMongoDBObj(parsedResult);

		document->write(myBinaryWriter);

		Poco::MemoryInputStream inStream(_buffer, bufferSize);
		Poco::BinaryReader myBinaryReader(inStream);

		auto con = Poco::MongoDB::Connection();
		con.connect(host, port);

		Poco::MongoDB::Database db(database);
		auto queryPtr = db.createQueryRequest(collection);
		queryPtr->selector().read(myBinaryReader);

		auto str = queryPtr->selector().toString();

		Poco::MongoDB::ResponseMessage response;
		con.sendRequest(*queryPtr, response);

		if (response.documents().size() > 0) {
			auto doc = *(response.documents()[0]);
		}
		else {
			// fail
		}

		con.disconnect();
	}
	catch (Poco::FileNotFoundException e) {
		MOJA_LOG_INFO << "FileNotFoundException: " << e.message();
	}
	catch (Poco::Exception e) {
		MOJA_LOG_INFO << "Poco::Exception: " << e.message();
	}
	catch (...) {

	}
}

// Use an assignement to load a poco::mongodb::document created with our poco json parsing code
// then put this document into a query and call mongo

BOOST_AUTO_TEST_CASE(datarepository_ProviderNoSQLPocoMongoDB_JSON_Parse_Test_002) {
	auto testName = boost::unit_test::framework::current_test_case().p_name;
	auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(boost::unit_test::framework::current_test_case().p_parent_id)).full_name();
	std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ') << testName << std::endl;

	try {
		ProviderNoSQLPocoMongoDB provider(settings);

		std::string host = settings["host"];
		int port = settings["port"];
		std::string database = settings["database"];
		std::string collection = settings["collection"];

		std::string queryStr = "{\"forest_type\": { \"$in\" : [2, 3]}}";

		Poco::JSON::Parser jsonParser;
		auto parsedJSON = jsonParser.parse(queryStr);
		auto parsedResult = jsonParser.result();
		auto document = moja::modules::poco::parsePocoJSONToMongoDBObj(parsedResult);

		auto con = Poco::MongoDB::Connection();
		con.connect(host, port);

		Poco::MongoDB::Database db(database);
		auto queryPtr = db.createQueryRequest(collection);
		queryPtr->selector() = *document;

		auto str = queryPtr->selector().toString();

		Poco::MongoDB::ResponseMessage response;
		con.sendRequest(*queryPtr, response);

		if (response.documents().size() > 0) {
			auto doc = *(response.documents()[0]);
		}
		else {
			// fail
		}

		con.disconnect();
	}
	catch (Poco::FileNotFoundException e) {
		MOJA_LOG_INFO << "FileNotFoundException: " << e.message();
	}
	catch (Poco::Exception e) {
		MOJA_LOG_INFO << "Poco::Exception: " << e.message();
	}
	catch (...) {

	}
}

// Use an assignement to load a poco::mongodb::document created with our poco json parsing code
// then put this document into a query and call mongo

BOOST_AUTO_TEST_CASE(datarepository_ProviderNoSQLPocoMongoDB_JSON_Parse_Test_003) {
	auto testName = boost::unit_test::framework::current_test_case().p_name;
	auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(boost::unit_test::framework::current_test_case().p_parent_id)).full_name();
	std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ') << testName << std::endl;

	try {
		ProviderNoSQLPocoMongoDB provider(settings);

		std::string queryStr = "{\"forest_type\": { \"$in\" : [2, 3]}}";

		auto ret = provider.GetDataSet(queryStr);
	}
	catch (Poco::FileNotFoundException e) {
		MOJA_LOG_INFO << "FileNotFoundException: " << e.message();
	}
	catch (Poco::Exception e) {
		MOJA_LOG_INFO << "Poco::Exception: " << e.message();
	}
	catch (...) {

	}
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderNoSQLPocoMongoDB_Command_Test_001) {
	auto testName = boost::unit_test::framework::current_test_case().p_name;
	auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(boost::unit_test::framework::current_test_case().p_parent_id)).full_name();
	std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ') << testName << std::endl;

	try {
		ProviderNoSQLPocoMongoDB provider(settings);

		std::string cmdStr = "{\"create\": \"test_collection_create_command\"}";
		auto ret = provider.SendCmdRequest(cmdStr);
	}
	catch (Poco::FileNotFoundException e) {
		MOJA_LOG_INFO << "FileNotFoundException: " << e.message();
	}
	catch (Poco::Exception e) {
		MOJA_LOG_INFO << "Poco::Exception: " << e.message();
	}
	catch (...) {

	}
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderNoSQLPocoMongoDB_Query_Test_001) {
	auto testName = boost::unit_test::framework::current_test_case().p_name;
	auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(boost::unit_test::framework::current_test_case().p_parent_id)).full_name();
	std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ') << testName << std::endl;

	try {
		std::string collection = settings["collection"];
		ProviderNoSQLPocoMongoDB provider(settings);

		//auto cmdStr = (boost::format("{\"find\":\"%1%\",\"projection\":{\"_id\": 1}}") % collection).str();
	 	//std::string cmdStr = "{find:test_collection_create_command,projection:{_id: 1}}";
		std::string queryStr = "{}";
		std::string fieldsStr = "{\"_id\": 1 }";
		auto ret = provider.SendQueryRequest(queryStr, fieldsStr);
	}
	catch (Poco::FileNotFoundException e) {
		MOJA_LOG_INFO << "FileNotFoundException: " << e.message();
	}
	catch (Poco::Exception e) {
		MOJA_LOG_INFO << "Poco::Exception: " << e.message();
	}
	catch (...) {

	}
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderNoSQLPocoMongoDB_Query_Test_002) {
	auto testName = boost::unit_test::framework::current_test_case().p_name;
	auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(boost::unit_test::framework::current_test_case().p_parent_id)).full_name();
	std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ') << testName << std::endl;

	try {
		std::string collection = settings["collection"];
		ProviderNoSQLPocoMongoDB provider(settings);

		//auto cmdStr = (boost::format("{\"find\":\"%1%\",\"projection\":{\"_id\": 1}}") % collection).str();
		//std::string cmdStr = "{find:test_collection_create_command,projection:{_id: 1}}";
		std::string queryStr = "{}";
		std::string fieldsStr = "{\"_id\": 1 }";
		auto idSet = provider.SendQueryRequest(queryStr, fieldsStr);

		auto current = 0;
		for (auto& lu : idSet) {
			auto obj = lu.extract<const moja::DynamicObject>();
			auto id = obj["_id"].extract<const std::string>();

		}
	}
	catch (Poco::FileNotFoundException e) {
		MOJA_LOG_INFO << "FileNotFoundException: " << e.message();
	}
	catch (Poco::Exception e) {
		MOJA_LOG_INFO << "Poco::Exception: " << e.message();
	}
	catch (...) {

	}
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderNoSQLPocoMongoDB_Count_Test_001) {
	auto testName = boost::unit_test::framework::current_test_case().p_name;
	auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(boost::unit_test::framework::current_test_case().p_parent_id)).full_name();
	std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ') << testName << std::endl;

	try {
		ProviderNoSQLPocoMongoDB provider(settings);

		auto count = provider.SendCountRequest();
	}
	catch (Poco::FileNotFoundException e) {
		MOJA_LOG_INFO << "FileNotFoundException: " << e.message();
	}
	catch (Poco::Exception e) {
		MOJA_LOG_INFO << "Poco::Exception: " << e.message();
	}
	catch (...) {

	}
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderNoSQLPocoMongoDB_INCAS_Test_001) {
	auto testName = boost::unit_test::framework::current_test_case().p_name;
	auto testSuiteName = (boost::unit_test::framework::get<boost::unit_test::test_suite>(boost::unit_test::framework::current_test_case().p_parent_id)).full_name();
	std::cout << std::setw(40) << std::setfill(' ') << testSuiteName << ": " << std::setw(50) << std::setfill(' ') << testName << std::endl;

	try {
		moja::DynamicObject settingsEstates;
		moja::DynamicObject settingsPlots;
		settingsEstates["host"] = host;
		settingsEstates["port"] = port;
		settingsEstates["database"] = "incas_moja";
		settingsEstates["collection"] = "estates";

		settingsPlots["host"] = host;
		settingsPlots["port"] = port;
		settingsPlots["database"] = "incas_moja";
		settingsPlots["collection"] = "stands";

		ProviderNoSQLPocoMongoDB providerEstates(settingsEstates);
		ProviderNoSQLPocoMongoDB providerPlots (settingsPlots);

		auto countEstates = providerEstates.SendCountRequest();
		auto countPlots  = providerPlots .SendCountRequest();

		// 
		std::string queryStrEstates  = "{}";
		std::string fieldsStrEstates = "{ \"_id\": 1, \"Plots.area\": 1, \"Plots.startStep\": 1, \"Plots.startYr\": 1, \"Plots.plotFile\": 1 }";

		auto resultEstates = providerEstates.SendQueryRequest(queryStrEstates, fieldsStrEstates);
		auto resultPlots = providerPlots.GetDataSet("{}");

		for (auto lu : resultEstates) {
			auto obj = lu.extract<const moja::DynamicObject>();
			auto id = obj["_id"].extract<const std::string>();

			auto queryStr = (boost::format("{\"_id\":\"ObjectId(%1%)\"}") % id).str();
			auto estate = providerEstates.SendQueryRequest(queryStr);
			auto plots = estate[0]["Plots"].extract<moja::DynamicVector>();

			for (auto plot : plots) {
				double area		= plot["area"];
				int startStep	= plot["startStep"];
				int startYr		= plot["startYr"];
				auto plotFileInfo = plot["plotFile"].extract<const moja::DynamicObject>();
				auto plotFileInfo_db  = plotFileInfo["$db"] .extract<const std::string>();
				auto plotFileInfo_id  = plotFileInfo["$id"] .extract<const std::string>();
				auto plotFileInfo_ref = plotFileInfo["$ref"].extract<const std::string>();
			}
		}
	}
	catch (Poco::FileNotFoundException e) {
		MOJA_LOG_INFO << "FileNotFoundException: " << e.message();
	}
	catch (Poco::Exception e) {
		MOJA_LOG_INFO << "Poco::Exception: " << e.message();
	}
	catch (...) {

	}
}

BOOST_AUTO_TEST_SUITE_END();
