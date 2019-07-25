#include "moja/flint/debugnotificationmodule.h"

#include <moja/logging.h>
#include <moja/notificationcenter.h>
#include <moja/signals.h>

namespace moja {
namespace flint {

void DebugNotificationModule::configure(const DynamicObject& config) {}

void DebugNotificationModule::subscribe(NotificationCenter& notificationCenter) {
   notificationCenter.subscribe(signals::SystemInit, &DebugNotificationModule::onSystemInit, *this);
   notificationCenter.subscribe(signals::SystemShutdown, &DebugNotificationModule::onSystemShutdown, *this);
   notificationCenter.subscribe(signals::LocalDomainInit, &DebugNotificationModule::onLocalDomainInit, *this);
   notificationCenter.subscribe(signals::LocalDomainShutdown, &DebugNotificationModule::onLocalDomainShutdown, *this);
   notificationCenter.subscribe(signals::LocalDomainProcessingUnitInit,
                                &DebugNotificationModule::onLocalDomainProcessingUnitInit, *this);
   notificationCenter.subscribe(signals::LocalDomainProcessingUnitShutdown,
                                &DebugNotificationModule::onLocalDomainProcessingUnitShutdown, *this);
   notificationCenter.subscribe(signals::PreTimingSequence, &DebugNotificationModule::onPreTimingSequence, *this);
   notificationCenter.subscribe(signals::TimingInit, &DebugNotificationModule::onTimingInit, *this);
   notificationCenter.subscribe(signals::TimingPostInit, &DebugNotificationModule::onTimingPostInit, *this);
   notificationCenter.subscribe(signals::TimingShutdown, &DebugNotificationModule::onTimingShutdown, *this);
   notificationCenter.subscribe(signals::TimingStep, &DebugNotificationModule::onTimingStep, *this);
   notificationCenter.subscribe(signals::TimingPreEndStep, &DebugNotificationModule::onTimingPreEndStep, *this);
   notificationCenter.subscribe(signals::TimingEndStep, &DebugNotificationModule::onTimingEndStep, *this);
   notificationCenter.subscribe(signals::TimingPostStep, &DebugNotificationModule::onTimingPostStep, *this);
   notificationCenter.subscribe(signals::OutputStep, &DebugNotificationModule::onOutputStep, *this);
   notificationCenter.subscribe(signals::DisturbanceEvent, &DebugNotificationModule::onDisturbanceEvent, *this);
   notificationCenter.subscribe(signals::PostDisturbanceEvent, &DebugNotificationModule::onPostDisturbanceEvent, *this);
   notificationCenter.subscribe(signals::PostNotification, &DebugNotificationModule::onPostNotification, *this);
}

void DebugNotificationModule::onSystemInit() { MOJA_LOG_DEBUG << _localDomainId; }

void DebugNotificationModule::onSystemShutdown() { MOJA_LOG_DEBUG << _localDomainId; }

void DebugNotificationModule::onLocalDomainInit() { MOJA_LOG_DEBUG << _localDomainId; }

void DebugNotificationModule::onLocalDomainShutdown() { MOJA_LOG_DEBUG << _localDomainId; }

void DebugNotificationModule::onLocalDomainProcessingUnitInit() { MOJA_LOG_DEBUG << _localDomainId; }

void DebugNotificationModule::onLocalDomainProcessingUnitShutdown() { MOJA_LOG_DEBUG << _localDomainId; }

void DebugNotificationModule::onPreTimingSequence() { MOJA_LOG_DEBUG << _localDomainId; }

void DebugNotificationModule::onTimingInit() { MOJA_LOG_DEBUG << _localDomainId; }

void DebugNotificationModule::onTimingPostInit() { MOJA_LOG_DEBUG << _localDomainId; }

void DebugNotificationModule::onTimingShutdown() { MOJA_LOG_DEBUG << _localDomainId; }

void DebugNotificationModule::onTimingStep() { MOJA_LOG_DEBUG << _localDomainId; }

void DebugNotificationModule::onTimingPreEndStep() { MOJA_LOG_DEBUG << _localDomainId; }

void DebugNotificationModule::onTimingEndStep() { MOJA_LOG_DEBUG << _localDomainId; }

void DebugNotificationModule::onTimingPostStep() { MOJA_LOG_DEBUG << _localDomainId; }

void DebugNotificationModule::onOutputStep() { MOJA_LOG_DEBUG << _localDomainId; }

void DebugNotificationModule::onError(std::string msg) { MOJA_LOG_DEBUG << _localDomainId; }

void DebugNotificationModule::onDisturbanceEvent(DynamicVar) { MOJA_LOG_DEBUG << _localDomainId; }

void DebugNotificationModule::onPostDisturbanceEvent() { MOJA_LOG_DEBUG << _localDomainId; }

void DebugNotificationModule::onPostNotification(short preMessageSignal) {
   MOJA_LOG_DEBUG << _localDomainId << " : Following notification: " << preMessageSignal;
}

}  // namespace flint
}  // namespace moja
