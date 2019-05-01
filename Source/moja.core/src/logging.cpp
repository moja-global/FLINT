#include "moja/logging.h"

#include "moja/environment.h"

#include <Poco/File.h>
#include <Poco/Path.h>

#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/from_stream.hpp>
#include <boost/log/attributes/clock.hpp>
#include <boost/log/trivial.hpp>

#include <fstream>

namespace moja {


const std::string Logging::_defaultFileName = "logging.conf";
bool Logging::_explicitConfigurationFileSet = false;
bool Logging::_explicitConfigurationTextSet = false;
std::string Logging::_explicitConfigurationFile = "";
std::string Logging::_explicitConfigurationText = "";
std::string Logging::_loggingConfigurationFile = "unknown";

void Logging::init() {
	namespace logging = boost::log;
	namespace attrs = boost::log::attributes;

	static auto initialized = false;
	if (initialized) { return; }

	logging::register_simple_formatter_factory<logging::trivial::severity_level, char>("Severity");
	logging::register_simple_filter_factory<logging::trivial::severity_level, char>("Severity");

	// Determine which log config to load, searching the working folder then the exe folder.
	if (_explicitConfigurationFileSet && Poco::File(_explicitConfigurationFile).exists()) {
		std::ifstream loggingConfig(_explicitConfigurationFile);
		logging::init_from_stream(loggingConfig);
		_loggingConfigurationFile = _explicitConfigurationFile;
	}
	else if (_explicitConfigurationTextSet && _explicitConfigurationText.length() > 0) {
		std::stringstream s;
		s << _explicitConfigurationText;
		boost::log::init_from_stream(s);
		_loggingConfigurationFile = "internal text";
	}
	else {
		std::string filenameToCheck = Poco::Path::current() + _defaultFileName;
		if (Poco::File(filenameToCheck).exists()) {
			std::ifstream loggingConfig(filenameToCheck);
			logging::init_from_stream(loggingConfig);
			_loggingConfigurationFile = filenameToCheck;
		}
		else {
			filenameToCheck = moja::Environment::startProcessFolder() + _defaultFileName;
			if (Poco::File(filenameToCheck).exists()) {
				std::ifstream loggingConfig(filenameToCheck);
				logging::init_from_stream(loggingConfig);
				_loggingConfigurationFile = filenameToCheck;
			}
			else {
				std::stringstream s;
				s << "[Sinks.console]" << std::endl;
				s << "Destination=Console" << std::endl;
				s << "Asynchronous = false" << std::endl;
				s << "AutoFlush = true" << std::endl;
				s << "Format = \"<%TimeStamp%> (%Severity%) - %Message%\"" << std::endl;
				s << "Filter = \"%Severity% >= info\"" << std::endl;
				boost::log::init_from_stream(s);
				_loggingConfigurationFile = "internal default";
			}
		}
	}

	logging::core::get()->add_global_attribute("TimeStamp", attrs::local_clock());
}

} // namespace moja
