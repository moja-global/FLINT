#pragma once

#include "moja/flint/itiming.h"
#include "moja/flint/librarymanager.h"
#include "moja/flint/sequencermodulebase.h"

#include <moja/datetime.h>

namespace moja::flint {

// This sequencer handles the events and fracOfStep calculations all in the
// one loop (rather than using EventHandlerModule & timingModule); makes the
// fracOfStep and notification messages easier to calculate.
class CalendarAndEventFlintDataSequencer : public SequencerModuleBase {
  public:
   CalendarAndEventFlintDataSequencer() {}
   virtual ~CalendarAndEventFlintDataSequencer() {}

   void configure(ITiming& timing) override {
      SequencerModuleBase::configure(timing);
      StartDate = timing.startDate();
      EndDate = timing.endDate();
   }

   bool Run(NotificationCenter& _notificationCenter, ILandUnitController& luc) override;

  private:
   DateTime StartDate;
   DateTime EndDate;
   DateTime ActualStartDate;
   DateTime ActualEndDate;
   int nSteps;

   const int StepsPerYear = 12;
};

}  // namespace moja::flint
