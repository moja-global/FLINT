SET(SEARCH_PATH_MOJA     "" CACHE PATH "Additional Moja search path")
SET(SEARCH_PATH_LIB_MOJA "" CACHE PATH "Additional Moja library search path")

SET(MOJA_INCLUDE_PATH_DESCRIPTION "top-level directory containing the moja include directories. E.g /usr/local/include/ or c:\\moja\\include\\moja_1_0_3")
SET(MOJA_INCLUDE_DIR_MESSAGE "Set the MOJA_INCLUDE_DIR cmake cache entry to the ${MOJA_INCLUDE_PATH_DESCRIPTION}")
SET(MOJA_LIBRARY_PATH_DESCRIPTION "top-level directory containing the moja libraries.")
SET(MOJA_LIBRARY_DIR_MESSAGE "Set the Moja_LIBRARY_DIR cmake cache entry to the ${MOJA_LIBRARY_PATH_DESCRIPTION}")

SET(SEARCH_PATH_MOJA "" CACHE PATH "Additional Moja search path")

if(CMAKE_SYSTEM MATCHES "Windows")

	SET(MOJA_DIR_SEARCH
		$ENV{MOJA_ROOT}
		${SEARCH_PATH_MOJA}
		${MOJA_DIR_SEARCH}
	)
	SET(MOJA_LIB_DIR_SEARCH
		$ENV{MOJA_ROOT}
		${SEARCH_PATH_LIB_MOJA}
		${MOJA_DIR_SEARCH}
	)
	
	SET(MOJA_LIBNAME "moja")
	if (EXISTS "C:/") 
		SET(MOJA_DIR_SEARCH
			${MOJA_DIR_SEARCH}
			"C:/Development/moja.global/dist"
			"C:/Development/moja.global/dist/lib/Debug"
			"C:/Development/moja.global/dist/lib/Release"
            "C:/Development/Software/moja/moja_develop"
		)
		SET(MOJA_LIB_DIR_SEARCH
			${MOJA_LIB_DIR_SEARCH}	
			"C:/Development/moja.global/dist"			
			"C:/Development/moja.global/dist/lib/Debug"
			"C:/Development/moja.global/dist/lib/Release"
            "C:/Development/Software/moja/moja_develop"
		)
	endif()
endif()

if (CMAKE_SYSTEM MATCHES "Linux" )

	SET(MOJA_DIR_SEARCH
		${MOJA_DIR_SEARCH}
#		/usr/local/include/
		/usr/local/
	)
	SET(MOJA_LIB_DIR_SEARCH
		${MOJA_LIB_DIR_SEARCH}
#		/usr/local/lib/
		/usr/local/
		/usr/lib/x86_64-linux-gnu/
	)
endif(CMAKE_SYSTEM MATCHES "Linux")

SET(SUFFIX_FOR_INCLUDE_PATH
	moja_develop
	moja.develop
	moja_1_0_5
	moja_1_0_4
	moja_1_0_3
	moja_1_0_1
	moja
)

SET(SUFFIX_FOR_LIBRARY_PATH
	moja_develop/lib
	moja_develop/bin
	moja.develop/lib
	moja.develop/bin
	moja_1_0_5/lib
	moja_1_0_5/bin
	moja_1_0_4/lib
	moja_1_0_4/bin
	moja_1_0_3/lib
	moja_1_0_3/bin
	moja_1_0_1/lib
	moja_1_0_1/bin
	lib
	bin
	moja/lib
	moja/bin
)

#
# Look for an installation.
#
FIND_PATH(
	MOJA_INCLUDE_DIR NAMES 
		include/moja/itiming.h
	PATH_SUFFIXES 
		${SUFFIX_FOR_INCLUDE_PATH} 
	PATHS
		# Look in other places.
		${MOJA_INCLUDE_DIR}
		${MOJA_DIR_SEARCH}
	DOC 
		# Help the user find it if we cannot.
		"The ${MOJA_INCLUDE_PATH_DESCRIPTION}"
)

#
# Look for standard unix include paths
#
IF(NOT MOJA_INCLUDE_DIR)
	FIND_PATH(
		MOJA_INCLUDE_DIR 
			moja/itiming.h 
		DOC 
			"The ${MOJA_INCLUDE_PATH_DESCRIPTION}"
	)
ENDIF(NOT MOJA_INCLUDE_DIR)

#
# Assume we didn't find it.
#
SET(Moja_FOUND 0)

#
# Now try to get the include and library path.
#
SET(Moja_INCLUDE_DIRS 
		${MOJA_INCLUDE_DIR}/include
	CACHE PATH 
		"Location of Moja include files"
)
SET(Moja_FOUND 1)

IF(NOT MOJA_LIBRARY_DIR)

	FIND_LIBRARY(
		MOJA_LIB NAMES 
			moja.core moja.cored 
		PATH_SUFFIXES 
			${SUFFIX_FOR_LIBRARY_PATH} 
		PATHS 
			# Look in other places.
			${Moja_INCLUDE_DIR}
			${MOJA_LIB_DIR_SEARCH}
		DOC 
			# Help the user find it if we cannot.
			"The ${MOJA_LIBRARY_PATH_DESCRIPTION}"
	)
	GET_FILENAME_COMPONENT(MOJA_LIBRARY_DIR ${MOJA_LIB} PATH CACHE)
	
	IF(Moja_LIBRARY_DIR)
		# Look for the moja binary path.
		SET(Moja_BINARY_DIR ${MOJA_INCLUDE_DIR})

		IF(Moja_BINARY_DIR AND EXISTS "${Moja_BINARY_DIR}/bin64")
			SET(Moja_BINARY_DIRS ${Moja_BINARY_DIR}/bin64 CACHE PATH "Path to Moja binaries")
		ENDIF(Moja_BINARY_DIR AND EXISTS "${Moja_BINARY_DIR}/bin64")
	ENDIF(Moja_LIBRARY_DIR)

	# Debug lubraries
	find_library(
		Moja_CORE_DEBUG NAMES 
			moja.cored 
			moja.cored_dll
		PATH_SUFFIXES 
			${SUFFIX_FOR_LIBRARY_PATH} 
			debug
		PATHS # Look in other places.
			${MOJA_INCLUDE_DIR}
			${MOJA_LIB_DIR_SEARCH}
	)

	find_library(
		Moja_DATAREPOSITORY_DEBUG NAMES 
			moja.datarepositoryd 
			moja.datarepositoryd_dll
		PATH_SUFFIXES 
			${SUFFIX_FOR_LIBRARY_PATH} 
			debug
		PATHS # Look in other places.
			${MOJA_INCLUDE_DIR}
			${MOJA_LIB_DIR_SEARCH}
	)
	
	find_library(
		Moja_FLINT_CONFIGURATION_DEBUG NAMES 
			moja.flint.configurationd 
			moja.flint.configurationd_dll
		PATH_SUFFIXES 
			${SUFFIX_FOR_LIBRARY_PATH} 
			debug
		PATHS # Look in other places.
			${MOJA_INCLUDE_DIR}
			${MOJA_LIB_DIR_SEARCH}
	)
	
	find_library(
		Moja_FLINT_DEBUG NAMES 
			moja.flintd 
			moja.flintd_dll
		PATH_SUFFIXES 
			${SUFFIX_FOR_LIBRARY_PATH} 
			debug
		PATHS # Look in other places.
			${MOJA_INCLUDE_DIR}
			${MOJA_LIB_DIR_SEARCH}
	)
	
	# Release lubraries
	find_library(
		Moja_CORE_RELEASE NAMES 
			moja.core 
			moja.core_dll
		PATH_SUFFIXES 
			${SUFFIX_FOR_LIBRARY_PATH} 
			release
		PATHS # Look in other places.
			${MOJA_INCLUDE_DIR}
			${MOJA_LIB_DIR_SEARCH}
	)

	find_library(
		Moja_DATAREPOSITORY_RELEASE NAMES 
			moja.datarepository 
			moja.datarepository_dll
		PATH_SUFFIXES 
			${SUFFIX_FOR_LIBRARY_PATH} 
			release
		PATHS # Look in other places.
			${MOJA_INCLUDE_DIR}
			${MOJA_LIB_DIR_SEARCH}
	)
	
	find_library(
		Moja_FLINT_CONFIGURATION_RELEASE NAMES 
			moja.flint.configuration 
			moja.flint.configuration_dll
		PATH_SUFFIXES 
			${SUFFIX_FOR_LIBRARY_PATH} 
			release
		PATHS # Look in other places.
			${MOJA_INCLUDE_DIR}
			${MOJA_LIB_DIR_SEARCH}
	)
	
	find_library(
		Moja_FLINT_RELEASE NAMES 
			moja.flint 
			moja.flint_dll
		PATH_SUFFIXES 
			${SUFFIX_FOR_LIBRARY_PATH} 
			release
		PATHS # Look in other places.
			${MOJA_INCLUDE_DIR}
			${MOJA_LIB_DIR_SEARCH}
	)

	if(Moja_CORE_DEBUG AND NOT Moja_CORE_RELEASE)
		set(Moja_CORE ${Moja_CORE_DEBUG} CACHE STRING "Moja core link library text")
	endif()
	if(Moja_CORE_RELEASE AND NOT Moja_CORE_DEBUG)
		set(Moja_CORE ${Moja_CORE_RELEASE} CACHE STRING "Moja core link library text")
	endif()
	if(Moja_CORE_RELEASE AND Moja_CORE_DEBUG)
		set(Moja_CORE 	debug     ${Moja_CORE_DEBUG}
						optimized ${Moja_CORE_RELEASE}
			CACHE STRING "Moja core link library text")
	endif()

	if(Moja_DATAREPOSITORY_DEBUG AND NOT Moja_DATAREPOSITORY_RELEASE)
		set(Moja_DATAREPOSITORY ${Moja_DATAREPOSITORY_DEBUG} CACHE STRING "Moja datarepository link library text")
	endif()
	if(Moja_DATAREPOSITORY_RELEASE AND NOT Moja_DATAREPOSITORY_DEBUG)
		set(Moja_DATAREPOSITORY ${Moja_DATAREPOSITORY_RELEASE} CACHE STRING "Moja datarepository link library text")
	endif()
	if(Moja_DATAREPOSITORY_RELEASE AND Moja_DATAREPOSITORY_DEBUG)
		set(Moja_DATAREPOSITORY 	debug     ${Moja_DATAREPOSITORY_DEBUG}
						optimized ${Moja_DATAREPOSITORY_RELEASE}
			CACHE STRING "Moja datarepository link library text")
	endif()

	if(Moja_FLINT_CONFIGURATION_DEBUG AND NOT Moja_FLINT_CONFIGURATION_RELEASE)
		set(Moja_FLINT_CONFIGURATION ${Moja_FLINT_CONFIGURATION_DEBUG} CACHE STRING "Moja flint configuration link library text")
	endif()
	if(Moja_FLINT_CONFIGURATION_RELEASE AND NOT Moja_FLINT_CONFIGURATION_DEBUG)
		set(Moja_FLINT_CONFIGURATION ${Moja_FLINT_CONFIGURATION_RELEASE} CACHE STRING "Moja flint configuration link library text")
	endif()
	if(Moja_FLINT_CONFIGURATION_RELEASE AND Moja_FLINT_CONFIGURATION_DEBUG)
		set(Moja_FLINT_CONFIGURATION 	debug     ${Moja_FLINT_CONFIGURATION_DEBUG}
						optimized ${Moja_FLINT_CONFIGURATION_RELEASE}
			CACHE STRING "Moja flint configuration link library text")
	endif()
	
	if(Moja_FLINT_DEBUG AND NOT Moja_FLINT_RELEASE)
		set(Moja_FLINT ${Moja_FLINT_DEBUG} CACHE STRING "Moja flint link library text")
	endif()
	if(Moja_FLINT_RELEASE AND NOT Moja_FLINT_DEBUG)
		set(Moja_FLINT ${Moja_FLINT_RELEASE} CACHE STRING "Moja flint link library text")
	endif()
	if(Moja_FLINT_RELEASE AND Moja_FLINT_DEBUG)
		set(Moja_FLINT 	debug     ${Moja_FLINT_DEBUG}
						optimized ${Moja_FLINT_RELEASE}
			CACHE STRING "Moja flint link library text")
	endif()

		
ENDIF(NOT MOJA_LIBRARY_DIR)
	
IF(NOT Moja_FOUND)
	IF(Moja_FIND_QUIETLY)
		message(STATUS "Moja was not found. ${MOJA_INCLUDE_DIR_MESSAGE}")
	ELSE(Moja_FIND_QUIETLY)
		IF(Moja_FIND_REQUIRED)
			message(FATAL_ERROR "Moja was not found. ${MOJA_INCLUDE_DIR_MESSAGE}")
		ENDIF(Moja_FIND_REQUIRED)
	ENDIF(Moja_FIND_QUIETLY)
ENDIF(NOT Moja_FOUND)

# handle the QUIETLY and REQUIRED arguments and set OPENAL_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MOJA DEFAULT_MSG MOJA_INCLUDE_DIR)

MARK_AS_ADVANCED(MOJA_INCLUDE_DIR)