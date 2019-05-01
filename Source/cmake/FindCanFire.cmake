SET(SEARCH_PATH_CANFIRE "" CACHE PATH "Additional CanFire search path")
SET(CANFIRE_DIR_SEARCH
	$ENV{CANFIRE_ROOT}
    ${SEARCH_PATH_CANFIRE}
	${CANFIRE_DIR_SEARCH}
)
if(CMAKE_SYSTEM MATCHES "Windows")
	if (EXISTS "C:/") 
		SET(CANFIRE_DIR_SEARCH
			${CANFIRE_DIR_SEARCH}
            "C:/Dev/CanFIREDLL_x64_v0.9/x64/"
            "C:/Development/CanFIREDLL_x64_v0.9/x64/"
		)
	endif() 
	if (EXISTS "D:/") 
		SET(CANFIRE_DIR_SEARCH
			${CANFIRE_DIR_SEARCH}
            "D:/Dev/CanFIREDLL_x64_v0.9/x64/"
            "D:/Development/CanFIREDLL_x64_v0.9/x64/"
		)
	endif() 

	find_path(CanFire_INCLUDE_PATH headers/CanFIRE.h
		PATH_SUFFIXES include 
		PATHS
			${CANFIRE_DIR_SEARCH}
	)

	find_library(CanFire_LIB CanFIREdll
		PATH_SUFFIXES lib
		PATHS
			${CANFIRE_DIR_SEARCH}
	)
endif()

set(ADDITIONAL_LIB_DIRS)
if (CMAKE_SYSTEM MATCHES "Linux" )
	if(CMAKE_SIZEOF_VOID_P EQUAL 4)
	else()
		set(ADDITIONAL_LIB_DIRS "/usr/lib64" "/usr/lib/x86_64-linux-gnu/")
	endif()

	SET(CANFIRE_LIB_SEARCH
		${ADDITIONAL_LIB_DIRS}
		${CANFIRE_LIB_SEARCH}
        /usr/local/CanFIREDLL_x64_v0.9/
        /usr/local/lib/
        /usr/lib/
	)

	find_library(CanFire_LIB z
		PATHS
			${CANFIRE_LIB_SEARCH}
	)

	SET(CANFIRE_INCLUDE_SEARCH
		${CANFIRE_INCLUDE_SEARCH}
        /usr/local/CanFIREDLL_x64_v0.9/headers/
        /usr/local/
        /usr/
	)

	find_path(CanFire_INCLUDE_PATH CanFIRE.h
		PATH_SUFFIXES include 
		PATHS
			${CANFIRE_INCLUDE_SEARCH}
	)

endif(CMAKE_SYSTEM MATCHES "Linux")

# handle the QUIETLY and REQUIRED arguments and set CANFIRE_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CanFire DEFAULT_MSG CanFire_INCLUDE_PATH)

MARK_AS_ADVANCED(CanFire_INCLUDE_PATH)
