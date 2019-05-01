#ifndef MOJA_FLINT_IVARIABLE_H_
#define MOJA_FLINT_IVARIABLE_H_

#include "moja/flint/_flint_exports.h"
#include "moja/dynamic.h"
#include <string>

namespace moja {
namespace flint {

struct VariableInfo {
	std::string name;
};

class FLINT_API IVariable {
public:
	virtual ~IVariable() = default;
	virtual const VariableInfo& info() const = 0;
	virtual const DynamicVar& value() const = 0;
	virtual void set_value(DynamicVar value) = 0;
	template <typename T>
	const T& value() const { return value().extract<T>(); }
	virtual void reset_value() = 0;
	virtual bool isExternal() const = 0;
	virtual bool isFlintData() const = 0;
};

}
} // moja::flint

#endif // MOJA_FLINT_IVARIABLE_H_
