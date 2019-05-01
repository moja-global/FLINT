#include "moja/modules/poco/providernosqlpocojson.h"
#include "moja/datarepository/datarepositoryexceptions.h"

#include "moja/dynamic.h"

#include <Poco/File.h>
#include <Poco/FileStream.h>

#include <boost/test/unit_test.hpp>

using moja::modules::poco::ProviderNoSQLPocoJSON;
using moja::datarepository::FileNotFoundException;
using moja::datarepository::QueryException;

struct ProviderNoSQLPocoJSONTestsFixture {
	const std::string FILE_PATH = "ProviderNoSQLPocoJSONTests.json";
	moja::DynamicObject settings;

	ProviderNoSQLPocoJSONTestsFixture() {
		settings["file_path"] = FILE_PATH;
		Poco::File file(FILE_PATH);
		file.createFile();
		Poco::FileOutputStream fos(FILE_PATH, std::ios::out);
		fos << "{\"TransactionCode\":\"000000\",\"FileRecordSequenceNumber\":\"111111\",\"TcrSequenceNumber\":\"222222\",\"TransactionRouteIndicator\":\"ABCDE\",\"MerchantEstablishmentNumber\":\"00000000000\",\"MerchantName\":\"BBBBBBBBB\",\"MerchantCity\":\"CCCCCCCC\"}";
		fos.close();
	}

	~ProviderNoSQLPocoJSONTestsFixture() {
		Poco::File file(FILE_PATH);
		file.remove();
	}
};

BOOST_FIXTURE_TEST_SUITE(ProviderNoSQLPocoJSONTests, ProviderNoSQLPocoJSONTestsFixture);

BOOST_AUTO_TEST_CASE(datarepository_ProviderNoSQLPocoJSON_ConstructorThrowsExceptionIfTargetDatabaseNotFound) {
	settings["file_path"] = "test.json";
	BOOST_CHECK_THROW(ProviderNoSQLPocoJSON provider(settings), FileNotFoundException);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderNoSQLPocoJSON_OpenValidDatabase) {
	ProviderNoSQLPocoJSON provider(settings);
}

BOOST_AUTO_TEST_CASE(datarepository_ProviderNoSQLPocoJSON_JSONParsing) {
	ProviderNoSQLPocoJSON provider(settings);
	provider.GetDataSet("test");
}

#if 0
BOOST_AUTO_TEST_CASE(datarepository_ProviderNoSQLPocoJSON_JSONParsing_1) {
	auto filePath = settings["file_path"].extract<const std::string>();
	moja::Stopwatch<> sw;
	std::ostringstream ostr;
	moja::DynamicObject ret;

	sw.start();
	sw.stop();

	std::string jsonStr = "{\"forest_type\": { \"$in\" : [2, 3]}}";

	std::cout << "Total of " << jsonStr.size() << " bytes," << std::endl << "loaded in " << sw.elapsed() << " [us]," << std::endl;

	std::cout << std::endl << "POCO JSON barebone parse" << std::endl;
	Poco::JSON::Parser sparser(0);
	sw.restart();
	sparser.parse(jsonStr);
	sw.stop();
	std::cout << "---------------------------------" << std::endl;
	std::cout << "[std::string] parsed in " << sw.elapsed() << " [us]" << std::endl;
	std::cout << "---------------------------------" << std::endl;

	Poco::JSON::Parser iparser(0);
	std::istringstream istr(jsonStr);
	sw.restart();
	iparser.parse(istr);
	sw.stop();
	std::cout << "----------------------------------------" << std::endl;
	std::cout << "[std::istringstream] parsed in " << sw.elapsed() << " [us]" << std::endl;
	std::cout << "----------------------------------------" << std::endl;

	std::cout << std::endl << "POCO JSON Handle/Stringify" << std::endl;
	try {
		Poco::JSON::Parser sparser;
		sw.restart();
		sparser.parse(jsonStr);
		Poco::DynamicAny result = sparser.result();
		ret = moja::datarepository::checkContents(result);

		sw.stop();
		std::cout << "-----------------------------------------" << std::endl;
		std::cout << "[std::string] parsed/handled in " << sw.elapsed() << " [us]" << std::endl;
		std::cout << "-----------------------------------------" << std::endl;

		Poco::JSON::Parser isparser;
		std::istringstream istr(jsonStr);
		sw.restart();
		isparser.parse(istr);
		result = isparser.result();
		sw.stop();
		std::cout << "------------------------------------------------" << std::endl;
		std::cout << "[std::istringstream] parsed/handled in " << sw.elapsed() << " [us]" << std::endl;
		std::cout << "------------------------------------------------" << std::endl;

		//Serialize to string
		Poco::JSON::Object::Ptr obj;
		if (result.type() == typeid(Poco::JSON::Object::Ptr))
			obj = result.extract<Poco::JSON::Object::Ptr>();

		std::ostringstream out;
		sw.restart();
		obj->stringify(out);
		sw.stop();
		std::cout << "-----------------------------------" << std::endl;
		std::cout << "stringified in " << sw.elapsed() << " [us]" << std::endl;
		std::cout << "-----------------------------------" << std::endl;
		std::cout << std::endl;

		// Parse the JSON and get the Results
		//
		Poco::JSON::Parser loParser;
		auto loParsedJson = loParser.parse(jsonStr);
		auto loParsedJsonResult = loParser.result();

		// Get the JSON Object
		//
		auto loJsonObject = loParsedJsonResult.extract<Poco::JSON::Object::Ptr>();

		// Get the values for the member variables
		//
		//
		//std::cout << "TransactionCode             " << moja::datarepository::GetValue(loJsonObject, "TransactionCode") << std::endl;
		//std::cout << "FileRecordSequenceNumber    " << moja::datarepository::GetValue(loJsonObject, "FileRecordSequenceNumber") << std::endl;
		//std::cout << "TcrSequenceNumber           " << moja::datarepository::GetValue(loJsonObject, "TcrSequenceNumber") << std::endl;
		//std::cout << "TransactionRouteIndicator   " << moja::datarepository::GetValue(loJsonObject, "TransactionRouteIndicator") << std::endl;
		//std::cout << "MerchantEstablishmentNumber " << moja::datarepository::GetValue(loJsonObject, "MerchantEstablishmentNumber") << std::endl;
		//std::cout << "MerchantName                " << moja::datarepository::GetValue(loJsonObject, "MerchantName") << std::endl;
		//std::cout << "MerchantCity                " << moja::datarepository::GetValue(loJsonObject, "MerchantCity") << std::endl;
	}
	catch (const Poco::JSON::JSONException& jsone) {
		std::cout << jsone.message() << std::endl;
	}
}

#endif

BOOST_AUTO_TEST_SUITE_END();
