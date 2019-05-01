set(PACKAGE "configuration")
set(LIBNAME "moja.flint.${PACKAGE}")
string(TOUPPER "${PACKAGE}" LIBNAME_EXPORT)

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

include_directories(include ../moja.core/include)

if(Poco_FOUND)
	link_directories(${Poco_BINARY_DIRS})
endif()

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
add_library(${LIBNAME} ${LIB_MODE} ${SRCS})
set_target_properties(${LIBNAME} 
	PROPERTIES
	VERSION ${MOJA_VERSION} SOVERSION ${MOJA_VERSION_MAJOR}
	DEFINE_SYMBOL ${LIBNAME_EXPORT}_EXPORTS)

target_link_libraries(
	${LIBNAME}
	moja.core 
	${Poco_FOUNDATION} 
	${Poco_JSON}
	)

if (CMAKE_SYSTEM MATCHES "Linux")
	target_link_libraries(${LIBNAME} dl)
endif(CMAKE_SYSTEM MATCHES "Linux")

add_dependencies(${LIBNAME} moja.core)

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
  add_subdirectory(tests)
endif()
