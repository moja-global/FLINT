#include "moja/modules/poco/pocomongoutils.h"

#include <moja/datarepository/datarepositoryexceptions.h>

#include <moja/dynamic.h>

#include <Poco/JSON/Parser.h>
#include <Poco/MongoDB/Array.h>
#include <Poco/MongoDB/Document.h>
#include <Poco/MongoDB/ObjectId.h>

#include <boost/format.hpp>

using moja::datarepository::ConnectionFailedException;
using moja::datarepository::FileName;
using moja::datarepository::FileNotFoundException;
using moja::datarepository::NotImplementedException;
using moja::datarepository::QueryException;
using moja::datarepository::SQL;

namespace moja {
namespace modules {
namespace poco {

// --------------------------------------------------------------------------------------------

DynamicVector ConvertPocoMongoDocumentToDynamic(Poco::MongoDB::Array::Ptr& arr) {
   DynamicVector vec;
   auto count = arr->size();
   for (int i = 0; i < count; i++) {
      if (arr->isType<Poco::MongoDB::Document::Ptr>(i)) {
         auto x = arr->get<Poco::MongoDB::Document::Ptr>(i);
         vec.push_back(ConvertPocoMongoDocumentToDynamic(x));
      } else if (arr->isType<Poco::MongoDB::Array::Ptr>(i)) {
         auto a = arr->get<Poco::MongoDB::Array::Ptr>(i);
         vec.push_back(ConvertPocoMongoDocumentToDynamic(a));
      } else if (arr->isType<int>(i)) {
         vec.push_back(arr->get<int>(i));
      } else if (arr->isType<double>(i)) {
         vec.push_back(arr->get<double>(i));
      } else if (arr->isType<std::string>(i)) {
         vec.push_back(arr->get<std::string>(i));
      } else if (arr->isType<bool>(i)) {
         vec.push_back(arr->get<bool>(i));
      } else if (arr->isType<Poco::MongoDB::ObjectId::Ptr>(i)) {
         vec.push_back(arr->get<Poco::MongoDB::ObjectId::Ptr>(i)->toString());
      }
   }
   return vec;
}

DynamicObject ConvertPocoMongoDocumentToDynamic(Poco::MongoDB::Document::Ptr& document) {
   DynamicObject result;

   std::vector<std::string> elementNames;
   document->elementNames(elementNames);

   for (auto& elementName : elementNames) {
      if (document->isType<Poco::MongoDB::Document::Ptr>(elementName)) {
         auto subDocument = document->get<Poco::MongoDB::Document::Ptr>(elementName);
         result[elementName] = ConvertPocoMongoDocumentToDynamic(subDocument);
      } else if (document->isType<Poco::MongoDB::Array::Ptr>(elementName)) {
         auto arr = document->get<Poco::MongoDB::Array::Ptr>(elementName);
         result[elementName] = ConvertPocoMongoDocumentToDynamic(arr);
      } else if (document->isType<Poco::MongoDB::ObjectId::Ptr>(elementName)) {
         auto objId = document->get<Poco::MongoDB::ObjectId::Ptr>(elementName);
         result[elementName] = objId->toString();
      } else if (document->isType<int>(elementName)) {
         result[elementName] = document->get<int>(elementName);
      } else if (document->isType<double>(elementName)) {
         result[elementName] = document->get<double>(elementName);
      } else if (document->isType<std::string>(elementName)) {
         result[elementName] = document->get<std::string>(elementName);
      } else if (document->isType<bool>(elementName)) {
         result[elementName] = document->get<bool>(elementName);
      }
   }

   return result;
}

void ConvertPocoMongoDocumentToDynamic(DynamicVar& dynamic, Poco::MongoDB::Document::Vector& documents) {
   // TODO: look into it
   // Not sure on this so will always return a Vector of documents
   // if (documents.size() == 1) {
   //	dynamic = ConvertPocoMongoDocumentToDynamic(documents[0]);
   //}
   if (documents.size() >= 1) {
      DynamicVector vec;
      for (auto& document : documents) {
         vec.push_back(ConvertPocoMongoDocumentToDynamic(document));
      }
      dynamic = vec;
   } else {
      dynamic = DynamicObject();
   }
}

void ConvertPocoMongoDocumentToDynamic(DynamicVector& vec, Poco::MongoDB::Document::Vector& documents) {
   // TODO: look into it
   // Not sure on this so will always return a Vector of documents
   // if (documents.size() == 1) {
   //	dynamic = ConvertPocoMongoDocumentToDynamic(documents[0]);
   //}
   if (documents.size() >= 1) {
      for (auto& document : documents) {
         vec.push_back(ConvertPocoMongoDocumentToDynamic(document));
      }
   } else {
   }
}

// --------------------------------------------------------------------------------------------

Poco::MongoDB::Array::Ptr parsePocoJSONToMongoDBObj(Poco::JSON::Array::Ptr& val) {
   Poco::MongoDB::Array::Ptr arrayDocument = new Poco::MongoDB::Array();
   auto count = val->size();
   for (auto i = 0; i < count; i++) {
      auto indexStr = (boost::format("%1%") % i).str();

      if (val->isObject(i)) {
         auto object = val->getObject(i);
         auto subDocument = parsePocoJSONToMongoDBObj(object);
         arrayDocument->add(indexStr, subDocument);
      } else if (val->isArray(i)) {
         auto object = val->getArray(i);
         auto subDocument = parsePocoJSONToMongoDBObj(object);
         arrayDocument->add(indexStr, subDocument);
      } else {
         auto object = val->get(i);
         if (object.isArray()) {
            auto msg =
                (boost::format("Unhandled data type in parse of json into poco::mongodb::document - Array")).str();
            BOOST_THROW_EXCEPTION(datarepository::AssertionViolationException()
                                  << moja::datarepository::AssertMsg(msg));
         } else if (object.isBoolean()) {
            auto value = object.extract<bool>();
            arrayDocument->add(indexStr, value);
         } else if (object.isDeque()) {
            auto msg =
                (boost::format("Unhandled data type in parse of json into poco::mongodb::document - Deque")).str();
            BOOST_THROW_EXCEPTION(datarepository::AssertionViolationException()
                                  << moja::datarepository::AssertMsg(msg));
         } else if (object.isEmpty()) {
         } else if (object.isInteger()) {
            auto value = object.extract<int>();
            arrayDocument->add(indexStr, value);
         } else if (object.isList()) {
            auto msg =
                (boost::format("Unhandled data type in parse of json into poco::mongodb::document - List")).str();
            BOOST_THROW_EXCEPTION(datarepository::AssertionViolationException()
                                  << moja::datarepository::AssertMsg(msg));
         } else if (object.isNumeric()) {
            auto value = object.extract<double>();
            arrayDocument->add(indexStr, value);
         } else if (object.isSigned()) {
            auto msg =
                (boost::format("Unhandled data type in parse of json into poco::mongodb::document - Signed")).str();
            BOOST_THROW_EXCEPTION(datarepository::AssertionViolationException()
                                  << moja::datarepository::AssertMsg(msg));
         } else if (object.isString()) {
            auto value = object.extract<std::string>();
            // TODO: Check other types here, Date etc...
            if (value.find("ObjectId(") != std::string::npos) {
               auto oidStr = value.substr(9, 24);
               Poco::MongoDB::ObjectId::Ptr oid = new Poco::MongoDB::ObjectId(oidStr);
               arrayDocument->add(indexStr, oid);
            } else
               arrayDocument->add(indexStr, value);
         } else if (object.isStruct()) {
            auto msg =
                (boost::format("Unhandled data type in parse of json into poco::mongodb::document - Struct")).str();
            BOOST_THROW_EXCEPTION(datarepository::AssertionViolationException()
                                  << moja::datarepository::AssertMsg(msg));
         } else if (object.isVector()) {
            auto msg =
                (boost::format("Unhandled data type in parse of json into poco::mongodb::document - Vector")).str();
            BOOST_THROW_EXCEPTION(datarepository::AssertionViolationException()
                                  << moja::datarepository::AssertMsg(msg));
         } else {
            auto msg =
                (boost::format("Unhandled data type in parse of json into poco::mongodb::document - Unexpected type"))
                    .str();
            BOOST_THROW_EXCEPTION(datarepository::AssertionViolationException()
                                  << moja::datarepository::AssertMsg(msg));
         }
      }
   }
   return arrayDocument;
}

Poco::MongoDB::Document::Ptr parsePocoJSONToMongoDBObj(Poco::JSON::Object::Ptr& val) {
   Poco::MongoDB::Document::Ptr document(new Poco::MongoDB::Document());
   auto& data = *(val.get());

   for (auto& var : data) {
      if (val->isObject(var.first)) {
         auto object = var.second.extract<Poco::JSON::Object::Ptr>();
         auto subDocument = parsePocoJSONToMongoDBObj(object);
         document->add(var.first, subDocument);
      } else if (val->isArray(var.first)) {
         auto object = var.second.extract<Poco::JSON::Array::Ptr>();
         auto subDocument = parsePocoJSONToMongoDBObj(object);
         document->add(var.first, subDocument);
      } else {
         if (var.second.isArray()) {
            auto msg =
                (boost::format("Unhandled data type in parse of json into poco::mongodb::document - Array")).str();
            BOOST_THROW_EXCEPTION(datarepository::AssertionViolationException()
                                  << moja::datarepository::AssertMsg(msg));
         } else if (var.second.isBoolean()) {
            auto value = var.second.extract<bool>();
            document->add(var.first, value);
         } else if (var.second.isDeque()) {
            auto msg =
                (boost::format("Unhandled data type in parse of json into poco::mongodb::document - Deque")).str();
            BOOST_THROW_EXCEPTION(datarepository::AssertionViolationException()
                                  << moja::datarepository::AssertMsg(msg));
         } else if (var.second.isEmpty()) {
         } else if (var.second.isInteger()) {
            auto value = var.second.extract<int>();
            document->add(var.first, value);
         } else if (var.second.isList()) {
            auto msg =
                (boost::format("Unhandled data type in parse of json into poco::mongodb::document - List")).str();
            BOOST_THROW_EXCEPTION(datarepository::AssertionViolationException()
                                  << moja::datarepository::AssertMsg(msg));
         } else if (var.second.isNumeric()) {
            auto value = var.second.extract<double>();
            document->add(var.first, value);
         } else if (var.second.isSigned()) {
            auto msg =
                (boost::format("Unhandled data type in parse of json into poco::mongodb::document - Signed")).str();
            BOOST_THROW_EXCEPTION(datarepository::AssertionViolationException()
                                  << moja::datarepository::AssertMsg(msg));
         } else if (var.second.isString()) {
            auto value = var.second.extract<std::string>();
            // TODO: Check other types here, Date etc...
            if (value.find("ObjectId(") != std::string::npos) {
               auto oidStr = value.substr(9, 24);
               Poco::MongoDB::ObjectId::Ptr oid = new Poco::MongoDB::ObjectId(oidStr);
               document->add(var.first, oid);
            } else
               document->add(var.first, value);
         } else if (var.second.isStruct()) {
            auto msg =
                (boost::format("Unhandled data type in parse of json into poco::mongodb::document - Struct")).str();
            BOOST_THROW_EXCEPTION(datarepository::AssertionViolationException()
                                  << moja::datarepository::AssertMsg(msg));
         } else if (var.second.isVector()) {
            auto msg =
                (boost::format("Unhandled data type in parse of json into poco::mongodb::document - Vector")).str();
            BOOST_THROW_EXCEPTION(datarepository::AssertionViolationException()
                                  << moja::datarepository::AssertMsg(msg));
         } else {
            auto msg =
                (boost::format("Unhandled data type in parse of json into poco::mongodb::document - Unexpected type"))
                    .str();
            BOOST_THROW_EXCEPTION(datarepository::AssertionViolationException()
                                  << moja::datarepository::AssertMsg(msg));
         }
      }
   }
   return document;
}

Poco::MongoDB::Document::Ptr parsePocoJSONToMongoDBObj(Poco::DynamicAny& data) {
   if (data.type() == typeid(Poco::JSON::Object::Ptr)) {
      auto object = data.extract<Poco::JSON::Object::Ptr>();
      return parsePocoJSONToMongoDBObj(object);
   }
   if (data.type() == typeid(Poco::JSON::Array::Ptr)) {
      auto object = data.extract<Poco::JSON::Array::Ptr>();
      return parsePocoJSONToMongoDBObj(object);
   }
   auto msg = (boost::format("Unhandled data type in parse of json into poco::mongodb::document")).str();
   BOOST_THROW_EXCEPTION(datarepository::AssertionViolationException() << moja::datarepository::AssertMsg(msg));
}

}  // namespace poco
}  // namespace modules
}  // namespace moja