#include "moja/flint/equalstepsequencer.h"

#include <moja/events.h>
#include <moja/notificationcenter.h>
#include <moja/signals.h>

namespace moja {
namespace flint {

bool EqualStepSequencer::Run(NotificationCenter& _notificationCenter, ILandUnitController& luc) {
   double daysInYear = 365.0;
   double daysInMonth = 365.0 / 12.0;
   auto stepLengthInYears = daysInMonth / daysInYear;

   _nSteps = ((_endYear - _startYear) * StepsPerYear) + _endStep - _startStep;
   _nSteps++;  // For init step

   auto start = DateTime(_startYear, int((_startStep - 1) / 12.0), 1);
   auto end = DateTime(_endYear, int((_endStep - 1) / 12.0), 31);
   _notificationCenter.postNotification(signals::TimingInit);
   _notificationCenter.postNotification(signals::TimingPostInit);

   int curYearStep = _startStep;
   int curYear = _startYear;

   for (int curStep = 1; curStep <= _nSteps; curStep++) {
      // double curStartStepDecimalDate = curYear + ((curYearStep-1) / 12.0);
      DateTime curStartStepDate(curYear, curYearStep, 1);
      curYearStep++;
      if (curYearStep > 12) {
         curYearStep = 1;
         curYear++;
      }
      // double curEndStepDecimalDate = curYear + ((curYearStep - 1) / 12.0);
      DateTime curEndStepDate(curYear, curYearStep, 1);

      _notificationCenter.postNotification(signals::TimingStep);
      _notificationCenter.postNotification(signals::TimingPreEndStep);
      _notificationCenter.postNotification(signals::TimingEndStep);
   }
   return true;
};

}  // namespace flint
}  // namespace moja
