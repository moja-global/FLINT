#include "moja/flint/aggregatorfluxstep.h"

#include "moja/signals.h"
#include "moja/notificationcenter.h"

namespace moja {
namespace flint {

void AggregatorFluxStep::configure(const DynamicObject& config) { }

void AggregatorFluxStep::subscribe(NotificationCenter& notificationCenter) {
	notificationCenter.subscribe(signals::LocalDomainInit,		&AggregatorFluxStep::onLocalDomainInit,		*this);
	notificationCenter.subscribe(signals::LocalDomainShutdown,	&AggregatorFluxStep::onLocalDomainShutdown,	*this);
	notificationCenter.subscribe(signals::TimingEndStep,		&AggregatorFluxStep::onTimingEndStep,		*this);
}

void AggregatorFluxStep::onLocalDomainInit		() { }
void AggregatorFluxStep::onLocalDomainShutdown	() { }
void AggregatorFluxStep::onTimingEndStep		() { }

}
} // namespace moja::flint
