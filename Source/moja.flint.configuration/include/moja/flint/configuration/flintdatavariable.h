#ifndef MOJA_FLINT_CONFIGURATION_FLINTDATAVARIABLE_H_
#define MOJA_FLINT_CONFIGURATION_FLINTDATAVARIABLE_H_

#include "moja/flint/configuration/_configuration_exports.h"
#include "moja/flint/configuration/flintdata.h"
#include "moja/flint/configuration/ivariable.h"

#include <string>

namespace moja {
namespace flint {
namespace configuration {

class CONFIGURATION_API FlintDataVariable : public IVariable {
public:
	explicit FlintDataVariable(const std::string& name, std::shared_ptr<FlintData> transform = nullptr);
	virtual ~FlintDataVariable() {}

	virtual inline const FlintData& flintdata() const { return *_flintdata; }

private:
	std::shared_ptr<FlintData> _flintdata;
};

}
}
} // namespace moja::flint::configuration

#endif // MOJA_FLINT_CONFIGURATION_FLINTDATAVARIABLE_H_