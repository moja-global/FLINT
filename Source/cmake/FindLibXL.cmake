SET(SEARCH_PATH_LIBXL "" CACHE PATH "Additional libXL search path")
SET(LIBXL_DIR_SEARCH
	$ENV{LIBXL_ROOT}
    ${SEARCH_PATH_LIBXL}
	${LIBXL_DIR_SEARCH}
)
if(CMAKE_SYSTEM MATCHES "Windows")
	if (EXISTS "C:/") 
		SET(LIBXL_DIR_SEARCH
			${LIBXL_DIR_SEARCH}
			"C:/Development/libxl-3.8.1.0"
		)
	endif() 
	if (EXISTS "D:/") 
		SET(LIBXL_DIR_SEARCH
			${LIBXL_DIR_SEARCH}
			"D:/Development/libxl-3.8.1.0"
		)
	endif() 

	find_path(libXL_INCLUDE_PATH libxl.h
		PATH_SUFFIXES include_cpp 
		PATHS
			${LIBXL_DIR_SEARCH}
	)

	find_library(libXL_LIB libxl
		PATH_SUFFIXES lib64
		PATHS
			${LIBXL_DIR_SEARCH}
	)
endif()

## Have not purchased the library for linux yet!
## So disabled Linux cmake part
##
##set(ADDITIONAL_LIB_DIRS)
##if (CMAKE_SYSTEM MATCHES "Linux" )
##	SET(LIBXL_LIB_SEARCH
##		${ADDITIONAL_LIB_DIRS}
##		${LIBXL_LIB_SEARCH}
##        /usr/local/libxl/
##        /usr/local/lib/
##        /usr/lib/
##	)
##
##	find_library(libXL_LIB z
##		PATHS
##			${LIBXL_LIB_SEARCH}
##	)
##
##	SET(ZLIB_INCLUDE_SEARCH
##		${ZLIB_INCLUDE_SEARCH}
##        /usr/local/zipper-master/
##        /usr/local/
##        /usr/
##	)
##
##	find_path(libXL_INCLUDE_PATH zlib.h
##		PATH_SUFFIXES 
##            include_cpp
##		PATHS
##			${ZLIB_INCLUDE_SEARCH}
##	)
##
##endif(CMAKE_SYSTEM MATCHES "Linux")

# handle the QUIETLY and REQUIRED arguments and set ZLIB_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(libXL DEFAULT_MSG libXL_INCLUDE_PATH)

MARK_AS_ADVANCED(libXL_INCLUDE_PATH)
