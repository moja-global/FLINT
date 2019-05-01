#include "moja/flint/aggregatorstockstep.h"
#include "moja/flint/ilandunitdatawrapper.h"
#include "moja/flint/ipool.h"
#include "moja/flint/itiming.h"

#include "moja/signals.h"
#include "moja/notificationcenter.h"

namespace moja {
namespace flint {

void AggregatorStockStep::configure(const DynamicObject& config) { }

void AggregatorStockStep::subscribe(NotificationCenter& notificationCenter) {
	notificationCenter.subscribe(signals::LocalDomainInit,	&AggregatorStockStep::onLocalDomainInit,	*this);
	notificationCenter.subscribe(signals::TimingInit,		&AggregatorStockStep::onTimingInit,			*this);
	notificationCenter.subscribe(signals::OutputStep,		&AggregatorStockStep::onOutputStep,			*this);
}

void AggregatorStockStep::onLocalDomainInit() {
	const auto timing = _landUnitData->timing();

	// Initalize the Stock data
	int poolCount = _landUnitData->getPoolCount();
	_stockByStep.resize(timing->nSteps());

	int i = 0;
	for (auto& record : _stockByStep) {
		record.stockValues.resize(poolCount);
		record.step = i++;
		for (auto& stock : record.stockValues) {
			stock = 0.0;
		}
	}
}

void AggregatorStockStep::onTimingInit() {
	const auto timing = _landUnitData->timing();

	if (timing->nSteps() > 0) {
		StepRecord& record = _stockByStep[0];
		record.date = timing->curStartDate();
		record.step = 0;
		record.fracOfStep = timing->fractionOfStep();
		record.lengthOfStepInYears = timing->stepLengthInYears();

		auto pools = _landUnitData->poolCollection();
		for (auto& it : pools) {
			//record.stockValues[it.idx()] += it.value();
			record.stockValues[it->idx()] += it->value();
		}
	}
}

void AggregatorStockStep::onOutputStep() {
	const auto timing = _landUnitData->timing();
	int curStep = timing->step();

	// aggregate the Stock data from this step
	StepRecord& record = _stockByStep[curStep];
	record.date = timing->curStartDate();
	record.step = curStep;
	record.fracOfStep = timing->fractionOfStep();
	record.lengthOfStepInYears = timing->stepLengthInYears();

	auto pools = _landUnitData->poolCollection();
	for (auto& it : pools) {
		record.stockValues[it->idx()] += it->value();
	}
}

}
} // namespace moja::flint
