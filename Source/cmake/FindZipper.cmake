find_path(Zipper_INCLUDE_DIR 
	NAMES zipper.h
	PATH_SUFFIXES zipper
)
if(CMAKE_SYSTEM MATCHES "Windows")

	find_library(Zipper_LIBRARY_DEBUG libZipper-staticd Zipper-staticd
		PATH_SUFFIXES lib )

	find_library(Zipper_LIBRARY_RELEASE libZipper-static Zipper-static
		PATH_SUFFIXES lib )

	include(SelectLibraryConfigurations)
	select_library_configurations(Zipper)

	set(Zipper_LIBRARY 
		debug ${Zipper_LIBRARY_DEBUG}
		optimized ${Zipper_LIBRARY_RELEASE}
					CACHE STRING "Zipper library")
endif()

if (CMAKE_SYSTEM MATCHES "Linux" )

	find_library(Zipper_LIBRARY Zipper-static
		PATH_SUFFIXES lib )

endif(CMAKE_SYSTEM MATCHES "Linux")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Zipper
    REQUIRED_VARS Zipper_INCLUDE_DIR Zipper_LIBRARY
)

mark_as_advanced(Zipper_FOUND Zipper_INCLUDE_DIR Zipper_LIBRARY)

if(Zipper_FOUND)
	set(Zipper_INCLUDE_DIRS ${Zipper_INCLUDE_DIR})
	if(NOT Zipper_LIBRARIES)
		set(Zipper_LIBRARIES ${Zipper_LIBRARY})
	endif()

	if(NOT TARGET Zipper::Zipper)
		add_library(Zipper::Zipper UNKNOWN IMPORTED)
		set_target_properties(Zipper::Zipper PROPERTIES
			IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
			INTERFACE_INCLUDE_DIRECTORIES "${Zipper_INCLUDE_DIR}"
		)

		if (Zipper_LIBRARY_RELEASE)
			set_property(TARGET Zipper::Zipper APPEND PROPERTY 
				IMPORTED_CONFIGURATIONS RELEASE)
			set_target_properties( Zipper::Zipper PROPERTIES 
				IMPORTED_LOCATION_RELEASE ${Zipper_LIBRARY_RELEASE} )
		endif ()

		if (Zipper_LIBRARY_DEBUG)
			set_property(TARGET Zipper::Zipper APPEND PROPERTY 
				IMPORTED_CONFIGURATIONS DEBUG)
			set_target_properties( Zipper::Zipper PROPERTIES 
				IMPORTED_LOCATION_DEBUG ${Zipper_LIBRARY_DEBUG} )
		endif ()

		if(NOT Zipper_LIBRARY_DEBUG AND NOT Zipper_LIBRARY_RELEASE)
			set_target_properties( Zipper::Zipper PROPERTIES 
				IMPORTED_LOCATION ${Zipper_LIBRARY} )
		endif ()

	endif()
endif()
