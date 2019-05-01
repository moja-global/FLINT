#ifndef MOJA_TEST_MOCKDATAREPOSITORY_H_
#define MOJA_TEST_MOCKDATAREPOSITORY_H_

#include <turtle/mock.hpp>
#include "moja/datarepository/datarepository.h"
#include "moja/datarepository/iproviderinterface.h"

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockDataRepository, moja::datarepository::DataRepository) {
	MOCK_METHOD(configure, 1, void(const std::vector<const flint::configuration::Provider*>&))
	MOCK_METHOD(getProvider, 1, std::shared_ptr<datarepository::IProviderInterface>(const std::string&))
};

}
} // namespace moja::test

#endif // MOJA_TEST_MOCKDATAREPOSITORY_H_