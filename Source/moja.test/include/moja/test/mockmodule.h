#ifndef MOJA_TEST_MOCKMODULE_H_
#define MOJA_TEST_MOCKMODULE_H_

#include <moja/flint/modulebase.h>

#include <moja/dynamic.h>

#include <turtle/mock.hpp>

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockModule, flint::ModuleBase) {
   MOCK_METHOD(metaData, 0, flint::ModuleMetaData & (void));
   MOCK_METHOD(setLandUnitController, 1, void(flint::ILandUnitController&));
   MOCK_METHOD(configure, 2, void(NotificationCenter&, DynamicObject&));
   MOCK_METHOD(results, 0, std::vector<flint::StepRecord> * (void));
   MOCK_METHOD(StartupModule, 1, void(const flint::ModuleMetaData&));
   MOCK_METHOD(ShutdownModule, 0, void());
   MOCK_METHOD(onSystemInit, 0, void());
   MOCK_METHOD(onSystemShutdown, 0, void());
   MOCK_METHOD(onLocalDomainInit, 0, void());
   MOCK_METHOD(onLocalDomainShutdown, 0, void());
   MOCK_METHOD(onPreTimingSequence, 0, void());
   MOCK_METHOD(onTimingInit, 0, void());
   MOCK_METHOD(onTimingPostInit, 0, void());
   MOCK_METHOD(onTimingShutdown, 0, void());
   MOCK_METHOD(onTimingStep, 0, void());
   MOCK_METHOD(onTimingPreEndStep, 0, void());
   MOCK_METHOD(onTimingEndStep, 0, void());
   MOCK_METHOD(onTimingPostStep, 0, void());
   MOCK_METHOD(onOutputStep, 0, void());
   MOCK_METHOD(onDisturbanceEvent, 1, void(DynamicVar));
   MOCK_METHOD(onPostDisturbanceEvent, 0, void());
   MOCK_METHOD(onPostNotification, 1, void(const std::string&));
};

}  // namespace test
}  // namespace moja

#endif  // MOJA_TEST_MOCKMODULE_H_