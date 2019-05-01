#ifndef MOJA_FLINT_DEBUGNOTIFICATIONMODULE_H_
#define MOJA_FLINT_DEBUGNOTIFICATIONMODULE_H_

#include "moja/flint/modulebase.h"

#include <string>

namespace moja {
namespace flint {

class DebugNotificationModule : public ModuleBase {
public:
	DebugNotificationModule() : ModuleBase(), _localDomainId(-1) {}
	virtual ~DebugNotificationModule() {}

	void configure(const DynamicObject& config) override;
	void subscribe(NotificationCenter& notificationCenter) override;

	void onSystemInit						()											override;
	void onSystemShutdown					()											override;
	void onLocalDomainInit					()											override;
	void onLocalDomainShutdown				()											override;
	void onLocalDomainProcessingUnitInit	()											override;
	void onLocalDomainProcessingUnitShutdown()											override;
	void onPreTimingSequence				()											override;
	void onTimingInit						()											override;
	void onTimingPostInit					()											override;
	void onTimingShutdown					()											override;
	void onTimingStep						()											override;
	void onTimingPreEndStep					()											override;
	void onTimingEndStep					()											override;
	void onTimingPostStep					()											override;
	void onOutputStep						()											override;
	void onError							(std::string msg)							override;
	void onDisturbanceEvent					(DynamicVar)								override;
	void onPostDisturbanceEvent				()											override;
	void onPostNotification					(short preMessageSignal)					override;

private:
	int _localDomainId;
};

}
} // namespace moja::flint

#endif // MOJA_FLINT_DEBUGNOTIFICATIONMODULE_H_