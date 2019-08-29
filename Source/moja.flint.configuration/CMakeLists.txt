set(PACKAGE "configuration")
set(LIBNAME "moja.flint.${PACKAGE}")
string(TOUPPER "${PACKAGE}" LIBNAME_EXPORT)

include(${CMAKE_MODULE_PATH}/generate_product_version.cmake) 

find_package(Poco REQUIRED Foundation JSON)

# Version Info
if (MSVC)
  generate_product_version(ProductVersionFiles
    NAME ${LIBNAME}
    FILE_DESCRIPTION "moja global configuration"
    VERSION_MAJOR ${MOJA_VERSION_MAJOR}
    VERSION_MINOR ${MOJA_VERSION_MINOR}
    VERSION_PATCH ${MOJA_VERSION_PATCH}
    VERSION_REVISION ${MOJA_VERSION_REVISION}
	COMPANY_NAME "moja global"
#    VERSION_REVISION ${BUILD_ID}
  )
endif ()

# HEADERS AND SOURCE

configure_file(../templates/exports.h ${CMAKE_CURRENT_SOURCE_DIR}/include/moja/flint/${PACKAGE}/_${PACKAGE}_exports.h)

set(MOJA_FLINT_Configuration_headers
	include/moja/flint/${PACKAGE}/_${PACKAGE}_exports.h
	include/moja/flint/configuration/configuration.h
	include/moja/flint/configuration/configurationexceptions.h
	include/moja/flint/configuration/configtile.h
	include/moja/flint/configuration/configblock.h
	include/moja/flint/configuration/configcell.h
    include/moja/flint/configuration/externalpool.h
	include/moja/flint/configuration/externalvariable.h
	include/moja/flint/configuration/flintdatavariable.h
	include/moja/flint/configuration/iconfigurationprovider.h
	include/moja/flint/configuration/json2configurationprovider.h
	include/moja/flint/configuration/landscape.h
	include/moja/flint/configuration/library.h
	include/moja/flint/configuration/localdomain.h
	include/moja/flint/configuration/module.h
	include/moja/flint/configuration/operationmanager.h
	include/moja/flint/configuration/pool.h
	include/moja/flint/configuration/provider.h
	include/moja/flint/configuration/spinup.h
	include/moja/flint/configuration/ivariable.h
	include/moja/flint/configuration/itiming.h
	include/moja/flint/configuration/transform.h
	include/moja/flint/configuration/flintdata.h
	include/moja/flint/configuration/variable.h
	include/moja/flint/configuration/iterationbase.h
	include/moja/flint/configuration/iterationaspatialindex.h
	include/moja/flint/configuration/iterationaspatialmongoindex.h
	include/moja/flint/configuration/iterationlandscapetiles.h
	include/moja/flint/configuration/iterationareaofinterest.h
	include/moja/flint/configuration/iterationtileindex.h
	include/moja/flint/configuration/iterationblockindex.h
	include/moja/flint/configuration/iterationcellindex.h
)

set(MOJA_FLINT_Configuration_sources
	src/configuration.cpp
	src/configtile.cpp
	src/configblock.cpp
	src/configcell.cpp
    src/externalpool.cpp
	src/externalvariable.cpp
	src/flintdatavariable.cpp
	src/json2configurationprovider.cpp
	src/landscape.cpp
	src/library.cpp
	src/localdomain.cpp
	src/module.cpp
	src/operationmanager.cpp
	src/pool.cpp
	src/provider.cpp
	src/spinup.cpp
	src/ivariable.cpp
	src/transform.cpp
	src/flintdata.cpp
	src/variable.cpp
	src/iterationbase.cpp
	src/iterationaspatialindex.cpp
	src/iterationaspatialmongoindex.cpp
	src/iterationlandscapetiles.cpp
	src/iterationareaofinterest.cpp
	src/iterationtileindex.cpp
	src/iterationblockindex.cpp
	src/iterationcellindex.cpp
)

set (SRCS ${MOJA_FLINT_Configuration_sources} ${MOJA_FLINT_Configuration_headers})

add_library(${LIBNAME} ${LIB_MODE} ${SRCS} ${ProductVersionFiles})
add_library(Moja::FlintConfiguration ALIAS ${LIBNAME})

#Set target properties
set_target_properties(${LIBNAME} 
	PROPERTIES
	VERSION ${MOJA_VERSION} SOVERSION ${MOJA_VERSION_MAJOR}
	DEFINE_SYMBOL ${LIBNAME_EXPORT}_EXPORTS)

target_include_directories(${LIBNAME}
    PUBLIC 
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${LIBNAME}
	PUBLIC
		Moja::Core Poco::Foundation Poco::JSON
	PRIVATE
		${SYSLIBS}
)

if (CMAKE_SYSTEM MATCHES "Linux")
	target_link_libraries(${LIBNAME} 
		PRIVATE	
			dl
	)
endif(CMAKE_SYSTEM MATCHES "Linux")


##############################################
# Installation instructions

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/MojaFlintConfiguration)

install(TARGETS ${LIBNAME}
    EXPORT ${LIBNAME}-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

#This is required so that the exported target has the name Core and not ${LIBNAME}
set_target_properties(${LIBNAME} PROPERTIES EXPORT_NAME FlintConfiguration)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT ${LIBNAME}-targets
  FILE
    MojaFlintConfigurationTargets.cmake
  NAMESPACE
    Moja::
  DESTINATION
    ${INSTALL_CONFIGDIR}
)

#Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/MojaFlintConfigurationConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/MojaFlintConfigurationConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/MojaFlintConfigurationConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

#Install the config, configversion and custom find modules
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/MojaFlintConfigurationConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/MojaFlintConfigurationConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

##############################################
## Exporting from the build tree
export(EXPORT ${LIBNAME}-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/MojaFlintConfigurationTargets.cmake NAMESPACE Moja::)

#Register package in user's package registry
export(PACKAGE MojaFlintConfiguration)

if(MSVC)
	INSTALL(
		FILES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/${LIBNAME}${CMAKE_DEBUG_POSTFIX}.pdb 
		DESTINATION ${CMAKE_INSTALL_BINDIR} 
		CONFIGURATIONS Debug
		)
endif()

if (ENABLE_TESTS)
  add_subdirectory(tests)
endif()
