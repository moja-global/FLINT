#include "moja/flint/calendarandeventsequencer.h"

#include "moja/flint/ilandunitdatawrapper.h"
#include "moja/flint/ivariable.h"

#include <moja/datetime.h>
#include <moja/dynamic.h>
#include <moja/signals.h>

namespace moja {
namespace flint {

bool CalendarAndEventSequencer::Run(NotificationCenter& _notificationCenter, ILandUnitController& luc) {
   auto& _events = _landUnitData->getVariable("events")->value().extract<DynamicVector>();
   auto _nextEvent = _events.begin();

   const auto timingL = _landUnitData->timing();
   ActualStartDate = DateTime(StartDate.year(), StartDate.month(), 1);
   ActualStartDate.addMonths(-1);
   ActualEndDate = DateTime(EndDate.year(), EndDate.month(), 1).addMonths(1);  // start of the next step
   nSteps = ((ActualEndDate.year() - ActualStartDate.year()) * StepsPerYear) + ActualEndDate.month() -
            ActualStartDate.month();

   DateTime initStepStartDate = ActualStartDate;
   DateTime initStepEndDate = ActualStartDate;
   initStepEndDate.addMonths(1);
   double daysInYear = DateTime::daysInYear(initStepStartDate.year());
   double daysInMonth = DateTime::daysOfMonth(initStepStartDate.year(), initStepStartDate.month());
   double stepLengthInYears = daysInMonth / daysInYear;

   timingL->setIsFullStep(true);
   timingL->setStartStepDate(initStepStartDate);
   timingL->setEndStepDate(initStepEndDate);
   timingL->setCurStartDate(initStepStartDate);
   timingL->setCurEndDate(initStepEndDate);
   timingL->setStepLengthInYears(stepLengthInYears);
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
      auto curSubStep = 1;  // if a step isbroken by events this will increment within the Step (1...N)

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
      while (_nextEvent != _events.end()) {
         auto& event = (*(_nextEvent));
         // auto& event = (*(_nextEvent)).extract<const DynamicObject>();
         DateTime date = event["date"].extract<DateTime>();
         if (date < curStepDate) {
            ++_nextEvent;
            continue;
         }
         if (date >= endStepDate) break;
         hadEvent = true;

         // calculate the fracOfStep
         auto span = date - useStartDate;
         double fracOfStep = (span.days() + (span.hours() / 24.0)) /
                             (timingL->stepLengthInYears() * DateTime::daysInYear(useStartDate.year()));

         // setup timing variables
         timingL->setSubStep(curSubStep);
         timingL->setIsFullStep(false);
         timingL->setFractionOfStep(fracOfStep);
         timingL->setCurStartDate(useStartDate);
         timingL->setCurEndDate(date);

         // Prepare & send notification messages for ending current portion of step
         _notificationCenter.postNotificationWithPostNotification(moja::signals::TimingStep);
         _notificationCenter.postNotification(moja::signals::TimingPreEndStep);
         _notificationCenter.postNotification(moja::signals::TimingEndStep);
         _notificationCenter.postNotification(moja::signals::TimingPostStep);

         // setup event timing variables
         timingL->setSubStep(curSubStep);
         timingL->setIsFullStep(true);
         timingL->setFractionOfStep(1.0);
         timingL->setCurStartDate(date);
         timingL->setCurEndDate(date);

         // Event notifications
         _notificationCenter.postNotification(moja::signals::DisturbanceEvent, event);
         _notificationCenter.postNotification(moja::signals::PrePostDisturbanceEvent);
         _notificationCenter.postNotification(moja::signals::PostDisturbanceEvent);
         _notificationCenter.postNotification(moja::signals::PostDisturbanceEvent2);

         // setup for next event
         useStartDate = date;
         ++curSubStep;
         ++_nextEvent;
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

}  // namespace flint
}  // namespace moja
