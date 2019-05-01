#include "moja/flint/transactionmanageraftersubmitmodule.h"
#include "moja/flint/ilandunitdatawrapper.h"

#include "moja/notificationcenter.h"
#include "moja/signals.h"

namespace moja {
namespace flint {

void TransactionManagerAfterSubmitModule::configure(const DynamicObject& config) { }

void TransactionManagerAfterSubmitModule::subscribe(NotificationCenter& notificationCenter) {
	notificationCenter.subscribe(signals::TimingPostStep	, &TransactionManagerAfterSubmitModule::onTimingPostStep,	*this);
	notificationCenter.subscribe(signals::PostNotification	, &TransactionManagerAfterSubmitModule::onPostNotification, *this);
}

void TransactionManagerAfterSubmitModule::onTimingPostStep() {
	//_landUnitData->clearLastAppliedOperationResults();
}

void TransactionManagerAfterSubmitModule::onPostNotification(short) {
	_landUnitData->applyOperations();
}

}
} // namespace moja::flint
