set(PACKAGE "poco")
set(LIBNAME "moja.modules.${PACKAGE}")
string(REPLACE "." "_" NEW_PACKAGE "${PACKAGE}")
string(TOUPPER "${NEW_PACKAGE}" LIBNAME_EXPORT)

# Poco
find_package(Poco REQUIRED Data MongoDB DataSQLite)

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
add_library(Moja::ModulesPoco ALIAS ${LIBNAME})

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
	PUBLIC Moja::Flint Poco::Data Poco::MongoDB Poco::DataSQLite
    )

##############################################
# Installation instructions

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/MojaModulesPoco)

install(TARGETS ${LIBNAME}
    EXPORT ${LIBNAME}-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

#This is required so that the exported target has the name ModulesPoco and not ${LIBNAME}
set_target_properties(${LIBNAME} PROPERTIES EXPORT_NAME ModulesPoco)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT ${LIBNAME}-targets
  FILE
    MojaModulesPocoTargets.cmake
  NAMESPACE
    Moja::
  DESTINATION
    ${INSTALL_CONFIGDIR}
)

#Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/MojaModulesPocoConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/MojaModulesPocoConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/MojaModulesPocoConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

#Install the config, configversion and custom find modules
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/MojaModulesPocoConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/MojaModulesPocoConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

##############################################
## Exporting from the build tree
export(EXPORT ${LIBNAME}-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/MojaModulesPocoTargets.cmake NAMESPACE Moja::)

#Register package in user's package registry
export(PACKAGE MojaModulesPoco)

if(MSVC)
	INSTALL(
		FILES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/${LIBNAME}${CMAKE_DEBUG_POSTFIX}.pdb 
		DESTINATION ${CMAKE_INSTALL_BINDIR} 
		CONFIGURATIONS Debug
		)
endif()

if(ENABLE_TESTS)
    add_subdirectory(tests)

	execute_process(
		COMMAND ${CMAKE_COMMAND} -E tar xvf ${CMAKE_CURRENT_SOURCE_DIR}/../../Examples/data.zip 
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests
		OUTPUT_QUIET)
endif()



