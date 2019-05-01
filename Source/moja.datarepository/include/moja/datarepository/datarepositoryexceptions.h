#ifndef MOJA_DATAREPOSITORY_DATAREPOSITORYEXCEPTIONS_H_
#define MOJA_DATAREPOSITORY_DATAREPOSITORYEXCEPTIONS_H_

#include "moja/datarepository/_datarepository_exports.h"

#include <boost/exception/all.hpp>

#include <string>

namespace moja {
namespace datarepository {

struct DATAREPOSITORY_API DataRepositoryException : virtual std::exception, virtual boost::exception {};

struct DATAREPOSITORY_API AssertionViolationException : virtual DataRepositoryException {};
typedef boost::error_info<struct tag_assert_msg, std::string> AssertMsg;

struct DATAREPOSITORY_API ConnectionFailedException : virtual DataRepositoryException {};
typedef boost::error_info<struct tag_connection_error, std::string> ConnectionError;

struct DATAREPOSITORY_API FileNotFoundException : virtual DataRepositoryException {};
typedef boost::error_info<struct tag_filename, std::string> FileName;

struct DATAREPOSITORY_API FileReadException : virtual DataRepositoryException {};
typedef boost::error_info<struct tag_filename, std::string> FileName;
typedef boost::error_info<struct tag_message, std::string> Message;

struct DATAREPOSITORY_API LayerNotFoundException : virtual DataRepositoryException {};
typedef boost::error_info<struct tag_layername, std::string> LayerName;

struct DATAREPOSITORY_API NotImplementedException : virtual DataRepositoryException {};
typedef boost::error_info<struct tag_message, std::string> Message;

struct DATAREPOSITORY_API QueryException : virtual DataRepositoryException {};
typedef boost::error_info<struct tag_sql, std::string> SQL;
typedef boost::error_info<struct tag_sql_error, std::string> SQLError;

struct DATAREPOSITORY_API LandscapeDefinitionException : virtual DataRepositoryException {};
typedef boost::error_info<struct tag_component, std::string> Component;
typedef boost::error_info<struct tag_constraint, std::string> Constraint;

struct DATAREPOSITORY_API TileBlockCellIndexerInvalidParameterException : virtual DataRepositoryException {};
typedef boost::error_info<struct tag_component, std::string> Component;
typedef boost::error_info<struct tag_constraint, std::string> Constraint;

// -- Provider exceptions
struct DATAREPOSITORY_API ProviderUnsupportedException : virtual DataRepositoryException {};
typedef boost::error_info<struct tag_provider_type, std::string> ProviderType;

struct DATAREPOSITORY_API ProviderNotFoundException : virtual DataRepositoryException {};
typedef boost::error_info<struct tag_provider_type, std::string> ProviderType;

struct DATAREPOSITORY_API ProviderAlreadyExistsException : virtual DataRepositoryException {};
typedef boost::error_info<struct tag_filename, std::string> ProviderName;
typedef boost::error_info<struct tag_provider_type, std::string> ProviderType;

}
} // namespace moja::DataRepository

#endif // MOJA_DATAREPOSITORY_DATAREPOSITORYEXCEPTIONS_H_
