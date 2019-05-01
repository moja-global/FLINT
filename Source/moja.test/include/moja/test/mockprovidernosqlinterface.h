#ifndef MOJA_TEST_MOCKPROVIDERRELATIONALINTERFACE_H_
#define MOJA_TEST_MOCKPROVIDERRELATIONALINTERFACE_H_

#include <turtle/mock.hpp>
//#include "moja/datarepository/iproviderrelationalinterface.h"
#include "moja/datarepository/iprovidernosqlinterface.h"
#include "moja/datarepository/providernosqlpocomongodb.h"

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockProviderNoSQL, datarepository::IProviderNoSQLInterface) {
	MOCK_METHOD(GetDataSet, 1, moja::Vector(const std::string&))
	MOCK_METHOD(Count, 0, int())
};

MOCK_BASE_CLASS(MockProviderNoSQLPocoMongoDB, datarepository::ProviderNoSQLPocoMongoDB) {
	MOCK_METHOD(GetDataSet, 1, moja::Vector(const std::string&))
	MOCK_METHOD(SendQueryRequest, 2, moja::Vector(const std::string&, int), SendQueryRequest_nofilter)
	MOCK_METHOD(SendQueryRequest, 3, moja::Vector(const std::string&, const std::string&, int), SendQueryRequest_filter)
	MOCK_METHOD(Count, 0, int())
};

}
} // namespace moja::test

#endif // MOJA_TEST_MOCKPROVIDERRELATIONALINTERFACE_H_