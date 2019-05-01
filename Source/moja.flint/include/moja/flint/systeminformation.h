#ifndef MOJA_FLINT_SYSTEMINFORMATION_H_
#define MOJA_FLINT_SYSTEMINFORMATION_H_

#include "moja/flint/_flint_exports.h"
#include "moja/flint/iflintdata.h"

#include "moja/dynamic.h"

namespace moja {
namespace flint {

class FLINT_API SystemInformation : public flint::IFlintData {
public:
	SystemInformation() = default;
	virtual ~SystemInformation() = default;
	SystemInformation(const SystemInformation&) = delete;
	SystemInformation& operator= (const SystemInformation&) = delete;

	void configure(DynamicObject config, const flint::ILandUnitController& landUnitController, datarepository::DataRepository& dataRepository) override {
		systemName		= "";
		systemVersion	= "";
		numberOfThreads	= 0;
		isThreadSystem	= false;

		if (config.contains("systemName"))		{ systemName		= config["systemName"].extract<const std::string>(); }
		if (config.contains("systemVersion"))	{ systemVersion		= config["systemVersion"].extract<const std::string>(); }
		if (config.contains("numberOfThreads"))	{ numberOfThreads	= config["numberOfThreads"]; }
		if (config.contains("isThreadSystem"))	{ isThreadSystem	= config["isThreadSystem"]; }
	}

	DynamicObject exportObject() const override {
		DynamicObject object;

		object["systemName"]		= systemName		;
		object["systemVersion"]		= systemVersion		;
		object["numberOfThreads"]	= numberOfThreads	;
		object["isThreadSystem"]	= isThreadSystem	;

		return object;
	};

	std::string systemName;
	std::string systemVersion;
	int numberOfThreads;
	bool isThreadSystem;
};

}
} // namespace moja::flint

#endif // MOJA_FLINT_SYSTEMINFORMATION_H_