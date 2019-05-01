#ifndef MOJA_CORE_COREEXCEPTIONS_H_
#define MOJA_CORE_COREEXCEPTIONS_H_

#include "_core_exports.h"

#include <boost/exception/all.hpp>

#include <string>
#include <stdexcept>


namespace moja {

struct CORE_API CoreException : virtual std::exception, virtual boost::exception {};

struct CORE_API CoreAssertionViolationException : virtual CoreException {};
typedef boost::error_info<struct tag_assert_msg, std::string> AssertMsg;

struct CORE_API CoreFileNotFoundException : virtual CoreException {};
typedef boost::error_info<struct tag_filename, std::string> FileName;

struct CORE_API CoreNotImplementedException : virtual CoreException {};
typedef boost::error_info<struct tag_message, std::string> Message;

struct CORE_API CoreUnhandledType : virtual CoreException {};
typedef boost::error_info<struct tag_message, std::string> TypeErrorMsg;

} // namespace moja

#endif // MOJA_CORE_COREEXCEPTIONS_H_