#ifndef MOJA_FLINT_CALENDARANDEVENTFLINTDATASEQUENCER_H_
#define MOJA_FLINT_CALENDARANDEVENTFLINTDATASEQUENCER_H_

#include "moja/flint/itiming.h"
#include "moja/flint/librarymanager.h"
#include "moja/flint/sequencermodulebase.h"

#include <moja/datetime.h>
#include <moja/events.h>
#include <moja/notificationcenter.h>

namespace moja {
namespace flint {

// This sequencer handles the events and fracOfStep calculations all in the
// one loop (rather than using EventHandlerModule & timingModule); makes the
// fracOfStep and notification messages easier to calculate.
class CalendarAndEventFlintDataSequencer : public flint::SequencerModuleBase {
  public:
   CalendarAndEventFlintDataSequencer(){};
   virtual ~CalendarAndEventFlintDataSequencer(){};

   void configure(ITiming& timing) override {
      SequencerModuleBase::configure(timing);
      StartDate = timing.startDate();
      EndDate = timing.endDate();
   };

   bool Run(NotificationCenter& _notificationCenter, flint::ILandUnitController& luc) override;

  private:
   moja::DateTime StartDate;
   moja::DateTime EndDate;
   moja::DateTime ActualStartDate;
   moja::DateTime ActualEndDate;
   int nSteps;

   const int StepsPerYear = 12;
};

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_CALENDARANDEVENTFLINTDATASEQUENCER_H_