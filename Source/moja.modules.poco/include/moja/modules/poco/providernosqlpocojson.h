#ifndef MOJA_MODULES_POCO_PROVIDERNOSQLPOCOJSON_H_
#define MOJA_MODULES_POCO_PROVIDERNOSQLPOCOJSON_H_

#include "moja/modules/poco/_modules.poco_exports.h"
#include "moja/datarepository/iprovidernosqlinterface.h"

#include "moja/dynamic.h"

#include <Poco/File.h>

#include <string>

namespace moja {
namespace modules {
namespace poco {

// --------------------------------------------------------------------------------------------
/**
* Moja Implmentation of a NoSQL data provider (using MongoDB as a data source) derived
* from IProviderSpatialVectorInterface.
*/
class POCO_API ProviderNoSQLPocoJSON : public datarepository::IProviderNoSQLInterface {

public:
	explicit ProviderNoSQLPocoJSON(DynamicObject settings);
	virtual ~ProviderNoSQLPocoJSON() {}

	virtual DynamicVector GetDataSet(const std::string& query) const override;

	virtual int Count() const override;

private:
	Poco::File _file;
	Poco::Timestamp _lastModified;

	std::string _filePath;
	std::string _jsonStr;

	DynamicVar _data;
};

}}}

#endif // MOJA_MODULES_POCO_PROVIDERNOSQLPOCOJSON_H_
