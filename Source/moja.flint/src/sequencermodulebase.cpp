#include "moja/flint/sequencermodulebase.h"
#include "moja/flint/landunitdatawrapper.h"
#include "moja/flint/itiming.h"

#include "moja/signals.h"
#include "moja/notificationcenter.h"

namespace moja {
namespace flint {
	void SequencerModuleBase::configure(ITiming& timing)
	{
		auto startYear = timing.startDate().year();
		auto startStep = timing.startDate().month();
		double daysInYear = DateTime::daysInYear(startYear);
		double daysInMonth = DateTime::daysOfMonth(startYear, startStep);
		double stepLengthInYears = daysInMonth / daysInYear;
		timing.setStepLengthInYears(stepLengthInYears);
	}

	bool SequencerModuleBase::Run(NotificationCenter& _notificationCenter, ILandUnitController& luc)
	{
		const auto timingL = _landUnitData->timing();
		timingL->setStep(0);
		timingL->setSubStep(0);

		_notificationCenter.postNotification(moja::signals::TimingInit);
		_notificationCenter.postNotification(moja::signals::OutputStep);
		_notificationCenter.postNotification(moja::signals::TimingShutdown);

		return true;
	}
}
}
