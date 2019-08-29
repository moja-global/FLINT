set(PACKAGE "datarepository")
set(LIBNAME "moja.${PACKAGE}")
string(TOUPPER "${PACKAGE}" LIBNAME_EXPORT)

include(${CMAKE_MODULE_PATH}/generate_product_version.cmake) 

find_package(Poco REQUIRED Foundation JSON)

find_package(SQLite3 REQUIRED)

# Version Info
if (MSVC)
  generate_product_version(ProductVersionFiles
    NAME ${LIBNAME}
    FILE_DESCRIPTION "moja global datarepository"
    VERSION_MAJOR ${MOJA_VERSION_MAJOR}
    VERSION_MINOR ${MOJA_VERSION_MINOR}
    VERSION_PATCH ${MOJA_VERSION_PATCH}
    VERSION_REVISION ${MOJA_VERSION_REVISION}
	COMPANY_NAME "moja global"
#    VERSION_REVISION ${BUILD_ID}
  )
endif ()


# HEADERS AND SOURCE

configure_file(../templates/exports.h ${CMAKE_CURRENT_SOURCE_DIR}/include/moja/${PACKAGE}/_${PACKAGE}_exports.h)

set(MOJA_DataRepository_Relational_headers
	include/moja/${PACKAGE}/providerrelationalsqlite.h
)

set(MOJA_DataRepository_Relational_sources
	src/providerrelationalsqlite.cpp
)

set(MOJA_DataRepository_NoSQL_headers
)
set(MOJA_DataRepository_NoSQL_sources
)

set(MOJA_DataRepository_SpatialRasterTiled_headers
	include/moja/${PACKAGE}/cube.h
	include/moja/${PACKAGE}/gridlayer.h
	include/moja/${PACKAGE}/layer.h
	include/moja/${PACKAGE}/providerspatialrastertiled.h
	include/moja/${PACKAGE}/rasterreader.h
	include/moja/${PACKAGE}/rasterreaderinterface.h
	include/moja/${PACKAGE}/stack.h
	include/moja/${PACKAGE}/stacklayer.h
	include/moja/${PACKAGE}/tile.h
)

set(MOJA_DataRepository_SpatialRasterTiled_sources
	src/providerspatialrastertiled.cpp
)

set(MOJA_DataRepository_SpatialRaster_headers
	include/moja/${PACKAGE}/spatialcellscollection.h
	include/moja/${PACKAGE}/spatialblockscollection.h
	include/moja/${PACKAGE}/spatialtilescollection.h
	include/moja/${PACKAGE}/tileblockcellindexer.h
)

set(MOJA_DataRepository_SpatialRaster_sources
	src/cube.cpp
	src/layer.cpp
	src/rasterreader.cpp
	src/tileblockcellindexer.cpp
)

set(MOJA_DataRepository_headers
	include/moja/${PACKAGE}/aspatialtileinfo.h
	include/moja/${PACKAGE}/aspatialtileinfocollection.h
	include/moja/${PACKAGE}/cube.h
	include/moja/${PACKAGE}/datarepository.h
	include/moja/${PACKAGE}/datarepositoryexceptions.h
	include/moja/${PACKAGE}/iproviderinterface.h
	include/moja/${PACKAGE}/iprovidernosqlinterface.h
	include/moja/${PACKAGE}/iproviderrelationalinterface.h
	include/moja/${PACKAGE}/iproviderspatialrasterstackinterface.h
	include/moja/${PACKAGE}/iproviderspatialrasterinterface.h
	include/moja/${PACKAGE}/iproviderspatialvectorinterface.h
	include/moja/${PACKAGE}/landunitinfo.h
)

set(MOJA_DataRepository_sources
	src/aspatialtileinfo.cpp
	src/aspatialtileinfocollection.cpp
	src/datarepository.cpp
	src/landunitinfo.cpp
)

source_group("header files\\relational"   FILES ${MOJA_DataRepository_Relational_headers})
source_group("source files\\relational"   FILES ${MOJA_DataRepository_Relational_sources})
											    
source_group("header files\\nosql"   FILES ${MOJA_DataRepository_NoSQL_headers})
source_group("source files\\nosql"   FILES ${MOJA_DataRepository_NoSQL_sources})

source_group("header files\\spatialraster" FILES ${MOJA_DataRepository_SpatialRaster_headers} ${MOJA_DataRepository_SpatialRasterTiled_headers} ${MOJA_DataRepository_SpatialRasterGDAL_headers})
source_group("source files\\spatialraster" FILES ${MOJA_DataRepository_SpatialRaster_sources} ${MOJA_DataRepository_SpatialRasterTiled_sources} ${MOJA_DataRepository_SpatialRasterGDAL_sources})

source_group("header files\\spatialvector"  FILES ${MOJA_DataRepository_SpatialVectorGDAL_headers})
source_group("source files\\spatialvector"  FILES ${MOJA_DataRepository_SpatialVectorGDAL_sources})

set(SRCS 
	${MOJA_DataRepository_sources}							${MOJA_DataRepository_headers}
	${MOJA_DataRepository_Relational_sources}				${MOJA_DataRepository_Relational_headers}
	${MOJA_DataRepository_NoSQL_sources}					${MOJA_DataRepository_NoSQL_headers}
	${MOJA_DataRepository_SpatialRasterTiled_sources}		${MOJA_DataRepository_SpatialRasterTiled_headers}
	${MOJA_DataRepository_SpatialRaster_sources}			${MOJA_DataRepository_SpatialRaster_headers}
)

add_library( ${LIBNAME} ${LIB_MODE} ${SRCS} ${ProductVersionFiles})
add_library(Moja::DataRepository ALIAS ${LIBNAME})

#Set target properties
set_target_properties( ${LIBNAME} 
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
		Moja::Core Moja::FlintConfiguration Poco::Foundation Poco::JSON
	PRIVATE		
		SQLite::SQLite3
	)

if (CMAKE_SYSTEM MATCHES "Linux" )
	target_link_libraries( ${LIBNAME} 
		PRIVATE 
			dl
	)
endif(CMAKE_SYSTEM MATCHES "Linux" )

##############################################
# Installation instructions

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/MojaDataRepository)

install(TARGETS ${LIBNAME}
    EXPORT ${LIBNAME}-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

#This is required so that the exported target has the name DataRepository and not ${LIBNAME}
set_target_properties(${LIBNAME} PROPERTIES EXPORT_NAME DataRepository)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT ${LIBNAME}-targets
  FILE
    MojaDataRepositoryTargets.cmake
  NAMESPACE
    Moja::
  DESTINATION
    ${INSTALL_CONFIGDIR}
)

#Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/MojaDataRepositoryConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/MojaDataRepositoryConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/MojaDataRepositoryConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

#Install the config, configversion and custom find modules
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/MojaDataRepositoryConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/MojaDataRepositoryConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

##############################################
## Exporting from the build tree
export(EXPORT ${LIBNAME}-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/MojaDataRepositoryTargets.cmake NAMESPACE Moja::)

#Register package in user's package registry
export(PACKAGE MojaDataRepository)

if(MSVC)
	INSTALL(
		FILES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/${LIBNAME}${CMAKE_DEBUG_POSTFIX}.pdb 
		DESTINATION ${CMAKE_INSTALL_BINDIR} 
		CONFIGURATIONS Debug
		)
endif()

if (ENABLE_TESTS)
	add_subdirectory( tests )

	execute_process(
		COMMAND ${CMAKE_COMMAND} -E tar xvf ${CMAKE_CURRENT_SOURCE_DIR}/../../Examples/data.zip 
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests
		OUTPUT_QUIET)

endif ()
