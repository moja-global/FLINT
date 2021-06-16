#ifndef MOJA_FLINT_FLINTEXCEPTIONS_H_
#define MOJA_FLINT_FLINTEXCEPTIONS_H_

#include "moja/flint/_flint_exports.h"

#include <boost/exception/all.hpp>

#include <string>

namespace moja {
namespace flint {

struct FLINT_API FLINTException : virtual std::exception, virtual boost::exception {};

struct FLINT_API LocalDomainError : virtual FLINTException {};
typedef boost::error_info<struct tag_details, std::string> Details;
typedef boost::error_info<struct tag_library, std::string> LibraryName;
typedef boost::error_info<struct tag_module, std::string> ModuleName;
typedef boost::error_info<struct tag_code, int> ErrorCode;

struct FLINT_API SimulationError : virtual FLINTException {};
typedef boost::error_info<struct tag_details, std::string> Details;
typedef boost::error_info<struct tag_library, std::string> LibraryName;
typedef boost::error_info<struct tag_module, std::string> ModuleName;
typedef boost::error_info<struct tag_code, int> ErrorCode;

}  // namespace flint
}  // namespace moja

#endif  // MOJA_FLINT_FLINTEXCEPTIONS_H_
