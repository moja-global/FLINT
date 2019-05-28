#include "moja/flint/transactionmanagerendofstepmodule.h"

#include "moja/flint/ilandunitdatawrapper.h"

#include <moja/notificationcenter.h>
#include <moja/signals.h>

namespace moja {
namespace flint {

void TransactionManagerEndOfStepModule::configure(const DynamicObject& config) {}

void TransactionManagerEndOfStepModule::subscribe(NotificationCenter& notificationCenter) {
   notificationCenter.subscribe(signals::TimingPrePostInit, &TransactionManagerEndOfStepModule::onTimingPrePostInit,
                                *this);
   notificationCenter.subscribe(signals::TimingPostInit2, &TransactionManagerEndOfStepModule::onTimingPostInit2, *this);
   notificationCenter.subscribe(signals::TimingPreEndStep, &TransactionManagerEndOfStepModule::onTimingPreEndStep,
                                *this);
   notificationCenter.subscribe(signals::TimingPostStep, &TransactionManagerEndOfStepModule::onTimingPostStep, *this);
   notificationCenter.subscribe(signals::PrePostDisturbanceEvent,
                                &TransactionManagerEndOfStepModule::onPrePostDisturbanceEvent, *this);
   notificationCenter.subscribe(signals::PostDisturbanceEvent2,
                                &TransactionManagerEndOfStepModule::onPostDisturbanceEvent2, *this);
   notificationCenter.subscribe(signals::Error, &TransactionManagerEndOfStepModule::onError, *this);
}

void TransactionManagerEndOfStepModule::onTimingPrePostInit() { _landUnitData->applyOperations(); }

void TransactionManagerEndOfStepModule::onTimingPostInit2() { _landUnitData->clearLastAppliedOperationResults(); }

void TransactionManagerEndOfStepModule::onTimingPreEndStep() { _landUnitData->applyOperations(); }

void TransactionManagerEndOfStepModule::onTimingPostStep() { _landUnitData->clearLastAppliedOperationResults(); }

void TransactionManagerEndOfStepModule::onPrePostDisturbanceEvent() { _landUnitData->applyOperations(); }

void TransactionManagerEndOfStepModule::onPostDisturbanceEvent2() { _landUnitData->clearLastAppliedOperationResults(); }

// TODO: Check if this should be the system default, We could leave it upto Systems to clear on Errors or NOT
void TransactionManagerEndOfStepModule::onError(std::string msg) { _landUnitData->clearAllOperationResults(); }

}  // namespace flint
}  // namespace moja
