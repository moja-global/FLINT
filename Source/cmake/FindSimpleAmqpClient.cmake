#Find the SimpleAmqpClient library

INCLUDE(LibFindMacros)

SET(SIMPLEAMQPCLIENT_DIR_SEARCH
	${SimpleAmqpClient_DIR}
	$ENV{SimpleAmqpClient_ROOT}
    ${SEARCH_PATH_SimpleAmqpClient}
	${SimpleAmqpClient_DIR_SEARCH}
)

if(CMAKE_SYSTEM MATCHES "Windows")
	if (EXISTS "C:/") 
		SET(SIMPLEAMQPCLIENT_DIR_SEARCH
			${SIMPLEAMQPCLIENT_DIR_SEARCH}
			"C:/Development/Software/SimpleAmqpClient"
			"C:/Development/SimpleAmqpClient-2.4.0/"
			"C:/Development/SimpleAmqpClient/"
		)
	endif() 

	if (EXISTS "D:/") 
		SET(SIMPLEAMQPCLIENT_DIR_SEARCH
			${SIMPLEAMQPCLIENT_DIR_SEARCH}
			"D:/Development/Software/SimpleAmqpClient"
			"D:/Development/SimpleAmqpClient-2.4.0/"
			"D:/Development/SimpleAmqpClient/"
		)
	endif() 
endif() 

if (CMAKE_SYSTEM MATCHES "Linux" )
	SET(SimpleAmqpClient_LIBNAME "SimpleAmqpClient")
	SET(SimpleAmqpClient_DIR_SEARCH
		${SIMPLEAMQPCLIENT_DIR_SEARCH}
		/usr/local/include/SimpleAmqpClient/
	)
	SET(SIMPLEAMQPCLIENT_LIB_DIR_SEARCH
		${SIMPLEAMQPCLIENT_LIB_DIR_SEARCH}
		/usr/local/lib/
	)
endif(CMAKE_SYSTEM MATCHES "Linux")

# Find the include directories
FIND_PATH(SimpleAmqpClient_INCLUDE_DIR
	NAMES SimpleAmqpClient/SimpleAmqpClient.h
    HINTS src
	PATH_SUFFIXES src 
	PATHS ${SIMPLEAMQPCLIENT_DIR_SEARCH}
	)

if(CMAKE_SYSTEM MATCHES "Windows")
	FIND_LIBRARY(SimpleAmqpClient_LIBRARY_Release
		NAMES SimpleAmqpClient.2
		HINTS build/Release
		PATH_SUFFIXES 
			lib 
			build/Release 

		PATHS ${SIMPLEAMQPCLIENT_DIR_SEARCH}
		)

	FIND_LIBRARY(SimpleAmqpClient_LIBRARY_Debug
		NAMES SimpleAmqpClient.2d
		HINTS build/Debug
		PATH_SUFFIXES 
			lib 
			build/Debug
		PATHS ${SIMPLEAMQPCLIENT_DIR_SEARCH}
	)

	set( SimpleAmqpClient_LIBRARY debug ${SimpleAmqpClient_LIBRARY_Debug}
						 optimized ${SimpleAmqpClient_LIBRARY_Release}
					CACHE STRING "SimpleAmqpClient library text")
endif() 

if (CMAKE_SYSTEM MATCHES "Linux" )
	FIND_LIBRARY(SimpleAmqpClient_LIBRARY
		NAMES ${SimpleAmqpClient_LIBNAME}
		PATHS ${SIMPLEAMQPCLIENT_LIB_DIR_SEARCH}
		)
endif(CMAKE_SYSTEM MATCHES "Linux")

# SET(SimpleAmqpClient_PROCESS_INCLUDES SimpleAmqpClient_INCLUDE_DIR)
# SET(SimpleAmqpClient_PROCESS_LIBS SimpleAmqpClient_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SIMPLEAMQPCLIENT DEFAULT_MSG SimpleAmqpClient_INCLUDE_DIR)

# LIBFIND_PROCESS(SimpleAmqpClient)
