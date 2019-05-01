SET(SEARCH_PATH_ZLIB "" CACHE PATH "Additional Zlib search path")
SET(ZLIB_DIR_SEARCH
	$ENV{ZLIB_ROOT}
    ${SEARCH_PATH_ZLIB}
	${ZLIB_DIR_SEARCH}
)
if(CMAKE_SYSTEM MATCHES "Windows")
	if (EXISTS "C:/") 
		SET(ZLIB_DIR_SEARCH
			${ZLIB_DIR_SEARCH}
			"C:/Development/zlib-1.2.11/dist/"
			"C:/Development/Software/zlib/"
		)
	endif() 
	if (EXISTS "D:/") 
		SET(ZLIB_DIR_SEARCH
			${ZLIB_DIR_SEARCH}
			"D:/Development/zlib-1.2.11/dist/"
			"D:/Development/Software/zlib/"
		)
	endif() 

	find_path(Zlib_INCLUDE_PATH zlib.h
		PATH_SUFFIXES include 
		PATHS
			${ZLIB_DIR_SEARCH}
	)

	find_library(Zlib_LIB zlib
		PATH_SUFFIXES lib
		PATHS
			${ZLIB_DIR_SEARCH}
	)
endif()

set(ADDITIONAL_LIB_DIRS)
if (CMAKE_SYSTEM MATCHES "Linux" )
	if(CMAKE_SIZEOF_VOID_P EQUAL 4)
	else()
		set(ADDITIONAL_LIB_DIRS "/usr/lib64" "/usr/lib/x86_64-linux-gnu/")
	endif()

	SET(ZLIB_LIB_SEARCH
		${ADDITIONAL_LIB_DIRS}
		${ZLIB_LIB_SEARCH}
        /usr/local/zipper-master/
        /usr/local/lib/
        /usr/lib/
	)

	find_library(Zlib_LIB z
		PATHS
			${ZLIB_LIB_SEARCH}
	)

	SET(ZLIB_INCLUDE_SEARCH
		${ZLIB_INCLUDE_SEARCH}
        /usr/local/zipper-master/
        /usr/local/
        /usr/
	)

	find_path(Zlib_INCLUDE_PATH zlib.h
		PATH_SUFFIXES include 
		PATHS
			${ZLIB_INCLUDE_SEARCH}
	)

endif(CMAKE_SYSTEM MATCHES "Linux")

# handle the QUIETLY and REQUIRED arguments and set ZLIB_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Zlib DEFAULT_MSG Zlib_INCLUDE_PATH)

MARK_AS_ADVANCED(Zlib_INCLUDE_PATH)
