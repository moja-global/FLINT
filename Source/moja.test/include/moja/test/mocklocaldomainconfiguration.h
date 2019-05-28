#ifndef MOJA_TEST_MOCKLOCALDOMAINCONFIGURATION_H_
#define MOJA_TEST_MOCKLOCALDOMAINCONFIGURATION_H_

#include <moja/flint/configuration/localdomain.h>

#include <moja/dynamic.h>

#include <turtle/mock.hpp>

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockLocalDomainConfiguration, moja::flint::configuration::LocalDomain) {
   MockLocalDomainConfiguration()
       : moja::flint::configuration::LocalDomain(moja::flint::configuration::LocalDomainType::SpatialTiled,
                                                 moja::flint::configuration::LocalDomainIterationType::LandscapeTiles,
                                                 true, 1, "a", "b", "c", "d", moja::DynamicObject()) {}

   MOCK_METHOD(type, 0, moja::flint::configuration::LocalDomainType());
   MOCK_METHOD(iterationType, 0, moja::flint::configuration::LocalDomainIterationType());
   MOCK_METHOD(doLogging, 0, bool());
   MOCK_METHOD(numThreads, 0, int());
   MOCK_METHOD(sequencerLibrary, 0, const std::string&());
   MOCK_METHOD(sequencer, 0, const std::string&());
   MOCK_METHOD(landUnitController, 0, const std::string&());
   MOCK_METHOD(simulateLandUnit, 0, const std::string&());
   MOCK_METHOD(landUnitBuildSuccess, 0, const std::string&());
   MOCK_METHOD(landscape, 0, moja::DynamicObject & ());
   MOCK_METHOD(operationMananger, 0, moja::DynamicObject & ());
};

}  // namespace test
}  // namespace moja

#endif  // MOJA_TEST_MOCKLOCALDOMAINCONFIGURATION_H_