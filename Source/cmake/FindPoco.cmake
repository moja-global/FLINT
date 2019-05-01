# - Find the Poco includes and libraries.
# The following variables are set if Poco is found.  If Poco is not
# found, Poco_FOUND is set to false.
#  Poco_FOUND        - True when the Poco include directory is found.
#  Poco_INCLUDE_DIRS - the path to where the poco include files are.
#  Poco_LIBRARY_DIRS - The path to where the poco library files are.
#  Poco_BINARY_DIRS - The path to where the poco dlls are.

# ----------------------------------------------------------------------------
# If you have installed Poco in a non-standard location.
# Then you have three options. 
# In the following comments, it is assumed that <Your Path>
# points to the root directory of the include directory of Poco. e.g
# If you have put poco in C:\development\Poco then <Your Path> is
# "C:/development/Poco" and in this directory there will be two
# directories called "include" and "lib".
# 1) After CMake runs, set Poco_INCLUDE_DIR to <Your Path>/poco<-version>
# 2) Use CMAKE_INCLUDE_PATH to set a path to <Your Path>/poco<-version>. This will allow FIND_PATH()
#    to locate Poco_INCLUDE_DIR by utilizing the PATH_SUFFIXES option. e.g.
#    SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "<Your Path>/include")
# 3) Set an environment variable called ${POCO_ROOT} that points to the root of where you have
#    installed Poco, e.g. <Your Path>. It is assumed that there is at least a subdirectory called
#    Foundation/include/Poco in this path.
#
# Note:
#  1) If you are just using the poco headers, then you do not need to use
#     Poco_LIBRARY_DIR in your CMakeLists.txt file.
#  2) If Poco has not been installed, then when setting Poco_LIBRARY_DIR
#     the script will look for /lib first and, if this fails, then for /stage/lib.
#
# Usage:
# In your CMakeLists.txt file do something like this:
# ...
# # Poco
# FIND_PACKAGE(Poco)
# ...
# INCLUDE_DIRECTORIES(${Poco_INCLUDE_DIRS})
# LINK_DIRECTORIES(${Poco_LIBRARY_DIR})
#
# In Windows, we make the assumption that, if the Poco files are installed, the default directory
# will be C:\poco or C:\Program Files\Poco or C:\Programme\Poco.

SET(POCO_INCLUDE_PATH_DESCRIPTION "top-level directory containing the poco include directories. E.g /usr/local/include/ or c:\\poco\\include\\poco-1.3.2")
SET(POCO_INCLUDE_DIR_MESSAGE "Set the Poco_INCLUDE_DIR cmake cache entry to the ${POCO_INCLUDE_PATH_DESCRIPTION}")
SET(POCO_LIBRARY_PATH_DESCRIPTION "top-level directory containing the poco libraries.")
SET(POCO_LIBRARY_DIR_MESSAGE "Set the Poco_LIBRARY_DIR cmake cache entry to the ${POCO_LIBRARY_PATH_DESCRIPTION}")

SET(SEARCH_PATH_POCO "" CACHE PATH "Additional Poco search path")

if(CMAKE_SYSTEM MATCHES "Windows")

  SET(POCO_DIR_SEARCH
    $ENV{POCO_ROOT}
    ${SEARCH_PATH_POCO}
    ${POCO_DIR_SEARCH}
  )

  if (EXISTS "C:/") 
	SET(POCO_DIR_SEARCH
        ${POCO_DIR_SEARCH}
        C:/Development
        C:/poco
        "C:/Program Files/poco"
        "C:/Programme/poco"
    )
  ENDIF()
  
  if (EXISTS "D:/") 
	SET(POCO_DIR_SEARCH
        ${POCO_DIR_SEARCH}
        D:/Development
        D:/poco
        "D:/Program Files/poco"
        "D:/Programme/poco"
    )
  ENDIF()
  
ENDIF()


if (CMAKE_SYSTEM MATCHES "Linux" )
  SET(POCO_DIR_SEARCH
    $ENV{POCO_ROOT}
    ${SEARCH_PATH_POCO}
    ${POCO_DIR_SEARCH}
	/usr/local 
  )
ENDIF()

# Add in some path suffixes. These will have to be updated whenever a new Poco version comes out.
SET(SUFFIX_FOR_INCLUDE_PATH
	poco-1.9.0
	poco-1.9.0-all
	poco-1.8.1-all
	poco-1.7.9-all
	poco-1.7.8p3-all
	poco-1.7.8-all
	poco-1.7.7-all
	poco-1.7.4-all
	poco-1.7.1-all
	poco.develop
	poco
)

SET(SUFFIX_FOR_LIBRARY_PATH
	bin64
	lib64
	poco//bin64
	poco//lib64
	poco.develop//bin64
	poco.develop//lib64
)

#
# Look for an installation.
#
FIND_PATH(
	Poco_INCLUDE_DIR NAMES 
		Foundation/include/Poco/AbstractCache.h 
	PATH_SUFFIXES 
		${SUFFIX_FOR_INCLUDE_PATH} PATHS
		# Look in other places.
		${Poco_INCLUDE_DIR}
		${POCO_DIR_SEARCH}
	DOC # Help the user find it if we cannot.
		"The ${POCO_INCLUDE_PATH_DESCRIPTION}"
)

IF(NOT Poco_INCLUDE_DIR)
	# Look for standard unix include paths
	FIND_PATH(Poco_INCLUDE_DIR Poco/Poco.h DOC "The ${POCO_INCLUDE_PATH_DESCRIPTION}")
ENDIF(NOT Poco_INCLUDE_DIR)

# Assume we didn't find it.
SET(Poco_FOUND 0)

# Now try to get the include and library path.
IF(EXISTS ${Poco_INCLUDE_DIR})
  IF(EXISTS "${Poco_INCLUDE_DIR}/Foundation")
    SET(Poco_INCLUDE_DIRS
      ${Poco_INCLUDE_DIR}/CppUnit/include
      ${Poco_INCLUDE_DIR}/Foundation/include
      ${Poco_INCLUDE_DIR}/Data/include
      ${Poco_INCLUDE_DIR}/Data/SQLite/include
      ${Poco_INCLUDE_DIR}/Data/ODBC/include
      ${Poco_INCLUDE_DIR}/MongoDB/include
      ${Poco_INCLUDE_DIR}/Net/include
      ${Poco_INCLUDE_DIR}/Util/include
      ${Poco_INCLUDE_DIR}/XML/include
      ${Poco_INCLUDE_DIR}/Zip/include
      ${Poco_INCLUDE_DIR}/JSON/include
	  CACHE PATH "Location of Poco include files"
    )
    SET(Poco_FOUND 1)
  ELSEIF(EXISTS "${Poco_INCLUDE_DIR}/Poco/Poco.h")
    SET(Poco_INCLUDE_DIRS
      ${Poco_INCLUDE_DIR}
	  CACHE PATH "Location of Poco include files"
    )
    SET(Poco_FOUND 1)
  ENDIF(EXISTS "${Poco_INCLUDE_DIR}/Foundation")

  IF(NOT Poco_LIBRARY_DIR)
    FIND_LIBRARY(Poco_FOUNDATION_LIB NAMES PocoFoundation PocoFoundationd 
		PATH_SUFFIXES 
			${SUFFIX_FOR_LIBRARY_PATH} 
		PATHS # Look in other places.
		  ${Poco_INCLUDE_DIR}
		  ${POCO_DIR_SEARCH}
		DOC # Help the user find it if we cannot.
			"The ${POCO_LIBRARY_PATH_DESCRIPTION}"
    )
    GET_FILENAME_COMPONENT(Poco_LIBRARY_DIR ${Poco_FOUNDATION_LIB} PATH CACHE)

    IF(Poco_LIBRARY_DIR)
      # Look for the poco binary path.
      SET(Poco_BINARY_DIR ${Poco_INCLUDE_DIR})

      IF(Poco_BINARY_DIR AND EXISTS "${Poco_BINARY_DIR}/bin64")
        SET(Poco_BINARY_DIRS ${Poco_BINARY_DIR}/bin64 CACHE PATH "Path to Poco binaries")
      ENDIF(Poco_BINARY_DIR AND EXISTS "${Poco_BINARY_DIR}/bin64")
    ENDIF(Poco_LIBRARY_DIR)

	find_library(Poco_DATA_SQLITE_DEBUG NAMES PocoDataSQLited PocoDataSQLited_dll
		PATH_SUFFIXES 
			${SUFFIX_FOR_LIBRARY_PATH} 
			debug
		PATHS # Look in other places.
			${Poco_INCLUDE_DIR}
			${POCO_DIR_SEARCH}
	)

	find_library(Poco_DATA_SQLITE_RELEASE NAMES PocoDataSQLite PocoDataSQLite_dll
		PATH_SUFFIXES 
			${SUFFIX_FOR_LIBRARY_PATH} 
			release
		PATHS # Look in other places.
			${Poco_INCLUDE_DIR}
			${POCO_DIR_SEARCH}
	)

	find_library(Poco_DATA_ODBC_DEBUG NAMES PocoDataODBCd PocoDataODBCd_dll
		PATH_SUFFIXES 
			${SUFFIX_FOR_LIBRARY_PATH} 
			debug
		PATHS # Look in other places.
			${Poco_INCLUDE_DIR}
			${POCO_DIR_SEARCH}
	)

	find_library(Poco_DATA_ODBC_RELEASE NAMES PocoDataODBC PocoDataODBC_dll
		PATH_SUFFIXES 
			${SUFFIX_FOR_LIBRARY_PATH} 
			release
		PATHS # Look in other places.
			${Poco_INCLUDE_DIR}
			${POCO_DIR_SEARCH}
	)

	find_library(Poco_FOUNDATION_DEBUG NAMES PocoFoundationd PocoFoundationd_dll
		PATH_SUFFIXES 
			${SUFFIX_FOR_LIBRARY_PATH} 
			debug
		PATHS # Look in other places.
			${Poco_INCLUDE_DIR}
			${POCO_DIR_SEARCH}
	)

	find_library(Poco_FOUNDATION_RELEASE NAMES PocoFoundation PocoFoundation_dll
		PATH_SUFFIXES 
			${SUFFIX_FOR_LIBRARY_PATH} 
			release
		PATHS # Look in other places.
			${Poco_INCLUDE_DIR}
			${POCO_DIR_SEARCH}
	)

	find_library(Poco_NET_DEBUG NAMES PocoNetd PocoNetd_dll
		PATH_SUFFIXES 
			${SUFFIX_FOR_LIBRARY_PATH} 
			debug
		PATHS # Look in other places.
			${Poco_INCLUDE_DIR}
			${POCO_DIR_SEARCH}
	)

	find_library(Poco_NET_RELEASE NAMES PocoNet PocoNet_dll
		PATH_SUFFIXES 
			${SUFFIX_FOR_LIBRARY_PATH} 
			release
		PATHS # Look in other places.
			${Poco_INCLUDE_DIR}
			${POCO_DIR_SEARCH}
	)

	find_library(Poco_MONGODB_DEBUG NAMES PocoMongoDBd PocoMongoDBd_dll
		PATH_SUFFIXES 
			${SUFFIX_FOR_LIBRARY_PATH} 
			debug
		PATHS # Look in other places.
			${Poco_INCLUDE_DIR}
			${POCO_DIR_SEARCH}
	)

	find_library(Poco_MONGODB_RELEASE NAMES PocoMongoDB PocoMongoDB_dll
		PATH_SUFFIXES 
			${SUFFIX_FOR_LIBRARY_PATH} 
			release
		PATHS # Look in other places.
			${Poco_INCLUDE_DIR}
			${POCO_DIR_SEARCH}
	)


	find_library(Poco_DATA_DEBUG NAMES PocoDatad PocoDatad_dll
		PATH_SUFFIXES 
			${SUFFIX_FOR_LIBRARY_PATH} 
			debug
		PATHS # Look in other places.
			${Poco_INCLUDE_DIR}
			${POCO_DIR_SEARCH}
	)

	find_library(Poco_DATA_RELEASE NAMES PocoData PocoData_dll
		PATH_SUFFIXES 
			${SUFFIX_FOR_LIBRARY_PATH} 
			release
		PATHS # Look in other places.
			${Poco_INCLUDE_DIR}
			${POCO_DIR_SEARCH}
	)

	find_library(Poco_JSON_DEBUG NAMES PocoJSONd PocoJSONd_dll
		PATH_SUFFIXES 
			${SUFFIX_FOR_LIBRARY_PATH} 
			debug
		PATHS # Look in other places.
			${Poco_INCLUDE_DIR}
			${POCO_DIR_SEARCH}
	)

	find_library(Poco_JSON_RELEASE NAMES PocoJSON PocoJSON_dll
		PATH_SUFFIXES 
			${SUFFIX_FOR_LIBRARY_PATH} 
			release
		PATHS # Look in other places.
			${Poco_INCLUDE_DIR}
			${POCO_DIR_SEARCH}
	)

	find_library(Poco_UTIL_DEBUG NAMES PocoUtild PocoUtild_dll
		PATH_SUFFIXES 
			${SUFFIX_FOR_LIBRARY_PATH} 
			debug
		PATHS # Look in other places.
			${Poco_INCLUDE_DIR}
			${POCO_DIR_SEARCH}
	)

	find_library(Poco_UTIL_RELEASE NAMES PocoUtil PocoUtil_dll
		PATH_SUFFIXES 
			${SUFFIX_FOR_LIBRARY_PATH} 
			release
		PATHS # Look in other places.
			${Poco_INCLUDE_DIR}
			${POCO_DIR_SEARCH}
	)

	if(Poco_FOUNDATION_DEBUG AND NOT Poco_FOUNDATION_RELEASE)
		set(Poco_FOUNDATION_RELEASE ${Poco_FOUNDATION_DEBUG})
	endif(Poco_FOUNDATION_DEBUG AND NOT Poco_FOUNDATION_RELEASE)

	set( Poco_FOUNDATION debug     ${Poco_FOUNDATION_DEBUG}
							optimized ${Poco_FOUNDATION_RELEASE}
					CACHE STRING "Poco Foundation link library text")

	if(Poco_DATA_SQLITE_DEBUG AND NOT Poco_DATA_SQLITE_RELEASE)
		set(Poco_DATA_SQLITE_RELEASE ${Poco_DATA_SQLITE_DEBUG})
	endif(Poco_DATA_SQLITE_DEBUG AND NOT Poco_DATA_SQLITE_RELEASE)

	set( Poco_DATA_SQLITE debug     ${Poco_DATA_SQLITE_DEBUG}
							optimized ${Poco_DATA_SQLITE_RELEASE} 
							CACHE STRING "Poco SQLite link library text")

	if(Poco_DATA_ODBC_DEBUG AND NOT Poco_DATA_ODBC_RELEASE)
		set(Poco_DATA_ODBC_RELEASE ${Poco_DATA_ODBC_DEBUG})
	endif(Poco_DATA_ODBC_DEBUG AND NOT Poco_DATA_ODBC_RELEASE)

	set( Poco_DATA_ODBC debug     ${Poco_DATA_ODBC_DEBUG}
							optimized ${Poco_DATA_ODBC_RELEASE} 
							CACHE STRING "Poco ODBC link library text")

	if(Poco_DATA_DEBUG AND NOT Poco_DATA_RELEASE)
		set(Poco_DATA_RELEASE ${Poco_DATA_DEBUG})
	endif(Poco_DATA_DEBUG AND NOT Poco_DATA_RELEASE)

	set( Poco_DATA debug     ${Poco_DATA_DEBUG}
					optimized ${Poco_DATA_RELEASE} 
					CACHE STRING "Poco Data link library text")

	if(Poco_JSON_DEBUG AND NOT Poco_JSON_RELEASE)
		set(Poco_JSON_RELEASE ${Poco_JSON_DEBUG})
	endif(Poco_JSON_DEBUG AND NOT Poco_JSON_RELEASE)

	set( Poco_JSON debug     ${Poco_JSON_DEBUG}
					optimized ${Poco_JSON_RELEASE}
					CACHE STRING "Poco JSON link library text")

	if(Poco_MONGODB_DEBUG AND NOT Poco_MONGODB_RELEASE)
		set(Poco_MONGODB_RELEASE ${Poco_MONGODB_DEBUG})
	endif(Poco_MONGODB_DEBUG AND NOT Poco_MONGODB_RELEASE)

	set( Poco_MONGODB debug     ${Poco_MONGODB_DEBUG}
					optimized ${Poco_MONGODB_RELEASE}
					CACHE STRING "Poco MONGODB link library text")

	if(Poco_NET_DEBUG AND NOT Poco_NET_RELEASE)
		set(Poco_NET_RELEASE ${Poco_NET_DEBUG})
	endif(Poco_NET_DEBUG AND NOT Poco_NET_RELEASE)

	set( Poco_NET debug     ${Poco_NET_DEBUG}
					optimized ${Poco_NET_RELEASE}
					CACHE STRING "Poco NET link library text")

	if(Poco_UTIL_DEBUG AND NOT Poco_UTIL_RELEASE)
		set(Poco_UTIL_RELEASE ${Poco_UTIL_DEBUG})
	endif(Poco_UTIL_DEBUG AND NOT Poco_UTIL_RELEASE)

	set( Poco_UTIL debug     ${Poco_UTIL_DEBUG}
					optimized ${Poco_UTIL_RELEASE}
					CACHE STRING "Poco Util link library text")

  ENDIF(NOT Poco_LIBRARY_DIR)

IF(NOT Poco_FOUND)
  IF(Poco_FIND_QUIETLY)
    message(STATUS "Poco was not found. ${POCO_INCLUDE_DIR_MESSAGE}")
  ELSE(Poco_FIND_QUIETLY)
    IF(Poco_FIND_REQUIRED)
      message(FATAL_ERROR "Poco was not found. ${POCO_INCLUDE_DIR_MESSAGE}")
    ENDIF(Poco_FIND_REQUIRED)
  ENDIF(Poco_FIND_QUIETLY)
ENDIF(NOT Poco_FOUND)

ENDIF()


