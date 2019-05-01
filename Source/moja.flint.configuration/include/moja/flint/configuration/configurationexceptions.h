#ifndef MOJA_FLINT_CONFIGURATION_CONFIGURATIONEXCEPTIONS_H_
#define MOJA_FLINT_CONFIGURATION_CONFIGURATIONEXCEPTIONS_H_

#include "moja/flint/configuration/_configuration_exports.h"

#include <boost/exception/all.hpp>

#include <string>
#include <vector>

namespace moja {
namespace flint {
namespace configuration {

struct CONFIGURATION_API ConfigurationException : virtual std::exception, virtual boost::exception {};

struct CONFIGURATION_API AssertionViolationException : virtual ConfigurationException {};
typedef boost::error_info<struct tag_assert_msg, std::string> AssertMsg;

struct CONFIGURATION_API FileNotFoundException : virtual ConfigurationException {};
typedef boost::error_info<struct tag_filename, std::string> FileName;

struct CONFIGURATION_API ModuleOrderOverlapException : virtual ConfigurationException {};
typedef boost::error_info<struct tag_order, int> Order;
typedef boost::error_info<struct tag_module, std::vector<std::string>> ModuleNames;

struct CONFIGURATION_API ModuleParamsException : virtual ConfigurationException {};
typedef boost::error_info<struct tag_param, std::string> Param;

struct CONFIGURATION_API SpinupModuleOrderOverlapException : virtual ConfigurationException {};
typedef boost::error_info<struct tag_order, int> Order;
typedef boost::error_info<struct tag_module, std::vector<std::string>> SpinupModuleNames;

struct CONFIGURATION_API LandscapeDefinitionException : virtual ConfigurationException {};
typedef boost::error_info<struct tag_component, std::string> Component;
typedef boost::error_info<struct tag_constraint, std::string> Constraint;

/// -- Provider exceptions
struct CONFIGURATION_API ProviderSettingsException : virtual ConfigurationException {};
typedef boost::error_info<struct tag_providername, std::string> ProviderName;
typedef boost::error_info<struct tag_providerlib,  std::string> ProviderLibrary;

struct CONFIGURATION_API ProviderMissingTypeException : virtual ConfigurationException {};
typedef boost::error_info<struct tag_providername, std::string> ProviderName;

struct CONFIGURATION_API ProviderInvalidNameTypeException: virtual ConfigurationException{};
typedef boost::error_info<struct tag_providername, std::string> ProviderName;
typedef boost::error_info<struct tag_providerlib,  std::string> ProviderLibrary;
typedef boost::error_info<struct tag_providertype, std::string> ProviderType;

}
}
} // namespace moja::flint::configuration

#endif // MOJA_FLINT_CONFIGURATION_CONFIGURATIONEXCEPTIONS_H_
