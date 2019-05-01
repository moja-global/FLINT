#ifndef MOJA_TEST_MOCKTIMING_H_
#define MOJA_TEST_MOCKTIMING_H_

#include <turtle/mock.hpp>
#include "moja/itiming.h"

namespace moja {
namespace test {

MOCK_BASE_CLASS(MockTiming, ITiming) {
	MockTiming() = default;

	MOCK_METHOD(stepping, 0, TimeStepping())

	MOCK_METHOD(startDate, 0, DateTime())
	MOCK_METHOD(endDate, 0, DateTime())

	MOCK_METHOD(startStepDate, 0, DateTime())
	MOCK_METHOD(endStepDate, 0, DateTime())

	MOCK_METHOD(curStartDate, 0, DateTime())
	MOCK_METHOD(curEndDate, 0, DateTime())

	MOCK_METHOD(fractionOfStep, 0, double())
	MOCK_METHOD(stepLengthInYears, 0, double())

	MOCK_METHOD(nSteps, 0, int())
	MOCK_METHOD(step, 0, int())
	MOCK_METHOD(subStep, 0, int())
	MOCK_METHOD(isFullStep, 0, bool())

	MOCK_METHOD(setStartDate, 1, void(DateTime))
	MOCK_METHOD(setEndDate, 1, void(DateTime))

	MOCK_METHOD(setStartStepDate, 1, void(DateTime))
	MOCK_METHOD(setEndStepDate, 1, void(DateTime))

	MOCK_METHOD(setCurStartDate, 1, void(DateTime))
	MOCK_METHOD(setCurEndDate, 1, void(DateTime))

	MOCK_METHOD(setFractionOfStep, 1, void(double))
	MOCK_METHOD(setStepLengthInYears, 1, void(double))
	MOCK_METHOD(setStep, 1, void(int))
	MOCK_METHOD(setSubStep, 1, void(int))
	MOCK_METHOD(setIsFullStep, 1, void(bool))

    MOCK_METHOD(setStepping, 1, void(TimeStepping))

	MOCK_METHOD(init, 0, void())
};

}
} // namespace moja::test

#endif // MOJA_TEST_MOCKTIMING_H_