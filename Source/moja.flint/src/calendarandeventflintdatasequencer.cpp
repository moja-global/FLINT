#include "moja/flint/calendarandeventflintdatasequencer.h"

#include "moja/flint/eventqueue.h"
#include "moja/flint/ilandunitdatawrapper.h"
#include "moja/flint/ivariable.h"

#include "moja/signals.h"
#include "moja/dynamic.h"

namespace moja {
namespace flint {

bool CalendarAndEventFlintDataSequencer::Run(NotificationCenter& _notificationCenter, flint::ILandUnitController& luc) {

	auto eventQueue = std::static_pointer_cast<flint::EventQueue>(_landUnitData->getVariable("eventqueue")->value().extract<std::shared_ptr<flint::IFlintData>>());
	auto nextEvent = eventQueue->begin();

	const auto timingL = _landUnitData->timing();
	ActualStartDate = DateTime(StartDate.year(), StartDate.month(), 1);
	ActualStartDate.addMonths(-1);
	ActualEndDate = DateTime(EndDate.year(), EndDate.month(), 1).addMonths(1); // start of the next step
	nSteps = ((ActualEndDate.year() - ActualStartDate.year()) * StepsPerYear) + ActualEndDate.month() - ActualStartDate.month();

	DateTime initStepEndDate = ActualStartDate;
	initStepEndDate.addMonths(1).addMilliseconds(-1);

	timingL->setCurStartDate(ActualStartDate);
	timingL->setCurEndDate(initStepEndDate);
	timingL->setStep(0);
	timingL->setSubStep(0);
	timingL->setFractionOfStep(1);

	_notificationCenter.postNotification(moja::signals::TimingInit);
	_notificationCenter.postNotification(moja::signals::TimingPrePostInit);
	_notificationCenter.postNotification(moja::signals::TimingPostInit);
	_notificationCenter.postNotification(moja::signals::TimingPostInit2);

	ActualStartDate.addMonths(1);
	auto curStep = 1;
	auto curStepDate = ActualStartDate;
	auto endStepDate = ActualStartDate;
	endStepDate.addMonths(1);

	while (curStepDate < ActualEndDate) {
		double daysInYear = DateTime::daysInYear(curStepDate.year());
		double daysInMonth = DateTime::daysOfMonth(curStepDate.year(), curStepDate.month());
		auto stepLengthInYears = daysInMonth / daysInYear;
		auto curSubStep = 1;	// if a step isbroken by events this will increment within the Step (1...N)

		timingL->setIsFullStep(true);
		timingL->setStartStepDate(curStepDate);
		timingL->setEndStepDate(endStepDate);
		timingL->setCurStartDate(curStepDate);
		timingL->setCurEndDate(endStepDate);
		timingL->setStepLengthInYears(stepLengthInYears);
		timingL->setStep(curStep);
		timingL->setSubStep(curSubStep);
		timingL->setFractionOfStep(1);

		auto useStartDate = curStepDate;

		bool hadEvent = false;
		while (nextEvent != eventQueue->end()) {
			const auto& curEvent = (*(nextEvent));	// Should be type EventQueue::EventQueueRecord
			if (curEvent._date < curStepDate) {
				++nextEvent;
				continue;
			}
			if (curEvent._date >= endStepDate)
				break;
			hadEvent = true;

			// calculate the fracOfStep
			auto span = curEvent._date - useStartDate;
			double fracOfStep = (span.days() + (span.hours() / 24.0)) / (timingL->stepLengthInYears() * DateTime::daysInYear(useStartDate.year()));

			// setup timing variables
			timingL->setSubStep(curSubStep);
			timingL->setIsFullStep(false);
			timingL->setFractionOfStep(fracOfStep);
			timingL->setCurStartDate(useStartDate);
			timingL->setCurEndDate(curEvent._date);

			// Prepare & send notification messages for ending current portion of step
			_notificationCenter.postNotificationWithPostNotification(moja::signals::TimingStep);
			_notificationCenter.postNotification(moja::signals::TimingPreEndStep);
			_notificationCenter.postNotification(moja::signals::TimingEndStep);
			_notificationCenter.postNotification(moja::signals::TimingPostStep);

			// setup curEvent timing variables
			timingL->setSubStep(curSubStep);
			timingL->setIsFullStep(true);
			timingL->setFractionOfStep(1.0);
			timingL->setCurStartDate(curEvent._date);
			timingL->setCurEndDate(curEvent._date);

			// Event notifications
			_notificationCenter.postNotification(moja::signals::DisturbanceEvent, &curEvent);
			_notificationCenter.postNotification(moja::signals::PrePostDisturbanceEvent);
			_notificationCenter.postNotification(moja::signals::PostDisturbanceEvent);
			_notificationCenter.postNotification(moja::signals::PostDisturbanceEvent2);

			// setup for next event
			useStartDate = curEvent._date;
			++curSubStep;
			++nextEvent;
		}

		if (hadEvent) {
			auto stepLengthInDays = timingL->stepLengthInYears() * DateTime::daysInYear(useStartDate.year());
			auto span = endStepDate - useStartDate;
			double fracOfStep = (span.days() + (span.hours() / 24.0)) / stepLengthInDays;

			timingL->setSubStep(curSubStep);
			timingL->setCurStartDate(useStartDate);
			timingL->setCurEndDate(endStepDate);
			timingL->setFractionOfStep(fracOfStep);
		}

		_notificationCenter.postNotificationWithPostNotification(moja::signals::TimingStep);
		_notificationCenter.postNotification(moja::signals::TimingPreEndStep);
		_notificationCenter.postNotification(moja::signals::TimingEndStep);
		_notificationCenter.postNotification(moja::signals::OutputStep);
		_notificationCenter.postNotification(moja::signals::TimingPostStep);

		curStepDate.addMonths(1);
		endStepDate = curStepDate;
		endStepDate.addMonths(1);
		++curStep;
	}
	_notificationCenter.postNotification(moja::signals::TimingShutdown);
	return true;
};

}
} // namespace moja::flint
