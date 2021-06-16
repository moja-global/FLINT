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
add_library(moja::moja.flint.configuration ALIAS ${LIBNAME})

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
		moja::moja.core Poco::Foundation Poco::JSON
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
MOJA_INSTALL(${LIBNAME})
MOJA_GENERATE_PACKAGE(${LIBNAME})

if (ENABLE_TESTS)
  add_subdirectory(tests)
endif()
