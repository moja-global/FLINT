#include "moja/datarepository/providerrelationalsqlite.h"

#include "moja/datarepository/datarepositoryexceptions.h"

#include <Poco/File.h>
#include <Poco/LRUCache.h>

#include <sqlite3.h>
#include <vector>

namespace moja {
namespace datarepository {

static constexpr auto COLUMN_TEXT = "TEXT";
static constexpr auto COLUMN_VARCHAR = "VARCHAR";
static constexpr auto COLUMN_INTEGER = "INTEGER";
static constexpr auto COLUMN_FLOAT = "FLOAT";

// Simple RAII class to handle connection
class SQLiteConnection {
   sqlite3* _conn;

  public:
   SQLiteConnection(const std::string& path) {
      sqlite3_config(SQLITE_CONFIG_URI, 1);

      if (path.find(":memory:") == std::string::npos) {
         Poco::File file(path);
         if (!file.exists()) {
            BOOST_THROW_EXCEPTION(FileNotFoundException() << FileName(path));
         }
      }

      if (sqlite3_open_v2(path.c_str(), &_conn, SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX, 0) != SQLITE_OK) {
         BOOST_THROW_EXCEPTION(ConnectionFailedException() << ConnectionError(sqlite3_errmsg(_conn)));
      }

      const char* pragma = "PRAGMA journal_mode = OFF";
      sqlite3_exec(_conn, pragma, NULL, NULL, NULL);
      sqlite3_busy_timeout(_conn, 60000);
   }
   ~SQLiteConnection() { sqlite3_close(_conn); }
   operator sqlite3*() const { return _conn; }

   // SQLiteConnection handles can't be cloned, so disable copying and assignment
   SQLiteConnection(const SQLiteConnection&) = delete;
   SQLiteConnection& operator=(const SQLiteConnection&) = delete;
};

// Simple RAII class to handle statement
class SQLiteStatement {
  public:
   enum class ColumnType { STRING, INTEGER, DOUBLE, NULLVAL };

   SQLiteStatement(const SQLiteConnection& conn, const std::string& query) {
      if (sqlite3_prepare_v2(conn, query.c_str(), int(query.size()), &_stmt, nullptr) != SQLITE_OK) {
         BOOST_THROW_EXCEPTION(QueryException() << SQL(query) << SQLError(sqlite3_errmsg(conn)));
      }
   }

   ~SQLiteStatement() { sqlite3_finalize(_stmt); }

   int step() const { return sqlite3_step(_stmt); }
   int n_cols() const { return sqlite3_column_count(_stmt); }
   std::string column_name(int col) const { return sqlite3_column_name(_stmt, col); }
   ColumnType column_type(int col) const {
      int columnType = sqlite3_column_type(_stmt, col);
      if (columnType != SQLITE_INTEGER && columnType != SQLITE_FLOAT && columnType != SQLITE_TEXT) {
         const auto columnTypeStr = sqlite3_column_decltype(_stmt, col);
         if (columnTypeStr == COLUMN_TEXT || columnTypeStr == COLUMN_VARCHAR) {
            columnType = SQLITE_TEXT;
         } else if (columnTypeStr == COLUMN_INTEGER) {
            columnType = SQLITE_INTEGER;
         } else if (columnTypeStr == COLUMN_FLOAT) {
            columnType = SQLITE_FLOAT;
         }
      }

      switch (columnType) {
         case SQLITE_INTEGER:
            return ColumnType::INTEGER;
         case SQLITE_FLOAT:
            return ColumnType::DOUBLE;
         case SQLITE_TEXT:
            return ColumnType::STRING;
         case SQLITE_NULL:
            return ColumnType::NULLVAL;
         default:
            throw std::runtime_error("Unsupported column type");
      }
   }
   double column_double(int col) const { return sqlite3_column_double(_stmt, col); }
   Int64 column_int64(int col) const { return sqlite3_column_int64(_stmt, col); }
   DynamicVar column_text(int col) const {
      const auto rawValue = reinterpret_cast<const char*>(sqlite3_column_text(_stmt, col));
      return rawValue == nullptr ? DynamicVar() : DynamicVar(std::string(rawValue));
   }

  private:
   sqlite3_stmt* _stmt;
};

class ProviderRelationalSQLite::impl {
  public:
   impl(DynamicObject settings) : _path(settings["path"].convert<std::string>()), _conn(_path), _cache(10000) {}

   DynamicVar GetDataSet(const std::string& query) const {
      auto cachedValue = _cache.get(query);
      if (!cachedValue.isNull()) {
         return *cachedValue;
      }

      SQLiteStatement stmt(_conn, query);

      int resultCode = stmt.step();
      while (resultCode == SQLITE_BUSY || resultCode == SQLITE_LOCKED || resultCode == SQLITE_IOERR) {
         resultCode = stmt.step();
      }

      switch (resultCode) {
         case SQLITE_ROW:
            break;
         case SQLITE_DONE:
            return std::vector<DynamicObject>();
            break;
         default:
            BOOST_THROW_EXCEPTION(QueryException() << SQL(query) << SQLError(sqlite3_errmsg(_conn)));
      }

      std::vector<DynamicObject> result;
      const int nCols = stmt.n_cols();
      std::vector<std::string> columnNames(nCols);
      for (int i = 0; i < nCols; i++) {
         columnNames[i] = stmt.column_name(i);
      }

      for (; resultCode == SQLITE_ROW || resultCode == SQLITE_BUSY || resultCode == SQLITE_LOCKED ||
             resultCode == SQLITE_IOERR;
           resultCode = stmt.step()) {
         DynamicObject row;

         for (int i = 0; i < nCols; i++) {
            switch (stmt.column_type(i)) {
               case SQLiteStatement::ColumnType::DOUBLE: {
                  row.insert(columnNames[i], stmt.column_double(i));
               } break;
               case SQLiteStatement::ColumnType::INTEGER: {
                  row.insert(columnNames[i], stmt.column_int64(i));
               } break;
               case SQLiteStatement::ColumnType::STRING: {
                  row.insert(columnNames[i], stmt.column_text(i));
               } break;
               case SQLiteStatement::ColumnType::NULLVAL: {
                  row.insert(columnNames[i], DynamicVar());
               } break;
               default:
                  throw std::runtime_error("Unsupported column type");
                  break;
            }
         }

         result.push_back(row);
      }

      DynamicVar dataset{result};
      _cache.add(query, dataset);

      return dataset;
   }

  private:
   std::string _path;
   SQLiteConnection _conn;
   mutable Poco::LRUCache<std::string, DynamicVar> _cache;
};

ProviderRelationalSQLite::ProviderRelationalSQLite(DynamicObject settings) : pimpl{std::make_unique<impl>(settings)} {}

ProviderRelationalSQLite::~ProviderRelationalSQLite() = default;

DynamicVar ProviderRelationalSQLite::GetDataSet(const std::string& query) const { return pimpl->GetDataSet(query); }

}  // namespace datarepository
}  // namespace moja
