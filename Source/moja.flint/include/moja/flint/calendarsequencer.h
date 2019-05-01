#ifndef MOJA_FLINT_CALENDARSEQUENCER_H_
#define MOJA_FLINT_CALENDARSEQUENCER_H_

#include "moja/flint/sequencermodulebase.h"
#include "moja/flint/itiming.h"

#include "moja/datetime.h"

namespace moja {
namespace flint {

// This sequencer handles the events and fracOfStep calculations
// all in the one loop (rather than using EventHandlerModule & timingModule)
// makes the fracOfStep and notification messages easier to calculate
class CalendarSequencer : public SequencerModuleBase {
public:
	CalendarSequencer() {};
	virtual ~CalendarSequencer() {};

	void configure(ITiming& timing) override {
		_startDate = timing.startDate();
		_endDate = timing.endDate();
	};

	bool Run(NotificationCenter& _notificationCenter, ILandUnitController& luc) override;

private:
	moja::DateTime _startDate;
	moja::DateTime _endDate;
	moja::DateTime _actualStartDate;
	moja::DateTime _actualEndDate;
	int nSteps;

	const int StepsPerYear = 12;
};

}
} // namespace moja::flint

#endif // MOJA_FLINT_CALENDARSEQUENCER_H_