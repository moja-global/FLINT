find_path(Zipper_INCLUDE_DIR 
	NAMES zipper/zipper.h
	PATH_SUFFIXES include
)
if(CMAKE_SYSTEM MATCHES "Windows")

	find_library(Zipper_LIBRARY_DEBUG libZipper-staticd Zipper-staticd
		PATH_SUFFIXES debug/lib/ )

	find_library(Zipper_LIBRARY_RELEASE libZipper-static Zipper-static
		PATH_SUFFIXES lib/ )

	if(Zipper_LIBRARY_DEBUG AND NOT Zipper_LIBRARY_RELEASE)
		set(Zipper_LIBRARY_RELEASE ${Zipper_LIBRARY_DEBUG})
	endif(Zipper_LIBRARY_DEBUG AND NOT Zipper_LIBRARY_RELEASE)

	set(Zipper_LIBRARY 
		debug ${Zipper_LIBRARY_DEBUG}
		optimized ${Zipper_LIBRARY_RELEASE}
					CACHE STRING "Zipper library")
endif()

if (CMAKE_SYSTEM MATCHES "Linux" )

	find_library(Zipper_LIBRARY Zipper-static
		PATH_SUFFIXES lib )

endif(CMAKE_SYSTEM MATCHES "Linux")

mark_as_advanced(Zipper_FOUND Zipper_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Zipper
    REQUIRED_VARS Zipper_INCLUDE_DIR
)

if(Zipper_FOUND)
    set(Zipper_INCLUDE_DIRS ${Zipper_INCLUDE_DIR})
endif()

if(Zipper_FOUND AND NOT TARGET Zipper::Zipper_static)
    add_library(Zipper::Zipper_static UNKNOWN IMPORTED)
    set_target_properties(Zipper::Zipper_static PROPERTIES
		IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
        INTERFACE_INCLUDE_DIRECTORIES "${Zipper_INCLUDE_DIR}"
    )
	if (Zipper_LIBRARY_RELEASE)
		set_target_properties( Zipper::Zipper_static PROPERTIES IMPORTED_LOCATION_RELEASE ${Zipper_LIBRARY_RELEASE} )
    endif ()
	if (Zipper_LIBRARY_DEBUG)
		set_target_properties( Zipper::Zipper_static PROPERTIES IMPORTED_LOCATION_DEBUG ${Zipper_LIBRARY_DEBUG} )
    endif ()

    set(Zipper_LIBRARY       Zipper::Zipper_static )
    set(Zipper_LIBRARIES     Zipper::Zipper_static )
	mark_as_advanced(Zipper_LIBRARY Zipper_LIBRARY_RELEASE Zipper_LIBRARY_DEBUG Zipper_INCLUDE_DIR)
endif()
