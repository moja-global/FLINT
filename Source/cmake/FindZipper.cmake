SET(SEARCH_PATH_ZIPPER "" CACHE PATH "Additional Zipper search path")
SET(ZIPPER_DIR_SEARCH
	$ENV{ZIPPER_ROOT}
    ${SEARCH_PATH_ZIPPER}
	${ZIPPER_DIR_SEARCH}
)
if(CMAKE_SYSTEM MATCHES "Windows")
	if (EXISTS "C:/") 
		SET(ZIPPER_DIR_SEARCH
			${ZIPPER_DIR_SEARCH}
			"C:/Development/SLEEK-TOOLS/zipper/dist/"
			"C:/Development/Software/zipper/"
		)
	endif() 
	if (EXISTS "D:/") 
		SET(ZIPPER_DIR_SEARCH
			${ZIPPER_DIR_SEARCH}
			"D:/Development/SLEEK-TOOLS/zipper/dist/"
			"D:/Development/Software/zipper/"
		)
	endif() 

	find_path(Zipper_INCLUDE_PATH zipper/zipper.h
		PATH_SUFFIXES 
			include
		PATHS
			${ZIPPER_DIR_SEARCH}
	)

	find_library(Zipper_LIB_DEBUG libZipper-staticd
		PATH_SUFFIXES 
			lib
		PATHS
			${ZIPPER_DIR_SEARCH}
	)

	find_library(Zipper_LIB_RELEASE libZipper-static
		PATH_SUFFIXES 
			lib 
		PATHS
			${ZIPPER_DIR_SEARCH}
	)

	set(Zipper_LIB
		debug ${Zipper_LIB_DEBUG}
		optimized ${Zipper_LIB_RELEASE}
		CACHE STRING "Zipper library")

endif()

if (CMAKE_SYSTEM MATCHES "Linux" )
	SET(ZIPPER_DIR_SEARCH
		${ZIPPER_DIR_SEARCH}
        /usr/local/
        /usr/
	)

	find_path(Zipper_INCLUDE_PATH zipper/zipper.h
		PATH_SUFFIXES 
			include 
		PATHS
			${ZIPPER_DIR_SEARCH}
	)

	find_library(Zipper_LIB Zipper-static
		PATH_SUFFIXES 
			lib 
		PATHS
			${ZIPPER_DIR_SEARCH}
	)

	# set(Zipper_LIB
		# ${Zipper_LIB_RELEASE}
		# CACHE STRING "Zipper library")

endif(CMAKE_SYSTEM MATCHES "Linux")

# handle the QUIETLY and REQUIRED arguments and set ZIPPER_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Zipper DEFAULT_MSG Zipper_INCLUDE_PATH)

MARK_AS_ADVANCED(Zipper_INCLUDE_PATH)
