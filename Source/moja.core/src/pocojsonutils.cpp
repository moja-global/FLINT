#include "moja/pocojsonutils.h"
#include "moja/coreexceptions.h"
#include "moja/exception.h"

#include <Poco/JSON/Parser.h>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

using moja::FileNotFoundException;
using moja::FileName;
using moja::NotImplementedException;

namespace moja {

// --------------------------------------------------------------------------------------------
// "yyyy/mm/dd" -> DateTime

DateTime parseSimpleDate(const std::string& yyyymmdd) {
	std::vector<std::string> tokens;
	boost::split(tokens, yyyymmdd, boost::is_any_of("/"));
	return DateTime(std::stoi(tokens[0]),
		std::stoi(tokens[1]),
		std::stoi(tokens[2]));
}

// --------------------------------------------------------------------------------------------

DynamicVector parsePocoJSONToDynamic(const Poco::JSON::Array::Ptr& val) {
	DynamicVector arrayDocument;
	const auto count = val->size();
	for (auto i = 0; i < count; i++) {
		
		if (val->isObject(i)) {
			auto object = val->getObject(i);
			auto subDocument = parsePocoJSONToDynamic(object);
			arrayDocument.push_back(subDocument);
		}
		else if (val->isArray(i)) {
			auto object = val->getArray(i);
			auto subDocument = parsePocoJSONToDynamic(object);
			arrayDocument.push_back(subDocument);
		}
		else {
			auto object = val->get(i);
			if (object.isArray()) {
				auto msg = (boost::format("Unhandled data type in parse of json into dynamic - Array")).str();
				BOOST_THROW_EXCEPTION(CoreAssertionViolationException() << moja::AssertMsg(msg));
			}
			else if (object.isBoolean()) {
				auto value = object.extract<bool>();
				arrayDocument.push_back(value);
			}
			else if (object.isDeque()) {
				auto msg = (boost::format("Unhandled data type in parse of json into dynamic - Deque")).str();
				BOOST_THROW_EXCEPTION(CoreAssertionViolationException() << moja::AssertMsg(msg));
			}
			else if (object.isEmpty()) {
			}
			else if (object.isInteger()) {
				try {
					auto value = object.extract<int>();
					arrayDocument.push_back(value);
				}
				catch (Poco::BadCastException) {
					auto value = object.extract<Int64>();
					arrayDocument.push_back(value);
				}
			}
			else if (object.isList()) {
				auto msg = (boost::format("Unhandled data type in parse of json into dynamic - List")).str();
				BOOST_THROW_EXCEPTION(CoreAssertionViolationException() << moja::AssertMsg(msg));
			}
			else if (object.isNumeric()) {
				auto value = object.extract<double>();
				arrayDocument.push_back(value);
			}
			else if (object.isSigned()) {
				auto msg = (boost::format("Unhandled data type in parse of json into dynamic - Signed")).str();
				BOOST_THROW_EXCEPTION(CoreAssertionViolationException() << moja::AssertMsg(msg));
			}
			else if (object.isString()) {
				auto value = object.extract<std::string>();
				arrayDocument.push_back(value);
			}
			else if (object.isStruct()) {
				auto msg = (boost::format("Unhandled data type in parse of json into dynamic - Struct")).str();
				BOOST_THROW_EXCEPTION(CoreAssertionViolationException() << moja::AssertMsg(msg));
			}
			else if (object.isVector()) {
				auto msg = (boost::format("Unhandled data type in parse of json into dynamic - Vector")).str();
				BOOST_THROW_EXCEPTION(CoreAssertionViolationException() << moja::AssertMsg(msg));
			}
			else {
				auto msg = (boost::format("Unhandled data type in parse of json into dynamic - Unexpected type")).str();
				BOOST_THROW_EXCEPTION(CoreAssertionViolationException() << moja::AssertMsg(msg));
			}
		}
	}
	return arrayDocument;
}

// --------------------------------------------------------------------------------------------

DynamicVar parsePocoJSONToDynamic(const Poco::JSON::Object::Ptr& val) {
	const auto& data = *(val.get());

	// Check for flint dspecial data $date, $time_series etc
	if (data.size() == 1) {
		auto item = *data.begin();
		if (item.first == "$date") {
			const auto& dateStr = item.second.extract<std::string>();
			DynamicVar result = parseSimpleDate(dateStr);
			return result;
		}
	}
	auto document = DynamicObject();
	for (auto& var : data) {
		if (val->isObject(var.first)) {
			const auto& object = var.second.extract<Poco::JSON::Object::Ptr>();
			const auto subDocument = parsePocoJSONToDynamic(object);
			document[var.first] = subDocument;
		}
		else if (val->isArray(var.first)) {
			const auto object = var.second.extract<Poco::JSON::Array::Ptr>();
			const auto subDocument = parsePocoJSONToDynamic(object);
			document[var.first] = subDocument;
		}
		else {
			if (var.second.isArray()) {
				auto msg = (boost::format("Unhandled data type in parse of json into Dynamic - Array")).str();
				BOOST_THROW_EXCEPTION(CoreAssertionViolationException() << moja::AssertMsg(msg));
			}
			else if (var.second.isBoolean()) {
				auto value = var.second.extract<bool>();
				document[var.first] = value;
			}
			else if (var.second.isDeque()) {
				auto msg = (boost::format("Unhandled data type in parse of json into Dynamic - Deque")).str();
				BOOST_THROW_EXCEPTION(CoreAssertionViolationException() << moja::AssertMsg(msg));
			}
			else if (var.second.isEmpty()) {
			}
			else if (var.second.isInteger()) {
				try {
					auto value = var.second.extract<int>();
					document[var.first] = value;
				}
				catch (Poco::BadCastException) {
					auto value = var.second.extract<Int64>();
					document[var.first] = value;
				}
			}
			else if (var.second.isList()) {
				auto msg = (boost::format("Unhandled data type in parse of json into Dynamic - List")).str();
				BOOST_THROW_EXCEPTION(CoreAssertionViolationException() << moja::AssertMsg(msg));
			}
			else if (var.second.isNumeric()) {
				auto value = var.second.extract<double>();
				document[var.first] = value;
			}
			else if (var.second.isSigned()) {
				auto msg = (boost::format("Unhandled data type in parse of json into Dynamic - Signed")).str();
				BOOST_THROW_EXCEPTION(CoreAssertionViolationException() << moja::AssertMsg(msg));
			}
			else if (var.second.isString()) {
				auto value = var.second.extract<std::string>();
				document[var.first] = value;
			}
			else if (var.second.isStruct()) {
				auto msg = (boost::format("Unhandled data type in parse of json into Dynamic - Struct")).str();
				BOOST_THROW_EXCEPTION(CoreAssertionViolationException() << moja::AssertMsg(msg));
			}
			else if (var.second.isVector()) {
				auto msg = (boost::format("Unhandled data type in parse of json into Dynamic - Vector")).str();
				BOOST_THROW_EXCEPTION(CoreAssertionViolationException() << moja::AssertMsg(msg));
			}
			else {
				auto msg = (boost::format("Unhandled data type in parse of json into Dynamic - Unexpected type")).str();
				BOOST_THROW_EXCEPTION(CoreAssertionViolationException() << moja::AssertMsg(msg));
			}
		}
	}
	return document;
}

// --------------------------------------------------------------------------------------------

DynamicVar parsePocoJSONToDynamic(const DynamicVar& data) {
	if (data.isEmpty())
		return DynamicVar();
	if (data.type() == typeid(Poco::JSON::Object::Ptr)) {
		const auto& object = data.extract<Poco::JSON::Object::Ptr>();
		return parsePocoJSONToDynamic(object);
	}
	if (data.type() == typeid(Poco::JSON::Array::Ptr)) {
		const auto& object = data.extract<Poco::JSON::Array::Ptr>();
		return parsePocoJSONToDynamic(object);
	}
	if (data.isEmpty()) {
		return DynamicVar();
	}
	if (data.isBoolean()) {
		DynamicVar result = data.extract<bool>();
		return result;
	}
	if (data.isString()) {
		DynamicVar result = data.extract<const std::string>();
		return result;
	}
	if (data.isInteger()) {
		DynamicVar result = data.extract<int>();
		return result;
	}
	if (data.isNumeric()) {
		DynamicVar result = data.extract<double>();
		return result;
	}
	auto msg = "Unhandled data type in parse of json into dynamic";
	BOOST_THROW_EXCEPTION(CoreAssertionViolationException() << moja::AssertMsg(msg));
}

// --------------------------------------------------------------------------------------------

DynamicVar parsePocoVarToDynamic(const DynamicVar& var) {
	if (var.isEmpty()) {
		return DynamicVar();
	}
	if (var.isString()) {
		return DynamicVar(var.extract<std::string>());
	}
	if (var.isBoolean()) {
		return DynamicVar(var.extract<bool>());
	}
	if (var.isInteger()) {
		try {
			return DynamicVar(var.extract<int>());
		}
		catch (Poco::BadCastException) {
			return DynamicVar(var.convert<Int64>());
		}
	}
	if (var.isNumeric()) {
		return DynamicVar(var.extract<double>());
	}	
    if (var.type() == typeid(Poco::JSON::Array::Ptr)) {
        auto arr = var.extract<Poco::JSON::Array::Ptr>();
        std::vector<DynamicVar> values;
        for (auto it = arr->begin(); it != arr->end(); ++it) {
            values.push_back(parsePocoVarToDynamic(*it));
        }
        return values;
    }
    if (var.isArray()) {
		auto arr = var.extract<Poco::JSON::Array::ValueVec>();
		std::vector<DynamicVar> values;
		for (auto value : arr) {
			values.push_back(parsePocoVarToDynamic(value));
		}
		return values;
	}
	if (var.isStruct()) {
		auto s = var.extract<Poco::DynamicStruct>();
		if (s.size() == 1) {	// We may have a special data type here - "$date" ...
			auto keyValPair = s.begin();
			if (keyValPair->first == "$date") {
				// we have found a date
				// so are expecting a string as the value
				// Expecting { "$date" : "2006/01/01" }
				auto dateStr = keyValPair->second.extract<std::string>();
				DynamicVar result = parseSimpleDate(dateStr);
				return result;
			}
			if (keyValPair->first == "$uint32") {
				// we have found a uint32
				// so are expecting a string as the value
				// Expecting { "$uint32" : 1158849901 }
				int num = keyValPair->second;
				DynamicVar result = UInt32(num);
				return result;
			}
		}
		DynamicObject result;
		for (auto item : s) {
			result[item.first] = parsePocoVarToDynamic(item.second);
		}
		return result;
	}

	try {
		auto obj = var.extract<Poco::JSON::Object::Ptr>();
		DynamicObject result;
		for (auto kvp : *obj) {
			result[kvp.first] = parsePocoVarToDynamic(kvp.second);
		}
		return result;
	}
	catch (std::exception) {}

	auto msg = (boost::format("Unhandled data type in parse of Poco::Dynamic::Var into Dynamic")).str();
	BOOST_THROW_EXCEPTION(CoreUnhandledType() << moja::TypeErrorMsg(msg));
}

} // namespace moja