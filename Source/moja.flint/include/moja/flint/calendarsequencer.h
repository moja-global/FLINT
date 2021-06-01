#pragma once

#include "moja/flint/itiming.h"
#include "moja/flint/sequencermodulebase.h"

#include <moja/datetime.h>

namespace moja::flint {

// This sequencer handles the events and fracOfStep calculations
// all in the one loop (rather than using EventHandlerModule & timingModule)
// makes the fracOfStep and notification messages easier to calculate
class CalendarSequencer : public SequencerModuleBase {
  public:
   CalendarSequencer(){};
   virtual ~CalendarSequencer() {}

   void configure(ITiming& timing) override {
      _startDate = timing.startDate();
      _endDate = timing.endDate();
   }

   bool Run(NotificationCenter& _notificationCenter, ILandUnitController& luc) override;

  private:
   DateTime _startDate;
   DateTime _endDate;
   DateTime _actualStartDate;
   DateTime _actualEndDate;
   int nSteps;

   const int StepsPerYear = 12;
};

}  // namespace moja::flint
