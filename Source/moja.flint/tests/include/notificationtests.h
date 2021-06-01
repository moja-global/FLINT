#pragma once

#include <moja/flint/aggregatorstockstep.h>
#include <moja/flint/debugnotificationmodule.h>
#include <moja/flint/localdomaincontrollerbase.h>
#include <moja/flint/sequencermodulebase.h>
#include <moja/flint/spinuplandunitcontroller.h>

#include <moja/dynamic.h>
#include <moja/signals.h>

#include <memory>

namespace flint {

// --------------------------------------------------------------------------------------------
// -- Testing Modules

class TestNotificationModule : public moja::flint::ModuleBase {
  public:
   TestNotificationModule(std::vector<std::string>& callSequence)
       : ModuleBase(), _callCount(0), _sharedCallSequence(callSequence), _localDomainId(-1) {}
   virtual ~TestNotificationModule() {}

   void configure(const moja::DynamicObject& config) override;
   
   void subscribe(moja::NotificationCenter& notificationCenter) override;

   void onSystemInit() override;
   void onSystemShutdown() override;
   void onLocalDomainInit() override;
   void onLocalDomainShutdown() override;
   void onLocalDomainProcessingUnitInit() override;
   void onLocalDomainProcessingUnitShutdown() override;
   void onPreTimingSequence() override;
   void onTimingInit() override;
   void onTimingPrePostInit() override;
   void onTimingPostInit() override;
   void onTimingPostInit2() override;
   void onTimingShutdown() override;
   void onTimingStep() override;
   void onTimingPreEndStep() override;
   void onTimingEndStep() override;
   void onTimingPostStep() override;
   void onOutputStep() override;
   void onError(std::string msg) override;
   void onDisturbanceEvent(moja::DynamicVar) override;
   void onPrePostDisturbanceEvent() override;
   void onPostDisturbanceEvent() override;
   void onPostDisturbanceEvent2() override;
   void onPostNotification(short preMessageSignal) override;

   /// For debugging checks
   int _callCount;
   std::vector<std::string> _callSequence;
   std::vector<std::string>& _sharedCallSequence;
   std::string _debugName;

  private:
   int _localDomainId;
};

// --------------------------------------------------------------------------------------------

#define POST_NOT_AND_RECORD(s, v)              \
   {                                           \
      _notificationCenter.postNotification(s); \
      v.push_back(#s);                         \
   }
#define POST_NOT_WITH_ARG_AND_RECORD(s, a, v)     \
   {                                              \
      _notificationCenter.postNotification(s, a); \
      v.push_back(#s);                            \
   }

class TestSequencer01 : public moja::flint::SequencerModuleBase {
  public:
   TestSequencer01(){}
   virtual ~TestSequencer01(){}
   bool Run(moja::NotificationCenter& _notificationCenter, moja::flint::ILandUnitController& luc) override {
      const moja::DynamicVar eventTest;
      short testSignal = 42;
      std::string errorMsg = "errorMsg";

      POST_NOT_AND_RECORD(moja::signals::SystemInit, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::SystemShutdown, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::LocalDomainInit, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::LocalDomainShutdown, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::LocalDomainProcessingUnitInit, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::LocalDomainProcessingUnitShutdown, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::PreTimingSequence, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::TimingInit, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::TimingPrePostInit, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::TimingPostInit, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::TimingPostInit2, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::TimingShutdown, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::TimingStep, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::TimingPreEndStep, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::TimingEndStep, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::TimingPostStep, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::OutputStep, _postSequence);
      POST_NOT_WITH_ARG_AND_RECORD(moja::signals::Error, errorMsg, _postSequence);
      POST_NOT_WITH_ARG_AND_RECORD(moja::signals::DisturbanceEvent, eventTest, _postSequence);  // ** has arg
      POST_NOT_AND_RECORD(moja::signals::PrePostDisturbanceEvent, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::PostDisturbanceEvent, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::PostDisturbanceEvent2, _postSequence);
      POST_NOT_WITH_ARG_AND_RECORD(moja::signals::PostNotification, testSignal, _postSequence);

      return true;
   };

   std::vector<std::string> _postSequence;
};

#undef POST_NOT_AND_RECORD
#undef POST_NOT_WITH_ARG_AND_RECORD

// --------------------------------------------------------------------------------------------

#define POST_NOT_AND_RECORD(s, v)                                  \
   {                                                               \
      _notificationCenter.postNotificationWithPostNotification(s); \
      v.push_back(#s);                                             \
   }
#define POST_NOT_WITH_ARG_AND_RECORD(s, a, v)                         \
   {                                                                  \
      _notificationCenter.postNotificationWithPostNotification(s, a); \
      v.push_back(#s);                                                \
   }

class TestSequencer02 : public moja::flint::SequencerModuleBase {
  public:
   TestSequencer02(){}
   virtual ~TestSequencer02(){}
   bool Run(moja::NotificationCenter& _notificationCenter, moja::flint::ILandUnitController& luc) override {
      moja::DynamicVar eventTest;
      short testSignal = 42;
      std::string errorMsg = "errorMsg";

      POST_NOT_AND_RECORD(moja::signals::SystemInit, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::SystemShutdown, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::LocalDomainInit, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::LocalDomainShutdown, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::LocalDomainProcessingUnitInit, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::LocalDomainProcessingUnitShutdown, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::PreTimingSequence, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::TimingInit, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::TimingPrePostInit, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::TimingPostInit, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::TimingPostInit2, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::TimingShutdown, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::TimingStep, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::TimingPreEndStep, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::TimingEndStep, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::TimingPostStep, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::OutputStep, _postSequence);
      POST_NOT_WITH_ARG_AND_RECORD(moja::signals::Error, errorMsg, _postSequence);
      POST_NOT_WITH_ARG_AND_RECORD(moja::signals::DisturbanceEvent, eventTest, _postSequence);  // ** has arg
      POST_NOT_AND_RECORD(moja::signals::PrePostDisturbanceEvent, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::PostDisturbanceEvent, _postSequence);
      POST_NOT_AND_RECORD(moja::signals::PostDisturbanceEvent2, _postSequence);
      POST_NOT_WITH_ARG_AND_RECORD(moja::signals::PostNotification, testSignal, _postSequence);

      return true;
   };

   std::vector<std::string> _postSequence;
};

#undef POST_NOT_AND_RECORD
#undef POST_NOT_WITH_ARG_AND_RECORD

// --------------------------------------------------------------------------------------------

#define POST_NOT_AND_RECORD(s, v)              \
   {                                           \
      _notificationCenter.postNotification(s); \
      v.push_back(#s);                         \
   }
#define POST_NOT_WITH_ARG_AND_RECORD(s, a, v)     \
   {                                              \
      _notificationCenter.postNotification(s, a); \
      v.push_back(#s);                            \
   }

class TestSequencer03 : public moja::flint::SequencerModuleBase {
  public:
   TestSequencer03(){}
   virtual ~TestSequencer03(){}
   bool Run(moja::NotificationCenter& _notificationCenter, moja::flint::ILandUnitController& luc) override {
      moja::DynamicObject eventTest;
      POST_NOT_AND_RECORD(moja::signals::SystemInit, _postSequence);

      return true;
   };

   std::vector<std::string> _postSequence;
};

#undef POST_NOT_AND_RECORD
#undef POST_NOT_WITH_ARG_AND_RECORD

// --------------------------------------------------------------------------------------------
// -- Module Factory Stuff

extern "C" MOJA_LIB_API int getModuleRegistrations(moja::flint::ModuleRegistration* outModuleRegistrations);
extern "C" MOJA_LIB_API int getTransformRegistrations(moja::flint::TransformRegistration* outTransformRegistrations);
extern "C" MOJA_LIB_API int getFlintDataRegistrations(moja::flint::FlintDataRegistration* outFlintDataRegistrations);
extern "C" MOJA_LIB_API int getFlintDataFactoryRegistrations(
    moja::flint::FlintDataFactoryRegistration* outFlintDataFactoryRegistrations);
extern "C" MOJA_LIB_API int getDataRepositoryProviderRegistrations(
    moja::flint::DataRepositoryProviderRegistration* outDataRepositoryProviderRegistration);

// --------------------------------------------------------------------------------------------
// -- Test LocalDomain

class TestLocalDomainController final : public moja::flint::LocalDomainControllerBase {
  public:
   TestLocalDomainController()
       : LocalDomainControllerBase(std::make_shared<moja::flint::FlintLibraryHandles>(
             "internal.moja.flint.test", getModuleRegistrations, getTransformRegistrations, getFlintDataRegistrations,
             getFlintDataFactoryRegistrations, getDataRepositoryProviderRegistrations)) {}
   ~TestLocalDomainController() = default;

   virtual void configure(const moja::flint::configuration::Configuration& config) override {
      moja::flint::LocalDomainControllerBase::configure(config);
   }
   virtual void run() override { _sequencer->Run(_notificationCenter, _landUnitController); };
};

}  // namespace flint

