#ifndef MOJA_TEST_MOCKLIBRARYCONFIGURATION_H_
#define MOJA_TEST_MOCKLIBRARYCONFIGURATION_H_

#include <moja/flint/configuration/library.h>

#include <turtle/mock.hpp>

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockLibraryConfiguration, moja::flint::configuration::Library){
   MockLibraryConfiguration() :
       moja::flint::configuration::Library("a", moja::flint::configuration::LibraryType::Internal){}

   MOCK_METHOD(name, 0, std::string&()) MOCK_METHOD(type, 0, moja::flint::configuration::LibraryType())
};

}
}  // namespace moja

#endif  // MOJA_TEST_MOCKLIBRARYCONFIGURATION_H_