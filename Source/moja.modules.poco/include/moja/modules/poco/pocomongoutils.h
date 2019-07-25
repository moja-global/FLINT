#ifndef MOJA_MODULES_POCO_MONGOUTILS_H_
#define MOJA_MODULES_POCO_MONGOUTILS_H_

#include "moja/modules/poco/_modules.poco_exports.h"

#include <moja/dynamic.h>

#include <Poco/JSON/Array.h>
#include <Poco/JSON/Object.h>
#include <Poco/MongoDB/Array.h>
#include <Poco/MongoDB/Document.h>

namespace moja {
namespace modules {
namespace poco {

DynamicVector POCO_API ConvertPocoMongoDocumentToDynamic(Poco::MongoDB::Array::Ptr& arr);
DynamicObject POCO_API ConvertPocoMongoDocumentToDynamic(Poco::MongoDB::Document::Ptr& document);
void POCO_API ConvertPocoMongoDocumentToDynamic(DynamicVar& dynamic, Poco::MongoDB::Document::Vector& documents);
void POCO_API ConvertPocoMongoDocumentToDynamic(DynamicVector& vec, Poco::MongoDB::Document::Vector& documents);

Poco::MongoDB::Document::Ptr POCO_API parsePocoJSONToMongoDBObj(Poco::DynamicAny& data);
Poco::MongoDB::Array::Ptr POCO_API parsePocoJSONToMongoDBObj(Poco::JSON::Array::Ptr& val);
Poco::MongoDB::Document::Ptr POCO_API parsePocoJSONToMongoDBObj(Poco::JSON::Object::Ptr& val);

}  // namespace poco
}  // namespace modules
}  // namespace moja

#endif  // MOJA_MODULES_POCO_MONGOUTILS_H_