#ifndef MOJA_FLINT_CONFIGURATION_MODULE_H_
#define MOJA_FLINT_CONFIGURATION_MODULE_H_

#include "moja/flint/configuration/_configuration_exports.h"

#include <moja/dynamic.h>

#include <string>

namespace moja {
namespace flint {
namespace configuration {

class CONFIGURATION_API Module {
public:
	Module(const std::string& libraryName, const std::string& name, int order, bool isProxy, DynamicObject settings);
	virtual ~Module() {}

	virtual const std::string& libraryName() const { return _libraryName; }
	virtual const std::string& name() const { return _name; }
	virtual int order() const { return _order; }
	virtual bool isProxy() const { return _isProxy; }
	virtual const DynamicObject& settings() const { return _settings; }

private:
	std::string _libraryName;
	std::string _name;
	int _order;
	bool _isProxy;
	DynamicObject _settings;
};

}
}
} // namespace moja::flint::configuration

#endif // MOJA_FLINT_CONFIGURATION_MODULE_H_
