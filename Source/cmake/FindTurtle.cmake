find_path(Turtle_INCLUDE_DIR 
    NAMES mock.hpp
    PATH_SUFFIXES turtle
)

# handle the QUIETLY and REQUIRED arguments and set TURTLE_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Turtle DEFAULT_MSG Turtle_INCLUDE_DIR)

MARK_AS_ADVANCED(Turtle_FOUND Turtle_INCLUDE_DIR)

if(Turtle_FOUND)
    set(Turtle_INCLUDE_DIRS ${Turtle_INCLUDE_DIR})

	if(NOT TARGET Turtle::Turtle)
		add_library(Turtle::Turtle INTERFACE IMPORTED)
		set_target_properties(Turtle::Turtle PROPERTIES
			INTERFACE_INCLUDE_DIRECTORIES "${Turtle_INCLUDE_DIR}"
		)
	endif()
endif()
