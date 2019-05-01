SET(SEARCH_PATH_MOJA.CANADA "" CACHE PATH "Additional Moja.Canada search path")
SET(MOJA.CANADA_DIR_SEARCH
	$ENV{MOJA.CANADA_ROOT}
    ${SEARCH_PATH_MOJA.CANADA}
	${MOJA.CANADA_DIR_SEARCH}
)
if(CMAKE_SYSTEM MATCHES "Windows")
	if (EXISTS "C:/") 
		SET(MOJA.CANADA_DIR_SEARCH
			${MOJA.CANADA_DIR_SEARCH}
			"C:/Development/SLEEK-TOOLS/Moja.Canada/dist/"
			"C:/Development/Software/Moja.Canada/"
		)
	endif() 
	if (EXISTS "D:/") 
		SET(MOJA.CANADA_DIR_SEARCH
			${MOJA.CANADA_DIR_SEARCH}
			"D:/Development/SLEEK-TOOLS/Moja.Canada/dist/"
			"D:/Development/Software/Moja.Canada/"
		)
	endif() 

	find_path(Moja.Canada_INCLUDE_PATH cbmmodulebase.h
		PATH_SUFFIXES 
			include/moja/modules/cbm
		PATHS
			${MOJA.CANADA_DIR_SEARCH}
	)

	find_library(Moja.Canada_LIB_DEBUG moja.modules.cbmd
		PATH_SUFFIXES 
			lib
		PATHS
			${MOJA.CANADA_DIR_SEARCH}
	)

	find_library(Moja.Canada_LIB_RELEASE moja.modules.cbm
		PATH_SUFFIXES 
			lib 
		PATHS
			${MOJA.CANADA_DIR_SEARCH}
	)

	set(Moja.Canada_LIB
		debug ${Moja.Canada_LIB_DEBUG}
		optimized ${Moja.Canada_LIB_RELEASE}
		CACHE STRING "Moja.Canada library")

endif()

if (CMAKE_SYSTEM MATCHES "Linux")
	SET(MOJA.CANADA_DIR_SEARCH
		${MOJA.CANADA_DIR_SEARCH}
        /usr/local/
        /usr/
	)

	find_path(Moja.Canada_INCLUDE_PATH cbmmodulebase.h
		PATH_SUFFIXES 
			include/moja/modules/cbm
		PATHS
			${MOJA.CANADA_DIR_SEARCH}
	)

	find_library(Moja.Canada_LIB moja.modules.cbm
		PATH_SUFFIXES 
			lib 
		PATHS
			${MOJA.CANADA_DIR_SEARCH}
	)

	# set(Moja.Canada_LIB
		# ${Moja.Canada_LIB_RELEASE}
		# CACHE STRING "Moja.Canada library")

endif(CMAKE_SYSTEM MATCHES "Linux")

# handle the QUIETLY and REQUIRED arguments and set MOJA.CANADA_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Moja.Canada DEFAULT_MSG Moja.Canada_INCLUDE_PATH)

MARK_AS_ADVANCED(Moja.Canada_INCLUDE_PATH)
