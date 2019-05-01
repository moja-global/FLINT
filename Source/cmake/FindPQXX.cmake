# Find PostGreSQL C++ library and header file
# Sets
#   PQXX_FOUND                to 0 or 1 depending on result
#   PQXX_INCLUDE_DIRECTORIES  to the directory containing pqxx/pqxx
#   PQXX_LIBRARIES            to the PQXX library (and any dependents required)
# If PQXX_REQUIRED is defined, then a fatal error message will be generated if libpqxx is not found

SET(SEARCH_PATH_PQXX "" CACHE PATH "Additional PQXX search path")
SET(PQXX_DIR_SEARCH
	$ENV{PQXX_ROOT}
    ${SEARCH_PATH_PQXX}
	${PQXX_DIR_SEARCH}
)

if(CMAKE_SYSTEM MATCHES "Windows")
	if (EXISTS "C:/") 
		SET(PQXX_DIR_SEARCH
			${PQXX_DIR_SEARCH}
			"C:/Development/SLEEK-TOOLS/libpqxx/dist/"
			"C:/Development/Software/libpqxx/"
		)
	endif() 
	if (EXISTS "D:/") 
		SET(PQXX_DIR_SEARCH
			${PQXX_DIR_SEARCH}
			"D:/Development/SLEEK-TOOLS/libpqxx/dist/"
			"D:/Development/Software/libpqxx/"
		)
	endif() 

	find_path(PQXX_INCLUDE_DIRECTORIES pqxx/pqxx
		PATH_SUFFIXES 
			include
		PATHS
			${PQXX_DIR_SEARCH}
	)

	find_library(PQXX_LIBRARIES pqxx
		PATH_SUFFIXES 
			lib
		PATHS
			${PQXX_DIR_SEARCH}
	)

	set(LibPqxx_LIB
		debug ${LibPqxx_LIB_DEBUG}
		optimized ${LibPqxx_LIB_RELEASE}
		CACHE STRING "PQXX library")

endif()

if (CMAKE_SYSTEM MATCHES "Linux")
	SET(PQXX_DIR_SEARCH
		${PQXX_DIR_SEARCH}
        /usr/local/
        /usr/
	)

	find_path(PQXX_INCLUDE_DIRECTORIES pqxx/pqxx
		PATH_SUFFIXES 
			include 
		PATHS
			${PQXX_DIR_SEARCH}
	)

	find_library(PQXX_LIBRARIES pqxx
		PATH_SUFFIXES 
			lib 
		PATHS
			${PQXX_DIR_SEARCH}
	)

endif(CMAKE_SYSTEM MATCHES "Linux")

# handle the QUIETLY and REQUIRED arguments and set PQXX_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PQXX DEFAULT_MSG PQXX_INCLUDE_DIRECTORIES)

MARK_AS_ADVANCED(PQXX_INCLUDE_DIRECTORIES)
