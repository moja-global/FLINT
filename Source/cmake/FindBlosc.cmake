find_path(BLOSC_INCLUDE_DIR blosc.h)

find_library(BLOSC_LIBRARY NAMES blosc)

if (BLOSC_INCLUDE_DIR AND BLOSC_LIBRARY)
    set(BLOSC_FOUND TRUE)
    message(STATUS "Found blosc library: ${BLOSC_LIBRARY}")
endif ()


SET(SEARCH_PATH_BLOSC "" CACHE PATH "Additional blosc search path")
SET(BLOSC_DIR_SEARCH
	$ENV{BLOSC_ROOT}
    ${SEARCH_PATH_BLOSC}
	${BLOSC_DIR_SEARCH}
)
if(CMAKE_SYSTEM MATCHES "Windows")
	if (EXISTS "C:/") 
		SET(BLOSC_DIR_SEARCH
			${BLOSC_DIR_SEARCH}
			"C:/Development/c-blosc-1.12.1/dist/"
			"C:/Development/Software/c-blosc/"
		)
	endif() 
	if (EXISTS "D:/") 
		SET(BLOSC_DIR_SEARCH
			${BLOSC_DIR_SEARCH}
			"D:/Development/c-blosc-1.12.1/dist/"
			"D:/Development/Software/c-blosc/"
		)
	endif() 

	find_path(Blosc_INCLUDE_PATH blosc.h
		PATH_SUFFIXES include 
		PATHS
			${BLOSC_DIR_SEARCH}
	)

	find_library(Blosc_LIB blosc
		PATH_SUFFIXES lib
		PATHS
			${BLOSC_DIR_SEARCH}
	)
endif()

set(ADDITIONAL_LIB_DIRS)
if (CMAKE_SYSTEM MATCHES "Linux" )
	if(CMAKE_SIZEOF_VOID_P EQUAL 4)
	else()
		set(ADDITIONAL_LIB_DIRS "/usr/lib64" "/usr/lib/x86_64-linux-gnu/")
	endif()

	SET(BLOSC_LIB_SEARCH
		${ADDITIONAL_LIB_DIRS}
		${BLOSC_LIB_SEARCH}
        /usr/local/c-blosc-master/
        /usr/local/lib/
        /usr/lib/
	)

	find_library(Blosc_LIB blosc
		PATHS
			${BLOSC_LIB_SEARCH}
	)

	SET(BLOSC_INCLUDE_SEARCH
		${BLOSC_INCLUDE_SEARCH}
        /usr/local/c-blosc-master/
        /usr/local/
        /usr/
	)

	find_path(Blosc_INCLUDE_PATH blosc.h
		PATH_SUFFIXES include 
		PATHS
			${BLOSC_INCLUDE_SEARCH}
	)

endif(CMAKE_SYSTEM MATCHES "Linux")

# handle the QUIETLY and REQUIRED arguments and set BLOSC_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Blosc DEFAULT_MSG Blosc_INCLUDE_PATH)

MARK_AS_ADVANCED(Blosc_INCLUDE_PATH)
