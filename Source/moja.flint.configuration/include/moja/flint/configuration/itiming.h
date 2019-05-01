#pragma once
#ifndef MOJA_FLINT_CONFIGURATION_ITIMING_H_
#define MOJA_FLINT_CONFIGURATION_ITIMING_H_

#include "moja/flint/configuration/_configuration_exports.h"

namespace moja {
namespace flint {
namespace configuration {

	enum class TimeStepping {
		Monthly = 0,
		Daily = 1,
		Annual = 3
	};


}
}
} // namespace moja::flint::configuration

#endif // MOJA_FLINT_CONFIGURATION_ITIMING_H_