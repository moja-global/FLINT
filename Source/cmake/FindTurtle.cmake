SET(SEARCH_PATH_TURTLE "" CACHE PATH "Additional Turtle search path")

SET(TURTLE_DIR_SEARCH
	$ENV{TURTLE_ROOT}
    ${SEARCH_PATH_TURTLE}
	${TURTLE_DIR_SEARCH}
)
if(CMAKE_SYSTEM MATCHES "Windows")
	if (EXISTS "C:/") 
		SET(TURTLE_DIR_SEARCH
			${TURTLE_DIR_SEARCH}
			"C:/Development/turtle/include/"
			"C:/Development/turtle-1.3.1/include/"
			"C:/Development/turtle-1.3.0/include/"
			"C:/Development/turtle-1.2.9/include/"
			"C:/Development/turtle-1.2.8/include/"
			"C:/Development/turtle-1.2.7/include/"
			"C:/Development/turtle-1.2.6/include/"
			"C:/Program Files/"
			"C:/Program Files/turtle/"
		)
	endif() 
	if (EXISTS "D:/") 
		SET(TURTLE_DIR_SEARCH
			${TURTLE_DIR_SEARCH}
			"D:/Development/turtle/include/"
			"D:/Development/turtle-1.3.1/include/"
			"D:/Development/turtle-1.3.0/include/"
			"D:/Development/turtle-1.2.9/include/"
			"D:/Development/turtle-1.2.8/include/"
			"D:/Development/turtle-1.2.7/include/"
			"D:/Development/turtle-1.2.6/include/"
			"D:/Program Files/"
			"D:/Program Files/turtle/"
		)
	endif() 
endif()

if (CMAKE_SYSTEM MATCHES "Linux" )
	SET(TURTLE_DIR_SEARCH
		${TURTLE_DIR_SEARCH}
		/usr/local/include
	)
endif(CMAKE_SYSTEM MATCHES "Linux")

find_path(Turtle_INCLUDE_PATH turtle/mock.hpp
    PATHS
        ${TURTLE_DIR_SEARCH}
)

# handle the QUIETLY and REQUIRED arguments and set TURTLE_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Turtle DEFAULT_MSG Turtle_INCLUDE_PATH)

MARK_AS_ADVANCED(Turtle_INCLUDE_PATH)