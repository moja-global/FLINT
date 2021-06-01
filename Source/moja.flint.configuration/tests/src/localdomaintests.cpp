#include <moja/flint/configuration/localdomain.h>

#include <Poco/Nullable.h>

#include <boost/format.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/test/unit_test.hpp>

namespace flint_configuration {

using moja::DynamicObject;
using moja::flint::configuration::LocalDomain;
using moja::flint::configuration::LocalDomainIterationType;
using moja::flint::configuration::LocalDomainType;

BOOST_AUTO_TEST_SUITE(LocalDomainConfigurationTests);

BOOST_AUTO_TEST_CASE(LocalDomain_ConstructorThrowsExceptionIfSequencerIsEmpty) {
   auto type = LocalDomainType::SpatialTiled;
   auto ittype = LocalDomainIterationType::LandscapeTiles;
   auto badSequencerNames = {"", "  "};
   for (auto sequencerName : badSequencerNames) {
      BOOST_CHECK_THROW(
          LocalDomain(type, ittype, true, 1, "sequencer_library", sequencerName, "dummy", "dummy", DynamicObject()),
          std::invalid_argument);
   }
}

BOOST_AUTO_TEST_CASE(LocalDomain_ConstructorThrowsExceptionIfSimulateLandUnitIsEmpty) {
   auto type = LocalDomainType::SpatialTiled;
   auto ittype = LocalDomainIterationType::LandscapeTiles;
   auto badSimLUNames = {"", "  "};
   for (auto simLUName : badSimLUNames) {
      BOOST_CHECK_THROW(
          LocalDomain(type, ittype, true, 1, "sequencer_library", "dummy", simLUName, "dummy", DynamicObject()),
          std::invalid_argument);
   }
}

BOOST_AUTO_TEST_CASE(LocalDomain_GetType) {
   auto type = LocalDomainType::SpatiallyReferencedSQL;
   auto ittype = LocalDomainIterationType::LandscapeTiles;
   LocalDomain localDomain(type, ittype, true, 1, "sequencer_library", "sequencer", "dummy", "dummy", DynamicObject());
   BOOST_CHECK(type == localDomain.type());
}

BOOST_AUTO_TEST_CASE(LocalDomain_GetSequencer) {
   auto type = LocalDomainType::SpatialTiled;
   auto ittype = LocalDomainIterationType::LandscapeTiles;
   auto sequencerName = "test";
   LocalDomain localDomain(type, ittype, true, 1, "sequencer_library", sequencerName, "dummy", "dummy",
                           DynamicObject());
   BOOST_CHECK_EQUAL(sequencerName, localDomain.sequencer());
}

BOOST_AUTO_TEST_CASE(LocalDomain_GetSimulateLandUnit) {
   auto type = LocalDomainType::SpatialTiled;
   auto ittype = LocalDomainIterationType::LandscapeTiles;
   auto simLUName = "test";
   LocalDomain localDomain(type, ittype, true, 1, "sequencer_library", "dummy", simLUName, "dummy", DynamicObject());
   BOOST_CHECK_EQUAL(simLUName, localDomain.simulateLandUnit());
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace flint_configuration
