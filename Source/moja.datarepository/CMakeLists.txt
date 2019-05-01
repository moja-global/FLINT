set(PACKAGE "datarepository")
set(LIBNAME "moja.${PACKAGE}")
string(TOUPPER "${PACKAGE}" LIBNAME_EXPORT)

include(${CMAKE_MODULE_PATH}/generate_product_version.cmake) 

if(MOJA_STATIC)
	set(CMAKE_CXX_FLAGS_RELEASE "/MT")
	set(CMAKE_CXX_FLAGS_DEBUG "/MTd")
	add_definitions(-DUSE_STATIC_BOOST)
    set(Boost_USE_STATIC_LIBS ON)
else(MOJA_STATIC)
	add_definitions(-DBOOST_ALL_DYN_LINK)
    set(Boost_USE_STATIC_LIBS OFF)
endif(MOJA_STATIC)

find_package(Boost)
if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
endif()

# Poco
if(Poco_FOUND)
	link_directories(${Poco_BINARY_DIRS})
    include_directories(${Poco_INCLUDE_DIRS})
endif()

# SQLite
find_package(SQLite)

include_directories(
	include 
	../moja.core/include  
	../moja.flint.configuration/include 
	${Poco_INCLUDE_DIRS}
	${Sqlite_INCLUDE_PATH}
	)

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
#   include/moja/${PACKAGE}/providerrelationalpocopostgresql.h
#	include/moja/${PACKAGE}/providerrelationallibpqpostgresql.h
)

set(MOJA_DataRepository_Relational_sources
	src/providerrelationalsqlite.cpp
	# src/providerrelationalpocopostgresql.cpp
#	src/providerrelationallibpqpostgresql.cpp
)

set(MOJA_DataRepository_NoSQL_headers
##	include/moja/${PACKAGE}/providernosqlpocomongodb.h
##	include/moja/${PACKAGE}/providernosqlpocojson.h
)
set(MOJA_DataRepository_NoSQL_sources
##	src/providernosqlpocomongodb.cpp
##	src/providernosqlpocojson.cpp
)

set(MOJA_DataRepository_SpatialRasterGDAL_headers
	include/moja/${PACKAGE}/providerspatialrastergdal.h
)
set(MOJA_DataRepository_SpatialRasterGDAL_sources
	src/providerspatialrastergdal.cpp
)

set(MOJA_DataRepository_SpatialVectorGDAL_headers
	include/moja/${PACKAGE}/providerspatialvectorgdal.h
)
set(MOJA_DataRepository_SpatialVectorGDAL_sources
	src/providerspatialvectorgdal.cpp
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
	${Sqlite_INCLUDE_PATH}/sqlite3.c 
	${MOJA_DataRepository_sources}							${MOJA_DataRepository_headers}
	${MOJA_DataRepository_Relational_sources}				${MOJA_DataRepository_Relational_headers}
	${MOJA_DataRepository_NoSQL_sources}					${MOJA_DataRepository_NoSQL_headers}
	${MOJA_DataRepository_SpatialRasterGDAL_sources}		${MOJA_DataRepository_SpatialRasterGDAL_headers}
	${MOJA_DataRepository_SpatialVectorGDAL_sources}		${MOJA_DataRepository_SpatialVectorGDAL_headers}
	${MOJA_DataRepository_SpatialRasterTiled_sources}		${MOJA_DataRepository_SpatialRasterTiled_headers}
	${MOJA_DataRepository_SpatialRaster_sources}			${MOJA_DataRepository_SpatialRaster_headers}
)

include_directories( include )

add_definitions( -DPOCO_NO_AUTOMATIC_LIBS )
add_library( ${LIBNAME} ${LIB_MODE} ${SRCS} ${ProductVersionFiles})
set_target_properties( ${LIBNAME} 
	PROPERTIES
	VERSION ${MOJA_VERSION} SOVERSION ${MOJA_VERSION_MAJOR}
	DEFINE_SYMBOL ${LIBNAME_EXPORT}_EXPORTS)

target_link_libraries( ${LIBNAME} 
	moja.core
	moja.flint.configuration 
	${Poco_FOUNDATION} 
	${Poco_JSON} 
	)

if (CMAKE_SYSTEM MATCHES "Linux" )
	target_link_libraries( ${LIBNAME} dl)
endif(CMAKE_SYSTEM MATCHES "Linux" )

add_dependencies(${LIBNAME} moja.core moja.flint.configuration)

install(
	DIRECTORY include/moja
	DESTINATION include
	PATTERN ".svn" EXCLUDE
	)

install(
	TARGETS ${LIBNAME}
	LIBRARY DESTINATION lib${LIB_SUFFIX}
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
	RUNTIME DESTINATION bin
	)

if(MSVC)
	INSTALL(
		FILES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/${LIBNAME}${CMAKE_DEBUG_POSTFIX}.pdb 
		DESTINATION bin 
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
