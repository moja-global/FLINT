#include "moja/modules/poco/providernosqlpocomongodb.h"
#include "moja/modules/poco/pocomongoutils.h"
#include "moja/datarepository/datarepositoryexceptions.h"

#include "moja/logging.h"

#include <Poco/MongoDB/Connection.h>
#include <Poco/MongoDB/Database.h>
#include <Poco/MongoDB/QueryRequest.h>
#include <Poco/MongoDB/Document.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Net/NetException.h>

#include <boost/format.hpp>

#include <vector>

using moja::datarepository::FileNotFoundException;
using moja::datarepository::NotImplementedException;
using moja::datarepository::FileName;
using moja::datarepository::QueryException;
using moja::datarepository::SQL;
using moja::datarepository::ConnectionFailedException;

namespace moja {
namespace modules {
namespace poco {

//#define POCO_MONGO_DEBUG_INFO

ProviderNoSQLPocoMongoDB::ProviderNoSQLPocoMongoDB(DynamicObject settings) : _connected(false), _cache(10000) {
	_host = settings["host"].convert<std::string>();
	_port = settings["port"];
	_collection = settings["collection"].convert<std::string>();
	_database = settings["database"].convert<std::string>();

	try {
		_connection.connect(_host, _port);
		_connected = true;
	}
	catch (Poco::Net::ConnectionRefusedException&) {
		auto connection_error_message = (boost::format("host %1%,  port=%2%, database=%3%") % _host % _port % _database).str();
		BOOST_THROW_EXCEPTION(ConnectionFailedException() << datarepository::ConnectionError(connection_error_message));
	}
	catch (...) {
		auto connection_error_message = (boost::format("host %1%,  port=%2%, database=%3%") % _host % _port % _database).str();
		BOOST_THROW_EXCEPTION(ConnectionFailedException() << datarepository::ConnectionError(connection_error_message));
	}
}

ProviderNoSQLPocoMongoDB::~ProviderNoSQLPocoMongoDB() {
	if (_connected) {
		_connection.disconnect();
		_connected = false;
	}
}

DynamicVector ProviderNoSQLPocoMongoDB::GetDataSet(const std::string& query) const {
    auto cachedValue = _cache.get(query);
    if (!cachedValue.isNull()) {
        return *cachedValue;
    }

    try {
		DynamicVector result;

		// >> PARSE JSON <<   
		Poco::JSON::Parser jsonParser;
		auto parsedJSON = jsonParser.parse(query);
		auto parsedResult = jsonParser.result();
		auto document = parsePocoJSONToMongoDBObj(parsedResult);

		// >> CONNECT <<   
		if (!_connected) {
			_connection.connect(_host, _port);
			_connected = true;
		}

		Poco::MongoDB::Database db(_database);
		auto queryPtr = db.createQueryRequest(_collection);

		queryPtr->selector() = *document;

		Poco::MongoDB::ResponseMessage response;
		_connection.sendRequest(*queryPtr, response);

#if defined(POCO_MONGO_DEBUG_INFO)
		// >> DEBUG <<   
		auto str = queryPtr->selector().toString();
		MOJA_LOG_DEBUG << "*****\tProviderNoSQLPocoMongoDB:GetDataSet:resloved query =\t" << str;
#endif
		// >> CHECK RESPONSE <<   
		if (response.documents().size() > 0)
			ConvertPocoMongoDocumentToDynamic(result, response.documents());

        _cache.add(query, result);
        return result;
	}
	catch (Poco::Net::ConnectionRefusedException&) {
		auto connection_error_message = (boost::format("host %1%,  port=%2%, database=%3%") % _host % _port % _database).str();
		BOOST_THROW_EXCEPTION(ConnectionFailedException() << datarepository::ConnectionError(connection_error_message));
	}
	catch (...) {
		auto connection_error_message = (boost::format("host %1%,  port=%2%, database=%3%") % _host % _port % _database).str();
		BOOST_THROW_EXCEPTION(ConnectionFailedException() << datarepository::ConnectionError(connection_error_message));
	}
}

int ProviderNoSQLPocoMongoDB::Count() const {
	return SendCountRequest();
}

///
/// For a list of commands see: https://docs.mongodb.org/manual/reference/command/
///
/// an example command would be:
/// { "create": "newCollectionName" }
///
DynamicVector ProviderNoSQLPocoMongoDB::SendCmdRequest(const std::string& command) const {
	try {
		DynamicVector result;

		// >> PARSE JSON <<   
		Poco::JSON::Parser jsonParser;
		auto parsedJSON = jsonParser.parse(command);
		auto parsedResult = jsonParser.result();
		auto document = parsePocoJSONToMongoDBObj(parsedResult);

		// >> CONNECT <<   
		if (!_connected) {
			_connection.connect(_host, _port);
			_connected = true;
		}

		Poco::MongoDB::Database db(_database);
		auto cmdPtr = db.createCommand();
		cmdPtr->selector() = *document;

		Poco::MongoDB::ResponseMessage response;
		_connection.sendRequest(*cmdPtr, response);

#if defined(POCO_MONGO_DEBUG_INFO)
		// >> DEBUG <<   
		auto str = cmdPtr->selector().toString();
		MOJA_LOG_DEBUG << "*****\tProviderNoSQLPocoMongoDB:SendCmdRequest:resloved query =\t" << str;
#endif
		// >> CHECK RESPONSE <<   
		if (response.documents().size() > 0)
			ConvertPocoMongoDocumentToDynamic(result, response.documents());

		return result;
	}
	catch (...) {
		auto connection_error_message = (boost::format("host %1%,  port=%2%, database=%3%") % _host % _port % _database).str();
		BOOST_THROW_EXCEPTION(ConnectionFailedException() << datarepository::ConnectionError(connection_error_message));
	}
};

int ProviderNoSQLPocoMongoDB::SendCountRequest() const {
	try {
		if (!_connected) {
			_connection.connect(_host, _port);
			_connected = true;
		}

		Poco::MongoDB::Database db(_database);
		auto countRequest = db.createCountRequest(_collection);
		Poco::MongoDB::ResponseMessage response;
		_connection.sendRequest(*countRequest, response);

		if (response.documents().size() > 0) {
			auto doc = response.documents()[0];
			return doc->get<int>("n");
		}

		return -1;
	}
	catch (...) {
		auto connection_error_message = (boost::format("host %1%,  port=%2%, database=%3%") % _host % _port % _database).str();
		BOOST_THROW_EXCEPTION(ConnectionFailedException() << datarepository::ConnectionError(connection_error_message));
	}
};

DynamicVector ProviderNoSQLPocoMongoDB::SendDeleteRequest(const std::string& query) const {
	try {
		DynamicVector result;
		return result;
	}
	catch (...) {
		auto connection_error_message = (boost::format("host %1%,  port=%2%, database=%3%") % _host % _port % _database).str();
		BOOST_THROW_EXCEPTION(ConnectionFailedException() << datarepository::ConnectionError(connection_error_message));
	}
};

DynamicVector ProviderNoSQLPocoMongoDB::SendInsertRequest(const std::string& query) const {
	try {
		DynamicVector result;
		return result;
	}
	catch (...) {
		auto connection_error_message = (boost::format("host %1%,  port=%2%, database=%3%") % _host % _port % _database).str();
		BOOST_THROW_EXCEPTION(ConnectionFailedException() << datarepository::ConnectionError(connection_error_message));
	}
};

DynamicVector ProviderNoSQLPocoMongoDB::SendQueryRequest(const std::string& query, int numberToReturn)	const {
	try {
		DynamicVector result;

		// >> PARSE QUERY JSON <<   
		Poco::JSON::Parser jsonParser;
		auto parsedJSON = jsonParser.parse(query);
		auto parsedResult = jsonParser.result();
		auto document = parsePocoJSONToMongoDBObj(parsedResult);

		// >> CONNECT <<   
		if (!_connected) {
			_connection.connect(_host, _port);
			_connected = true;
		}

		Poco::MongoDB::Database db(_database);
		auto queryPtr = db.createQueryRequest(_collection);
		queryPtr->setNumberToReturn(numberToReturn);

		queryPtr->selector() = *document;

		Poco::MongoDB::ResponseMessage response;
		_connection.sendRequest(*queryPtr, response);

#if defined(POCO_MONGO_DEBUG_INFO)
		// >> DEBUG <<   
		auto str = queryPtr->selector().toString();
		MOJA_LOG_DEBUG << "*****\tProviderNoSQLPocoMongoDB:SendQueryRequest:resloved query =\t" << str;
#endif
		// >> CHECK RESPONSE <<   
		if (response.documents().size() > 0)
			ConvertPocoMongoDocumentToDynamic(result, response.documents());

		return result;
	}
	catch (Poco::Net::ConnectionRefusedException&) {
		auto connection_error_message = (boost::format("host %1%,  port=%2%, database=%3%") % _host % _port % _database).str();
		BOOST_THROW_EXCEPTION(ConnectionFailedException() << datarepository::ConnectionError(connection_error_message));
	}
	catch (...) {
		auto connection_error_message = (boost::format("host %1%,  port=%2%, database=%3%") % _host % _port % _database).str();
		BOOST_THROW_EXCEPTION(ConnectionFailedException() << datarepository::ConnectionError(connection_error_message));
	}
}

DynamicVector ProviderNoSQLPocoMongoDB::SendQueryRequest(const std::string& query, const std::string& fields, int numberToReturn)	const {
	try {
		DynamicVector result;

		// >> PARSE QUERY JSON <<   
		Poco::JSON::Parser jsonParser;
		auto parsedJSON = jsonParser.parse(query);
		auto parsedResult = jsonParser.result();
		auto document = parsePocoJSONToMongoDBObj(parsedResult);

		// >> PARSE FIELDS JSON <<   
		Poco::JSON::Parser jsonParser2;
		auto parsedFieldsJSON = jsonParser2.parse(fields);
		auto parsedFieldsResult = jsonParser2.result();
		auto documentFields = parsePocoJSONToMongoDBObj(parsedFieldsResult);

		// >> CONNECT <<   
		if (!_connected) {
			_connection.connect(_host, _port);
			_connected = true;
		}

		Poco::MongoDB::Database db(_database);
		auto queryPtr = db.createQueryRequest(_collection);
		queryPtr->setNumberToReturn(numberToReturn);

		queryPtr->selector() = *document;
		queryPtr->returnFieldSelector() = *documentFields;

		Poco::MongoDB::ResponseMessage response;
		_connection.sendRequest(*queryPtr, response);

#if defined(POCO_MONGO_DEBUG_INFO)
		// >> DEBUG <<   
		auto str = queryPtr->selector().toString();
		MOJA_LOG_DEBUG << "*****\tProviderNoSQLPocoMongoDB:SendQueryRequest:resloved query =\t" << str;
#endif
		// >> CHECK RESPONSE <<   
		if (response.documents().size() > 0)
			ConvertPocoMongoDocumentToDynamic(result, response.documents());

		return result;
	}
	catch (Poco::Net::ConnectionRefusedException&) {
		auto connection_error_message = (boost::format("host %1%,  port=%2%, database=%3%") % _host % _port % _database).str();
		BOOST_THROW_EXCEPTION(ConnectionFailedException() << datarepository::ConnectionError(connection_error_message));
	}
	catch (...) {
		auto connection_error_message = (boost::format("host %1%,  port=%2%, database=%3%") % _host % _port % _database).str();
		BOOST_THROW_EXCEPTION(ConnectionFailedException() << datarepository::ConnectionError(connection_error_message));
	}
};

DynamicVector ProviderNoSQLPocoMongoDB::SendUpdateRequest(const std::string& query) const {
	try {
		DynamicVector result;
		return result;
	}
	catch (...) {
		auto connection_error_message = (boost::format("host %1%,  port=%2%, database=%3%") % _host % _port % _database).str();
		BOOST_THROW_EXCEPTION(ConnectionFailedException() << datarepository::ConnectionError(connection_error_message));
	}
};

#undef POCO_MONGO_DEBUG_INFO

}}}