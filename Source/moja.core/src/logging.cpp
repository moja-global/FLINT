#include "moja/logging.h"

#include "moja/environment.h"

#include <boost/log/attributes/clock.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/from_stream.hpp>

#include <filesystem>
#include <fstream>

namespace moja {

namespace fs = std::filesystem;

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
   if (initialized) {
      return;
   }

   logging::register_simple_formatter_factory<logging::trivial::severity_level, char>("Severity");
   logging::register_simple_filter_factory<logging::trivial::severity_level, char>("Severity");

   // Determine which log config to load, searching the working folder then the exe folder.
   if (_explicitConfigurationFileSet && fs::exists(fs::path(_explicitConfigurationFile))) {
      std::ifstream loggingConfig(_explicitConfigurationFile);
      logging::init_from_stream(loggingConfig);
      _loggingConfigurationFile = _explicitConfigurationFile;
   } else if (_explicitConfigurationTextSet && _explicitConfigurationText.length() > 0) {
      std::stringstream s;
      s << _explicitConfigurationText;
      boost::log::init_from_stream(s);
      _loggingConfigurationFile = "internal text";
   } else {
      auto filename_to_check = fs::current_path().append(_defaultFileName);
      if (fs::exists(filename_to_check)) {
         std::ifstream loggingConfig(filename_to_check);
         logging::init_from_stream(loggingConfig);
         _loggingConfigurationFile = filename_to_check.string();
      } else {
         filename_to_check = moja::Environment::startProcessFolder() + _defaultFileName;
         if (fs::exists(filename_to_check)) {
            std::ifstream loggingConfig(filename_to_check);
            logging::init_from_stream(loggingConfig);
            _loggingConfigurationFile = filename_to_check.string();
         } else {
            std::stringstream s;
            s << R"(
            [Sinks.console]
                Destination=Console
                Asynchronous = false
                AutoFlush = true
                Format = "<%TimeStamp%> (%Severity%) - %Message%"
                Filter = "%Severity% >= info")";
            boost::log::init_from_stream(s);
            _loggingConfigurationFile = "internal default";
         }
      }
   }

   logging::core::get()->add_global_attribute("TimeStamp", attrs::local_clock());
}

}  // namespace moja
