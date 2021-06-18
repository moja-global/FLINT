#include "moja/modules/libpq/providerrelationallibpqpostgresql.h"

#include <moja/logging.h>

#include <Poco/LRUCache.h>
#include <Poco/NumberParser.h>
#include <Poco/StringTokenizer.h>

#include <boost/format.hpp>

#include <libpq-fe.h>
#include <vector>

/*
https://www.postgresql.org/docs/9.0/static/populate.html

https://www.postgresql.org/docs/current/static/libpq-copy.html

http://stackoverflow.com/questions/10510539/simple-postgresql-libpq-code-too-slow

*/

namespace moja {
namespace modules {
namespace libpq {

static void noticeProcessor(void* arg, const char* message) {
   // MOJA_LOG_INFO << message;
}

static void noticeReceiver(void* arg, const PGresult* res) {
   // MOJA_LOG_INFO << message;
}

// Simple RAII class to handle connection
class PostgresConnection {
   PGconn* _conn;

  public:
   PostgresConnection(const std::string& connInfo) : _conn{PQconnectdb(connInfo.c_str())} {
      /* Check to see that the backend connection was successfully made */
      if (PQstatus(_conn) != CONNECTION_OK) {
         std::string msg = (boost::format("Database connection failed - '%1%' with connection string '%2%") %
                            PQerrorMessage(_conn) % connInfo)
                               .str();
         MOJA_LOG_ERROR << "PostgresConnection: " << msg;
         throw std::runtime_error("Error Postgres connection failed: " + msg);
      }
      PQsetNoticeReceiver(_conn, noticeReceiver, nullptr);
      PQsetNoticeProcessor(_conn, noticeProcessor, nullptr);
   }
   ~PostgresConnection() { PQfinish(_conn); }
   operator PGconn*() const { return _conn; }

   // PGConnection handles can't be cloned, so disable copying and assignment
   PostgresConnection(const PostgresConnection&) = delete;
   PostgresConnection& operator=(const PostgresConnection&) = delete;
};

// Simple RAII class to handle result
class PostgresResult {
  public:
   enum class ColumnType : int {
      BOOLEAN = 16,
      CHARACTER = 18,
      NAME = 19,
      INT8OID = 20,
      INT2OID = 21,
      INTEGER = 23,
      REGPROCOID = 24,
      STRING = 25,
      OIDOID = 26,
      JSONOID = 114,
      FLOAT = 700,
      DOUBLE = 701,
      MONEY = 790,
      FLOAT4ARRAYOID = 1021,
      FLOAT8ARRAYOID = 1022,
      VARCHAR2 = 1042,
      VARCHAR = 1043
   };

   PostgresResult(const PostgresConnection& conn, const std::string& query) { _result = PQexec(conn, query.c_str()); }

   ~PostgresResult() { PQclear(_result); }

   int result_code() const { return PQresultStatus(_result); }
   int n_rows() const { return PQntuples(_result); }
   int n_cols() const { return PQnfields(_result); }
   std::string column_name(int col) const { return PQfname(_result, col); }
   ColumnType column_type(int col) const {
      int columnType = PQftype(_result, col);
      if (columnType != 16 && columnType != 18 && columnType != 19 && columnType != 20 && columnType != 21 &&
          columnType != 23 && columnType != 24 && columnType != 25 && columnType != 26 && columnType != 114 &&
          columnType != 700 && columnType != 701 && columnType != 790 && columnType != 1021 && columnType != 1022 &&
          columnType != 1042 && columnType != 1043) {
         const auto columnTypeInt = PQfformat(_result, col);
         if (columnTypeInt == 0) {  // handling only text data
            columnType = 25;
         }
      }
      return ColumnType(columnType);
   }
   bool is_field_null(int row, int col) const { return PQgetisnull(_result, row, col) == 1; }
   const char* field_value(int row, int col) const { return PQgetvalue(_result, row, col); }

  private:
   PGresult* _result;
};

class ProviderRelationalLibpqPostgreSQL::impl {
  public:
   impl(DynamicObject settings)
       : _dbConnString(settings["connectionstring"].convert<std::string>()), _conn(_dbConnString), _cache(10000) {
      if (settings.contains("schema")) {
         PQexec(_conn, ("SET search_path TO " + settings["schema"].convert<std::string>()).c_str());
      }
   }

   DynamicVar GetDataSet(const std::string& query) const {
      auto cachedValue = _cache.get(query);
      if (!cachedValue.isNull()) {
         return *cachedValue;
      }

      std::vector<DynamicObject> result;

      /// - results from this query come back as text, hence the Poco::tryParseXXX methods below
      /// We can get results as binary, currently text works but Int, doubles, floats are all in network byte order
      /// which we need to convert to local byte order.
      /// (http://www.postgresql.org/docs/current/static/libpq-example.html)
      PostgresResult pg_result(_conn, query);

      //// - this query call gets results in binary format

      // const char *paramValues[1];
      // paramValues[0] = "joe's place";
      // pResult = PQexecParams(
      //	_conn,
      //	query.c_str(),
      //	0,       /* one param */
      //	NULL,    /* let the backend deduce param type */
      //	paramValues,
      //	NULL,    /* don't need param lengths since text */
      //	NULL,    /* default to all text params */
      //	1);      /* ask for binary results */

      const int resultcode = pg_result.result_code();

      switch (resultcode) {
         case PGRES_EMPTY_QUERY:
            // std::cout << "EMPTY QUERY" << std::endl;
            break;
         case PGRES_COMMAND_OK:
            // std::cout << "QUERY SUCCESS AND NO DATA FOUND" << std::endl;
            break;
         case PGRES_TUPLES_OK: {
            // std::cout << "Number of tuples returned  ::::   " << PQntuples(pResult) << std::endl;

            const int nRows = pg_result.n_rows();  // # of rows
            const int nCols = pg_result.n_cols();  // # of cols
            std::vector<PostgresResult::ColumnType> columnTypes(nCols);
            std::vector<std::string> columnNames(nCols);

            if (nRows != 0) {
               result.reserve(nRows);
               for (int j = 0; j < nCols; j++) {
                  columnNames[j] = pg_result.column_name(j);
                  columnTypes[j] = pg_result.column_type(j);
               }  // end for loop j

               for (int i = 0; i < nRows; i++) {
                  DynamicObject row;
                  for (int k = 0; k < nCols; k++) {
                     if (pg_result.is_field_null(i, k)) {
                        // std::cout << "value is null" << std::endl;
                        row.insert(columnNames[k], DynamicVar());
                     } else {
                        // std::cout << "Fetch Data" << std::endl;
                        auto rawValue = pg_result.field_value(i, k);

                        switch (columnTypes[k]) {
                           case PostgresResult::ColumnType::BOOLEAN: {
                              bool value;
                              if ('t' == *rawValue) {
                                 value = true;
                              } else {
                                 value = false;
                              }
                              row.insert(columnNames[k], value);
                              break;
                           }
                           case PostgresResult::ColumnType::CHARACTER:
                              row.insert(columnNames[k], rawValue);
                              break;
                           case PostgresResult::ColumnType::NAME:
                              row.insert(columnNames[k], rawValue);
                              break;
                           case PostgresResult::ColumnType::INTEGER: {
                              Poco::Int32 data;
                              if (!Poco::NumberParser::tryParse(rawValue, data))
                                 row.insert(columnNames[k], DynamicVar());
                              else {
                                 int value = data;
                                 row.insert(columnNames[k], value);
                              }
                              break;
                           }
                           case PostgresResult::ColumnType::JSONOID:
                              row.insert(columnNames[k], rawValue);
                              break;
                           case PostgresResult::ColumnType::FLOAT: {
                              double data;
                              if (!Poco::NumberParser::tryParseFloat(rawValue, data))
                                 row.insert(columnNames[k], DynamicVar());
                              else {
                                 if (data > std::numeric_limits<float>().max())
                                    throw std::runtime_error("Float value truncation.");
                                 row.insert(columnNames[k], float(data));
                              }
                              break;
                           }
                           case PostgresResult::ColumnType::DOUBLE: {
                              double value;
                              if (!Poco::NumberParser::tryParseFloat(rawValue, value))
                                 row.insert(columnNames[k], DynamicVar());
                              else {
                                 row.insert(columnNames[k], value);
                              }
                              break;
                           }
                           case PostgresResult::ColumnType::MONEY:
                              row.insert(columnNames[k], rawValue);
                              break;
                           case PostgresResult::ColumnType::VARCHAR2:
                              row.insert(columnNames[k], rawValue);
                              break;
                           case PostgresResult::ColumnType::STRING:
                           case PostgresResult::ColumnType::VARCHAR: {
                              auto value = rawValue == nullptr ? DynamicVar() : DynamicVar(std::string(rawValue));
                              row.insert(columnNames[k], value);
                              break;
                           }
                           case PostgresResult::ColumnType::OIDOID:
                              row.insert(columnNames[k], rawValue);
                              break;
                           case PostgresResult::ColumnType::REGPROCOID:
                              row.insert(columnNames[k], rawValue);
                              break;
                           case PostgresResult::ColumnType::INT2OID: {
                              int data;
                              if (!Poco::NumberParser::tryParse(rawValue, data))
                                 row.insert(columnNames[k], DynamicVar());
                              else {
                                 if (data > std::numeric_limits<Int16>().max())
                                    throw std::runtime_error("Int value truncation.");
                                 row.insert(columnNames[k], Int16(data));
                              }
                              break;
                           }
                           case PostgresResult::ColumnType::INT8OID: {
                              Poco::Int64 data;
                              if (!Poco::NumberParser::tryParse64(rawValue, data))
                                 row.insert(columnNames[k], DynamicVar());
                              else {
                                 Int64 value = data;
                                 row.insert(columnNames[k], value);
                              }
                              break;
                           }
                           case PostgresResult::ColumnType::FLOAT4ARRAYOID: {
                              std::string array(rawValue);
                              replace_all(array, "{", "");
                              replace_all(array, "}", "");

                              Poco::StringTokenizer st(array, ",");
                              double value;

                              DynamicVector values;
                              for (const auto& tok : st) {
                                 if (Poco::NumberParser::tryParseFloat(tok, value))
                                    values.push_back(float(value));
                                 else
                                    values.push_back(DynamicVar());
                              }
                              row.insert(columnNames[k], values);
                              break;
                           }
                           case PostgresResult::ColumnType::FLOAT8ARRAYOID: {
                              std::string array(rawValue);
                              replace_all(array, "{", "");
                              replace_all(array, "}", "");

                              Poco::StringTokenizer st(array, ",");
                              double value;

                              moja::DynamicVector values;
                              for (const auto& tok : st) {
                                 if (Poco::NumberParser::tryParseFloat(tok, value))
                                    values.push_back(value);
                                 else
                                    values.push_back(DynamicVar());
                              }
                              row.insert(columnNames[k], values);
                              break;
                           }
                           default:
                              throw std::runtime_error("Unsupported column type");
                        }
                     }
                  }  // end for loop k -- cols
                  result.push_back(row);
               }  // end for loop i -- rows
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
            std::string msg = (boost::format("PostgreSQL fatal error - '%1%' with connection string '%2%") %
                               PQerrorMessage(_conn) % _dbConnString)
                                  .str();
            MOJA_LOG_ERROR << "ProviderRelationalLibpqPostgreSQL: " << msg;
            throw std::runtime_error("Error " + msg);
            break;
         }
         case PGRES_COPY_BOTH:
            break;
         case PGRES_SINGLE_TUPLE:
            MOJA_LOG_ERROR << "ProviderRelationalLibpqPostgreSQL: "
                           << "Not sure what to do with PGRES_SINGLE_TUPLE";
            // std::cout << PQgetvalue(pResult, 0, 0) << std::endl;
            break;
         default:
            break;
      }  // end switch resultcode

      DynamicVar dataset{result};
      _cache.add(query, dataset);
      return dataset;
   }

  private:
   static void replace_all(std::string& str, const std::string& from, const std::string& to) {
      size_t start_pos = 0;
      while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
         str.replace(start_pos, from.length(), to);
         start_pos += to.length();  // Handles case where 'to' is a substring of 'from'
      }
   }
   std::string _dbConnString;
   PostgresConnection _conn;
   mutable Poco::LRUCache<std::string, DynamicVar> _cache;
};

ProviderRelationalLibpqPostgreSQL::ProviderRelationalLibpqPostgreSQL(DynamicObject settings)
    : pimpl{std::make_unique<impl>(settings)} {}

ProviderRelationalLibpqPostgreSQL::~ProviderRelationalLibpqPostgreSQL() = default;

DynamicVar ProviderRelationalLibpqPostgreSQL::GetDataSet(const std::string& query) const {
   return pimpl->GetDataSet(query);
}

}  // namespace libpq
}  // namespace modules
}  // namespace moja
