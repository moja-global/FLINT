SET(SEARCH_PATH_MONGODBCPLUSPLUSDRIVER "" CACHE PATH "Additional MongoDBCPlusPlusDriver search path")

if(CMAKE_SYSTEM MATCHES "Windows")

	set(MONGODBCPLUSPLUSDRIVER_DIR_SEARCH
		$ENV{MONGODBCPLUSPLUSDRIVER_ROOT}
        ${SEARCH_PATH_MONGODBCPLUSPLUSDRIVER}
		${MONGODBCPLUSPLUSDRIVER_DIR_SEARCH}
	)
	
	if (EXISTS "C:/") 
		set(MONGODBCPLUSPLUSDRIVER_DIR_SEARCH
			${MONGODBCPLUSPLUSDRIVER_DIR_SEARCH}
			C:/Development
			"C:/Development/moja.global/Third Party"
			"C:/Program Files/"
			"C:/Program Files/mongo"
			"C:/Program Files/mongo-cxx-driver"
		)
	endif()
	
	if (EXISTS "D:/") 
		set(MONGODBCPLUSPLUSDRIVER_DIR_SEARCH
			${MONGODBCPLUSPLUSDRIVER_DIR_SEARCH}
			D:/Development
			"D:/Development/moja.global/Third Party"
			"D:/Program Files/"
			"D:/Program Files/mongo"
			"D:/Program Files/mongo-cxx-driver"
		)
	endif()

endif()

if (CMAKE_SYSTEM MATCHES "Linux" )
	set(MONGODBCPLUSPLUSDRIVER_DIR_SEARCH
		$ENV{MONGODBCPLUSPLUSDRIVER_ROOT}
        ${SEARCH_PATH_MONGODBCPLUSPLUSDRIVER}
		${MONGODBCPLUSPLUSDRIVER_DIR_SEARCH}
		/home/vagrant/moja.global/Third Party/
		/usr/
		/Library/Frameworks
		/System/Library/Frameworks
		/Network/Library/Frameworks
	)
endif(CMAKE_SYSTEM MATCHES "Linux")

# Add in some path suffixes. These will have to be updated whenever a new Driver version version comes out.
SET(SUFFIX_FOR_INCLUDE_PATH
	mongo-cxx-driver-legacy-1.1.0/include
	mongo-cxx-driver-legacy-1.0.6/include
	mongo-cxx-driver/include
)

# MESSAGE("SUFFIX_FOR_INCLUDE_PATH: " ${SUFFIX_FOR_INCLUDE_PATH})

find_path(MONGODBCPLUSPLUSDRIVER_INCLUDE_PATH mongo/config.h
    HINTS
        $ENV{MONGODBCPLUSPLUSDRIVER_ROOT}
    PATH_SUFFIXES ${SUFFIX_FOR_INCLUDE_PATH}
    PATHS
        ${MONGODBCPLUSPLUSDRIVER_DIR_SEARCH}
)

SET(SUFFIX_FOR_LIBRARY_PATH
	mongo-cxx-driver-legacy-1.1.0/lib
	mongo-cxx-driver-legacy-1.0.6/lib
	mongo-cxx-driver/lib
)

IF(NOT MONGODBCPLUSPLUSDRIVER_LIBRARY_DIR)

FIND_LIBRARY(MONGODBCPLUSPLUSDRIVER_LIB NAMES mongoclient mongoclient-gd libmongoclient libmongoclient-gd
	PATH_SUFFIXES 
		${SUFFIX_FOR_LIBRARY_PATH} 
	PATHS # Look in other places.
	  ${MONGODBCPLUSPLUSDRIVER_DIR_SEARCH}
)
GET_FILENAME_COMPONENT(MONGODBCPLUSPLUSDRIVER_LIBRARY_DIR ${MONGODBCPLUSPLUSDRIVER_LIB} PATH CACHE)

ENDIF(NOT MONGODBCPLUSPLUSDRIVER_LIBRARY_DIR)

# handle the QUIETLY and REQUIRED arguments and set OPENAL_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MONGODBCPLUSPLUSDRIVER DEFAULT_MSG MONGODBCPLUSPLUSDRIVER_INCLUDE_PATH)

mark_as_advanced(MONGODBCPLUSPLUSDRIVER_INCLUDE_PATH)