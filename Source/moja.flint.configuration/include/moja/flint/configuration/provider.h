#ifndef MOJA_FLINT_CONFIGURATION_PROVIDER_H_
#define MOJA_FLINT_CONFIGURATION_PROVIDER_H_

#include "moja/flint/configuration/_configuration_exports.h"

#include "moja/dynamic.h"

#include <string>

namespace moja {
namespace flint {
namespace configuration {

class CONFIGURATION_API Provider {
public:
	//Provider(const std::string& name, const std::string& providerType, const DynamicObject& settings);
	Provider(const std::string& name, const std::string& library, const std::string& providerType, const DynamicObject& settings);
	virtual ~Provider() {}

	virtual const std::string& name() const { return _name; }
	virtual const std::string& library() const { return _library; }
	virtual const std::string& providerType() const { return _providerType; }
	virtual const DynamicObject& settings() const { return _settings; }

private:
	std::string _name;
	std::string _library;
	std::string _providerType;
	DynamicObject _settings;
};

}
}
} // namespace moja::flint::configuration

#endif // MOJA_FLINT_CONFIGURATION_PROVIDER_H_
