#ifndef MOJA_TEST_MOCKPROVIDERRELATIONALINTERFACE_H_
#define MOJA_TEST_MOCKPROVIDERRELATIONALINTERFACE_H_

#include <moja/datarepository/iproviderrelationalinterface.h>

#include <turtle/mock.hpp>

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockProviderRelational, datarepository::IProviderRelationalInterface) {
   MOCK_METHOD(GetDataSet, 1, DynamicVar(const std::string&));
};

}
}  // namespace moja

#endif  // MOJA_TEST_MOCKPROVIDERRELATIONALINTERFACE_H_