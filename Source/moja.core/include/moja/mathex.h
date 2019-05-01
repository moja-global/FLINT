#ifndef MOJA_CORE_MATHEX_H_
#define MOJA_CORE_MATHEX_H_

#include "moja/_core_exports.h"

#include <cmath>

namespace moja {

class CORE_API MathEx {
public:
	static const double k0Plus; // = 0.000000001;
	static double Exp0(double x);
};

inline double MathEx::Exp0(double x) {
	// Rounds to 0.0 to avoid underflow in exp.
	return x < -20.0 ? 0.0 : std::exp(x);
}

} // namespace moja

#endif // MOJA_CORE_MATHEX_H_
