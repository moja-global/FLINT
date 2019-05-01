#ifndef MOJA_FLINT_FLINTEXCEPTIONS_H_
#define MOJA_FLINT_FLINTEXCEPTIONS_H_

#include <string>
#include <stdexcept>
#include <boost/exception/all.hpp>

#include "moja/flint/_flint_exports.h"

namespace moja {
namespace flint {

struct FLINT_API FLINTException : virtual std::exception, virtual boost::exception { };

struct FLINT_API PoolNotFoundException : virtual FLINTException { };
typedef boost::error_info<struct tag_poolname, std::string> PoolName;
typedef boost::error_info<struct tag_poolindex, int> PoolIndex;

struct FLINT_API VariableNotFoundException : virtual FLINTException { };
typedef boost::error_info<struct tag_variablename, std::string> VariableName;

struct FLINT_API VariableEmptyWhenValueExpectedException : virtual FLINTException {};
typedef boost::error_info<struct tag_variablename, std::string> VariableName;

struct FLINT_API SequencerNotFoundException : virtual FLINTException { };
typedef boost::error_info<struct tag_details, std::string> Details;
typedef boost::error_info<struct tag_library, std::string> LibraryName;
typedef boost::error_info<struct tag_module, std::string> SequencerName;

struct FLINT_API TableNotFoundException : virtual FLINTException { };
typedef boost::error_info<struct tag_tablename, std::string> TableName;

struct FLINT_API PreconditionViolatedException : virtual FLINTException { };
typedef boost::error_info<struct tag_precondition, std::string> Precondition;

struct FLINT_API LandscapeDefinitionException : virtual FLINTException { };
typedef boost::error_info<struct tag_component, std::string> Component;
typedef boost::error_info<struct tag_constraint, std::string> Constraint;

struct FLINT_API IncompleteConfigurationException : virtual FLINTException { };
typedef boost::error_info<struct tag_item, std::string> Item;
typedef boost::error_info<struct tag_details, std::string> Details;

struct FLINT_API DuplicateModuleDefinedException : virtual FLINTException {};
typedef boost::error_info<struct tag_details, std::string> Details;
typedef boost::error_info<struct tag_library, std::string> LibraryName;
typedef boost::error_info<struct tag_module, std::string> ModuleName;

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

}
} // namespace moja::flint

#endif // MOJA_FLINT_FLINTEXCEPTIONS_H_
