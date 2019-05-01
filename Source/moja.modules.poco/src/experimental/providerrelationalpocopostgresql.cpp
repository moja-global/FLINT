#include "moja/modules/poco/providerrelationalpocopostgresql.h"
#include "moja/datarepository/datarepositoryexceptions.h"
#include "Poco/Data/PostgreSQL/Connector.h"
#include <vector>
#include <moja/logging.h>

namespace moja {
namespace datarepository {

static void noticeProcessor(void *arg, const char *message) {
	//MOJA_LOG_INFO << message;
}

static void noticeReceiver(void *arg, const PGresult *res) {
	//MOJA_LOG_INFO << message;
}

#if 0
const char* COL_TEXT = "TEXT";
const char* COL_VARCHAR = "VARCHAR";
const char* COL_INTEGER = "INTEGER";
const char* COL_FLOAT = "FLOAT";

/* pg_type.h has more codes for DataColumnTypes kindly refer the codes for more data types*/
enum class DataColumnType {
	BOOLEAN = 16,
	CHARACTER = 18,
	NAME = 19,
	INTEGER = 23,
	STRING = 25,
	JSONOID = 114,
	FLOAT = 700,   //FLOAT4 in db -- single precision floating point
	DOUBLE = 701, // FLOAT8 in db -- double precision floating point
	MONEY = 790,
	VARCHAR2 = 1042,
	VARCHAROID = 1043,
	REGPROCOID = 24,
	OIDOID = 26,
	INT2OID = 21,
	INT8OID = 20,  //BigInt
	POINTOID = 600,  // geometric point
	LSEGOID = 601, // geometric line segment
	BOXOID = 603,  // geometric box
	POLYGONOID = 604,  // geometric polygon
	LINEOID = 628  //geometric line
};
#endif

enum class ColumnType {
	BOOLEAN,
	CHARACTER,
	NAME,
	INTEGER,
	STRING,
	JSONOID,
	FLOAT,
	DOUBLE,
	MONEY,
	VARCHAR,
	VARCHAR2,
	OIDOID,
	REGPROCOID,
	INT2OID,
	INT8OID
};


ProviderRelationalPocoPostgreSQL::ProviderRelationalPocoPostgreSQL(DynamicObject settings) :
    _cache(10000) {

	_dbConnString = settings["connectionstring"].convert<std::string>();
	Poco::Data::PostgreSQL::Connector::registerConnector();
	_conn = PQconnectdb(_dbConnString.c_str());

	/* Check to see that the backend connection was successfully made */
	if (PQstatus(_conn) != CONNECTION_OK) {
		std::string msg = (boost::format("Database connection failed - '%1%' with connection string '%2%") % PQerrorMessage(_conn) % _dbConnString).str();
		MOJA_LOG_ERROR << "ProviderRelationalPocoPostgreSQL: " << msg;
		BOOST_THROW_EXCEPTION(ConnectionFailedException() << ConnectionError(msg));
	}
	PQsetNoticeReceiver(_conn, noticeReceiver, nullptr);
	PQsetNoticeProcessor(_conn, noticeProcessor, nullptr);
}

ProviderRelationalPocoPostgreSQL::~ProviderRelationalPocoPostgreSQL() {
	PQfinish(_conn);
	Poco::Data::PostgreSQL::Connector::unregisterConnector();
}

Dynamic ProviderRelationalPocoPostgreSQL::GetDataSet(const std::string& query) const {
    auto cachedValue = _cache.get(query);
    if (!cachedValue.isNull()) {
        return *cachedValue;
    }

    std::vector<DynamicObject> result;

	/// - results from this query come back as text, hence the Poco::tryParseXXX methods below
	/// We can get results as binary, currently text works but Int, doubles, floats are all in network byte order
	/// which we need to convert to local byte order.
	/// (http://www.postgresql.org/docs/current/static/libpq-example.html)
    pResult = PQexec(_conn, query.c_str());

	//// - this query call gets results in binary format

	//const char *paramValues[1];
	//paramValues[0] = "joe's place";
	//pResult = PQexecParams(
	//	_conn, 
	//	query.c_str(),
	//	0,       /* one param */
	//	NULL,    /* let the backend deduce param type */
	//	paramValues,
	//	NULL,    /* don't need param lengths since text */
	//	NULL,    /* default to all text params */
	//	1);      /* ask for binary results */

	int resultcode = PQresultStatus(pResult);

	switch (resultcode) {
	case PGRES_EMPTY_QUERY:
		//std::cout << "EMPTY QUERY" << std::endl;
		break;
	case PGRES_COMMAND_OK:
		//std::cout << "QUERY SUCCESS AND NO DATA FOUND" << std::endl;
		break;
	case PGRES_TUPLES_OK: {
		//std::cout << "Number of tuples returned  ::::   " << PQntuples(pResult) << std::endl;

		int nRows = PQntuples(pResult);  // # of rows
		int nCols = PQnfields(pResult); // # of cols
		std::vector<ColumnType> columnTypes(nCols);
		std::vector<std::string> columnNames(nCols);

		if (PQntuples(pResult) != 0) {
			for (int j = 0; j < nCols; j++) {
				std::string name = PQfname(pResult, j);
				columnNames[j] = name;

				int columnType = PQftype(pResult, j);
				if (columnType != 16
					&& columnType != 18
					&& columnType != 19
					&& columnType != 20
					&& columnType != 21
					&& columnType != 23
					&& columnType != 24
					&& columnType != 25
					&& columnType != 26
					&& columnType != 114
					&& columnType != 700
					&& columnType != 701
					&& columnType != 790
					&& columnType != 1042
					&& columnType != 1043) {
					auto columnTypeInt = PQfformat(pResult, j);
					if (columnTypeInt == 0) { //handling only text data 
						columnType = 25;
					}
				}

				switch (columnType) {
				case 16:	columnTypes[j] = ColumnType::BOOLEAN;				break;
				case 18:	columnTypes[j] = ColumnType::CHARACTER;				break;
				case 19:	columnTypes[j] = ColumnType::NAME;					break;
				case 20:	columnTypes[j] = ColumnType::INT8OID;				break;
				case 21:	columnTypes[j] = ColumnType::INT2OID;				break;
				case 23:	columnTypes[j] = ColumnType::INTEGER;				break;
				case 24:	columnTypes[j] = ColumnType::REGPROCOID;			break;
				case 25:	columnTypes[j] = ColumnType::STRING;				break;
				case 26:	columnTypes[j] = ColumnType::OIDOID;				break;
				case 114:	columnTypes[j] = ColumnType::JSONOID;				break;
				case 700:	columnTypes[j] = ColumnType::FLOAT;					break;
				case 701:	columnTypes[j] = ColumnType::DOUBLE;				break;
				case 790:	columnTypes[j] = ColumnType::MONEY;					break;
				case 1042:	columnTypes[j] = ColumnType::VARCHAR2;				break;
				case 1043:	columnTypes[j] = ColumnType::VARCHAR;				break;
				default:
					PQclear(pResult);
					throw std::runtime_error("Unsupported column type");
				}
			} // end for loop j

			for (int i = 0; i < nRows; i++) {
				DynamicObject row;
				for (int k = 0; k < nCols; k++) {
					if (PQgetisnull(pResult, i, k)) {
						//std::cout << "value is null" << std::endl;
						row.insert(columnNames[k], Dynamic());
					} else {
						//std::cout << "Fetch Data" << std::endl;
						auto rawValue = PQgetvalue(pResult, i, k);

						switch (columnTypes[k]) {
						case ColumnType::BOOLEAN: {
							bool value;
							if ('t' == *rawValue) {
								value = true;
							} else {
								value = false;
							}
							row.insert(columnNames[k], value);
							break;
						}
						case ColumnType::CHARACTER:		row.insert(columnNames[k], rawValue);	break;
						case ColumnType::NAME:			row.insert(columnNames[k], rawValue);	break;
						case ColumnType::INTEGER: {
							Poco::Int32 data;
							if (!Poco::NumberParser::tryParse(rawValue, data))
								row.insert(columnNames[k], Dynamic());
							else {
								int value = data;
								row.insert(columnNames[k], value);
							}
							break;
						}
						case ColumnType::JSONOID:		row.insert(columnNames[k], rawValue);	break;
						case ColumnType::FLOAT: {
							double data;
							if (!Poco::NumberParser::tryParseFloat(rawValue, data))
								row.insert(columnNames[k], Dynamic());
							else {
								float value = data;
								row.insert(columnNames[k], value);
							}
							break;
						}
						case ColumnType::DOUBLE: {
							auto x = static_cast<char*>(rawValue);

							double value;
							if (!Poco::NumberParser::tryParseFloat(rawValue, value))
								row.insert(columnNames[k], Dynamic());
							else {
								row.insert(columnNames[k], value);
							}
							break;
						}
						case ColumnType::MONEY:			row.insert(columnNames[k], rawValue);	break;
						case ColumnType::VARCHAR2:		row.insert(columnNames[k], rawValue);	break;
                        case ColumnType::STRING:
						case ColumnType::VARCHAR: {
							auto value = rawValue == nullptr ? Dynamic() : Dynamic(std::string(static_cast<char*>(rawValue)));
							row.insert(columnNames[k], value);
							break;
						}
						case ColumnType::OIDOID:		row.insert(columnNames[k], rawValue);	break;
						case ColumnType::REGPROCOID:	row.insert(columnNames[k], rawValue);	break;
						case ColumnType::INT2OID:		row.insert(columnNames[k], rawValue);	break;
						case ColumnType::INT8OID: {
							Poco::Int64 data;
							if (!Poco::NumberParser::tryParse64(rawValue, data))
								row.insert(columnNames[k], Dynamic());
							else {
								Int64 value = data;
								row.insert(columnNames[k], value);
							}
							break;
						}
						default:
							throw std::runtime_error("Unsupported column type");
						}
					}
				} // end for loop k -- cols
				result.push_back(row);
			} // end for loop i -- rows

		}
		break;
	}
	case PGRES_COPY_OUT:
		break;
	case PGRES_COPY_IN:
		break;
	case PGRES_BAD_RESPONSE:
		break;
	case PGRES_NONFATAL_ERROR:
		break;
	case PGRES_FATAL_ERROR: {
		std::string msg = (boost::format("PostgreSQL fatal error - '%1%' with connection string '%2%") % PQerrorMessage(_conn) % _dbConnString).str();
		MOJA_LOG_ERROR << "ProviderRelationalPocoPostgreSQL: " << msg;
		BOOST_THROW_EXCEPTION(QueryException() << SQL(query) << SQLError(msg));
		break;
	}
	case PGRES_COPY_BOTH:
		break;
	case PGRES_SINGLE_TUPLE:
		MOJA_LOG_ERROR << "ProviderRelationalPocoPostgreSQL: " << "Not sure what to do with PGRES_SINGLE_TUPLE";
		//std::cout << PQgetvalue(pResult, 0, 0) << std::endl;
		break;
	default:
		break;
	} // end switch resultcode

	PQclear(pResult);

    Dynamic dataset{ result };
    _cache.add(query, dataset);
    return dataset;

}
}
} // moja::datarepository
