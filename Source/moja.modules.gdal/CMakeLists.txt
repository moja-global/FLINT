set(PACKAGE "gdal")
set(LIBNAME "moja.modules.${PACKAGE}")
string(REPLACE "." "_" NEW_PACKAGE "${PACKAGE}")
string(TOUPPER "${NEW_PACKAGE}" LIBNAME_EXPORT)

find_package(GDAL REQUIRED)
find_package(fmt REQUIRED)

configure_file(
    ../templates/exports.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/moja/modules/${PACKAGE}/_modules.${PACKAGE}_exports.h
    )

set(PROJECT_HEADERS
    include/moja/modules/${PACKAGE}/libraryfactory.h
    include/moja/modules/${PACKAGE}/_modules.${PACKAGE}_exports.h
)

set(PROJECT_SOURCES
    src/libraryfactory.cpp
)

set(PROJECT_MODULE_HEADERS
	include/moja/modules/${PACKAGE}/writevariablegeotiff.h
)
 
set(PROJECT_MODULE_SOURCES
	src/writevariablegeotiff.cpp
)

set(PROJECT_PROVIDER_HEADERS
    src/gdalcpp.h
	include/moja/modules/${PACKAGE}/rasterreadergdal.h
)
 
set(PROJECT_PROVIDER_SOURCES
	src/rasterreadergdal.cpp
)
 
set(PROJECT_TRANSFORM_HEADERS
)

set(PROJECT_TRANSFORM_SOURCES
)
 
set(PROJECT_FLINTDATA_HEADERS
)

set(PROJECT_FLINTDATA_SOURCES
)

source_group("header files\\other"					FILES ${PROJECT_HEADERS})
source_group("source files\\other"					FILES ${PROJECT_SOURCES})
source_group("header files\\modules"				FILES ${PROJECT_MODULE_HEADERS})
source_group("source files\\modules"				FILES ${PROJECT_MODULE_SOURCES})
source_group("header files\\transforms"				FILES ${PROJECT_TRANSFORM_HEADERS})
source_group("source files\\transforms"				FILES ${PROJECT_TRANSFORM_SOURCES})
source_group("header files\\flintdata"				FILES ${PROJECT_FLINTDATA_HEADERS})
source_group("source files\\flintdata"				FILES ${PROJECT_FLINTDATA_SOURCES})
source_group("header files\\providers"				FILES ${PROJECT_PROVIDER_HEADERS})
source_group("source files\\providers"				FILES ${PROJECT_PROVIDER_SOURCES})

set(SRCS 
	${PROJECT_SOURCES} ${PROJECT_HEADERS}
	${PROJECT_MODULE_SOURCES} ${PROJECT_MODULE_HEADERS}
	${PROJECT_TRANSFORM_SOURCES} ${PROJECT_TRANSFORM_HEADERS}
	${PROJECT_FLINTDATA_SOURCES} ${PROJECT_FLINTDATA_HEADERS}
    ${PROJECT_PROVIDER_HEADERS} ${PROJECT_PROVIDER_SOURCES}
)

add_library(${LIBNAME} ${LIB_MODE} ${SRCS})
add_library(${PROJECT_NAME}::${LIBNAME} ALIAS ${LIBNAME})

#Set target properties
set_target_properties(${LIBNAME} 
    PROPERTIES
    VERSION ${MOJA_VERSION} SOVERSION ${MOJA_VERSION_MAJOR}
    DEFINE_SYMBOL ${LIBNAME_EXPORT}_EXPORTS
)

target_include_directories(${LIBNAME}
    PUBLIC 
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${LIBNAME} 
    PRIVATE
		fmt::fmt-header-only
	PUBLIC
		moja::moja.flint ${GDAL_LIBRARY}
)

##############################################
# Installation instructions

include(GNUInstallDirs)

MOJA_INSTALL(${LIBNAME})
MOJA_GENERATE_PACKAGE(${LIBNAME})

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif()
