#ifndef MOJA_FLINT_CONFIGURATION_TRANSFORM_H_
#define MOJA_FLINT_CONFIGURATION_TRANSFORM_H_

#include "moja/flint/configuration/_configuration_exports.h"

#include "moja/dynamic.h"

namespace moja {
namespace flint {
namespace configuration {

class CONFIGURATION_API Transform {
public:
	Transform(const std::string& libraryName, const std::string& typeName, DynamicObject settings);
	virtual ~Transform() {}

	virtual const std::string& libraryName() const { return _libraryName; }
	virtual const std::string& typeName() const { return _typeName; }
	virtual const DynamicObject& settings() const { return _settings; }

private:
	std::string _libraryName;
	std::string _typeName;
	DynamicObject _settings;
};

}
}
} // namespace moja::flint::configuration

#endif // CONFIGURATION_TRANSFORM_H_
