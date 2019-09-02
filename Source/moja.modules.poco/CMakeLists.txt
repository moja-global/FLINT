set(PACKAGE "poco")
set(LIBNAME "moja.modules.${PACKAGE}")
string(REPLACE "." "_" NEW_PACKAGE "${PACKAGE}")
string(TOUPPER "${NEW_PACKAGE}" LIBNAME_EXPORT)

# Poco
find_package(Poco REQUIRED DataSQLite MongoDB)

#if (MSVC)
#  generate_product_version(ProductVersionFiles
#    NAME ${LIBNAME}
#    FILE_DESCRIPTION "moja modules poco"
#    VERSION_MAJOR ${MOJA_VERSION_MAJOR}
#    VERSION_MINOR ${MOJA_VERSION_MINOR}
#    VERSION_PATCH ${MOJA_VERSION_PATCH}
#    VERSION_REVISION ${MOJA_VERSION_REVISION}
#	COMPANY_NAME "moja global"
##    VERSION_REVISION ${BUILD_ID}
#  )
#endif ()

configure_file(
    ../templates/exports.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/moja/modules/${PACKAGE}/_modules.${PACKAGE}_exports.h
    )

set(PROJECT_HEADERS
    include/moja/modules/${PACKAGE}/libraryfactory.h
	include/moja/modules/${PACKAGE}/pocomongoutils.h
    include/moja/modules/${PACKAGE}/_modules.${PACKAGE}_exports.h
)

set(PROJECT_SOURCES
    src/libraryfactory.cpp
	src/pocomongoutils.cpp
)

set(PROJECT_MODULE_HEADERS
)
 
set(PROJECT_MODULE_SOURCES
)
 
set(PROJECT_TRANSFORM_HEADERS
    include/moja/modules/${PACKAGE}/nosqlcollectiontransform.h
    include/moja/modules/${PACKAGE}/pocomongodbquerytransform.h
)

set(PROJECT_TRANSFORM_SOURCES
    src/nosqlcollectiontransform.cpp
    src/pocomongodbquerytransform.cpp
)
 
set(PROJECT_FLINTDATA_HEADERS
)

set(PROJECT_FLINTDATA_SOURCES
)
 
set(PROJECT_PROVIDER_HEADERS
    include/moja/modules/${PACKAGE}/providernosqlpocomongodb.h
    include/moja/modules/${PACKAGE}/providernosqlpocojson.h
)

set(PROJECT_PROVIDER_SOURCES
    src/providernosqlpocomongodb.cpp
    src/providernosqlpocojson.cpp
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

add_library(${LIBNAME} ${LIB_MODE} ${SRCS} )
add_library(${PROJECT_NAME}::${LIBNAME} ALIAS ${LIBNAME})

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
	PUBLIC
		moja::moja.flint Poco::DataSQLite Poco::MongoDB
    )

##############################################
# Installation instructions

include(GNUInstallDirs)

MOJA_INSTALL(${LIBNAME})
MOJA_GENERATE_PACKAGE(${LIBNAME})

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif()
