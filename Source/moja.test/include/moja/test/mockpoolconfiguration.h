#ifndef MOJA_TEST_MOCKPOOLCONFIGURATION_H_
#define MOJA_TEST_MOCKPOOLCONFIGURATION_H_

#include <moja/flint/configuration/pool.h>

#include <turtle/mock.hpp>

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockPoolConfiguration, moja::flint::configuration::Pool){
   MockPoolConfiguration() : moja::flint::configuration::Pool("a", 0.0){}

   MOCK_METHOD(name, 0, const std::string&()) MOCK_METHOD(initValue, 0, double())
};

}
}  // namespace moja

#endif  // MOJA_TEST_MOCKPOOLCONFIGURATION_H_