set(PACKAGE "libpq")
set(LIBNAME "moja.modules.${PACKAGE}")
string(REPLACE "." "_" NEW_PACKAGE "${PACKAGE}")
string(TOUPPER "${NEW_PACKAGE}" LIBNAME_EXPORT)

find_package(Boost)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

#for Moja
#find_package(Moja)

# Libpq
find_package(Libpq)

include_directories( 
	include 
    ../moja.core/include
    ../moja.flint/include
    ../moja.datarepository/include
	${LIBPQ_INCLUDE_PATH}
)

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
)
 
set(PROJECT_MODULE_SOURCES
)
 
set(PROJECT_TRANSFORM_HEADERS
)

set(PROJECT_TRANSFORM_SOURCES
)
 
set(PROJECT_FLINTDATA_HEADERS
)

set(PROJECT_FLINTDATA_SOURCES
)
 
set(PROJECT_PROVIDER_HEADERS
    include/moja/modules/${PACKAGE}/providerrelationallibpqpostgresql.h
)

set(PROJECT_PROVIDER_SOURCES
    src/providerrelationallibpqpostgresql.cpp
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

#add_dependencies(
#	${LIBNAME} 
#)

target_link_libraries(
	${LIBNAME} 
	moja.core 
	moja.flint
	moja.datarepository
	${LIBPQ_LIB}
	${Poco_FOUNDATION} 
	${Poco_JSON} 
)

# Set local include path
include_directories(${Poco_INCLUDE_DIRS})

install(TARGETS ${LIBNAME}
        LIBRARY DESTINATION lib${LIB_SUFFIX}
        ARCHIVE DESTINATION lib${LIB_SUFFIX}
        RUNTIME DESTINATION bin)

	if(MSVC)
	INSTALL(
		FILES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/${LIBNAME}${CMAKE_DEBUG_POSTFIX}.pdb 
		DESTINATION bin 
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
