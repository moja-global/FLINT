#ifndef MOJA_CORE_POCOUTILS_H_
#define MOJA_CORE_POCOUTILS_H_

#include "_core_exports.h"

#include "moja/dynamic.h"
#include "moja/datetime.h"

#include <Poco/JSON/Array.h>
#include <Poco/JSON/Object.h>

namespace moja {

DateTime		CORE_API parseSimpleDate(const std::string& yyyymmdd);
	
DynamicVar		CORE_API parsePocoJSONToDynamic(const DynamicVar& data);
DynamicVector	CORE_API parsePocoJSONToDynamic(const Poco::JSON::Array::Ptr& val);
DynamicVar		CORE_API parsePocoJSONToDynamic(const Poco::JSON::Object::Ptr& val);

DynamicVar		CORE_API parsePocoVarToDynamic(const DynamicVar& var);

} // moja

#endif // MOJA_CORE_POCOUTILS_H_