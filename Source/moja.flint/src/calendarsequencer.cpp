#include "moja/flint/calendarsequencer.h"

#include "moja/flint/ilandunitdatawrapper.h"

#include <moja/notificationcenter.h>
#include <moja/signals.h>

namespace moja {
namespace flint {

bool CalendarSequencer::Run(NotificationCenter& _notificationCenter, ILandUnitController& luc) {
   const auto timing = _landUnitData->timing();

   _actualStartDate = DateTime(_startDate.year(), _startDate.month(), 1);
   _actualStartDate.addMonths(-1);
   _actualEndDate = DateTime(_endDate.year(), _endDate.month(), 1).addMonths(1);  // start of the next step
   nSteps = ((_actualEndDate.year() - _actualStartDate.year()) * StepsPerYear) + _actualEndDate.month() -
            _actualStartDate.month();

   timing->setCurStartDate(_actualStartDate);
   timing->setCurEndDate(_actualStartDate);
   timing->setStep(0);
   timing->setSubStep(0);

   _notificationCenter.postNotification(moja::signals::TimingInit);
   _notificationCenter.postNotification(moja::signals::TimingPrePostInit);
   _notificationCenter.postNotification(moja::signals::TimingPostInit);
   _notificationCenter.postNotification(moja::signals::TimingPostInit2);
   _actualStartDate.addMonths(1);

   auto curStep = 1;
   auto curStepDate = _actualStartDate;
   auto endStepDate = _actualStartDate;
   endStepDate.addMonths(1);
   while (curStepDate < _actualEndDate) {
      double daysInYear = DateTime::daysInYear(curStepDate.year());
      double daysInMonth = DateTime::daysOfMonth(curStepDate.year(), curStepDate.month());
      auto stepLengthInYears = daysInMonth / daysInYear;

      timing->setIsFullStep(true);
      timing->setStartStepDate(curStepDate);
      timing->setEndStepDate(endStepDate);
      timing->setCurStartDate(curStepDate);
      timing->setCurEndDate(endStepDate);
      timing->setStepLengthInYears(stepLengthInYears);
      timing->setStep(curStep);
      timing->setSubStep(1);
      timing->setFractionOfStep(1);

      auto useStartDate = curStepDate;

      _notificationCenter.postNotification(moja::signals::TimingStep);
      _notificationCenter.postNotification(moja::signals::TimingPreEndStep);
      _notificationCenter.postNotification(moja::signals::TimingEndStep);
      _notificationCenter.postNotification(moja::signals::OutputStep);
      _notificationCenter.postNotification(moja::signals::TimingPostStep);

      curStepDate.addMonths(1);
      endStepDate = curStepDate;
      endStepDate.addMonths(1);
      curStep++;
   }
   _notificationCenter.postNotification(moja::signals::TimingShutdown);

   return true;
};

}  // namespace flint
}  // namespace moja
