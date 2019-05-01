set(PACKAGE "poco")
set(LIBNAME "moja.modules.${PACKAGE}")
string(REPLACE "." "_" NEW_PACKAGE "${PACKAGE}")
string(TOUPPER "${NEW_PACKAGE}" LIBNAME_EXPORT)

find_package(Boost)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

#for Moja
#find_package(Moja)

# Poco
find_package(Poco REQUIRED)

if(Poco_FOUND)
	link_directories(${Poco_BINARY_DIRS})
    include_directories(${Poco_INCLUDE_DIRS})
endif()

include_directories( 
	include 
    ../moja.core/include
    ../moja.flint/include
    ../moja.datarepository/include
)

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
#    include/moja/modules/${PACKAGE}/providerrelationalpocopostgresql.h
)

set(PROJECT_PROVIDER_SOURCES
    src/providernosqlpocomongodb.cpp
    src/providernosqlpocojson.cpp
#	src/providerrelationalpocopostgresql.cpp
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

add_definitions( -DPOCO_NO_AUTOMATIC_LIBS )

add_library(
    ${LIBNAME} 
    ${LIB_MODE} 
    ${SRCS}
    )

set_target_properties(${LIBNAME} 
    PROPERTIES
    VERSION ${MOJA_MULLIONGROUP_VERSION} 
	SOVERSION ${MOJA_MULLIONGROUP_VERSION_MAJOR}
    DEFINE_SYMBOL ${LIBNAME_EXPORT}_EXPORTS
    )

target_link_libraries(
	${LIBNAME} 
	moja.core 
	moja.flint
	moja.datarepository
	${Poco_FOUNDATION} 
	${Poco_DATA} 
	${Poco_NET} 
	${Poco_MONGODB} 
	${Poco_JSON} 
	${Poco_DATA_SQLITE}
	${Poco_DATA_POSTGRESQL}
    )

# Set local include path
include_directories(${Poco_INCLUDE_DIRS})

install(TARGETS ${LIBNAME}
        LIBRARY DESTINATION lib${LIB_SUFFIX}
        ARCHIVE DESTINATION lib${LIB_SUFFIX}
        RUNTIME DESTINATION bin)

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif()
