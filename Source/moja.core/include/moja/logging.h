#ifndef MOJA_CORE_LOGGING_H_
#define MOJA_CORE_LOGGING_H_

#include "moja/_core_exports.h"

#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/trivial.hpp>

namespace moja {

/** <summary>    Logging services for the Moja framework. </summary> */
class CORE_API Logging {
private:
    static const std::string _defaultFileName;
    static bool _explicitConfigurationFileSet;
    static std::string _explicitConfigurationFile;
    static bool _explicitConfigurationTextSet;
    static std::string _explicitConfigurationText;
    static std::string _loggingConfigurationFile;

public:
    /**
     * <summary>    Gets the filename that the logging configuration was loaded from. </summary>
     */
    static const std::string& configurationFilename() {
        return _loggingConfigurationFile;
    }

    /**
     * <summary>    Sets the logging configuration from a file. </summary>
     *
     * <param name="explicitFilename">  Path to the logging configuration file. </param>
     */
    static void setConfigurationFile(std::string explicitFilename) {
        _explicitConfigurationFileSet = true;
        _explicitConfigurationTextSet = false;
        _explicitConfigurationFile = explicitFilename;
    }

    /**
     * <summary>    Sets the logging configuration from a string. </summary>
     *
     * <param name="configText">    The configuration text. </param>
     */
    static void setConfigurationText(std::string configText) {
        _explicitConfigurationFileSet = false;
        _explicitConfigurationTextSet = true;
        _explicitConfigurationText = configText;
    }

    /** <summary>  
     * Initializes the logging system using the first of:
     *   - explicitly set configuration file path  
     *   - explicitly set configuration string  
     *   - configuration file in the working directory  
     *   - configuration file in the executable directory  
     *   - internal default configuration
     * </summary>
     */
	static void init();
};

BOOST_LOG_INLINE_GLOBAL_LOGGER_INIT(mojalogger, boost::log::sources::severity_logger<boost::log::trivial::severity_level>) {
    Logging::init();
    return boost::log::sources::severity_logger<boost::log::trivial::severity_level>();
}

#define _MOJA_LOG(level) BOOST_LOG_SEV(moja::mojalogger::get(), boost::log::trivial::level) << std::string()
#define _MOJA_LOG_TRACE(level) _MOJA_LOG(level) << __FUNCTION__ << "(" << __LINE__ << ") - "

#define MOJA_LOG_TRACE _MOJA_LOG_TRACE(trace)
#define MOJA_LOG_DEBUG _MOJA_LOG_TRACE(debug)
#define MOJA_LOG_INFO  _MOJA_LOG(info)
#define MOJA_LOG_ERROR _MOJA_LOG_TRACE(error)
#define MOJA_LOG_FATAL _MOJA_LOG_TRACE(fatal)

} // namespace moja
#endif // MOJA_CORE_LOGGING_H_
