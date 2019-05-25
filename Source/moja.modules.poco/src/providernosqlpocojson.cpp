#include "moja/modules/poco/providernosqlpocojson.h"

#include <moja/datarepository/datarepositoryexceptions.h>

#include <moja/dynamic.h>
#include <moja/pocojsonutils.h>

#include <Poco/FileStream.h>
#include <Poco/JSON/Parser.h>
#include <Poco/StreamCopier.h>

#include <sstream>
#include <string>
#include <vector>

using moja::datarepository::FileName;
using moja::datarepository::FileNotFoundException;

namespace moja {
namespace modules {
namespace poco {

ProviderNoSQLPocoJSON::ProviderNoSQLPocoJSON(DynamicObject settings)
    : _filePath(settings["file_path"].convert<std::string>()) {
   _file = Poco::File(_filePath);
   if (!_file.exists()) {
      BOOST_THROW_EXCEPTION(FileNotFoundException() << FileName(_filePath));
   }
   _lastModified = _file.getLastModified();
   std::ostringstream ostr;
   if (_file.exists()) {
      Poco::FileInputStream fis(_filePath);
      Poco::StreamCopier::copyStream(fis, ostr);
      fis.close();
   }
   _jsonStr = ostr.str();

   Poco::JSON::Parser jsonParser;
   auto parsedJSON = jsonParser.parse(ostr.str());
   auto parsedResult = jsonParser.result();
   _data = parsePocoJSONToDynamic(parsedResult);
}

DynamicVector ProviderNoSQLPocoJSON::GetDataSet(const std::string& query) const {
   DynamicVector results;
   results.push_back(_data);
   return results;
}

int ProviderNoSQLPocoJSON::Count() const { return 0; }

}  // namespace poco
}  // namespace modules
}  // namespace moja