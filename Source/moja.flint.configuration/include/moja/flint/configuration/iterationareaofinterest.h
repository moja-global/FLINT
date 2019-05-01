#ifndef MOJA_FLINT_CONFIGURATION_ITERATIONAREAOFINTEREST_H_
#define MOJA_FLINT_CONFIGURATION_ITERATIONAREAOFINTEREST_H_

#include "moja/flint/configuration/_configuration_exports.h"
#include "moja/flint/configuration/iterationbase.h"

namespace moja {
namespace flint {
namespace configuration {

class CONFIGURATION_API IterationAreaOfInterest : public IterationBase {
public:
	explicit IterationAreaOfInterest();
	virtual ~IterationAreaOfInterest() {}

private:
};

}
}
} // namespace moja::flint::configuration

#endif // MOJA_FLINT_CONFIGURATION_ITERATIONAREAOFINTEREST_H_
