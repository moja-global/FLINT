#include "moja/flint/timing.h"

namespace moja {
namespace flint {

// Timing::Init
// Prepare a timing object for use
// Generally only the start and end dates are set, calling this method prepares
// the other member variables for simulation
void Timing::init() {
	auto startYear = _startDate.year();
	auto startStep = _startDate.month();

	setFractionOfStep(1.0);
	_step = 0;

	_startStepDate = _startDate;
	_endStepDate = _startDate;
	_curStartDate = _startDate;
	_curEndDate = _startDate;
}

}
} // namespace moja::flint
