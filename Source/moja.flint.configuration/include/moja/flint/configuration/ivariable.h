#ifndef MOJA_FLINT_CONFIGURATION_IVARIABLE_H_
#define MOJA_FLINT_CONFIGURATION_IVARIABLE_H_

#include "moja/flint/configuration/_configuration_exports.h"

#include <boost/algorithm/string.hpp>

#include <string>

namespace moja {
namespace flint {
namespace configuration {

enum class VariableType {
	Internal = 0,
	External = 1,
	FlintData = 2
};

class CONFIGURATION_API IVariable {
public:
	IVariable(VariableType variableType, const std::string& name) : _variableType(variableType), _name(name) {

		if (name.length() == 0 || all(name, boost::algorithm::is_space())) {
			throw std::invalid_argument("Config variable name cannot be empty");
		}
	}
	virtual ~IVariable() {}

	virtual VariableType variableType() const { return _variableType; }
	virtual const std::string& name() const { return _name; }

protected:
	VariableType _variableType;
	std::string _name;
};

}
}
} // namespace moja::flint::configuration

#endif // MOJA_FLINT_CONFIGURATION_IVARIABLE_H_